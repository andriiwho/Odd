#include "NullSwapChain.h"
#include "NullRHICommon.h"

namespace Odd
{

    NullSwapChain::NullSwapChain(RHIDevice* device, const SwapChainCreateInfo& createInfo)
        : RHISwapChain(device, createInfo)
    {
    }

    void NullSwapChain::Present()
    {
        ODD_WARN_NULL_RHI();
    }

} // namespace Odd