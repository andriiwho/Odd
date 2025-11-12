#pragma once

#include "OddCore.h"
#include "ObjPtr.h"
#include "RootObj.h"
#include "LuaRef.h"

extern "C"
{
    using lua_State = struct lua_State;
}

namespace Odd
{
	class LuaRuntime final : public RootObj
	{
    public:
		LuaRuntime();
        ~LuaRuntime();

		void RegisterRawFunction(StringView name, int (*function)(lua_State* L));
		void ExecuteFileSimple(StringView path);

		ObjPtr<LuaRef> GetRef(int stackIndex = -1);

		static LuaRuntime& Get();
	};
}