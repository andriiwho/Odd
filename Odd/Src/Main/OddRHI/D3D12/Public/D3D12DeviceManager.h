#pragma once

#include "RHIDeviceManager.h"

namespace Odd::D3D12
{
	class D3D12DeviceManager final : public RHIDeviceManager
	{
    public:
        void CreateWindowAndDevice(const String& title, const WindowSize& size, const WindowFlags& flags) override;
	};
}