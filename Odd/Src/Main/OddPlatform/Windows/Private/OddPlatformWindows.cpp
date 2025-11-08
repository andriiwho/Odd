#include "OddPlatformWindows.h"
#include "OddWindowsWindow.h"

namespace Odd::Windows
{
    WindowsPlatform::WindowsPlatform()
    {
        ODD_LOG_INFO("Initializing WindowsPlatform...");

        m_HInstance = GetModuleHandle(nullptr);

        ODD_LOG_INFO("WindowsPlatform initialized.");
    }

    WindowsPlatform::~WindowsPlatform()
    {
    }

    IWindow* WindowsPlatform::CreatePlatformWindow(const String& title, const WindowSize& size, const WindowFlags& flags)
    {
        return MakeObject<WindowsWindow>(title, size, flags);
    }

    void WindowsPlatform::PollEventsSimple()
    {
        MSG msg;
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
} // namespace Odd::Windows

namespace Odd::Internal
{
    using namespace Odd::Windows;
    IPlatform* InitializePlatform_Internal()
    {
        return OddNew<WindowsPlatform>();
    }

    void ShutdownPlatform_Internal(IPlatform* platform)
    {
        if (platform)
            OddDelete(platform);
    }
} // namespace Odd::Internal