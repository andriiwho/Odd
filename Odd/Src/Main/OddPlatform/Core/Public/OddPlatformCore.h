#pragma once

#include "OddCore.h"
#include "OddPlatformWindow.h"

namespace Odd
{
    struct IPlatform
    {
        IPlatform();
        virtual ~IPlatform();

        IPlatform(const IPlatform&) = delete;
        IPlatform& operator=(const IPlatform&) = delete;

        IPlatform(IPlatform&&) noexcept = delete;
        IPlatform& operator=(IPlatform&&) noexcept = delete;

        virtual IWindow* CreatePlatformWindow(const PlatformWindowCreateInfo& createInfo) = 0;
        virtual void     PollEventsSimple() = 0;
    };

    extern IPlatform* GPlatform;

    // Initializes and returns the platform instance.
    void InitializePlatform();
    void ShutdownPlatform();
} // namespace Odd