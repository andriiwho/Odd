#pragma once

#include <sol/sol.hpp>

namespace Odd
{
    class LuaState
    {
    public:
        LuaState();
        ~LuaState();
        sol::state& GetState() { return m_LuaState; }

    private:
        sol::state m_LuaState;
    };
} // namespace Odd