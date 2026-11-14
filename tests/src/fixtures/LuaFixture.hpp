#pragma once

#include <stc/test/TestDirectory.hpp>

#include "kitsune/api/LuaApi.hpp"
#include "kitsune/automation/PipelineController.hpp"
#include "kitsune/components/commands/CommandDispatcher.hpp"
#include "kitsune/config/Config.hpp"
#include <kitsune/gtk/Operators.hpp>

namespace test {

struct LuaFixture {
    kitsune::Config conf;
    std::shared_ptr<kitsune::CommandDispatcher> dispatcher;
    kitsune::PipelineController pipelineController;
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
