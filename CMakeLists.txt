cmake_minimum_required(VERSION 3.31)
project(kitsune)

set (CMAKE_CXX_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)
# TODO: why does this seem to require the 2.91 part?
pkg_check_modules(VTE vte-2.91-gtk4 REQUIRED)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

include(FetchContent)
FetchContent_Declare(stc
    GIT_REPOSITORY https://codeberg.org/LunarWatcher/stc
    GIT_TAG v2026-09-13
)
FetchContent_Declare(minilog
    GIT_REPOSITORY https://codeberg.org/LunarWatcher/minilog
    GIT_TAG adfca0bcd67d4416778621e5071f512e8bf27bc4
)
FetchContent_Declare(CLI12
    GIT_REPOSITORY https://codeberg.org/LunarWatcher/CLI12
    GIT_TAG v3.1.0
)

find_package(Lua 5.5 REQUIRED)
string(SUBSTRING ${LUA_VERSION_STRING} 0 3 LUA_BINARY_NAME)
message(STATUS "Using Lua ${LUA_BINARY_NAME}")

FetchContent_MakeAvailable(
    stc
    CLI12
    minilog
)

add_subdirectory(src)
add_subdirectory(tests EXCLUDE_FROM_ALL)

add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test kitsune")
# vim:ft=cmake
