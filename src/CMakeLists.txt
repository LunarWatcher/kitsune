add_library(kitsunesrc STATIC
    kitsune/gtk/VTEWidget.cpp

    kitsune/app/KitsuneWindow.cpp

    kitsune/app/modes/ModalInputProcessor.cpp
    kitsune/app/modes/ModeController.cpp

    kitsune/components/TermList.cpp
    kitsune/components/Modeline.cpp
)
add_executable(kitsune kitsune/Main.cpp)

target_include_directories(
    kitsunesrc
PUBLIC
    ./
    ${GTKMM_INCLUDE_DIRS}
    ${VTE_INCLUDE_DIRS}

    ${LUA_INCLUDE_DIR}
)
target_link_libraries(
    kitsunesrc
PUBLIC
    ${GTKMM_LIBRARIES}
    ${VTE_LIBRARIES}

    stc
    CLI12::CLI12

    ${LUA_LIBRARIES}
)

target_link_libraries(kitsune kitsunesrc)

# vim:ft=cmake
