#include "LuaRegistry.hpp"

#include "kitsune/api/LuaApi.hpp"
#include "kitsune/api/TypeRegistry.hpp"

namespace kitsune {

void LuaRegistry::init(lua_State* L) {
    lua_pushstring(L, RegistryKey.c_str());
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

LuaRegistry::WithRegistry::WithRegistry(lua_State* L) : state(L) {
    lua_pushstring(L, RegistryKey.c_str());
    lua_gettable(L, LUA_REGISTRYINDEX);
}

LuaRegistry::WithRegistry::~WithRegistry() {
    release();
}

void LuaRegistry::WithRegistry::release() {
    if (released) {
        return;
    }
    released = true;

    lua_pop(state, -1);
}

// TODO: is this really the best way to go about this? It's much more state-driven, which I like, but is that really
// worth it?
LuaApi* LuaRegistry::WithRegistry::getApi() {
    lua_pushstring(state, ApiKey.c_str());
    lua_gettable(state, -2);

    auto** api = (LuaApi**) luaL_checkudata(state, -1, UDataLuaApi);

    // Pop result (udata)
    lua_pop(state, 1);

    return *api;
}

}
