#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace kitsune {

struct Error {
    std::string errorMessage;

    const auto what() {
        return errorMessage;
    }
};

class CommandDispatcher {
public:
    using CommandHandler = std::function<std::optional<Error>(const std::vector<std::string>& args)>;
private:
    std::unordered_map<std::string, CommandHandler> handlers;

public:
    CommandDispatcher();

    bool registerCommand(const std::string& command, const CommandHandler& cb);

    std::optional<Error> invoke(
        const std::string& message,
        const std::vector<std::string>& arglist
    );
};

}
