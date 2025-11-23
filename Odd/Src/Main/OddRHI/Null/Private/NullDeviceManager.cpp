#include "NullDeviceManager.h"
#include "NullRHICommon.h"
#include "OddPlatformCore.h"
#include "NullDevice.h"

namespace Odd
{
    void NullDeviceManager::CreateWindowAndDevice(const PlatformWindowCreateInfo& windowCreateInfo)
    {
        ODD_WARN_NULL_RHI();
        MainWindow = GPlatform->CreatePlatformWindow(windowCreateInfo);
        Device = ObjPtr(MakeObject<NullDevice>());
    }
}