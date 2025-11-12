#include "LuaRuntime.h"
#include "LuaState.h"

namespace Odd
{
    static LuaState*   GLuaState;
    static LuaRuntime* GRuntime;

    LuaRuntime::LuaRuntime()
    {
        GRuntime = this;

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

    void LuaRuntime::RegisterRawFunction(StringView name, int (*function)(lua_State* L))
    {
        lua_register(GLuaState->GetState(), name.data(), function);
    }

    void LuaRuntime::ExecuteFileSimple(StringView path)
    {
        luaL_dofile(GLuaState->GetState(), path.data());
    }

    ObjPtr<LuaRef> LuaRuntime::GetRef(int stackIndex /*= -1*/)
    {
        return ObjPtr(MakeObject<LuaRef>(GLuaState->GetState(), stackIndex));
    }

    LuaRuntime& LuaRuntime::Get()
    {
        return *GRuntime;
    }

} // namespace Odd