#include "RHIDeviceChild.h"

namespace Odd
{

    RHIDeviceChild::RHIDeviceChild(RHIDevice* device, RHIDeviceChildType type)
        : m_Device(device)
        , m_DeviceChildType(type)
    {
    }

} // namespace Odd