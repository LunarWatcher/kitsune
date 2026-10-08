#pragma once

#include "lua.hpp"

#include <functional>

namespace kitsune::util {

template <typename StringType>
StringType getStringArg(lua_State* L, int arg) {
    size_t len;
    auto str = luaL_checklstring(L, arg, &len);

    return StringType {
        str, len
    };
}

extern void iterateTable(
    lua_State* L,
    LUA_INTEGER tableIndex,
    const std::function<void()>& next
);

extern LUA_INTEGER length(lua_State* L, LUA_INTEGER stackIdx);

}
