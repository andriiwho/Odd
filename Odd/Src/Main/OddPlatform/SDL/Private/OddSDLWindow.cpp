#include "OddSDLWindow.h"
#include "OddCore.h"

#include "SDL3/SDL.h"

namespace Odd::SDL
{
    SDLWindow::SDLWindow(const PlatformWindowCreateInfo& createInfo)
        : m_Flags(createInfo.Flags)
    {
        SDL_WindowFlags windowFlags{};
        // TODO: For future APIs
        switch (createInfo.Flags.ApiSupport)
        {
            default:
                break;
        }

        if (createInfo.Flags.Borderless)
            windowFlags |= SDL_WINDOW_BORDERLESS;
        if (createInfo.Flags.Fullscreen)
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        if (createInfo.Flags.Resizable)
            windowFlags |= SDL_WINDOW_RESIZABLE;

        m_Window = SDL_CreateWindow(
            createInfo.Title.c_str(),
            scast(int, createInfo.Size.Width),
            scast(int, createInfo.Size.Height),
            windowFlags);
        if (!m_Window)
        {
            const String errorString = SDL_GetError();
            SDL_ClearError();
            oddCrash("Failed to create SDL window. {}", errorString);
        }

        SDL_PropertiesID properties = SDL_GetWindowProperties(m_Window);
        SDL_SetPointerProperty(properties, ODD_SDL_PROP_WINDOW_USER_POINTER, this);
    }

    SDLWindow::~SDLWindow()
    {
        if (m_Window)
        {
            SDL_DestroyWindow(m_Window);
        }
    }

    WindowSize SDLWindow::GetSize() const noexcept
    {
        int width, height;
        oddValidate(SDL_GetWindowSize(m_Window, &width, &height));
        return {
            WindowSize{
                .Width = scast(uint32_t, width),
                .Height = scast(uint32_t, height),
            },
        };
    }

    void SDLWindow::SetSize(const WindowSize& size)
    {
        SDL_SetWindowSize(m_Window, scast(int, size.Width), scast(int, size.Height));
    }

    String SDLWindow::GetTitle() const noexcept
    {
        return SDL_GetWindowTitle(m_Window);
    }

    void SDLWindow::SetTitle(const String& title)
    {
        SDL_SetWindowTitle(m_Window, title.c_str());
    }

    void SDLWindow::Show()
    {
        SDL_ShowWindow(m_Window);
    }

    void SDLWindow::Hide()
    {
        SDL_HideWindow(m_Window);
    }

    WindowFlags SDLWindow::GetFlags() const noexcept
    {
        return WindowFlags();
    }

    void* SDLWindow::GetPlatformNativeHandle() const noexcept
    {
        const auto properties = SDL_GetWindowProperties(m_Window);
#ifdef _WIN32
        void* outPtr = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        oddValidate(outPtr != nullptr);
        return outPtr;
#else
        // Not yet supported.
        return nullptr;
#endif
    }

    void* SDLWindow::GetPlatformDisplayHandle() const noexcept
    {
        return nullptr;
    }
} // namespace Odd::SDL