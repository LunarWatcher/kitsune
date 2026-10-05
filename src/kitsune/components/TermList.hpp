#pragma once

#include "giomm/liststore.h"
#include "gtkmm/listview.h"
#include "gtkmm/paned.h"
#include "gtkmm/singleselection.h"

#include "gtkmm/stack.h"
#include "gtkmm/stacksidebar.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/app/modes/ModeController.hpp"
#include "kitsune/model/TerminalModel.hpp"

namespace kitsune {

class TermList {
private:
    std::shared_ptr<ModeController> modeController;

    Gtk::Paned rootContainer;
    Gtk::ListView rootView;

    Gtk::Stack termContainer;

    Glib::RefPtr<Gio::ListStore<TerminalModel>> dataModel;
    Glib::RefPtr<Gtk::SingleSelection> selectionModel;

    std::shared_ptr<ModalInputProcessor> listInputProcessor;
public:
    TermList(
        const std::shared_ptr<ModeController>& modeController
    );

    Gtk::Widget* root() { return &rootContainer; }
    void addTerminal(const Config& conf);
};

}
