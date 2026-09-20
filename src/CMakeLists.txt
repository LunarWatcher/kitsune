add_library(kitsunesrc STATIC 
    kitsune/gtk/VTEWidget.cpp
)
add_executable(kitsune kitsune/Main.cpp)

target_include_directories(
    kitsunesrc
PUBLIC
    ./
    ${GTKMM_INCLUDE_DIRS}
    ${VTE_INCLUDE_DIRS}
)
target_link_libraries(
    kitsunesrc
PUBLIC
    ${GTKMM_LIBRARIES}
    ${VTE_LIBRARIES}
)

target_link_libraries(kitsune kitsunesrc)

# vim:ft=cmake
