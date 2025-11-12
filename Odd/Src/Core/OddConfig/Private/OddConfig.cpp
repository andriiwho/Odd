#include "OddConfig.h"
#include "LuaRuntime.h"

#include <unordered_map>
#include <variant>

namespace Odd
{
    using ConfigScalarType = std::variant<
        bool,
        int64_t,
        float64_t,
        String>;

    using ConfigArrayType = std::variant<
        Vector<bool>,
        Vector<int64_t>,
        Vector<float64_t>,
        Vector<String>>;

    using ConfigValueType = std::variant<
        ConfigScalarType,
        ConfigArrayType>;

    struct ConfigState
    {
        HashMap<String, ConfigValueType> Values{};
    };
    static ConfigState* GState = nullptr;


    static int Lua_SetConfigTable(lua_State* state);

    void ConfigInit()
    {
        GState = OddNew<ConfigState>();
        LuaRuntime::Get().RegisterRawFunction("Config", Lua_SetConfigTable);
    }

    void ConfigShutdown()
    {
        if (GState)
        {
            OddDelete(GState);
        }
    }
} // namespace Odd

namespace Odd
{
    int Lua_SetConfigTable(lua_State* state)
    {
        luaL_checktype(state, 1, LUA_TTABLE);
    }
}