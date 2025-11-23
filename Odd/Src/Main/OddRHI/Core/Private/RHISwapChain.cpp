#include "RHISwapChain.h"

namespace Odd
{

    RHISwapChain::RHISwapChain(RHIDevice* parent, const SwapChainCreateInfo& createInfo)
        : RHIDeviceChild(parent, RHIDeviceChildType::SwapChain)
    {
    }

} // namespace Odd