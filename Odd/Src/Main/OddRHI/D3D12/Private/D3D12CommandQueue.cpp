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

            // Create a fence
            oddHrValidate(d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
        }
    }

    D3D12CommandQueue::~D3D12CommandQueue()
    {
        Flush();
    }

    void D3D12CommandQueue::Flush()
    {
        ++m_FenceValue;

        oddHrValidate(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));
        if (m_Fence->GetCompletedValue() < m_FenceValue)
        {
            if (HANDLE eventHandle = ::CreateEvent(nullptr, FALSE, FALSE, nullptr))
            {
                oddHrValidate(m_Fence->SetEventOnCompletion(m_FenceValue, eventHandle));
                ::WaitForSingleObject(eventHandle, INFINITE);
                ::CloseHandle(eventHandle);
            }
        }
    }

} // namespace Odd::D3D12