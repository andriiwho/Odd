#pragma once

#include "OddCore.h"
#include "OddPlatformWindow.h"

namespace Odd
{
    struct IPlatform
    {
        IPlatform();
        virtual ~IPlatform();

        virtual IWindow* CreatePlatformWindow(const String& title, const WindowSize& size, const WindowFlags& flags) = 0;
        virtual void     PollEventsSimple() = 0;
    };

    extern IPlatform* GPlatform;

    // Initializes and returns the platform instance.
    void InitializePlatform();
    void ShutdownPlatform();
} // namespace Odd