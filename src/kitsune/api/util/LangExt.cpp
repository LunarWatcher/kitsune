#include "LangExt.hpp"
#include <functional>

namespace kitsune {



void util::iterateTable(
    lua_State* L,
    LUA_INTEGER tableIndex,
    const std::function<void()>& next
) {
    lua_pushnil(L);
    while (lua_next(L, tableIndex) != 0) {
        next();
        lua_pop(L, 1);
    }
}

LUA_INTEGER util::length(lua_State* L, LUA_INTEGER stackIdx) {
    lua_len(L, stackIdx);
    auto val = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return val;
}

}
