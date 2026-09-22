#pragma once

#include "gtkmm/box.h"
#include "gtkmm/label.h"
#include "gtkmm/window.h"

#include "kitsune/components/TermList.hpp"
#include "kitsune/config/Config.hpp"

namespace kitsune {

class MainWindow : public Gtk::Window {
private:
    Gtk::Box root;
    TermList terminals;

    kitsune::Config conf;
public:
    MainWindow();
};

}
