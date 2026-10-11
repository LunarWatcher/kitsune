#include "LuaFixture.hpp"

#include "catch2/catch_test_macros.hpp"

#include <fstream>

namespace test {

LuaFixture::LuaFixture()
    : conf({
            .envConfig {
                .configRoot = "./test-data/config"
            }
        }),
      dir(conf.envConfig.configRoot, true),
      api(
          // TODO: termList = nullptr is going to cause problems for at least a few of the automation APIs, but I'm not
          // sure how I can best test this part. Maybe with a mock TermList? It is a pointer, so would be doable
          nullptr,
          &conf
      )
{}

LuaFixture& LuaFixture::withInitFile(
    const std::string& content,
    bool useLocalFile
) {
    std::string file = useLocalFile ? "local.lua" : "init.lua";
    std::ofstream f(conf.envConfig.configRoot / file);

    INFO("Writing " << (conf.envConfig.configRoot / file).string());
    REQUIRE(bool(f));
    f << content;

    return *this;
}

LuaFixture& LuaFixture::loadConfig() {
    api.loadConfig(
        conf.envConfig.configRoot
    );

    return *this;
}

}
