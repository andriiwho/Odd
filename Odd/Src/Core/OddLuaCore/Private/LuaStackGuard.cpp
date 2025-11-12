#include "LuaStackGuard.h"
#include "OddLua.h"

namespace Odd
{

    LuaStackGuard::LuaStackGuard(lua_State* L)
        : L(L)
        , Top(lua_gettop(L))
    {
    }

    LuaStackGuard::~LuaStackGuard()
    {
        lua_settop(L, Top);
    }

} // namespace Odd