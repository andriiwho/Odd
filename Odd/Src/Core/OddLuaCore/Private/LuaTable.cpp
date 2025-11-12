#include "LuaTable.h"

namespace Odd
{

    LuaTable::LuaTable(lua_State* L, int idx)
        : LuaRef(L, idx)
    {
        oddValidate(IsTable());
    }

    LuaTable::LuaTable(lua_State* L, const char* globalName)
        : LuaRef(L, globalName)
    {
        oddValidate(IsTable());
    }

    LuaTable::LuaTable(lua_State* L, Adopt a, int existingRef)
        : LuaRef(L, a, existingRef)
    {
        oddValidate(IsTable());
    }

} // namespace Odd