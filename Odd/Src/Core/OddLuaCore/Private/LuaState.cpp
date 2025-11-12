#include "LuaState.h"
#include "OddMalloc.h"
#include "Logging.h"

namespace Odd
{
    static void* LuaAllocate(void* ud, void* ptr, size_t osize, size_t nsize)
    {
        // Use Odd's global memory pool for Lua allocations
        if (nsize == 0)
        {
            OddFree(ptr);
            return nullptr;
        }

        return OddRealloc(ptr, nsize);
    }

    LuaState::LuaState()
        : m_LuaState(luaL_newstate())
    {
        ODD_LOG_INFO("Initializing lua runtime");

        lua_setallocf(m_LuaState, LuaAllocate, nullptr);
        luaL_openlibs(m_LuaState);

        ODD_LOG_INFO("Lua runtime initialized.");
    }

    LuaState::~LuaState()
    {
        if (m_LuaState)
            lua_close(m_LuaState);
    }

    lua_State* LuaState::GetState()
    {
        return m_LuaState;
    }

} // namespace Odd