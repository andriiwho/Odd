#pragma once

#include "RHIDeviceChild.h"
#include "RHICommandQueue.h"
#include "WeakObjPtr.h"

namespace Odd
{
    class RHICommandList;

    class RHICommandContext : public RHIDeviceChild
    {
    public:
        RHICommandContext(RHIDevice* device, RHICommandQueueType queueType = RHICommandQueueType::Graphics);
        ~RHICommandContext() override;

        inline RHICommandQueueType GetAssociatedQueueType() const { return m_QueueType; }

        virtual void BeginRecording();
        virtual void EndRecording();
        virtual void Submit(RHICommandQueue& commandQueue) = 0;

        inline RHICommandList& GetCommandList()
        {
            oddValidate(m_CommandList != nullptr);
            return *m_CommandList;
        }

    private:
        RHICommandList*     m_CommandList;
        RHICommandQueueType m_QueueType{};
    };
} // namespace Odd