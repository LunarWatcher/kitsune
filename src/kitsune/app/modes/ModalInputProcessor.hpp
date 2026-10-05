#pragma once

#include "gdkmm/enums.h"
#include "gtkmm/eventcontrollerkey.h"
#include "kitsune/app/modes/ModeController.hpp"

namespace kitsune {

enum class ModalInputType {
    InsertOnly,
    ForwardNavigation,
    CommandInput,
    Window,
};

class ModalInputProcessor : public Gtk::EventControllerKey {
private:
    std::shared_ptr<ModeController> modeController;

    bool handleInsertModeOnly(guint k, guint, Gdk::ModifierType modifiers);
    bool handleNavigationAllowedInput(guint k, guint, Gdk::ModifierType modifiers);
    bool handleCommandInput(guint k, guint, Gdk::ModifierType modifiers);
    bool handleWindowInput(guint k, guint, Gdk::ModifierType modifiers);

    bool handleNormalModeInput(
        guint k, Gdk::ModifierType modifiers
    );
    bool handleInsertModeInput(
        guint k, Gdk::ModifierType modifiers
    );

    void recursiveNavigate(Gtk::DirectionType direction);
public:
    ModalInputProcessor(
        const std::shared_ptr<ModeController>& modeController,
        ModalInputType type
    );
};

}
