#pragma once

#include "glibmm/refptr.h"
#include "kitsune/model/TerminalModel.hpp"
#include <lua.hpp>
#include <optional>
#include <string>
#include <vector>

namespace kitsune {

struct Executor {
    Glib::RefPtr<TerminalModel> state;

    virtual void getUData(lua_State* L);

    virtual void spawnShell(
        const std::string& command,
        const std::optional<std::string>& shellIdentifier = std::nullopt
    );
    virtual void spawnCommand(
        const std::vector<std::string>& command
    );

};

}
