#include "KitsuneWindow.hpp"
#include "gtk/gtk.h"
#include "gtkmm/cssprovider.h"

#include <thread>
#include <chrono>

namespace kitsune {

MainWindow::MainWindow() : root(Gtk::Orientation::HORIZONTAL, 8) {
    set_title("Kitsune");
    set_default_size(720, 480);

    root.append(*terminals.root());
    set_child(root);

    auto css = Gtk::CssProvider::create();
    css->load_from_string(
        #include "AppStyle.css"
    );

    Gtk::StyleContext::add_provider_for_display(
        this->get_display(),
        css,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // new std::thread([this]() {
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        for (size_t i = 0; i < 3; ++i) {
            terminals.addTerminal(conf);
            // std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    // });
}

}
