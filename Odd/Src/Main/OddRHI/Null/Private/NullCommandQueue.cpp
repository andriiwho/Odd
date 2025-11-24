#include "NullCommandQueue.h"
#include "NullRHICommon.h"

namespace Odd
{
    NullCommandQueue::NullCommandQueue(RHIDevice* device, RHICommandQueueType type)
        : RHICommandQueue(device, type)
    {
    }

    void NullCommandQueue::Flush()
    {
        ODD_WARN_NULL_RHI();
    }

}