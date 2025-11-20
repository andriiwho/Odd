#include "RHI.h"
#include "OddConfig.h"

namespace Odd
{

    RHI::RHI()
        : m_DeviceManager(InitRHIDeviceManager())
    {
        PlatformWindowCreateInfo windowCreateInfo = {
            .Title = ODD_CONF_REQUIRED(String, "Engine.MainWindow.Title"),
            .Size = {
                .Width = ODD_CONF_REQUIRED(uint32_t, "Engine.MainWindow.Width"),
                .Height = ODD_CONF_REQUIRED(uint32_t, "Engine.MainWindow.Height") },
            .Flags = {
                // Unnecessary
            },
        };
        m_DeviceManager->CreateWindowAndDevice(windowCreateInfo);
        m_Device = &m_DeviceManager->GetDevice();
        m_GraphicsCommandQueue = WeakObjPtr(m_Device->CreateCommandQueue(RHICommandQueueType::Graphics));
    }

    RHI::~RHI()
    {
    }

} // namespace Odd