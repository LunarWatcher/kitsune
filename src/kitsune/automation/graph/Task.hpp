#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <atomic>

#include "kitsune/automation/graph/ExecutionEnums.hpp"
#include "Executor.hpp"

namespace kitsune {

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

    std::unordered_map<std::string, InputDescriptor> inputs;

    std::optional<std::string> workingDirectory;

    Run runSpec = Run::OnDemand;
    // TODO: make Rerun a lua_CFunction instead? I want to have a CFunction along with it to make scripting possible,
    // but it would be much easier if it always was a CFunction. Should be fairly easy to do since Rerun is a global,
    // should just be replacing the ints with CFuncs, probably defined in C++ entirely?
    Rerun rerunSpec = Rerun::IfCompleted;
    Notify notifySpec = Notify::Never;

    Executor executor;

    [[nodiscard("Run result should never be discarded, as doing so will cause subsequent errors if this doesn't return true")]]
    bool run(
        lua_State* L,
        const std::atomic<bool>& running
    );

    const std::string& getDisplayName() const { return displayName; }
    const std::string& getId() const { return id; }

    bool shouldRun();

    const std::shared_ptr<VTEWidget> getTerminal() {
        return executor.state->terminalView;
    }
};

}
