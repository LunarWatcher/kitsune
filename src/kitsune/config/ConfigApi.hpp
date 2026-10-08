#pragma once

#include <lua.hpp>

namespace kitsune::ConfigApi {

extern int setTermColours(lua_State* L);
extern int setTermFont(lua_State* L);
extern int setPipelineLookups(lua_State* L);

extern int luaopen_kitsune_config(lua_State* L);

}
