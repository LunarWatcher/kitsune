#include "CommandDispatcher.hpp"

namespace kitsune {

CommandDispatcher::CommandDispatcher() {}

bool CommandDispatcher::registerCommand(
    const std::string& command, const CommandHandler& cb
) {
    if (handlers.contains(command)) {
        return false;
    }
    handlers[command] = cb;
    return true;
}

std::optional<Error> CommandDispatcher::invoke(
    const std::string& command,
    const std::vector<std::string>& arglist
) {
    if (auto it = handlers.find(command); it != handlers.end()) {
        return it->second(arglist);
    }

    return Error {
        "Command not found: " + command
    };
}

}
