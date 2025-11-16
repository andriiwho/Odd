#include "D3D12DeviceManager.h"
#include "D3D12Device.h"
#include "OddPlatformCore.h"

namespace Odd::D3D12
{
    void D3D12DeviceManager::CreateWindowAndDevice(const String& title, const WindowSize& size, const WindowFlags& flags)
    {
        ODD_LOG_INFO("Initializing window and D3D12 device.");

        // Create window
        MainWindow = GPlatform->CreatePlatformWindow({ title, size, flags });
        oddValidateMsg(MainWindow.IsValid(), "Failed to create main window.");
        MainWindow->Show();

        // Create the device
        Device.reset(OddNew<D3D12Device>());
    }
} // namespace Odd::D3D12