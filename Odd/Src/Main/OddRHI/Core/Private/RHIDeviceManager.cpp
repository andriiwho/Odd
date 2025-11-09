#include "RHIDeviceManager.h"
#include "PlatformMacros.h"

#if ODD_PLATFORM_WINDOWS
    #include "D3D12DeviceManager.h"
#endif

namespace Odd
{
    RHIDeviceManager* GRHIDeviceManager = nullptr;

    RHIDeviceManager* InitRHIDeviceManager()
    {
#if ODD_PLATFORM_WINDOWS
        return OddNew<D3D12::D3D12DeviceManager>();
#else
    #error "Only D3D12 is supported."
#endif
    }

    RHIDeviceManager::RHIDeviceManager()
    {
        GRHIDeviceManager = this;
    }

} // namespace Odd