#pragma once

#include "kitsune/theming/TermColour.hpp"
#include "stc/Environment.hpp"
#include <filesystem>

namespace kitsune {

/**
 * Internal-only config; used to allow tests to override certain parts of config that's normally hard-coded outside
 * tests (for example where to look for config).
 */
struct EnvConfig {
    std::filesystem::path configRoot = stc::expandUserPath(
        "~/.config/kitsune"
    );
};

struct Config {
    EnvConfig envConfig;

    TermColour scheme;
};

}
