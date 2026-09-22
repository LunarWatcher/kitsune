add_library(kitsunesrc STATIC
    kitsune/gtk/VTEWidget.cpp

    kitsune/app/KitsuneWindow.cpp

    kitsune/components/TermList.cpp
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

    stc
    CLI12::CLI12
)

target_link_libraries(kitsune kitsunesrc)

# vim:ft=cmake
