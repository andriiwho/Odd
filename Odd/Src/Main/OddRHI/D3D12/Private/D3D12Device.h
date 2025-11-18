#pragma once

#include "RHIDevice.h"
#include "OddD3D12.h"

namespace Odd::D3D12
{
    class D3D12Device final : public RHIDevice
    {
    public:
        D3D12Device();
        ~D3D12Device();

        D3D12Device(const D3D12Device&) = delete;
        D3D12Device& operator=(const D3D12Device&) = delete;
        D3D12Device(D3D12Device&&) noexcept = delete;
        D3D12Device& operator=(D3D12Device&&) noexcept = delete;

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