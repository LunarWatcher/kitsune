#include "TaskGraph.hpp"

#include <queue>
#include <format>

namespace kitsune {

std::expected<bool, std::string> TaskGraph::push(const std::shared_ptr<Task>& task) {
    // As far as I can tell, it should be sufficient to trace the deps one way from the added node. In fact, because we
    // don't allow adding constraints to tasks that haven't been defined yet, we only need to check the immediate deps.
    // * We not not have anything pointing to this node that doesn't exist yet, so we don't need to check the entire
    //   graph to make sure we don't get a cycle from existing connections.
    // * The only connections that appear from adding this node are the deps this node has, so we don't need to check
    //   the deps of the immediate deps, even
    //
    // Strictly speaking, that makes this a multi-rooted tree and not a proper graph, but it's the easiest modelling
    // option
    // (Pretty sure it's still mathematically considered a graph though)

    for (auto& dep : task->depends) {
        if (dep == task->id) {
            return std::unexpected(
                std::format("Task {} depends on itself!", task->id)
            );
        }

        if (!tasks.contains(dep)) {
            return std::unexpected(
                std::format("Task {} tried depending on {}, which has not been loaded yet", task->id, dep)
            );
        }

    }

    tasks[task->id] = task;

    return true;
}

}
