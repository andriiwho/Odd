#include "RHICommandContext.h"

#include "RHIDevice.h"
#include "RHICommands.h"

namespace Odd
{
    // 4 kb is enough for now.
    static constexpr size_t DEFAULT_COMMAND_POOL_CAPACITY = 4 * 1024 * 1024;

    RHICommandContext::RHICommandContext(RHIDevice* device, RHICommandQueueType queueType)
        : RHIDeviceChild(device, RHIDeviceChildType::CommandContext)
        , m_QueueType(queueType)
    {
        m_CommandList = OddNew<RHICommandList>();
    }

    RHICommandContext::~RHICommandContext()
    {
        if (m_CommandList != nullptr)
            OddDelete(m_CommandList);
    }

    void RHICommandContext::BeginRecording()
    {
        m_CommandList->BeginRecording(DEFAULT_COMMAND_POOL_CAPACITY);
    }

    void RHICommandContext::EndRecording()
    {
        m_CommandList->EndRecording();
        m_CommandList->Execute(*this);
    }

}