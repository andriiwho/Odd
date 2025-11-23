#pragma once

#include "NullSwapChain.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12SwapChain final : public ODD_NULL_PREFIX(SwapChain)
    {
    public:
        D3D12SwapChain(RHIDevice* device, const SwapChainCreateInfo& createInfo);
        ~D3D12SwapChain() override;

        void Present() override;

    private:
        ComPtr<IDXGISwapChain4> m_SwapChain{};
    };
} // namespace Odd::D3D12