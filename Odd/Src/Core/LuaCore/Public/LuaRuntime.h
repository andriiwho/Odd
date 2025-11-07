#pragma once

#include "OddCore.h"
#include "RootObj.h"

namespace Odd
{
	class LuaRuntime final : public RootObj
	{
    public:
		LuaRuntime();
        ~LuaRuntime();
	};
}