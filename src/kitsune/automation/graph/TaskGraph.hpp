#pragma once

#include "kitsune/automation/graph/ExecutionEnums.hpp"
#include "Executor.hpp"
#include <lua.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <unordered_set>
#include <vector>

namespace kitsune {

class LuaApi;
class TaskRunner;

/**
 * Currently just a wrapper around an std::string, but I want to expand this into more involved input specs in the
 * future. Particularly, I want to be able to declare array input
 */
struct InputDescriptor {
    std::string description;
};

struct Task {
    const int luaRefKey;

    const std::string id;
    std::string displayName;

    std::unordered_set<std::string> depends;
    std::unordered_set<std::string> transitiveDependencies;

    std::map<std::string, InputDescriptor> inputs;

    std::optional<std::string> workingDirectory;

    Run runSpec = Run::OnDemand;
    // TODO: make Rerun a lua_CFunction instead? I want to have a CFunction along with it to make scripting possible,
    // but it would be much easier if it always was a CFunction. Should be fairly easy to do since Rerun is a global,
    // should just be replacing the ints with CFuncs, probably defined in C++ entirely?
    Rerun rerunSpec = Rerun::IfCompleted;
    Notify notifySpec = Notify::Never;

    Executor executor;

    void run(lua_State* L);

    const std::string& getDisplayName() const { return displayName; }
    const std::string& getId() const { return id; }
};

// TODO: Rename to pipeline
class TaskGraph {
private:
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;
    const std::string name;
public:
    TaskGraph(const std::string& name) : name(name) {}

    std::expected<bool, std::string> push(
        const std::shared_ptr<Task>& task,
        LuaApi* api
    );

    std::shared_ptr<TaskRunner> start(
        const std::vector<std::string>& taskList,
        LuaApi* api
    );

    const std::string& getName() const { return name; }
    const decltype(tasks)& getTasks() const { return tasks; }
};

}
