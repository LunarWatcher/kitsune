#pragma once

#include "kitsune/automation/PipelineController.hpp"
#include "kitsune/components/TermList.hpp"
#include <filesystem>
#include <lua.hpp>

namespace kitsune {

/**
 * Bidirectional connector between lua and C++. Stored in the lua registry for easy access by C++ functions. Anything
 * single-instanced that should be accessible to lua callbacks should be present in this class. For multi-instanced
 * objects, create standard UData instead.
 */
class LuaApi {
private:
    lua_State* state;

    // TODO: need to handle concurrent access probably?
    TermList* termList;
    Config* conf;
    PipelineController* pipelineController;

    void initGlobals();
    void initApis();

    void registerApi(const char* libname, lua_CFunction func);
public:
    LuaApi(
        TermList* termList,
        Config* conf,
        PipelineController* pipelineController
    );
    ~LuaApi();

    bool run(const std::filesystem::path& path);

    /**
     * Run a lua script stored in a string.
     * This should not be used outside tests.
     */
    std::expected<bool, std::string> runString(const std::string& script);

    void loadConfig(
        const std::filesystem::path& configRoot
    );

    TermList* getTermList() { return termList; }
    Config* getConfig() { return conf; }
    PipelineController* getPipelineController() { return pipelineController; }

    lua_State* getState() { return state; }
};

}
