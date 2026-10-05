#pragma once

#include "Mode.hpp"
#include "gtkmm/object.h"
#include <shared_mutex>

namespace kitsune {

class ModeController {
public:
    using SignalModeChanged = sigc::signal<void(Mode)>;
private:
    // TODO: do we actually need this, or is all input etc. going to be single-threaded?
    std::shared_mutex m;

    Mode mode = Mode::Insert;

    SignalModeChanged signalModeChanged;
public:
    ModeController();

    Mode getMode();
    void setMode(Mode newMode);

    SignalModeChanged signal_mode_changed() {
        return signalModeChanged;
    }
};

}
