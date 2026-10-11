add_executable(tests
    src/Main.cpp
    src/fixtures/LuaFixture.cpp
    
    src/lua/ConfigTests.cpp
)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.16.1
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2 stc::testutil)

target_link_libraries(tests kitsunesrc)
target_include_directories(tests PUBLIC ./src/)

# vim:ft=cmake
