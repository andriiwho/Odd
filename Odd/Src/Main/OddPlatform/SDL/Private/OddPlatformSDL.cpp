#include "OddPlatformSDL.h"

namespace Odd::SDL
{

    IWindow* SDLPlatform::CreatePlatformWindow(const PlatformWindowCreateInfo& createInfo)
    {
        return nullptr;
    }

    void SDLPlatform::PollEventsSimple()
    {
    }
} // namespace Odd

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
}