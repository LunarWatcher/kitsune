#include "TaskGraph.hpp"
#include "kitsune/automation/TaskRunner.hpp"
#include "kitsune/gtk/VTEWidget.hpp"

#include <format>
#include <kitsune/algorithm/DAG.hpp>
#include <kitsune/api/LuaApi.hpp>
#include <lua.hpp>

namespace kitsune {

std::expected<bool, std::string> TaskGraph::push(
    const std::shared_ptr<Task>& task,
    LuaApi* api
) {
    // As far as I can tell, it should be sufficient to trace the deps one way from the added node. In fact, because we
    // don't allow adding constraints to tasks that haven't been defined yet, we only need to check the immediate deps.
    // * We not not have anything pointing to this node that doesn't exist yet, so we don't need to check the entire
    //   graph to make sure we don't get a cycle from existing connections.
    // * The only connections that appear from adding this node are the deps this node has, so we don't need to check
    //   the deps of the immediate deps, even
    //
    // This is a DAG

    for (auto& dep : task->depends) {
        if (dep == task->id) {
            return std::unexpected(
                std::format("Task {} depends on itself!", task->id)
            );
        }

        auto it = tasks.find(dep);
        if (it == tasks.end()) {
            return std::unexpected(
                std::format("Task {} tried depending on {}, which has not been loaded yet", task->id, dep)
            );
        }

        // Insert all the dependencies from the previous level
        task->transitiveDependencies.insert(
            it->second->transitiveDependencies.begin(),
            it->second->transitiveDependencies.end()
        );
        task->transitiveDependencies.insert(
            it->second->depends.begin(),
            it->second->depends.end()
        );
    }

    // Transitive reduction (ensures the graph can be treated as linear)
    std::erase_if(
        task->depends,
        [&](const auto& dep) {
            return task->transitiveDependencies.contains(dep);
        }
    );

    task->executor.state = api->getTermList()->addBlankTerminal(
        *api->getConfig(),
        task->displayName
    );
    tasks[task->id] = task;

    return true;
}

std::shared_ptr<TaskRunner> TaskGraph::start(
    const std::vector<std::string>& taskList,
    LuaApi* api
) {
    auto executionOrder = dag::dfs(
        taskList,
        this->tasks
    );

    return std::make_shared<TaskRunner>(
        api,
        std::move(executionOrder)
    );
}

void Task::run(lua_State* L) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, this->luaRefKey);
    minilog::debug("Task::run for id = {}", id);
    lua_getfield(L, -1, "command");

    if (!lua_isfunction(L, -1)) {
        minilog::warn("Could not execute {}: no command func", this->id);
        return;
    }

    executor.getUData(L);
    // TODO: inputs

    lua_call(L, 1, 0);
    lua_pop(L, lua_gettop(L) - 1);

    lua_getfield(L, -1, "liveness");

    if (lua_isfunction(L, -1)) {
        minilog::debug("id = {} has liveness probe");

        while (true) {
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
        while (true) {
            auto state = executor.state->terminalView->state;
            if (state == VTEState::ExitOk) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    lua_pop(L, lua_gettop(L));

    minilog::info("{} is completed or live.", id);

}

}
