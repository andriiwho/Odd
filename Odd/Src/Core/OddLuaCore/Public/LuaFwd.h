#pragma once

extern "C"
{
    using lua_State = struct lua_State;
}

#ifndef LUA_NOREF
    #define LUA_NOREF (-2);
#endif

#ifndef LUA_REFNIL
    #define LUA_REFNIL (-1);
#endif

#ifndef LUA_REGISTRYINDEX
    #define LUA_REGISTRYINDEX(-10000)
#endif

#ifndef LUA_ENVIRONINDEX
    #define LUA_ENVIRONINDEX (-10001)
#endif

#ifndef LUA_GLOBALSINDEX
    #define LUA_GLOBALSINDEX (-10002)
#endif