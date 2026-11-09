#include "TaskRunner.hpp"
#include "glibmm/main.h"

namespace kitsune {

struct TaskRunnerShutdownSig {
    TaskRunner& runner;

    ~TaskRunnerShutdownSig() {
        try {
            runner.signalDone();
        } catch (const std::exception& e) {
            minilog::error(
                "An exception occurred while sending signal: {}",
                e.what()
            );
        }
    }
};

TaskRunner::TaskRunner(LuaApi* api, std::vector<Task*>&& taskOrder, const std::vector<std::string>& manuallyRequested)
    : api(api),
      threadState(lua_newthread(api->getState())),
      taskOrder(std::move(taskOrder)),
      manuallyRequested(manuallyRequested)
{
    ref = luaL_ref(api->getState(), LUA_REGISTRYINDEX);

    for (auto& task : this->taskOrder) {
        // Preflight: figure out which tasks need to be killed, and reset the view
        if (
            task->shouldRun()
            || std::find(manuallyRequested.begin(), manuallyRequested.end(), task->getId())
                   != manuallyRequested.end()
        ) {
            task->executor.state->terminalView->reset();
        }
    }
}

TaskRunner::~TaskRunner()
{
    running.store(false);
    minilog::debug("About to wait for thread join");
    if (runControl.joinable()) {
        runControl.join();
    }
    minilog::debug("Thread join cleared");

    luaL_unref(threadState, LUA_REGISTRYINDEX, ref);
}

void TaskRunner::start()
{
    minilog::debug("Preparing to start TaskRunner with {} tasks", taskOrder.size());
    runControl = std::thread([this]() {
        TaskRunnerShutdownSig sig(*this);
        try {
            for (auto task : taskOrder) {
                if (
                    task->shouldRun()
                    || std::find(manuallyRequested.begin(), manuallyRequested.end(), task->getId())
                    != manuallyRequested.end()
                ) {
                    auto result = task->run(this->threadState, running);
                    if (!result) {
                        return;
                    }
                }
            }
            this->success = true;
        } catch (const std::exception& e) {
            minilog::error(
                "An error occurred while processing pipeline startup: {}",
                e.what()
            );
        }
    });
}

void TaskRunner::signalDone() {
    Glib::signal_timeout().connect(
        [this]() -> bool {
            running.store(false);
            runComplete.emit(success);

            return false;
        },
        0
    );
}

bool TaskRunner::isRunning() {
    return running.load();
}

}
