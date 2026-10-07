#pragma once

#include <lua.hpp>

namespace kitsune::util {

inline void registerMetatable(
    lua_State* L,
    const luaL_Reg* table,
    const char* type
) {
    luaL_newmetatable(L, type);
    luaL_setfuncs(L, table, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

}
