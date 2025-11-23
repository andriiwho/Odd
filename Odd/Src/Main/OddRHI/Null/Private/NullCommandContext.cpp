#include "NullCommandContext.h"

namespace Odd
{

    NullCommandContext::NullCommandContext(RHIDevice* device, RHICommandQueueType queueType)
        : RHICommandContext(device, queueType)
    {
    }

}