#include "Task.hpp"
#include "kitsune/gtk/VTEWidget.hpp"
#include <thread>

namespace kitsune {

bool Task::shouldRun() {
    // PreLaunch is always set on the first run, so if it's PreLaunch, it is executable
    if (getTerminal()->state == VTEState::PreLaunch) {
        return true;
    }

    if (rerunSpec == Rerun::Never) {
        return false;
    } else if (
        rerunSpec == Rerun::IfCompleted
        && getTerminal()->isRunning()
    ) {
        return false;
    }

    return true;
}

bool Task::run(
    lua_State* L,
    const std::atomic<bool>& running
) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, this->luaRefKey);
    minilog::debug("Task::run for id = {}", id);
    lua_getfield(L, -1, "command");

    if (!lua_isfunction(L, -1)) {
        minilog::warn("Could not execute {}: no command func", this->id);
        return false;
    }

    executor.getUData(L);
    // TODO: inputs

    lua_call(L, 1, 0);
    lua_pop(L, lua_gettop(L) - 1);

    lua_getfield(L, -1, "liveness");

    if (lua_isfunction(L, -1)) {
        minilog::debug("id = {} has liveness probe");

        while (running.load()) {
            // Clear either the liveness probe on the stack used to check if it exists, or the bool value from a
            // previous iteration of this function
            lua_pop(L, 1);
            lua_getfield(L, -1, "liveness");
            lua_call(L, 0, 1);

            if (lua_toboolean(L, -1)) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } else {
        while (running.load()) {
            auto state = getTerminal()->state;
            if (
                state == VTEState::ExitOk
                || state == VTEState::ExitError
                || state == VTEState::LaunchFail
            ) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    lua_pop(L, lua_gettop(L));

    if (!running.load()) {
        minilog::warn("Pipeline containing {} is shutting down");
        return false;
    }

    auto state = getTerminal()->state;
    if (state == VTEState::ExitError) {
        minilog::error("{} failed!", id);
        return false;
    } else if (state == VTEState::ExitOk) {
        minilog::info("{} is complete", id);
        return true;
    } else if (state == VTEState::Running) {
        minilog::info("{} is live", id);
        return true;
    } else {
        minilog::error("Got unexpected state {} for task {}", static_cast<int>(state), id);
        return false;
    }
}

}
