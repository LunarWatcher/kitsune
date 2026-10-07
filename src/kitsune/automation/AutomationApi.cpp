#include "AutomationApi.hpp"
#include "kitsune/api/util/LangExt.hpp"
#include "kitsune/api/util/LuaRegistry.hpp"
#include "kitsune/log/Logger.hpp"
#include "lua.hpp"
#include <string>

namespace kitsune {

extern int AutomationApi::createTask(lua_State* L) {
    auto id = util::getStringArg<std::string>(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    auto* api = LuaRegistry::WithRegistry(L).getApi();
    logger::info("API == nullptr: %i", api == nullptr);

    logger::info("Good girl :3 Func called with id = %s", id.c_str());
    return 0;
}

int AutomationApi::luaopen_kitsune_pipelines(lua_State* L) {
    static const luaL_Reg functions[] {
        { "createTask", createTask },
        { nullptr, nullptr },
    };

    luaL_newlib(L, functions);
    return 1;
}

}
