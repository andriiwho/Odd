#pragma once

#include "RHIDeviceManager.h"

namespace Odd
{
    class NullDeviceManager : public RHIDeviceManager
    {
    public:
        void CreateWindowAndDevice(const PlatformWindowCreateInfo& windowCreateInfo);        
    private:
    };
} // namespace Odd