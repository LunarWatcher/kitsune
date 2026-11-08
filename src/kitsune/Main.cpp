#include "gtkmm/application.h"
#include "kitsune/app/KitsuneWindow.hpp"

#include <gtkmm.h>

int main(int argc, char** argv) {

    // The docs say this is fine: https://developer.gnome.org/documentation/tutorials/application-id.html
    auto gui = Gtk::Application::create(
        "org.codeberg.lunarwatcher.kitsune",
        Gio::Application::Flags::NON_UNIQUE
    );

    gui->add_main_option_entry(
        Gio::Application::OptionType::BOOL,
        "debug",
        'd',
        "Whether or not to enable debug logging"
    );
    gui->signal_handle_local_options()
        .connect([](const std::shared_ptr<Glib::VariantDict>& argc) -> int{
            bool debug = false;
            argc->lookup_value("debug", debug);
            if (debug) {
                minilog::setLevel(minilog::Level::Debug);
            }
            return -1;
        }, false);

    return gui->make_window_and_run<kitsune::MainWindow>(
        argc, argv, gui
    );

}
