#include "D3D12CommandContext.h"
#include "D3D12Device.h"
#include "D3D12Types.h"
#include "D3D12CommandQueue.h"

namespace Odd::D3D12
{
    D3D12CommandContext::D3D12CommandContext(RHIDevice* device, RHICommandQueueType queueType /*= RHICommandQueueType::Graphics*/)
        : ODD_NULL_PREFIX(CommandContext)(device, queueType)
        , m_D3D12Device(dcast(D3D12Device*, device))
    {
        oddValidate(m_D3D12Device != nullptr);
        if (auto pNativeDevice = m_D3D12Device->GetDeviceNativeHandle())
        {
            oddHrValidate(pNativeDevice->CreateCommandAllocator(ToD3D12CommandQueueType(queueType), IID_PPV_ARGS(&m_CommandAllocator)));
            oddHrValidate(
                pNativeDevice->CreateCommandList1(0,
                    ToD3D12CommandQueueType(queueType),
                    D3D12_COMMAND_LIST_FLAG_NONE,
                    IID_PPV_ARGS(&m_CommandList)));

            // Create fence
            oddHrValidate(pNativeDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
        }
    }

    void D3D12CommandContext::BeginRecording()
    {
        if (m_Fence->GetCompletedValue() < m_FenceValue)
        {
            m_FenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (m_FenceEvent)
            {
                m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
            }
            WaitForFenceEvent();
        }

        oddHrValidate(m_CommandAllocator->Reset());
        oddHrValidate(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
        m_Open = true;

        RHICommandContext::BeginRecording();
    }

    void D3D12CommandContext::EndRecording()
    {
        RHICommandContext::EndRecording();

        oddValidate(m_Open == true);
        oddHrValidate(m_CommandList->Close());
        m_Open = false;
    }

    D3D12CommandContext::~D3D12CommandContext()
    {
        if (m_FenceEvent)
        {
            WaitForFenceEvent();
        }
    }

    void D3D12CommandContext::Submit(RHICommandQueue& commandQueue)
    {
        oddValidateMsg(m_Open == false, "Command list should be closed before submitting");
        oddValidate(commandQueue.GetQueueType() == GetAssociatedQueueType());
        D3D12CommandQueue& queue = scast(D3D12CommandQueue&, commandQueue);

        ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
        queue.SubmitCommandLists(cmdLists, 1, m_Fence.Get(), ++m_FenceValue);
    }

    void D3D12CommandContext::WaitForFenceEvent()
    {
        if (!m_FenceEvent)
            return;

        ::WaitForSingleObject(m_FenceEvent, INFINITE);
        ::CloseHandle(m_FenceEvent);
        m_FenceEvent = 0;
    }

} // namespace Odd::D3D12