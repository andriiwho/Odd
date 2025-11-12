#include "D3D12Device.h"

namespace Odd::D3D12
{
    D3D12Device::D3D12Device()
    {
    }

    D3D12Device::~D3D12Device()
    {
    }

    void D3D12Device::InitFactory()
    {
        ODD_LOG_TRACE("Initializing DXGI factory");
    }

} // namespace Odd::D3D12