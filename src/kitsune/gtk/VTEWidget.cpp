#include "VTEWidget.hpp"
#include "glibmm/main.h"
#include "kitsune/theming/TermColour.hpp"
#include "minilog/minilog.hpp"
#include <vte/vte.h>

namespace kitsune {

static void termCallback(VteTerminal* term, GPid pid, GError* error, gpointer udata) {
    VTEWidget* widget = (VTEWidget*) udata;

    if (pid == -1) {
        widget->state = VTEState::LaunchFail;
    } else {
        widget->state = VTEState::Running;
        // vte_terminal_watch_child(term, pid);
    }
}

static void childExit(VteTerminal* term, gint status, gpointer udata) {
    VTEWidget* widget = (VTEWidget*) udata;

    minilog::debug("Child proc exited with {}", status);
    if (status == 0) {
        widget->state = VTEState::ExitOk;
    } else {
        widget->state = VTEState::ExitError;
    }
}

std::expected<std::shared_ptr<VTEWidget>, std::string> VTEWidget::create(
    const Config& conf
) {
    auto* terminal = vte_terminal_new();
    if (terminal == nullptr) {
        // TODO: can this even happen?
        return std::unexpected("Failed to init VTE");
    }

    return std::shared_ptr<VTEWidget>(
        new VTEWidget(conf, terminal)
    );
}

VTEWidget::VTEWidget(const Config& conf, GtkWidget* terminal) noexcept : terminal(terminal) {
    this->widget = Glib::wrap(terminal, false);

    g_signal_connect(
        terminal,
        "child-exited",
        G_CALLBACK(childExit),
        this
    );

    applyTheming(conf);
}

// TODO: I don't _think_ a destructor is necessary for widgets added to the node, but GtkWidget is a bit unclear. Since
// we let Glib::wrap take ownership (take_copy = false), I don't _think_ anything should leak, but this may need to be
// sanity-checked.
VTEWidget::~VTEWidget() = default;

void VTEWidget::applyTheming(
    const Config& conf
) {
    const auto& scheme = conf.scheme;
    vte_terminal_set_colors(
        VTE_TERMINAL(terminal),
        &scheme.foreground(),
        &scheme.background(),
        scheme.palette().data(),
        scheme.palette().size()
    );

    auto& font = scheme.font();
    if (font.has_value()) {
        auto desc = pango_font_description_from_string(
            font->c_str()
        );
        vte_terminal_set_font(VTE_TERMINAL(terminal), desc);
    }
}

void VTEWidget::spawn(const std::vector<std::string>& command) {
    if (command.size() == 0) {
        throw std::runtime_error("Cannot run empty command");
    }

    state = VTEState::PreLaunch;
    // Forces the terminal spawning to run on the main thread, because gtk isn't multithreaded
    // Didn't cause problems in local testing, but there be dragons without this apparently:
    // https://stackoverflow.com/a/60949270
    //
    // The rest of the VTE calls are async anyway, and we observe the results of that via variables stored in this
    // instance. Syncing isn't important, since all we risk doing is wasting a *gasp* whole entire second.
    Glib::signal_timeout().connect(
        [this, command]() {

            std::vector<gchar*> converted;
            converted.reserve(command.size() + 1);
            for (auto& str : command) {
                converted.push_back((gchar*) str.c_str());
            }
            converted.push_back(nullptr);

            vte_terminal_spawn_async(
                VTE_TERMINAL(terminal),
                VTE_PTY_DEFAULT,
                nullptr,
                (gchar**) converted.data(),
                nullptr,
                GSpawnFlags::G_SPAWN_DEFAULT,
                nullptr, nullptr,
                nullptr,
                -1,
                nullptr,
                termCallback,
                this
            );

            return false;
        },
        0
    );
}

std::string VTEWidget::getContent() {
    auto content = vte_terminal_get_text_format(
        VTE_TERMINAL(terminal),
        VteFormat::VTE_FORMAT_TEXT // HTML might be a good candidate for more semantic parsing, but I think most of
        // my parsing is just going to be text-based, so I guess this is easier
    );

    if (content == nullptr) {
        return "";
    }

    std::string str(content);
    free(content);

    return str;
}

}
