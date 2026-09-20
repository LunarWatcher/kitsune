#include "gtkmm/application.h"
#include "gtkmm/widget.h"
#include "kitsune/gtk/VTEWidget.hpp"

#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <chrono>

#include <CLI/CLI.hpp>

class WindowImpl : public Gtk::Window {
private:
    Gtk::Frame f;

    std::shared_ptr<kitsune::VTEWidget> terminal;
public:
    WindowImpl() {
        set_title("*pounces* rawr x3");
        set_default_size(720, 480);

        f.set_margin(10);
        f.set_label("danger zone");

        set_child(f);

        auto result = kitsune::VTEWidget::create();

        if (result) {
            this->terminal = *result;
        } else {
            throw std::runtime_error(result.error());
        }
        f.set_child(*terminal->ptr());

        terminal->spawn({
            "/usr/bin/bash"
        });
        auto t = new std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "Term content:\n" << terminal->getContent() << std::endl;
        });
    }
};

int main(int argc, char** argv) {

    CLI::App app {
        "CLI and GUI dev support tool"
    };

    CLI12_PARSE(app, argc, argv);

    auto gui = Gtk::Application::create(
        "lunarwatcher.kitsune.test"
    );

    return gui->make_window_and_run<WindowImpl>(argc, argv);

}
