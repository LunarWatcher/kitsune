#include "KitsuneWindow.hpp"

#include <thread>
#include <chrono>

namespace kitsune {

MainWindow::MainWindow() : root(Gtk::Orientation::HORIZONTAL, 8) {
    set_title("Kitsune");
    set_default_size(720, 480);

    root.append(*terminals.root());
    set_child(root);

    // new std::thread([this]() {
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        for (size_t i = 0; i < 3; ++i) {
            terminals.addTerminal(conf);
            // std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    // });
}

}
