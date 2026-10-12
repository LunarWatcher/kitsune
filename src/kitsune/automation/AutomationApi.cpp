#include "AutomationApi.hpp"
#include "kitsune/api/TypeRegistry.hpp"
#include "kitsune/api/util/LangExt.hpp"
#include "kitsune/api/util/LuaRegistry.hpp"
#include "kitsune/api/LuaApi.hpp"
#include "kitsune/api/util/UDataUtils.hpp"
#include "kitsune/automation/graph/ExecutionEnums.hpp"
#include "kitsune/automation/graph/TaskGraph.hpp"
#include "lua.hpp"
#include <string>

namespace kitsune {

int AutomationApi::createTask(lua_State* L) {
    auto** taskGraph = (TaskGraph**) luaL_checkudata(L, 1, UDataPipeline);
    auto id = util::getStringArg<std::string>(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    // Move the table to the registry so the funcrefs stay valid
    int refKey = luaL_ref(L, LUA_REGISTRYINDEX);

    // Put the table back on the stack
    lua_rawgeti(L, LUA_REGISTRYINDEX, refKey);
    luaL_checktype(L, 3, LUA_TTABLE);

    auto* api = LuaRegistry::WithRegistry(L).getApi();

    std::shared_ptr<Task> task(
        new Task {
            .luaRefKey = refKey,
            .id = std::move(id),
        }
    );

    util::iterateTable(L, 3, [&]() {
        std::string k = util::getStringArg<std::string>(L, -2);

        if (k == "name") {
            std::string v = util::getStringArg<std::string>(L, -1);
            task->displayName = v;
        } else if (k == "depends") {
            luaL_argcheck(L, lua_type(L, -1) == LUA_TTABLE, 3, "depends must be an array");
            util::iterateTable(L, 5, [&]() {
                task->depends.insert(util::getStringArg<std::string>(L, -1));
            });
        } else if (k == "run") {
            auto v = luaL_checkinteger(L, -1);
            switch (v) {
            case static_cast<int>(Run::Auto):
                task->runSpec = Run::Auto;
                break;
            case static_cast<int>(Run::DependsOnly):
                task->runSpec = Run::DependsOnly;
                break;
            case static_cast<int>(Run::OnDemand):
                task->runSpec = Run::OnDemand;
                break;
            default:
                luaL_argerror(L, 3, "run must be one of the Run enum values.");
            }
        } else if (k == "rerun") {
            auto v = luaL_checkinteger(L, -1);
            switch (v) {
            case static_cast<int>(Rerun::Always):
                task->rerunSpec = Rerun::Always;
                break;
            case static_cast<int>(Rerun::IfCompleted):
                task->rerunSpec = Rerun::IfCompleted;
                break;
            case static_cast<int>(Rerun::Never):
                task->rerunSpec = Rerun::Never;
                break;

            default:
                luaL_argerror(L, 3, "run must be one of the Run enum values.");
            }
        } else if (
            k == "liveness"
            || k == "env"
            || k == "hooks"
            || k == "command"
        ) {
            // We cannot process these because there isn't a way to reference Lua functions in C code. They'll instead
            // be loaded from the registry
        } else if (k == "workingDirectory") {
            task->workingDirectory = util::getStringArg<std::string>(L, -1);
        } else if (k == "inputs") {
            luaL_argcheck(L, lua_type(L, -1) == LUA_TTABLE, 3, "inputs must be a table");
            util::iterateTable(L, 5, [&]() {
                auto k = util::getStringArg<std::string>(L, -2);
                task->inputs[std::move(k)] = InputDescriptor {
                    .description = util::getStringArg<std::string>(L, -1),
                };
            });
        } else if (k == "notify") {
            if (lua_type(L, -1) == LUA_TNIL) {
                task->notifySpec = Notify::Never;
                return;
            }
            auto v = luaL_checkinteger(L, -1);
            switch (v) {
            case static_cast<int>(Notify::OnError):
                task->notifySpec = Notify::OnError;
                break;
            case static_cast<int>(Notify::OnExit):
                task->notifySpec = Notify::OnExit;
                break;

            default:
                luaL_argerror(L, 3, "run must be one of the Run enum values.");
            }
        } else {
            luaL_error(L, "Found unrecognized argument passed to pipeline::task(): %s", k.c_str());
        }
    });

    if (task->displayName == "") {
        task->displayName = task->id;
    }

    auto result = (**taskGraph).push(
        task,
        api
    );
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
    api->getPipelineController()->getPipelines()[name] = pipeline;

    auto** pipelineUdata = (TaskGraph**) lua_newuserdata(L, sizeof(TaskGraph**));
    luaL_setmetatable(L, UDataPipeline);
    *pipelineUdata = pipeline.get();

    return 1;
}

int AutomationApi::executor::spawnShell(lua_State* L) {
    auto** executor = (Executor**) luaL_checkudata(L, 1, UDataExecutor);

    auto command = util::getStringArg(L, 2);
    // TODO: handle opt shell arg 

    (**executor).spawnShell(
        command
    );
    
    return 0;
}

int AutomationApi::executor::spawnCommand(lua_State* L) {
    auto** executor = (Executor**) luaL_checkudata(L, 1, UDataExecutor);

    std::vector<std::string> command;

    util::iterateTable(L, 2, [&]() {
        command.push_back(
            util::getStringArg(L, -1)
        );
    });

    (**executor).spawnCommand(
        command
    );
    return 0;
}

int AutomationApi::luaopen_kitsune_pipelines(lua_State* L) {
    static const luaL_Reg functions[] {
        { "new", newPipeline },
        { nullptr, nullptr }
    };

    luaL_newlib(L, functions);

    {
        lua_newtable(L);
        lua_pushinteger(L, static_cast<lua_Integer>(Run::Auto));
        lua_setfield(L, -2, "Auto");

        lua_pushinteger(L, static_cast<lua_Integer>(Run::OnDemand));
        lua_setfield(L, -2, "OnDemand");

        lua_pushinteger(L, static_cast<lua_Integer>(Run::DependsOnly));
        lua_setfield(L, -2, "DependsOnly");

        lua_setglobal(L, "Run");
    }
    {
        lua_newtable(L);
        lua_pushinteger(L, static_cast<lua_Integer>(Run::Auto));
        lua_setfield(L, -2, "Always");

        lua_pushinteger(L, static_cast<lua_Integer>(Run::OnDemand));
        lua_setfield(L, -2, "IfCompleted");

        lua_pushinteger(L, static_cast<lua_Integer>(Run::DependsOnly));
        lua_setfield(L, -2, "Never");

        lua_setglobal(L, "Rerun");
    }

    static const luaL_Reg pipelineFuncs[] {
        { "task", createTask },
        { nullptr, nullptr },
    };
    util::registerMetatable(L, pipelineFuncs, UDataPipeline);

    static const luaL_Reg executorFuncs[] {
        { "shell", executor::spawnShell },
        { "command", executor::spawnCommand },
        { nullptr, nullptr },
    };
    util::registerMetatable(L, executorFuncs, UDataExecutor);

    return 1;
}

}
