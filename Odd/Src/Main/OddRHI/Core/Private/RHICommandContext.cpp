#include "RHICommandContext.h"

namespace Odd
{
    RHICommandContext::RHICommandContext(RHIDevice* device, RHICommandQueueType queueType)
        : RHIDeviceChild(device, RHIDeviceChildType::CommandContext)
        , m_QueueType(queueType)
    {
    }
}