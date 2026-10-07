#pragma once

#include "lua.hpp"

namespace kitsune::util {

template <typename StringType>
StringType getStringArg(lua_State* L, int arg) {
    size_t len;
    auto str = luaL_checklstring(L, arg, &len);

    return StringType {
        str, len
    };
}

}
