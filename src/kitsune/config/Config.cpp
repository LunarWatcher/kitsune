#include "Config.hpp"
#include "minilog/minilog.hpp"
#include <filesystem>

#include <stc/unix/Process.hpp>

namespace kitsune {

std::vector<std::string> Config::getDefaultPipelineLookups() {
    std::filesystem::path currPath = std::filesystem::current_path();
    stc::Unix::Process p({
            "/usr/bin/env", "git", "rev-parse", "--show-toplevel"
        }, stc::Unix::Pipes::separate(false)
    );

    std::vector<std::string> out {
        currPath / ".kitsune/pipelines"
    };

    auto result = p.block();
    if (result != 0) {
        minilog::debug("Not in a git repo, can't use git repo root lookup");
    } else {
        auto result = p.getStdoutBuffer();
        result.erase(
            std::remove(result.begin(), result.end(), '\n'),
            result.cend()
        );

        std::filesystem::path gitRoot = result;

        if (gitRoot != currPath) {
            out.push_back(
                gitRoot / ".kitsune/pipelines"
            );
        }
    }

    return out;
}

}
