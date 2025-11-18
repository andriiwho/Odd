#pragma once

#include "OddPlatformWindow.h"

extern "C"
{
    typedef struct SDL_Window SDL_Window;
}

namespace Odd::SDL
{
    inline constexpr const char* ODD_SDL_PROP_WINDOW_USER_POINTER = "Odd.WindowUserPointer";

    class SDLWindow : public IWindow
    {
    public:
        SDLWindow(const PlatformWindowCreateInfo& createInfo);
        ~SDLWindow() override;

        // Inherited via IWindow
        WindowSize  GetSize() const noexcept override;
        void        SetSize(const WindowSize& size) override;
        String      GetTitle() const noexcept override;
        void        SetTitle(const String& title) override;
        void        Show() override;
        void        Hide() override;
        WindowFlags GetFlags() const noexcept override;
        void*       GetPlatformNativeHandle() const noexcept override;
        void*       GetPlatformDisplayHandle() const noexcept override;

    private:
        SDL_Window* m_Window;

        WindowFlags m_Flags{};
    };
} // namespace Odd::SDL