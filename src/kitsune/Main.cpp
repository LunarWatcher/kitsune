#include "gdkmm/rgba.h"
#include "gtkmm/application.h"
#include "gtkmm/widget.h"
#include "pango/pango-font.h"

#include <gtkmm.h>
#include <vte/vte.h>

#include <iostream>
#include <thread>
#include <chrono>

static void
child_ready(VteTerminal *terminal, GPid pid, GError *error, gpointer user_data) {
    std::cout << "Callback invoked" << std::endl;

    auto t = new std::thread([terminal]() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        auto content = vte_terminal_get_text_format(
            terminal,
            VteFormat::VTE_FORMAT_TEXT // HTML might be a good candidate for more semantic parsing, but I think most of
            // my parsing is just going to be text-based, so I guess this is easier
        );

        std::cout << "Term content:\n" << content << std::endl;
    });
}

class WindowImpl : public Gtk::Window {
private:
    Gtk::Frame f;

    GtkWidget* term;
    Gtk::Widget* termWrapper;
public:
    WindowImpl() {
        set_title("*pounces* rawr x3");
        set_default_size(720, 480);

        f.set_margin(10);
        f.set_label("danger zone");

        set_child(f);

        gchar **envp = g_get_environ();
        gchar* command[] { "/usr/bin/zsh", NULL };
        g_strfreev(envp);

        term = vte_terminal_new();
        vte_terminal_spawn_async(VTE_TERMINAL(term),
            VTE_PTY_DEFAULT,
            NULL,
            command,
            NULL,
            (GSpawnFlags) 0,
            NULL, NULL,
            NULL,
            -1,
            NULL,
            child_ready,
            NULL);
#define CLR_R(x)   (((x) & 0xff0000) >> 16)
#define CLR_G(x)   (((x) & 0x00ff00) >>  8)
#define CLR_B(x)   (((x) & 0x0000ff) >>  0)
#define CLR_16(x)  ((double)(x) / 0xff)
#define CLR_GDK(x) (const GdkRGBA){ .red = CLR_16(CLR_R(x)),    \
            .green = CLR_16(CLR_G(x)),                          \
            .blue = CLR_16(CLR_B(x)),                           \
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
        vte_terminal_set_colors(VTE_TERMINAL(term),
            &foreground,
            &background,
            it.data(), it.size()
        );
        auto desc = pango_font_description_from_string(
            "SauceCodePro Nerd Font 12"
        );
        vte_terminal_set_font(VTE_TERMINAL(term), desc);

        termWrapper = Glib::wrap(term, true);

        f.set_child(*termWrapper);
    }
};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create(
        "lunarwatcher.kitsune.test"
    );

    return app->make_window_and_run<WindowImpl>(argc, argv);
}
