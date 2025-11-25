#pragma once

#include "RootObj.h"

#include "RHICommandQueue.h"
#include "RHISwapChain.h"
#include "RHICommandContext.h"
#include "RHIResources.h"

namespace Odd
{
    class RHIDevice : public RootObj
    {
    public:
        virtual RHICommandQueue*   CreateCommandQueue(RHICommandQueueType type) = 0;
        virtual RHISwapChain*      CreateSwapChain(const SwapChainCreateInfo& createInfo) = 0;
        virtual RHICommandContext* CreateCommandContext(RHICommandQueueType queueType) = 0;

        virtual RHIBuffer* CreateBuffer(const RHIBufferCreateInfo& createInfo) = 0;
    };
} // namespace Odd