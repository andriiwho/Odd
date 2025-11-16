#pragma once

#include "OddPlatformCore.h"

namespace Odd::SDL
{
    struct SDLPlatform final : public Odd::IPlatform
    {
    public:
        virtual IWindow* CreatePlatformWindow(const PlatformWindowCreateInfo& createInfo) override;
        virtual void PollEventsSimple() override;
    };
} // namespace Odd