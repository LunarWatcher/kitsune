#include "TaskGraph.hpp"
#include "kitsune/automation/TaskRunner.hpp"

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

    if (task->id == "") {
        return std::unexpected("Invalid ID!");
    } else if (tasks.contains(task->id)) {
        return std::unexpected(std::format("ID {} is already in use in this task graph", task->id));
    }

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

    // TODO: this should be delayed until the pipeline is loaded
    task->executor.state = api->getTermList()->addBlankTerminal(
        *api->getConfig(),
        task,
        this
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
        std::move(executionOrder),
        taskList
    );
}

}
