#include "AutomationApi.hpp"
#include "kitsune/api/TypeRegistry.hpp"
#include "kitsune/api/util/LangExt.hpp"
#include "kitsune/api/util/LuaRegistry.hpp"
#include "kitsune/api/LuaApi.hpp"
#include "kitsune/api/util/UDataUtils.hpp"
#include "kitsune/automation/graph/TaskGraph.hpp"
#include "lua.hpp"
#include <lauxlib.h>
#include <lua.h>
#include <string>

namespace kitsune {

int AutomationApi::createTask(lua_State* L) {
    auto** taskGraph = (TaskGraph**) luaL_checkudata(L, 1, UDataPipeline);
    auto id = util::getStringArg<std::string>(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    auto* api = LuaRegistry::WithRegistry(L).getApi();
    
    std::shared_ptr<Task> task(
        new Task {
            .id = std::move(id)
        }
    );
    auto result = (**taskGraph).push(task);
    if (!result) {
        return luaL_error(
            L, "%s", result.error().c_str()
        );
    }

    return 0;
}

int AutomationApi::newPipeline(lua_State* L) {
    auto name = util::getStringArg<std::string>(L, 1);

    auto* api = LuaRegistry::WithRegistry(L).getApi();

    auto pipeline = std::make_shared<TaskGraph>(name);
    api->getPipelines()[name] = pipeline;

    auto** pipelineUdata = (TaskGraph**) lua_newuserdata(L, sizeof(TaskGraph**));
    luaL_setmetatable(L, UDataPipeline);
    *pipelineUdata = pipeline.get();

    return 1;
}

int AutomationApi::luaopen_kitsune_pipelines(lua_State* L) {
    static const luaL_Reg functions[] {
        { "new", newPipeline },
        { nullptr, nullptr }
    };

    luaL_newlib(L, functions);

    static const luaL_Reg pipelineFuncs[] {
        { "createTask", createTask },
        { nullptr, nullptr },
    };
    util::registerMetatable(L, pipelineFuncs, UDataPipeline);

    return 1;
}

}
