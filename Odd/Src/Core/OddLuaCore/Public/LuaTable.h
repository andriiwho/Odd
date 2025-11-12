#pragma once

#include "LuaFwd.h"
#include "LuaRef.h"

namespace Odd
{
    class LuaTable final : public LuaRef
    {
    public:
        LuaTable(lua_State* L, int idx);
        LuaTable(lua_State* L, const char* globalName);
        LuaTable(lua_State* L, Adopt a, int existingRef);

    private:
    };
} // namespace Odd