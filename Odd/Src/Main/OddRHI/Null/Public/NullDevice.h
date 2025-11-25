#pragma once

#include "NullPrefix.h"
#include "RHIDevice.h"

namespace Odd
{
	class NullDevice : public RHIDevice
	{
    public:
        virtual RHICommandQueue* CreateCommandQueue(RHICommandQueueType type) override;
        virtual RHISwapChain*    CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
        virtual RHICommandContext* CreateCommandContext(RHICommandQueueType queueType) override;

        virtual RHIBuffer* CreateBuffer(const RHIBufferCreateInfo& createInfo) override;
	};
}