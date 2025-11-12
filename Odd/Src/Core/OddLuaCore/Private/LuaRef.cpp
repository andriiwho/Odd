#include "LuaRef.h"
#include "OddLua.h"
#include "LuaStackGuard.h"

namespace Odd
{

    LuaRef::LuaRef(lua_State* inL, int idx)
        : L(inL)
        , Ref(LUA_NOREF)
    {
        // Copy value at idx into registry and pop the copy
        lua_pushvalue(L, idx);
        Ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    LuaRef::LuaRef(lua_State* inL, Adopt, int existingRef)
        : L(inL)
        , Ref(existingRef)
    {
    }

    LuaRef::LuaRef(lua_State* inL, const char* globalName)
        : L(inL)
        , Ref(LUA_NOREF)
    {
        lua_getglobal(L, globalName);
        oddValidateMsg(!lua_isnil(L, -1), "Global '{}' does not exist!", globalName);
        Ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    void LuaRef::Reset()
    {
        if (IsValid())
        {
            luaL_unref(L, LUA_REGISTRYINDEX, Ref);
        }

        Ref = LUA_NOREF;
        L = nullptr;
    }

    int LuaRef::GetRef() const
    {
        return Ref;
    }

    bool LuaRef::IsValid() const
    {
        return L != nullptr && Ref != LUA_NOREF && Ref != LUA_REFNIL;
    }

    void LuaRef::PushToStack() const
    {
        lua_rawgeti(L, LUA_ENVIRONINDEX, Ref);
    }

    bool LuaRef::IsTable() const
    {
        LuaStackGuard guard(L);
        PushToStack();
        return lua_istable(L, -1);
    }

    bool LuaRef::IsFunction() const
    {
        LuaStackGuard guard(L);
        PushToStack();
        return lua_isfunction(L, -1);
    }

    bool LuaRef::IsString() const
    {
        LuaStackGuard guard(L);
        PushToStack();
        return lua_isstring(L, -1);
    }

    bool LuaRef::IsNumber() const
    {
        LuaStackGuard guard(L);
        PushToStack();
        return lua_isnumber(L, -1);
    }
} // namespace Odd