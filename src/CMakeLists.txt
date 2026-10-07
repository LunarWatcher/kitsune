add_library(kitsunesrc STATIC
    kitsune/api/LuaApi.cpp
    kitsune/api/util/LuaRegistry.cpp

    kitsune/app/KitsuneWindow.cpp

    kitsune/app/modes/ModalInputProcessor.cpp
    kitsune/app/modes/ModeController.cpp

    kitsune/automation/AutomationApi.cpp
    kitsune/automation/graph/TaskGraph.cpp

    kitsune/components/TermList.cpp
    kitsune/components/Modeline.cpp

    kitsune/gtk/VTEWidget.cpp
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
