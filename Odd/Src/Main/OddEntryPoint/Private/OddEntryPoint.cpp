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
            // This is safe to call here, because window is not immediately deleted on close.
            // But e.g. next frame it would be really gone.
            // Because the memory is reclaimed only when we flush expired root objects.
            if (mainWindow->Expired())
                break;

            // Collect "garbage" every frame. In a real application this would be done less frequently.
            // But technically this isn't garbage collection, just cleaning up expired root objects.
            // And since they use internal allocator, which doesn't return memory to the OS immediately,
            // this is a lightweight operation.
            // So we can afford to do this every frame.
            // So yeah, flush expired root objects every frame.
            // Just call the function.
            // Here we go:
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