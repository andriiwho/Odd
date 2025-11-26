#pragma once

#include "RHIDeviceChild.h"
#include "RHICommandQueue.h"
#include "WeakObjPtr.h"
#include "RHITypes.h"

namespace Odd
{
    class RHICommandList;
    class RHIResource;

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

        virtual void CMDTransitionResource(RHIResource* resource, const RHIResourceTransitionInfo& transitionInfo) = 0;

    private:
        RHICommandList*     m_CommandList;
        RHICommandQueueType m_QueueType{};
    };
} // namespace Odd