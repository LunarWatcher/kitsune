#include "Modeline.hpp"
#include "gdkmm/enums.h"
#include "gtkmm/applicationwindow.h"
#include "gtkmm/enums.h"
#include "kitsune/app/modes/ModalInputProcessor.hpp"
#include <iostream>

namespace kitsune {

// TODO: Bug list:
// * commandInput does not block navigation, or otherwise deactivate command mode
// * commandInput does not restore focus, and seems to prioritize the VTE widget when using the arrows to get some kind
//   of focus.
//   We can get the focus from ApplicationWindow and maybe store that in the modeline, but feels like a nasty hack. Gtk
//   must surely have a mechanism for temporary focus? It's not an obscure use-case
// * ```
//   (kitsune:225068): Gtk-WARNING **: 18:01:12.804: GtkText - did not receive a focus-out event.
//   If you handle this event, you must return GDK_EVENT_PROPAGATE so the default handler gets the event as well
//   ```
// * The focus updates currently break when the focus enters the listview. up and down work, but left and right do not


Modeline::Modeline(
    const std::shared_ptr<ModeController>& modeController,
    Gtk::ApplicationWindow* window
) : modeController(modeController),
    container(Gtk::Orientation::VERTICAL, 0),
    modelineContainer(Gtk::Orientation::HORIZONTAL, 0),
    commandInputContainer(Gtk::Orientation::HORIZONTAL, 0),
    commandInputProcessor(
        std::make_shared<ModalInputProcessor>(
            modeController,
            ModalInputType::CommandInput
        )
    ),
    window(window)
{
    container.add_css_class("modeline-root");
    modelineContainer.add_css_class("modeline-container");
    commandInputContainer.add_css_class("input-container");

    modelineContainer.add_css_class("pad-medium");
    commandInputContainer.add_css_class("pad-medium");
    
    commandInputProcessor->signal_key_pressed().connect(
        [this](guint k, guint, Gdk::ModifierType mod) -> bool {
            // Block arrow key nav (?). In the future, these will navigate an autocomplete dropdown instead, maybe
            if (k == GDK_KEY_Up
                || k == GDK_KEY_Down
                || k == GDK_KEY_Left
                || k == GDK_KEY_Right
               ) {
                return true;
            }
            return false;
        },
        true
    );
    
    commandInput.add_controller(commandInputProcessor);
    commandInput.set_sensitive(false);
    commandInput.signal_activate().connect([this]() {
        // activate means enter is pressed (in Gtk::Text anyway)
        auto text = commandInput.get_text();
        std::cout << "Exec " << text << std::endl;

        this->modeController->setMode(Mode::Normal);
    });
    commandInput.set_expand(true);

    commandInput.signal_state_flags_changed().connect([this](Gtk::StateFlags flags) {
        // TODO: I think we can use this to address bug 1, but I don't entirely understand how. I think I can
        // if ((flags & Gtk::StateFlags::FOCUSED) == 0)
        // (or possibly using bitwise NOT, I'm not sure)
        // to intercept it. There's no explicit focus change signal as far as I can tell (which is weird, because I keep
        // getting a log message whining about the focus changed listener not being received)
        std::cout << (int) flags << std::endl;
    });

    commandInputContainer.append(commandLabel);
    commandInputContainer.append(commandInput);

    container.append(modelineContainer);
    container.append(commandInputContainer);

    modeController->signal_mode_changed().connect([this](Mode mode) {
        commandInput.set_sensitive(mode == Mode::Command);
        if (mode == Mode::Command) {
            commandLabel.set_text(":");
            commandInput.grab_focus();
        } else {
            commandLabel.set_text("");
            commandInput.set_text("");
        }
    });

}

}
