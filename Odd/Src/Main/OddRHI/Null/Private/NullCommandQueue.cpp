#include "NullCommandQueue.h"

namespace Odd
{
    NullCommandQueue::NullCommandQueue(RHIDevice* device, RHICommandQueueType type)
        : RHICommandQueue(device, type)
    {
    }
}