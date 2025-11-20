#include "RHICommandQueue.h"

namespace Odd
{

    RHICommandQueue::RHICommandQueue(RHIDevice* device, RHICommandQueueType queueType)
        : RHIDeviceChild(device, RHIDeviceChildType::CommandQueue)
        , m_QueueType(queueType)
    {
    }

    RHICommandQueue::~RHICommandQueue()
    {
    }

} // namespace Odd