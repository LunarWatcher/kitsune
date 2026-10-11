#pragma once

#include "kitsune/automation/graph/ExecutionEnums.hpp"
#include "kitsune/model/TerminalModel.hpp"
#include <lua.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace kitsune {

/**
 * Currently just a wrapper around an std::string, but I want to expand this into more involved input specs in the
 * future. Particularly, I want to be able to declare array input
 */
struct InputDescriptor {
    std::string description;
};

struct Hooks {
    lua_CFunction onLive = nullptr;
    lua_CFunction onTerminated = nullptr;
};

struct Task {
    const int luaRefKey;
    
    const std::string id;
    std::string displayName;

    std::vector<std::string> depends;
    std::map<std::string, std::variant<lua_CFunction, std::string>> env;
    std::map<std::string, InputDescriptor> inputs;
    
    lua_CFunction command = nullptr;
    lua_CFunction livenessProbe = nullptr;

    std::optional<std::string> workingDirectory;

    Run runSpec = Run::OnDemand;
    // TODO: make Rerun a lua_CFunction instead? I want to have a CFunction along with it to make scripting possible,
    // but it would be much easier if it always was a CFunction. Should be fairly easy to do since Rerun is a global,
    // should just be replacing the ints with CFuncs, probably defined in C++ entirely?
    Rerun rerunSpec = Rerun::IfCompleted;
    Notify notifySpec = Notify::Never;

    Hooks hooks;

    Glib::RefPtr<TerminalModel> commandExecutionState;
};

// TODO: Rename to pipeline
class TaskGraph {
private:
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;
    std::string name;
public:
    TaskGraph(const std::string& name) : name(name) {}

    std::expected<bool, std::string> push(const std::shared_ptr<Task>& task);
};

}
