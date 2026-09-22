#include "gtkmm/application.h"
#include "kitsune/app/KitsuneWindow.hpp"

#include <gtkmm.h>

#include <CLI/CLI.hpp>

int main(int argc, char** argv) {

    CLI::App app {
        "CLI and GUI dev support tool"
    };

    CLI12_PARSE(app, argc, argv);

    // The docs says this is fine: https://developer.gnome.org/documentation/tutorials/application-id.html
    auto gui = Gtk::Application::create(
        "org.codeberg.lunarwatcher.kitsune"
    );

    return gui->make_window_and_run<kitsune::MainWindow>(argc, argv);

}
