#include "OddWindowsWindow.h"
#include "OddWindowsWindowProcs.h"
#include "OddPlatformWindows.h"

namespace Odd::Windows
{
    static bool GWindowClassesRegistered = false;

    // Default window class
    static ATOM        GDefaultWindowClassAtom = 0;
    static const char* GDefaultWindowClassName = "OddDefaultWindowClass";

    static void RegisterWindowClasses(HINSTANCE hInstance)
    {
        if (GWindowClassesRegistered)
            return;

        WNDCLASSEXA wc = {
            .cbSize = sizeof(WNDCLASSEXA),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = OddDefaultWindowProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = hInstance,
            .hIcon = nullptr,
            .hCursor = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = GDefaultWindowClassName,
            .hIconSm = nullptr,
        };
        GDefaultWindowClassAtom = RegisterClassExA(&wc);
        oddValidateMsg(GDefaultWindowClassAtom != 0, "Failed to register default window class.");

        // This is the final one. Register this boolean as true so that other calls skip registration.
        GWindowClassesRegistered = true;
    }

    WindowsWindow::WindowsWindow(const String& title, const WindowSize& size, const WindowFlags& flags)
        : m_Size(size)
        , m_Flags(flags)
    {
        const HINSTANCE hInstance = GetModuleHandle(nullptr);
        if (!GWindowClassesRegistered)
        {
            RegisterWindowClasses(hInstance);
        }

        // TODO: These should be revisited when we have more window features.
        UINT windowStyle = WS_OVERLAPPEDWINDOW;
        if (!flags.Resizable)
        {
            windowStyle &= ~WS_THICKFRAME;
            windowStyle &= ~WS_MAXIMIZEBOX;
        }

        if (flags.Borderless)
        {
            windowStyle &= ~WS_CAPTION;
            windowStyle &= ~WS_SYSMENU;
            windowStyle &= ~WS_THICKFRAME;
            windowStyle &= ~WS_MINIMIZEBOX;
            windowStyle &= ~WS_MAXIMIZEBOX;
        }

        if (flags.Fullscreen)
        {
            windowStyle = WS_POPUP;
        }

        HWND hWnd = ::CreateWindowExA(
            0,
            GDefaultWindowClassName,
            title.c_str(),
            windowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            scast(int, size.Width),
            scast(int, size.Height),
            nullptr,
            nullptr,
            hInstance,
            this);
        oddValidateMsg(hWnd != nullptr, "Failed to create Windows window.");
        m_HWnd = hWnd;
    }

    WindowsWindow::~WindowsWindow()
    {
        if (m_HWnd)
        {
            ::ShowWindow(m_HWnd, SW_HIDE);
            ::DestroyWindow(m_HWnd);
            m_HWnd = nullptr;
        }
    }

    WindowSize WindowsWindow::GetSize() const noexcept
    {
        return m_Size;
    }

    void WindowsWindow::SetSize(const WindowSize& size)
    {
        oddValidate(m_HWnd != nullptr);
        ::SetWindowPos(
            m_HWnd,
            nullptr,
            0, 0,
            scast(int, size.Width),
            scast(int, size.Height),
            SWP_NOMOVE | SWP_NOZORDER);
        m_Size = size;
    }

    String WindowsWindow::GetTitle() const noexcept
    {
        const int windowTitleLength = ::GetWindowTextLength(m_HWnd);
        String    title;
        title.resize(windowTitleLength + 1, '\0');
        ::GetWindowTextA(m_HWnd, title.data(), windowTitleLength + 1);
        return title;
    }

    void WindowsWindow::SetTitle(const String& title)
    {
        oddValidate(m_HWnd != nullptr);
        ::SetWindowTextA(m_HWnd, title.c_str());
    }

    void WindowsWindow::Show()
    {
        oddValidate(m_HWnd != nullptr);
        ::ShowWindow(m_HWnd, SW_SHOW);
    }

    void WindowsWindow::Hide()
    {
        oddValidate(m_HWnd != nullptr);
        ::ShowWindow(m_HWnd, SW_HIDE);
    }

    WindowFlags WindowsWindow::GetFlags() const noexcept
    {
        return m_Flags;
    }

    void* WindowsWindow::GetPlatformNativeHandle() const noexcept
    {
        return m_HWnd;
    }

    void* WindowsWindow::GetPlatformDisplayHandle() const noexcept
    {
        return nullptr;
    }

    void WindowsWindow::OnCloseMessage_Internal()
    {
        scast(WindowsPlatform*, GPlatform)->OnWindowClosed_Internal(this);

        // This will mark the object as expired.
        ForceExpire();
    }

} // namespace Odd::Windows