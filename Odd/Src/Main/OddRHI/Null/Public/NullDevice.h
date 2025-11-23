#pragma once

#include "RHIDevice.h"

namespace Odd
{
	class NullDevice : public RHIDevice
	{
    public:
        virtual RHICommandQueue* CreateCommandQueue(RHICommandQueueType type) override;
        virtual RHISwapChain*    CreateSwapChain(const SwapChainCreateInfo& createInfo) override;
	};
}