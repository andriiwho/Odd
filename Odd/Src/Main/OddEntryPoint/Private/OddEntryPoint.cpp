#include "OddEntryPoint.h"
#include "RootObj.h"
#include "OddPlatformCore.h"
#include "WeakObjPtr.h"
#include "RHIDeviceManager.h"
#include "OddConfig.h"

#include <filesystem>

namespace Odd
{
    // ============================================================================
    // Statics

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
        UniquePtr<RHIDeviceManager> rhiDeviceManager(InitRHIDeviceManager());
        oddValidate(rhiDeviceManager != nullptr);
        rhiDeviceManager->CreateWindowAndDevice({
            .Title = ODD_CONF_OR(String, "Engine.MainWindow.Title", "Odd"),
            .Size = {
                .Width = ODD_CONF_OR(uint32_t, "Engine.MainWindow.Width", 800),
                .Height = ODD_CONF_OR(uint32_t, "Engine.MainWindow.Height", 600),
            },
            .Flags = {},

        });

        WeakObjPtr<IWindow> mainWindow = rhiDeviceManager->GetMainWindow();
        while (true)
        {
            GPlatform->PollEventsSimple();
            if (!mainWindow.IsValid())
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
    static void InitializeRootFolder()
    {
        static constexpr int  maxDepth = 50;
        int                   currentDepth = 0;
        std::filesystem::path current = std::filesystem::current_path();
        while (!std::filesystem::exists(current / "OddProject.yaml"))
        {
            if (current.has_parent_path())
            {
                current = current.parent_path();
            }
            else
            {
                oddValidateMsg(false, "Failed to find OddProject.lua");
            }

            currentDepth++;
            oddValidateMsg(currentDepth < maxDepth, "Failed to find OddProject.lua");
        }

        std::filesystem::current_path(current);
    }

    // ============================================================================
    void InitializeCore()
    {
        InitializeRootFolder();

        // We need to initialize config before anything else
        Internal::InitializeConfig();

        // Init memory framework
        auto memConfig = MemoryPoolConfig::Default();
        InitializeMemoryPool(memConfig);
        Internal::InitializeRootObjectSystem();

        InitializeLogging();
    }

    // ============================================================================
    void ShutdownCore()
    {
        // Shutdown other systems
        ShutdownLogging();

        // Now we can destroy all expired root objects.
        Internal::ShutdownRootObjectSystem();
#ifndef NDEBUG
        // Before shutting down the memory pool, we can print stats for debugging
        PrintGlobalMemoryStats();
#endif
        ShutdownMemoryPool();

        Internal::ShutdownConfig();
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