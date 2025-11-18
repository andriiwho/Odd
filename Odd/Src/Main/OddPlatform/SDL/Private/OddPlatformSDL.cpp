#include "OddPlatformSDL.h"
#include "OddSDLWindow.h"

#include "SDL3/SDL.h"
#include "WeakObjPtr.h"

namespace Odd::SDL
{
    static WeakObjPtr<IWindow> GetWindowHandleFromID(SDL_WindowID id)
    {
        SDL_Window* window = SDL_GetWindowFromID(id);
        if (!window)
            return {};

        SDL_PropertiesID props = SDL_GetWindowProperties(window);
        return scast(IWindow*, SDL_GetPointerProperty(props, ODD_SDL_PROP_WINDOW_USER_POINTER, nullptr));
    }

    SDLPlatform::SDLPlatform()
    {
        oddValidate(SDL_Init(SDL_INIT_VIDEO));
    }

    SDLPlatform::~SDLPlatform()
    {
        SDL_Quit();
    }

    IWindow* SDLPlatform::CreatePlatformWindow(const PlatformWindowCreateInfo& createInfo)
    {
        return MakeObject<SDLWindow>(createInfo);
    }

    void SDLPlatform::PollEventsSimple()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    if (auto window = GetWindowHandleFromID(e.window.windowID); window.IsValid())
                    {
                        window->ForceExpire();
                    }
                }
                break;
                default:
                    break;
            }
        }
    }
} // namespace Odd::SDL

namespace Odd::Internal
{
    IPlatform* InitializePlatform_Internal()
    {
        return OddNew<SDL::SDLPlatform>();
    }

    void ShutdownPlatform_Internal(IPlatform* platform)
    {
        OddDelete(platform);
    }
} // namespace Odd::Internal