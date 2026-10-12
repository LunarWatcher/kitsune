#pragma once

#include "kitsune/api/LuaApi.hpp"
#include <lua.hpp>
#include <thread>

namespace kitsune {

struct TaskRunner {
    LuaApi* api;
    lua_State* threadState;
    int ref;

    std::thread runControl;
    
    TaskRunner(
        LuaApi* api,
        std::vector<Task*>&& taskOrder
    )
        : api(api),
          threadState(lua_newthread(api->getState()))
    {
        ref = luaL_ref(api->getState(), LUA_REGISTRYINDEX);

        runControl = std::thread(
            [this, taskOrder = std::move(taskOrder)]() {

            for (auto task : taskOrder) {
                task->run(this->threadState);
            }
        });
    }

    TaskRunner(TaskRunner&) = delete;
    TaskRunner(TaskRunner&&) = delete;

    ~TaskRunner() {
        luaL_unref(threadState, LUA_REGISTRYINDEX, ref);
        runControl.join();
    }

};

}
