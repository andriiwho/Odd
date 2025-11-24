#pragma once

#include "NullCommandQueue.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12Device;

    class D3D12CommandQueue final : public ODD_NULL_PREFIX(CommandQueue)
    {
    public:
        D3D12CommandQueue(RHIDevice* device, RHICommandQueueType queueType);
        ~D3D12CommandQueue() override;

        inline ID3D12CommandQueue* GetHandle() const { return m_CommandQueue.Get(); }
        virtual void               Flush() override;

    private:
        D3D12Device*               m_D3D12Device{};
        ComPtr<ID3D12CommandQueue> m_CommandQueue{};

        ComPtr<ID3D12Fence> m_Fence{};
        uint64_t            m_FenceValue{};
    };
} // namespace Odd::D3D12