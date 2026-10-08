#include "ConfigApi.hpp"
#include "gdkmm/rgba.h"
#include "kitsune/api/util/LangExt.hpp"
#include "kitsune/api/util/LuaRegistry.hpp"
#include "kitsune/api/LuaApi.hpp"
#include "kitsune/gtk/ColourUtil.hpp"
#include <iostream>
#include <lua.hpp>


namespace kitsune {


int ConfigApi::setTermColours(lua_State* L) {
    int32_t foreground = luaL_checkinteger(L, 1);
    int32_t background = luaL_checkinteger(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);
    luaL_argcheck(L, util::length(L, 3) == 16, 1, "Arg must be of length 16");

    std::vector<GdkRGBA> paletteData;
    util::iterateTable(
        L,
        3,
        [&]() {
            paletteData.push_back(util::colour(luaL_checkinteger(L, -1)));
        });

    if (paletteData.size() != 16) {
        return luaL_argerror(L, 1, "Illegal colour values passed to setTermColours!");
    }
    auto* api = LuaRegistry::WithRegistry(L).getApi();
    api->getConfig()->scheme.replacePalette(
        util::colour(foreground),
        util::colour(background),
        std::move(paletteData)
    );

    return 0;
}

int ConfigApi::setTermFont(lua_State* L) {
    auto font = util::getStringArg<std::string>(L, 1);
    auto* api = LuaRegistry::WithRegistry(L).getApi();
    api->getConfig()->scheme.setFont(
        std::move(font)
    );
    return 0;
}

int ConfigApi::setPipelineLookups(lua_State* L) {
    // TODO: implement (lookup locations are not implemented at all yet)
    return 0;
}

int ConfigApi::luaopen_kitsune_config(lua_State* L) {
    static const luaL_Reg functions[] {
        { "setTermColours", setTermColours },
        { "setTermFont", setTermFont },
        { "setPipelineLookups", setPipelineLookups },
        { nullptr, nullptr }
    };
    luaL_newlib(L, functions);
    return 1;
}

}
