#pragma once

#include "NullDeviceManager.h"

namespace Odd::D3D12
{
	class D3D12DeviceManager final : public NullDeviceManager
	{
    public:
        void CreateWindowAndDevice(const PlatformWindowCreateInfo& createInfo) override;
	};
}