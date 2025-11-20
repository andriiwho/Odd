#pragma once

#include "RHIDeviceChild.h"

namespace Odd
{
    enum class RHICommandQueueType
    {
        Graphics,
        Compute,
        Transfer,
    };

    class RHICommandQueue : public RHIDeviceChild
    {
    public:
        RHICommandQueue(RHIDevice* device, RHICommandQueueType queueType);
        ~RHICommandQueue() override;

        inline RHICommandQueueType GetQueueType() const { return m_QueueType; }

    private:
        RHICommandQueueType m_QueueType{};
    };
} // namespace Odd