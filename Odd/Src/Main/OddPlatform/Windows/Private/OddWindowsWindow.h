#pragma once

#include "OddWindows.h"
#include "OddPlatformWindow.h"

namespace Odd::Windows
{
    class WindowsWindow final : public IWindow
    {
    public:
        WindowsWindow(const String& title, const WindowSize& size, const WindowFlags& flags);
        ~WindowsWindow() override;
        WindowSize  GetSize() const noexcept override;
        void        SetSize(const WindowSize& size) override;
        String      GetTitle() const noexcept override;
        void        SetTitle(const String& title) override;
        void        Show() override;
        void        Hide() override;
        WindowFlags GetFlags() const noexcept override;
        void*       GetPlatformNativeHandle() const noexcept override;
        void*       GetPlatformDisplayHandle() const noexcept override;

        // Internal
        void OnCloseMessage_Internal();

    private:
        HWND        m_HWnd = nullptr;
        WindowSize  m_Size{};
        WindowFlags m_Flags{};
    };
} // namespace Odd::Windows