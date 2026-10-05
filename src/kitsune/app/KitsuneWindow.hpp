#pragma once

#include "gtkmm/box.h"
#include "gtkmm/applicationwindow.h"

#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/app/modes/ModeController.hpp"
#include "kitsune/components/Modeline.hpp"
#include "kitsune/components/TermList.hpp"
#include "kitsune/config/Config.hpp"

namespace kitsune {

class MainWindow : public Gtk::ApplicationWindow {
private:
    Gtk::Box root;

    std::shared_ptr<ModeController> modeController;
    std::shared_ptr<ModalInputProcessor> fallbackInputProcessor;

    TermList terminals;
    Modeline modeline;

    kitsune::Config conf;

    void initRootContainer();
    void loadCSS();
public:
    MainWindow(const Glib::RefPtr<Gtk::Application>& app);
};

}
