#pragma once

#include "OddPlatformCore.h"
#include "OddWindows.h"

namespace Odd::Windows
{
    class WindowsPlatform final : public IPlatform
    {
    public:
        WindowsPlatform();
        ~WindowsPlatform() override;

        IWindow* CreatePlatformWindow(const String& title, const WindowSize& size, const WindowFlags& flags) override;
        void     PollEventsSimple() override;

    private:
        HINSTANCE m_HInstance = nullptr;
    };
} // namespace Odd::Windows