#include "LuaRuntime.h"
#include "LuaState.h"

namespace Odd
{
    static LuaState* GLuaState;

    LuaRuntime::LuaRuntime()
    {
        if (!GLuaState)
        {
            GLuaState = OddNew<LuaState>();
        }
    }

    LuaRuntime::~LuaRuntime()
    {
        if (GLuaState)
        {
            OddDelete(GLuaState);
            GLuaState = nullptr;
        }
    }

} // namespace Odd