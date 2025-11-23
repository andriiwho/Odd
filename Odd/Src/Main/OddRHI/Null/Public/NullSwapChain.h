#pragma once

#include "RHISwapChain.h"

namespace Odd
{
    class NullSwapChain : public RHISwapChain
    {
    public:
        NullSwapChain(RHIDevice* device, const SwapChainCreateInfo& createInfo);

        virtual void Present() override;
    };
} // namespace Odd