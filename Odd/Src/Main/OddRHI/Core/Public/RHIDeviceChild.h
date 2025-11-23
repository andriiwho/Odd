#pragma once

#include "RootObj.h"

namespace Odd
{
    class RHIDevice;

    enum class RHIDeviceChildType
    {
        CommandQueue,
        SwapChain,
    };

    class RHIDeviceChild : public RootObj
    {
        ODD_ROOT_OBJECT(RHIDeviceChild);
    public:
        RHIDeviceChild(RHIDevice* device, RHIDeviceChildType type);

        inline RHIDevice*         GetDevice() const { return m_Device; }
        inline RHIDeviceChildType GetDeviceChildType() const { return m_DeviceChildType; }

    private:
        RHIDevice*         m_Device{};
        RHIDeviceChildType m_DeviceChildType{};
    };
} // namespace Odd