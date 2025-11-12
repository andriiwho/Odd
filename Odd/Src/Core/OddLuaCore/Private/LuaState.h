#pragma once

#include "OddLua.h"

namespace Odd
{
    class LuaState
    {
    public:
        LuaState();
        ~LuaState();
        lua_State* GetState();

    private:
        lua_State* m_LuaState{};
    };
} // namespace Odd