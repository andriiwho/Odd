#include "NullDevice.h"
#include "NullCommandQueue.h"
#include "NullRHICommon.h"
#include "NullSwapChain.h"
#include "NullCommandContext.h"

namespace Odd
{

    RHICommandQueue* NullDevice::CreateCommandQueue(RHICommandQueueType type)
    {
        ODD_WARN_NULL_RHI();
        return CreateChildObject<NullCommandQueue>(this, type);
    }

    RHISwapChain* NullDevice::CreateSwapChain(const SwapChainCreateInfo& createInfo)
    {
        ODD_WARN_NULL_RHI();
        return CreateChildObject<NullSwapChain>(this, createInfo);
    }

    RHICommandContext* NullDevice::CreateCommandContext(RHICommandQueueType queueType)
    {
        ODD_WARN_NULL_RHI();
        return CreateChildObject<NullCommandContext>(this, queueType);
    }

}