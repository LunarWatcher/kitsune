#pragma once

#include "gtkmm/box.h"
#include "gtkmm/label.h"
#include "gtkmm/text.h"
#include "gtkmm/applicationwindow.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include "kitsune/app/modes/ModeController.hpp"
#include "kitsune/components/commands/CommandDispatcher.hpp"
namespace kitsune {

class Modeline {
private:
    std::shared_ptr<ModeController> modeController;
    std::shared_ptr<CommandDispatcher> dispatcher;

    Gtk::Box container;
    Gtk::Box modelineContainer;
    Gtk::Box commandInputContainer;

    Gtk::Label commandLabel;
    Gtk::Text commandInput;

    Gtk::Label modeLabel;
    Gtk::Label errorLabel;

    void initCSS();
    void initModeline();
    void initCommandInput();

    std::shared_ptr<ModalInputProcessor> commandInputProcessor;
    Gtk::ApplicationWindow* window;
public:
    Modeline(
        const std::shared_ptr<ModeController>& modeController,
        Gtk::ApplicationWindow* window,
        const std::shared_ptr<CommandDispatcher>& dispatcher
    );

    Gtk::Widget* root() { return &container; }

    void setError(const std::string& errorMessage);
};

}
