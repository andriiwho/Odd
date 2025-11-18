#include "D3D12DeviceManager.h"
#include "D3D12Device.h"
#include "OddPlatformCore.h"

namespace Odd::D3D12
{
    void Odd::D3D12::D3D12DeviceManager::CreateWindowAndDevice(const PlatformWindowCreateInfo& createInfo)
    {
        ODD_LOG_INFO("Initializing window and D3D12 device.");

        // Create window
        MainWindow = GPlatform->CreatePlatformWindow(createInfo);
        oddValidateMsg(MainWindow.IsValid(), "Failed to create main window.");
        MainWindow->Show();

        // Create the device
        Device.reset(OddNew<D3D12Device>());
    }
} // namespace Odd::D3D12