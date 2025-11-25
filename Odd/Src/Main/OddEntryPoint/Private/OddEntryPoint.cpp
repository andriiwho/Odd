#include "OddEntryPoint.h"
#include "RootObj.h"
#include "OddPlatformCore.h"
#include "WeakObjPtr.h"
#include "RHIDeviceManager.h"
#include "OddConfig.h"

#include <filesystem>
#include "RHI.h"

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
        SharedPtr<RHI>      rhi = MakeShared<RHI>();
        WeakObjPtr<IWindow> mainWindow = rhi->GetDeviceManager()->GetMainWindow();

        RHIBuffer* buffer = rhi->GetDevice()->CreateBuffer({
            .SizeInBytes = 1024,
            .Flags = {
                .Staging = true,
            },
        });

        while (true)
        {
            GPlatform->PollEventsSimple();
            if (!mainWindow.IsValid())
                break;

            RHICommandList& cmd = rhi->GetCommandListForGraphics();

            // Submit command lists
            rhi->EndFrameAndSubmit();

            // Handle presentation of all swap chains
            rhi->PresentAll();

            // Collect "garbage" every frame.
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