#include "VTEWidget.hpp"
#include <vte/vte.h>

namespace kitsune {

std::expected<std::shared_ptr<VTEWidget>, std::string> VTEWidget::create() {
    auto* terminal = vte_terminal_new();
    if (terminal == nullptr) {
        // TODO: can this even happen?
        return std::unexpected("Failed to init VTE");
    }

    return std::shared_ptr<VTEWidget>(
        new VTEWidget(terminal)
    );
}

VTEWidget::VTEWidget(GtkWidget* terminal) noexcept : terminal(terminal) {
    this->widget = Glib::wrap(terminal, false);

    applyTheming();
}

// TODO: I don't _think_ a destructor is necessary for widgets added to the node, but GtkWidget is a bit unclear. Since
// we let Glib::wrap take ownership (take_copy = false), I don't _think_ anything should leak, but this may need to be
// sanity-checked.
VTEWidget::~VTEWidget() = default;

void VTEWidget::applyTheming() {
    // TODO: use GdkRGBA directly instead of this
#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_16(x)  ((double)(x) / 0xff)
#define CLR_GDK(x) (const GdkRGBA){ .red = CLR_16(CLR_R(x)),    \
                                    .green = CLR_16(CLR_G(x)),  \
                                    .blue = CLR_16(CLR_B(x)),   \
                                    .alpha = 0 }
    auto foreground = CLR_GDK(0x000000);
    auto background = CLR_GDK(0xffffff);
    // Export of my current konsole scheme (which itself is an export from a gnome terminal default scheme)
    std::vector<GdkRGBA> it = {
        CLR_GDK(0x171421),
        CLR_GDK(0xc01c28),
        CLR_GDK(0x26a269),
        CLR_GDK(0xa2734c),
        CLR_GDK(0x12488b),
        CLR_GDK(0xa347ba),
        CLR_GDK(0x2aa1b3),
        CLR_GDK(0xd0cfcc),
        CLR_GDK(0x5e5c64),
        CLR_GDK(0xf66151),
        CLR_GDK(0x33d17a),
        CLR_GDK(0xe9ad0c),
        CLR_GDK(0x2a7bde),
        CLR_GDK(0xc061cb),
        CLR_GDK(0x33c7de),
        CLR_GDK(0xffffff),
    };
    vte_terminal_set_colors(
        VTE_TERMINAL(terminal),
        &foreground,
        &background,
        it.data(), it.size()
    );
    auto desc = pango_font_description_from_string(
        "SauceCodePro Nerd Font 12"
    );
    vte_terminal_set_font(VTE_TERMINAL(terminal), desc);
}

void VTEWidget::spawn(const std::vector<std::string>& command) {
    if (command.size() == 0) {
        throw std::runtime_error("Cannot run empty command");
    }

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
        (GSpawnFlags) 0,
        nullptr, nullptr,
        nullptr,
        -1,
        nullptr,
        nullptr,
        nullptr
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
