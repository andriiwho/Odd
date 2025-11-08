#pragma once

#include "OddCore.h"
#include "RootObj.h"

namespace Odd
{
    struct WindowSize
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
    };

    struct WindowFlags
    {
        bool Resizable : 1 = true;
        bool Fullscreen : 1 = false;
        bool Borderless : 1 = false;
    };

    class IWindow : public RootObj
    {
    public:
        virtual ~IWindow() = default;

        virtual WindowSize GetSize() const noexcept = 0;
        virtual void       SetSize(const WindowSize& size) = 0;

        // Title management
        virtual String GetTitle() const noexcept = 0;
        virtual void   SetTitle(const String& title) = 0;

        // Show / hide
        virtual void Show() = 0;
        virtual void Hide() = 0;

        // Flags
        virtual WindowFlags GetFlags() const noexcept = 0;
        
        // Platform-specific handle retrieval
        virtual void* GetPlatformNativeHandle() const noexcept = 0;

        // Display handle. Can be null on platforms that do not separate display and window handles.
        virtual void* GetPlatformDisplayHandle() const noexcept = 0;

    protected:
        friend class IPlatform;
        IWindow() = default;
    };
} // namespace Odd