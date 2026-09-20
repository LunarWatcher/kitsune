#add_library(kitsunesrc STATIC 
    # Non-executable files go here
#)
add_executable(kitsune kitsune/Main.cpp)

target_include_directories(
    kitsune
PUBLIC
    ./
    ${GTKMM_INCLUDE_DIRS}
    ${VTE_INCLUDE_DIRS}
)
target_link_libraries(
    kitsune
PUBLIC
    ${GTKMM_LIBRARIES}
    ${VTE_LIBRARIES}
)

# vim:ft=cmake
