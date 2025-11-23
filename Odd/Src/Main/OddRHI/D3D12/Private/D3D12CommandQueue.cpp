#include "D3D12CommandQueue.h"
#include "D3D12Device.h"
#include "D3D12Types.h"

namespace Odd::D3D12
{
    D3D12CommandQueue::D3D12CommandQueue(RHIDevice* device, RHICommandQueueType queueType)
        : ODD_NULL_PREFIX(CommandQueue)(device, queueType)
        , m_D3D12Device(scast(D3D12Device*, device))
    {
        D3D12_COMMAND_QUEUE_DESC desc{
            .Type = ToD3D12CommandQueueType(queueType),
            .Priority = {},
            .Flags = {},
            .NodeMask = 0,
        };
        
        if (auto d3d12Device = m_D3D12Device->GetDeviceNativeHandle())
        {
            oddHrValidate(d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)));
        }
    }

    D3D12CommandQueue::~D3D12CommandQueue()
    {
    }

} // namespace Odd::D3D12