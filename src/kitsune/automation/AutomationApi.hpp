#pragma once

#include <lua.hpp>

namespace kitsune::AutomationApi {

namespace executor {

extern int spawnShell(lua_State* L);
extern int spawnCommand(lua_State* L);

}

extern int createTask(lua_State* L);

extern int newPipeline(lua_State* L);

extern int luaopen_kitsune_pipelines(lua_State* L);

}
