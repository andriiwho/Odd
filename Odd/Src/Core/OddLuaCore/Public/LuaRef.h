#pragma once

#include "OddCore.h"
#include "LuaFwd.h"
#include "RootObj.h"

namespace Odd
{
    class LuaRef : public RootObj
    {
    public:
        // Whether to take ownership of a registry reference.
        enum class Adopt
        {
            Yes
        };

        LuaRef() = default;
        LuaRef(lua_State* inL, int idx);
        LuaRef(lua_State* inL, Adopt, int existingRef);
        LuaRef(lua_State* inL, const char* globalName);
        ~LuaRef() override { Reset(); }

        void Reset();
        int  GetRef() const;
        bool IsValid() const;
        void PushToStack() const;

        bool IsTable() const;
        bool IsFunction() const;
        bool IsString() const;
        bool IsNumber() const;

    protected:
        lua_State* L{ nullptr };
        int        Ref = LUA_NOREF;
    };
} // namespace Odd