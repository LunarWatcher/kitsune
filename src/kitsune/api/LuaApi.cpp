#include "LuaApi.hpp"

#include "kitsune/api/TypeRegistry.hpp"
#include "kitsune/api/util/LangExt.hpp"
#include "kitsune/api/util/UDataUtils.hpp"

#include "kitsune/automation/AutomationApi.hpp"
#include "kitsune/config/ConfigApi.hpp"

#include "minilog/minilog.hpp"
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
        minilog::error(
            "Failed to load lua file: {}",
            lua_tostring(state, -1)
        );
    }
    return res == 0;
}

std::expected<bool, std::string> LuaApi::runString(const std::string& script) {
    int res = luaL_dostring(state, script.c_str());
    if (res != 0) {
        std::string error = util::getStringArg<std::string>(state, -1);
        minilog::error(
            "Failed to run script: {}",
            error
        );
        return std::unexpected(error);
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

void LuaApi::loadConfig(
    const std::filesystem::path& configRoot
) {
    std::vector<std::string> files = {"init.lua", "local.lua"};
    for (const auto& file : files) {
        std::filesystem::path f = configRoot / file;
        if (!std::filesystem::exists(f)) {
            minilog::debug("{} does not exist", f.c_str());
        } else {
            minilog::info("Loading {}", f.c_str());
            if (!run(f)) {
                minilog::error("Failed to load config file!");
                throw std::runtime_error("Failed to load config file!");
            }
        }
    }
}

}
