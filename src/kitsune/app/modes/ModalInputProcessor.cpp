#include "ModalInputProcessor.hpp"
#include "gdk/gdkkeysyms.h"
#include "gdkmm/enums.h"
#include "gtkmm/enums.h"

namespace kitsune {

ModalInputProcessor::ModalInputProcessor(
    const std::shared_ptr<ModeController>& modeController,
    ModalInputType type
) : modeController(modeController) {
    switch (type) {

    case ModalInputType::InsertOnly:
        signal_key_pressed().connect(
            sigc::mem_fun(*this, &ModalInputProcessor::handleInsertModeOnly),
            true
        );
        break;
    case ModalInputType::ForwardNavigation:
        signal_key_pressed().connect(
            sigc::mem_fun(*this, &ModalInputProcessor::handleNavigationAllowedInput),
            true
        );
        break;
    case ModalInputType::CommandInput:
        signal_key_pressed().connect(
            sigc::mem_fun(*this, &ModalInputProcessor::handleCommandInput),
            true
        );
        break;
    case ModalInputType::Window:
        signal_key_pressed().connect(
            sigc::mem_fun(*this, &ModalInputProcessor::handleWindowInput),
            true
        );
    }
}
bool ModalInputProcessor::handleWindowInput(guint k, guint, Gdk::ModifierType modifiers) {
    if (modeController->getMode() == Mode::Normal) {
        return handleNormalModeInput(k, modifiers);
    }
    return false;
}

bool ModalInputProcessor::handleInsertModeOnly(guint k, guint, Gdk::ModifierType modifiers) {
    if (modeController->getMode() != Mode::Insert) {
        if (modeController->getMode() == Mode::Normal) {
            handleNormalModeInput(k, modifiers);
        }
        return true;
    }
    return handleInsertModeInput(k, modifiers);
}

bool ModalInputProcessor::handleNavigationAllowedInput(guint k, guint, Gdk::ModifierType modifiers) {
    if (modeController->getMode() == Mode::Normal) {
        switch (k) {
        case GDK_KEY_Return:
            // Nav key; allow
            return false;
        }
        handleNormalModeInput(k, modifiers);
        return true;
    } else if (modeController->getMode() == Mode::Insert) {
        return handleInsertModeInput(k, modifiers);
    }
    return false;
}

bool ModalInputProcessor::handleCommandInput(guint k, guint, Gdk::ModifierType modifiers) {
    if (modeController->getMode() != Mode::Command) {
        // Theoretically reundant: do not accept input into the field outside command mode, and  route stuff to normal
        // mode if applicable.
        if (modeController->getMode() == Mode::Normal) {
            handleNormalModeInput(k, modifiers);
        }
        return true;
    }
    if (k == GDK_KEY_Escape) {
        modeController->setMode(Mode::Normal);
        return true;
    }
    return false;
}

bool ModalInputProcessor::handleNormalModeInput(
    guint k, Gdk::ModifierType modifiers
) {
    switch (k) {
    case GDK_KEY_i:
    case GDK_KEY_a:
        modeController->setMode(Mode::Insert);
        return true;
    case GDK_KEY_colon:
        modeController->setMode(Mode::Command);
        return true;

    case GDK_KEY_Up:
        recursiveNavigate(Gtk::DirectionType::UP);
        return true;
    case GDK_KEY_Down:
        recursiveNavigate(Gtk::DirectionType::DOWN);
        return true;
    case GDK_KEY_Left:
        recursiveNavigate(Gtk::DirectionType::LEFT);
        return true;
    case GDK_KEY_Right:
        recursiveNavigate(Gtk::DirectionType::RIGHT);
        return true;
    }
    return false;
}

void ModalInputProcessor::recursiveNavigate(Gtk::DirectionType direction) {
    Gtk::Widget* widget = get_widget()->get_parent();

    // TODO: this doesn't seem to work with left/right nav when a ListViewItem gains focus 
    while (widget != nullptr) {
        if (widget->child_focus(direction)) {

            // This just prints gtkmm_GtkWidget. Not particularly helpful
            // logger::debug(
            //     "Navigating on %s",
            //     g_type_name(widget->get_type())
            // );
            break;
        }
        widget = widget->get_parent();
    }

}

bool ModalInputProcessor::handleInsertModeInput(
    guint k, Gdk::ModifierType modifiers
) {
    switch (k) {
    case GDK_KEY_Escape:
        if (modifiers == Gdk::ModifierType::ALT_MASK) {
            modeController->setMode(Mode::Normal);
            return true;
        }
    }
    return false;
}

}
