#include <catch2/catch_test_macros.hpp>

#include "fixtures/LuaFixture.hpp"

TEST_CASE_METHOD(test::LuaFixture, "Test colourscheme and font loading") {
    SECTION("Should allow loading a new colourscheme") {
        withInitFile(
            R"(local config = require("kitsune.config")
config.setTermColours(0x0000ff, 0xffff00, {
    0x000001, 0x000002, 0x000003, 0x000004,
    0x000005, 0x000006, 0x000007, 0x000008,
    0x000009, 0x00000a, 0x00000b, 0x00000c,
    0x00000d, 0x00000e, 0x00000f, 0x0000010,
}))"
        )
            .loadConfig();

        REQUIRE(
            kitsune::util::reverse(conf.scheme.foreground()) == 0x0000ff
        );
        REQUIRE(
            kitsune::util::reverse(conf.scheme.background()) == 0xffff00
        );

        for (size_t i = 1; i <= 16; ++i) {
            REQUIRE(
                kitsune::util::reverse(
                    conf.scheme.palette().at(i - 1)
                ) == i
            );
        }
    }
    SECTION("Should reject invalid palettes") {
        auto beforePalette = conf.scheme.palette();
        auto beforeFg = conf.scheme.foreground();
        auto beforeBg = conf.scheme.background();

        std::vector<std::string> cases = {
            "{0x000000}",
            R"({
0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000,
0x000000
})"
        };

        for (const auto& testCase : cases) {
            auto result = api.runString(
                std::format(
                    "local config = require(\"kitsune.config\");\n"
                    "config.setTermColours(0x000000, 0x000000, {})",
                    testCase
                )
            );
            REQUIRE_FALSE(bool(result));
            REQUIRE(result.error().contains("bad argument #3 to 'setTermColours' (Arg must be of length 16)"));

            REQUIRE(
                conf.scheme.foreground() == beforeFg
            );
            REQUIRE(
                conf.scheme.background() == beforeBg
            );
            REQUIRE(
                conf.scheme.palette() == beforePalette
            );
        }
    }
    SECTION("Should allow loading a new font") {
        REQUIRE_FALSE(conf.scheme.font().has_value());
        withInitFile(
            R"(local config = require("kitsune.config")
config.setTermFont("hewwo world"))"
        )
            .loadConfig();

        REQUIRE(conf.scheme.font().has_value());
        REQUIRE(*conf.scheme.font() == "hewwo world");
    }
}
