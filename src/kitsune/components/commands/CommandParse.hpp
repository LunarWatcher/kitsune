#pragma once

#include <expected>
#include <string>
#include <vector>

namespace kitsune {

struct CommandParse {
    std::string command;
    std::vector<std::string> args;

    static std::expected<CommandParse, std::string> parse(const std::string& in);
};

}
