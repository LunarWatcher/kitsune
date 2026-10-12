#include "Executor.hpp"
#include "kitsune/api/TypeRegistry.hpp"

namespace kitsune {

void Executor::getUData(lua_State* L) {
    auto udata = (Executor**) lua_newuserdata(L, sizeof(Executor**));
    luaL_setmetatable(L, UDataExecutor);
    *udata = this;
}

void Executor::spawnShell(
    const std::string& command,
    const std::optional<std::string>& shellIdentifier
) {
    std::vector<std::string> args;
    args.reserve(4);
    auto shell = shellIdentifier.value_or(getenv("SHELL"));

    if (!shell.starts_with("/")) {
        args.push_back("/usr/bin/env");
    }
    args.push_back(shell);
    args.push_back("-c");
    args.push_back(command);

    spawnCommand(args);
}

void Executor::spawnCommand(
    const std::vector<std::string>& command
) {
    this->state->terminalView->spawn(command);
}

}
