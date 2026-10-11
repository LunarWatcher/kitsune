#include "LangExt.hpp"
#include "minilog/minilog.hpp"
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

void util::dumpStack(lua_State* L) {
    int top = lua_gettop(L);
    minilog::info("Stack size is %i", top);
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L,i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("%g\n",lua_tonumber(L,i));
            break;
        case LUA_TSTRING:
            printf("%s\n",lua_tostring(L,i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n",lua_topointer(L,i));
            break;
        }
    }
}

}
