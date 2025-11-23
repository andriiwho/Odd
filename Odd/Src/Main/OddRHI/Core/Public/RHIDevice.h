#pragma once

#include "RootObj.h"

#include "RHICommandQueue.h"
#include "RHISwapChain.h"

namespace Odd
{
    class RHIDevice : public RootObj
    {
    public:
        virtual RHICommandQueue* CreateCommandQueue(RHICommandQueueType type) = 0;
        virtual RHISwapChain*    CreateSwapChain(const SwapChainCreateInfo& createInfo) = 0;
    };
} // namespace Odd