#pragma once

#include "giomm/liststore.h"
#include "gtkmm/box.h"
#include "gtkmm/frame.h"
#include "gtkmm/listview.h"
#include "gtkmm/singleselection.h"

#include "gtkmm/stack.h"
#include "gtkmm/stacksidebar.h"
#include "kitsune/model/TerminalModel.hpp"

namespace kitsune {

class TermList {
private:
    Gtk::Box rootContainer;
    Gtk::ListView rootView;

    Gtk::Stack termContainer;

    Glib::RefPtr<Gio::ListStore<TerminalModel>> dataModel;
    Glib::RefPtr<Gtk::SingleSelection> selectionModel;

public:
    TermList();

    Gtk::Widget* root() { return &rootContainer; }
    void addTerminal(const Config& conf);
};

}
