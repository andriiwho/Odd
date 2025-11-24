#include "D3D12SwapChain.h"
#include "D3D12CommandQueue.h"
#include "D3D12Device.h"

namespace Odd::D3D12
{

    D3D12SwapChain::D3D12SwapChain(RHIDevice* device, const SwapChainCreateInfo& createInfo)
        : ODD_NULL_PREFIX(SwapChain)(device, createInfo)
    {
        oddValidate(createInfo.AssociatedWindow.IsValid());
        oddValidate(createInfo.MinImageCount > 0);
        oddValidate(createInfo.CommandQueue.IsValid());

        D3D12Device* d3d12Device = scast(D3D12Device*, device);
        oddValidate(d3d12Device);

        DXGI_SWAP_CHAIN_DESC1 desc{
            .Width = 0,             // Fill entire window
            .Height = 0,            // Fill entire window
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,       // TODO: Make this dynamic or user assignable.
            .Stereo = FALSE,
            .SampleDesc = { 1, 0 },
            .BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT,  // TODO: Do we need shader input?
            .BufferCount = std::min(createInfo.MinImageCount, scast(uint32_t, DXGI_MAX_SWAP_CHAIN_BUFFERS)),
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = !createInfo.EnableVSync ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u,
        };

        if (auto pFactory = d3d12Device->GetFactoryNativeHandle())
        {
            D3D12CommandQueue* d3d12Queue = dcast(D3D12CommandQueue*, createInfo.CommandQueue.Get());
            oddValidate(d3d12Queue);
            ComPtr<IDXGISwapChain1> swapChain;
            oddHrValidate(pFactory->CreateSwapChainForHwnd(
                d3d12Queue->GetHandle(),
                scast(HWND, createInfo.AssociatedWindow->GetPlatformNativeHandle()),
                &desc,
                nullptr,
                nullptr,
                &swapChain));

            oddValidate(swapChain != nullptr);
            oddHrValidate(swapChain.As(&m_SwapChain));
        }
    }

    D3D12SwapChain::~D3D12SwapChain()
    {
    }

    void D3D12SwapChain::Present()
    {
        UINT presentFlags = 0;
        UINT interval = 1;
        if (!GetCreateInfo().EnableVSync)
        {
            presentFlags = DXGI_PRESENT_ALLOW_TEARING;
            interval = 0;
        }

        oddHrValidate(m_SwapChain->Present(interval, presentFlags));
    }

} // namespace Odd::D3D12