#pragma once

#include "LuaFwd.h"
#include "OddCore.h"

namespace Odd
{
    class LuaStackGuard
    {
    public:
        LuaStackGuard(lua_State* L);
        ~LuaStackGuard();

        LuaStackGuard(const LuaStackGuard&) = delete;
        LuaStackGuard& operator=(const LuaStackGuard&) = delete;
        LuaStackGuard(LuaStackGuard&&) = delete;
        LuaStackGuard& operator=(LuaStackGuard&&) = delete;

    private:
        lua_State* L;
        int        Top;
    };
} // namespace Odd