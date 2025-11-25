#pragma once

#include "NullCommandContext.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12Device;

    class D3D12CommandContext final : public ODD_NULL_PREFIX(CommandContext)
    {
    public:
        D3D12CommandContext(RHIDevice* device, RHICommandQueueType queueType = RHICommandQueueType::Graphics);
        ~D3D12CommandContext() override;

        void BeginRecording() override;
        void EndRecording() override;

        void Submit(RHICommandQueue& commandQueue) override;

        inline ID3D12CommandAllocator*    GetCommandAllocator() const { return m_CommandAllocator.Get(); }
        inline ID3D12CommandList*         GetCommandList() const { return m_CommandList.Get(); }
        inline ID3D12GraphicsCommandList* GetGraphicsCommandList() const { return m_CommandList.Get(); }

    private:
        void WaitForFenceEvent();

        D3D12Device*                      m_D3D12Device{};
        ComPtr<ID3D12CommandAllocator>    m_CommandAllocator{};
        ComPtr<ID3D12GraphicsCommandList> m_CommandList{};

        ComPtr<ID3D12Fence> m_Fence{};
        HANDLE              m_FenceEvent{};
        uint64_t            m_FenceValue{};

        bool m_Open{};
    };
} // namespace Odd::D3D12