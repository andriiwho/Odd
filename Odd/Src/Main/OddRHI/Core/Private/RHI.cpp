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

        // Create swapchain for main window
        CreateSwapChain(m_DeviceManager->GetMainWindow());
    }

    RHI::~RHI()
    {
    }

    void RHI::CreateSwapChain(WeakObjPtr<IWindow> window)
    {
        oddValidate(window.IsValid());

        auto swapChain = m_Device->CreateSwapChain(SwapChainCreateInfo{
            .AssociatedWindow = window,
            .CommandQueue = m_GraphicsCommandQueue,
            .EnableVSync = ODD_CONF_OR(bool, "Engine.RHI.VSync.Enable", true),
            .MinImageCount = DEFAULT_SWAPCHAIN_MIN_IMAGE_NUM,
        });

        m_SwapChains.push_back(swapChain);
        const Internal::RootObjectID windowId = window->GetRootObjectID();
        m_SwapChainsPerWindow[windowId] = swapChain;
    }

    void RHI::PresentSwapChain(WeakObjPtr<IWindow> window)
    {
        oddValidate(window.IsValid());

        const auto windowId = window->GetRootObjectID();
        if (auto iter = m_SwapChainsPerWindow.find(windowId); iter != m_SwapChainsPerWindow.end())
        {
            WeakObjPtr<RHISwapChain> swapChain = iter->second;
            oddValidate(swapChain.IsValid());
            swapChain->Present();
        }
    }

    void RHI::PresentAll()
    {
        for (WeakObjPtr<RHISwapChain> swapChain : m_SwapChains)
        {
            if (swapChain.IsValid())
            {
                swapChain->Present();
            }
        }
    }

} // namespace Odd