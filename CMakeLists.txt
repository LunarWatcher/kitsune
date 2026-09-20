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

add_subdirectory(src)
#add_subdirectory(tests)

#add_custom_target(test
    #COMMAND tests
    #DEPENDS tests
    #COMMENT "Test kitsune")
# vim:ft=cmake
