#pragma once

#include "RHICommandQueue.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12Device;

    class D3D12CommandQueue final : public RHICommandQueue
    {
    public:
        D3D12CommandQueue(RHIDevice* device, RHICommandQueueType queueType);
        ~D3D12CommandQueue() override;

    private:
        D3D12Device* m_D3D12Device{};
        ComPtr<ID3D12CommandQueue> m_CommandQueue{};
    };
} // namespace Odd::D3D12