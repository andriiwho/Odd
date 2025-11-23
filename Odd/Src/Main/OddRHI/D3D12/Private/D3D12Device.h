#pragma once

#include "NullDevice.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12Device final : public NullDevice
    {
    public:
        D3D12Device();
        ~D3D12Device();

        D3D12Device(const D3D12Device&) = delete;
        D3D12Device& operator=(const D3D12Device&) = delete;
        D3D12Device(D3D12Device&&) noexcept = delete;
        D3D12Device& operator=(D3D12Device&&) noexcept = delete;

        inline ID3D12Device5* GetDeviceNativeHandle() const { return m_Device.Get(); }
        inline IDXGIAdapter4* GetAdapterNativeHandle() const { return m_Adapter.Get(); }
        inline IDXGIFactory6* GetFactoryNativeHandle() const { return m_Factory.Get(); }

        RHICommandQueue* CreateCommandQueue(RHICommandQueueType type) override;
        RHISwapChain*    CreateSwapChain(const SwapChainCreateInfo& createInfo) override;

    private:
        ComPtr<IDXGIFactory6> m_Factory{};
        ComPtr<IDXGIAdapter4> m_Adapter{};
        ComPtr<ID3D12Device5> m_Device{};

        void InitFactory();
        void InitDebugInterface();
        void SelectAdapter();
        void CreateDevice();
    };
} // namespace Odd::D3D12