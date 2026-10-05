#include "ModeController.hpp"

#include <shared_mutex>
#include <mutex>

namespace kitsune {

ModeController::ModeController() = default;

Mode ModeController::getMode() {
    std::shared_lock l(m);
    return mode;
}

void ModeController::setMode(Mode newMode) {
    std::unique_lock l(m);
    mode = newMode;

    signalModeChanged.emit(newMode);
}

}
