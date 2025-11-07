#include "LuaState.h"
#include "OddMalloc.h"

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
        : m_LuaState{ sol::default_at_panic, LuaAllocate }
    {
        m_LuaState.open_libraries();
    }

    LuaState::~LuaState()
    {
    }

} // namespace Odd