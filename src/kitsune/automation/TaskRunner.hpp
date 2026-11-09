#pragma once

#include "kitsune/api/LuaApi.hpp"
#include <lua.hpp>
#include <thread>

namespace kitsune {

class TaskRunner {
public:
    using SignalRunComplete = sigc::signal<void(bool)>;
private:
    SignalRunComplete runComplete;

    LuaApi* api;
    lua_State* threadState;
    int ref;
    std::atomic<bool> running = true;
    bool success = false;
    std::vector<Task*> taskOrder;
    std::vector<std::string> manuallyRequested;

    std::thread runControl;

    void signalDone();

public:
    TaskRunner(LuaApi* api, std::vector<Task*>&& taskOrder, const std::vector<std::string>& manuallyRequested);

    TaskRunner(TaskRunner&) = delete;
    TaskRunner(TaskRunner&&) = delete;

    ~TaskRunner();

    void start();

    SignalRunComplete signal_run_complete() {
        return runComplete;
    }

    bool isRunning();

    friend struct TaskRunnerShutdownSig;
};

}
