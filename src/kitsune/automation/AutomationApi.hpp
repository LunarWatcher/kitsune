#pragma once

#include <lua.hpp>

namespace kitsune::AutomationApi {

extern int createTask(lua_State* L);

extern int newPipeline(lua_State* L);

extern int luaopen_kitsune_pipelines(lua_State* L);

}
