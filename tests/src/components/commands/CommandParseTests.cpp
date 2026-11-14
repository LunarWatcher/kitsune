#include "kitsune/components/commands/CommandParse.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

void assertResult(
    const std::expected<kitsune::CommandParse, std::string> actual,
    const std::string& expectedCommandName,
    const std::vector<std::string>& expectedArgs
) {
    {
        INFO((actual.has_value() ? "[value]" : actual.error()));
        REQUIRE(actual.has_value());
    }

    REQUIRE(actual->command == expectedCommandName);
    auto& actualArgs = actual->args;
    REQUIRE(actualArgs.size() == expectedArgs.size());

    for (size_t i = 0; i < actualArgs.size(); ++i) {
        INFO("At idx " << i);
        REQUIRE(
            actualArgs.at(i)
            ==
            expectedArgs.at(i)
        );
    }
}

void assertError(
    const std::expected<kitsune::CommandParse, std::string> actual,
    const std::string& errorMessage
) {
    REQUIRE(!actual.has_value());
    REQUIRE(actual.error() == errorMessage);
}

TEST_CASE("Validate CommandParse::parse") {
    assertResult(
        kitsune::CommandParse::parse(
            "hello world"
        ),
        "hello",
        {
            "world"
        }
    );
    assertResult(
        kitsune::CommandParse::parse(
            "run dev build"
        ),
        "run",
        {
            "dev",
            "build"
        }
    );
    assertResult(
        kitsune::CommandParse::parse(
            "run \"dev build\""
        ),
        "run",
        {
            "dev build"
        }
    );
    assertResult(
        kitsune::CommandParse::parse(
            "run 'dev build'"
        ),
        "run",
        {
            "dev build"
        }
    );

    assertError(
        kitsune::CommandParse::parse(
            "run 'dev build"
        ),
        "Command has unterminated quotes"
    );
    assertError(
        kitsune::CommandParse::parse(
            "run \"dev build"
        ),
        "Command has unterminated quotes"
    );
}

}
