#pragma once

#include "RHIDeviceChild.h"
#include "RHICommandQueue.h"

namespace Odd
{
    class RHICommandContext : public RHIDeviceChild
    {
    public:
        RHICommandContext(RHIDevice* device, RHICommandQueueType queueType = RHICommandQueueType::Graphics);

        inline RHICommandQueueType GetAssociatedQueueType() const { return m_QueueType; }

    private:
        RHICommandQueueType m_QueueType{};
    };
} // namespace Odd