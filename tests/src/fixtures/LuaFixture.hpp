#pragma once

#include <stc/test/TestDirectory.hpp>

#include "kitsune/api/LuaApi.hpp"
#include "kitsune/config/Config.hpp"
#include "kitsune/gtk/Operators.hpp" // In use by tests, do not remove

namespace test {

struct LuaFixture {
    kitsune::Config conf;
    stc::testutil::TestDirectory dir;
    kitsune::LuaApi api;

    LuaFixture();

    LuaFixture& withInitFile(
        const std::string& content,
        bool useLocalFile = false
    );
    LuaFixture& loadConfig();
};

}
