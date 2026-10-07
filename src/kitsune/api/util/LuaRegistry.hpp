#pragma once

#include <string>
#include <lua.hpp>

namespace kitsune { class LuaApi; }

namespace kitsune::LuaRegistry {

static inline std::string RegistryKey = "LunarWatcher/kitsune";
static inline std::string ApiKey = "__kitsune_internal_api__";

void init(lua_State* L);

// TODO: not sure if this pattern makes any sense. It _does_, but it might make more sense to do this for _all_ values
// that aren't subsequently nuked. As in a broad RAII wrapper of some kind that applies to all kinds of fields
struct WithRegistry {
private:
    bool released = false;
    lua_State* state;
public:
    [[nodiscard(
        "This class is a lock for the registry on the stack. Releasing this object will immediately free the table from the stack"
    )]]
    WithRegistry(lua_State* L);
    ~WithRegistry();

    LuaApi* getApi();
    void initApi(LuaApi* api);

    void release();
};

}
