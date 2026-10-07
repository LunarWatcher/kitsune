#pragma once

#include "kitsune/automation/graph/TaskGraph.hpp"
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
    std::unordered_map<std::string, std::shared_ptr<TaskGraph>> pipelines;

    void initGlobals();
    void initApis();

    void registerApi(const char* libname, lua_CFunction func);
public:
    LuaApi();
    ~LuaApi();

    void run(const std::filesystem::path& path);

    TermList* getTermList() { return termList; }
    // TODO: this has too much business logic attached to it, it should be separated
    std::unordered_map<std::string, std::shared_ptr<TaskGraph>>& getPipelines() { return pipelines; }

};

}
