#include "LuaApi.hpp"

#include "kitsune/api/TypeRegistry.hpp"
#include "kitsune/api/util/UDataUtils.hpp"

#include "kitsune/automation/AutomationApi.hpp"
#include "kitsune/config/ConfigApi.hpp"

#include "kitsune/log/Logger.hpp"
#include "util/LuaRegistry.hpp"

#include <stc/Environment.hpp>

#include <lua.hpp>

namespace kitsune {

LuaApi::LuaApi(
    TermList* termList,
    Config* conf
) : state(luaL_newstate()),
    termList(termList),
    conf(conf)
{
    luaL_openlibs(state);

    initGlobals();
    initApis();
}

LuaApi::~LuaApi() {
    lua_close(state);
}

bool LuaApi::run(const std::filesystem::path& path) {
    int res = luaL_dofile(state, path.c_str());
    if (res != 0) {
        logger::error(
            "Failed to load lua file: %s",
            lua_tostring(state, -1)
        );
    }
    return res == 0;
}

void LuaApi::registerApi(const char* libname, lua_CFunction func) {
    luaL_requiref(state, libname, func, 1);
    lua_pop(state, 1);
}

void LuaApi::initGlobals() {
    LuaRegistry::init(state);
    LuaRegistry::WithRegistry reg(state);
    luaL_Reg globalMetatable[] = {
        {nullptr, nullptr },
    };

    util::registerMetatable(state, globalMetatable, UDataLuaApi);

    lua_pushstring(state, LuaRegistry::ApiKey.c_str());
    auto** apiInterface = (LuaApi**) lua_newuserdata(state, sizeof(LuaApi**));
    luaL_setmetatable(state, UDataLuaApi);
    *apiInterface = this;

    lua_settable(state, -3);
}

void LuaApi::initApis() {
    registerApi(
        "kitsune.pipelines",
        AutomationApi::luaopen_kitsune_pipelines
    );
    registerApi(
        "kitsune.config",
        ConfigApi::luaopen_kitsune_config
    );
}

void LuaApi::loadConfig() {
    std::vector<std::string> files = {"init.lua", "local.lua"};
    for (const auto& file : files) {
        std::filesystem::path f = stc::expandUserPath("~/.config/kitsune/" + file);
        if (!std::filesystem::exists(f)) {
            logger::debug("%s does not exist", f.c_str());
        } else {
            logger::info("Loading %s", f.c_str());
            if (!run(f)) {
                logger::error("Failed to load config file!");
                throw std::runtime_error("Failed to load config file!");
            }
        }
    }
}

}
