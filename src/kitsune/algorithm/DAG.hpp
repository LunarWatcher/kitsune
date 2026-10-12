#pragma once

#include "kitsune/automation/graph/TaskGraph.hpp"
#include <queue>
#include <unordered_map>
#include <vector>


/**
 * This implementation is based on the DFS implementation for topological sort, as described by wikipedia:
 * https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
 *
 * Some differences:
 * * There are no cycle checks here, since we handle that during graph construction. A cyclic graph cannot be
 *   constructed, so those checks can be ignored.
 * * Push order weirdness (see comment in `kitsune::dag::visit`)
 */
namespace kitsune::dag {

template <typename T>
struct DAGProxy {
    T item;
    bool mark = false;
};

inline void visit(
    std::vector<Task*>& out,
    DAGProxy<Task*>& item,
    std::unordered_map<std::string, DAGProxy<Task*>>& subgraph
) {
    if (item.mark) {
        return;
    }

    item.mark = true;

    for (auto& nodeRef : item.item->depends) {
        auto& nextItem = subgraph.at(nodeRef);
        visit(out, nextItem, subgraph);
    }

    // Somewhat interestingly, the DFS implementation this is based on calls for _prepending_ rather than appending, but
    // I assume this is caused by this technically being done the wrong way; we're handling all the dependencies rather
    // than the dependents. The current task graph in dev.lua is not complicated enough to test this properly.
    // TODO: unit test the shit out of this class - algorithms fucken hard
    out.push_back(item.item);
}

inline std::vector<Task*> dfs(
    const std::vector<std::string>& start,
    const std::unordered_map<std::string, std::shared_ptr<Task>>& data
) {
    // TODO: this function should probably return an `std::vector<std::vector<Task*>>` to signal task groups as well as
    // just the raw sort.
    // Unfortunately, I suck at algorithms, so I'm leaving this as a problem for future me. All it means is that
    // parallel process spawning will have to wait. this is not a big deal
    std::vector<Task*> out;
    std::unordered_map<std::string, DAGProxy<Task*>> subgraph;
    std::queue<std::string> buildQueue;
    std::unordered_set<std::string> exhausted;
    buildQueue.push_range(start);

    // Build subgraph; we want to exclude nodes that aren't being executed so they aren't in the final execution order.
    while (buildQueue.size()) {
        auto next = buildQueue.front();
        buildQueue.pop();

        if (exhausted.contains(next)) {
            continue;
        }
        exhausted.insert(next);

        auto task = data.at(next).get();
        buildQueue.push_range(task->depends);
        subgraph[task->id] = DAGProxy<Task*>{
            .item = task,
        };
    }

    // The entire graph is a single pass, we don't need to iterate the full set multiple times. We could search for the
    // entry nodes in particular, but I don't think this matters.
    // I'm not entirely sure if I understand _why_ this doesn't need to be sorted
    for (auto& it : subgraph) {
        visit(out, it.second, subgraph);
    }

    return out;
}

}
