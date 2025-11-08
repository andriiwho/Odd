#include "OddEntryPoint.h"
#include "LuaRuntime.h"
#include "RootObj.h"
#include "OddPlatformCore.h"
#include "ObjPtr.h"

namespace Odd
{
    // ============================================================================
    // Statics
    static LuaRuntime* GLuaRuntime = nullptr;

    // ============================================================================
    // Core iniitalization functions
    void InitializeCore();
    void ShutdownCore();

    // ============================================================================
    // Runtime initialization functions
    void InitializeRuntime();
    void ShutdownRuntime();

    EntryPoint::EntryPoint(std::span<const char* const> commandLineArgs)
    {
        InitializeCore();
        InitializeRuntime();
    }

    EntryPoint::~EntryPoint()
    {
        ShutdownRuntime();
        ShutdownCore();
    }

    void EntryPoint::Main()
    {
        IWindow* mainWindow = GPlatform->CreatePlatformWindow(
            "A Window",
            WindowSize{
                .Width = 800,
                .Height = 600,
            },
            WindowFlags{
                .Resizable = true,
                .Fullscreen = false,
                .Borderless = false,
            });
        oddValidateMsg(mainWindow != nullptr, "Failed to create main window.");
        mainWindow->Show();

        while (true)
        {
            GPlatform->PollEventsSimple();
            if (mainWindow->Expired())
                break;

            Internal::FlushExpiredRootObjects();
        }
    }
} // namespace Odd

namespace Odd
{
    // ============================================================================
    void InitializeCore()
    {
        // Init memory framework
        auto memConfig = MemoryPoolConfig::Default();
        InitializeMemoryPool(memConfig);
        Internal::InitializeRootObjectSystem();

        // Other systems that should go before lua
        InitializeLogging();

        // Init lua runtime.
        GLuaRuntime = MakeObject<LuaRuntime>();
        oddValidateMsg(GLuaRuntime != nullptr, "Failed to create LuaRuntime");
        ODD_LOG_INFO("LuaRuntime initialized.");
    }

    // ============================================================================
    void ShutdownCore()
    {
        // Shutdown lua
        if (GLuaRuntime)
        {
            GLuaRuntime->Release();
            GLuaRuntime = nullptr;
            ODD_LOG_INFO("LuaRuntime shut down.");
        }

        // Shutdown other systems
        ShutdownLogging();

        // Now we can destroy all expired root objects.
        Internal::ShutdownRootObjectSystem();
#ifndef NDEBUG
        // Before shutting down the memory pool, we can print stats for debugging
        PrintGlobalMemoryStats();
#endif
        ShutdownMemoryPool();
    }

    // ============================================================================
    void InitializeRuntime()
    {
        InitializePlatform();
    }

    // ============================================================================
    void ShutdownRuntime()
    {
        ShutdownPlatform();
    }
} // namespace Odd