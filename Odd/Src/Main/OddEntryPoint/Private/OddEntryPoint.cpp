#include "OddEntryPoint.h"
#include "LuaRuntime.h"
#include "RootObj.h"

namespace Odd
{
    // ============================================================================
    // Statics
    static LuaRuntime* GLuaRuntime = nullptr;

    // ============================================================================
    // Core iniitalization functions
    void InitializeCore();
    void ShutdownCore();

    EntryPoint::EntryPoint(std::span<const char* const> commandLineArgs)
    {
        InitializeCore();
    }

    EntryPoint::~EntryPoint()
    {
        ShutdownCore();
    }

    void EntryPoint::Main()
    {
    }
} // namespace Odd

namespace Odd
{
    // ============================================================================
    void InitializeCore()
    {
        auto memConfig = MemoryPoolConfig::Default();
        InitializeMemoryPool(memConfig);
        Internal::InitializeRootObjectSystem();

        InitializeLogging();

        GLuaRuntime = MakeObject<LuaRuntime>();
        oddValidateMsg(GLuaRuntime != nullptr, "Failed to create LuaRuntime");
        ODD_LOG_INFO("LuaRuntime initialized.");
    }

    // ============================================================================
    void ShutdownCore()
    {
        if (GLuaRuntime)
        {
            GLuaRuntime->Release();
            GLuaRuntime = nullptr;
            ODD_LOG_INFO("LuaRuntime shut down.");
        }

        ShutdownLogging();

#ifndef NDEBUG
        // Before shutting down the memory pool, we can print stats for debugging
        PrintGlobalMemoryStats();
#endif
        Internal::ShutdownRootObjectSystem();
        ShutdownMemoryPool();
    }
} // namespace Odd