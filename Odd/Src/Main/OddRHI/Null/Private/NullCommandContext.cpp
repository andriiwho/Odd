#include "NullCommandContext.h"
#include "NullRHICommon.h"

namespace Odd
{

    NullCommandContext::NullCommandContext(RHIDevice* device, RHICommandQueueType queueType)
        : RHICommandContext(device, queueType)
    {
    }

    void NullCommandContext::BeginRecording()
    {
        ODD_WARN_NULL_RHI();
        RHICommandContext::BeginRecording();
    }

    void NullCommandContext::EndRecording()
    {
        ODD_WARN_NULL_RHI();
        RHICommandContext::EndRecording();
    }

    void NullCommandContext::Submit(RHICommandQueue& commandQueue)
    {
        ODD_WARN_NULL_RHI();
    }

} // namespace Odd