#include "RHI.h"
#include "OddConfig.h"

namespace Odd
{
    RHI* GRHI = nullptr;

    RHI::RHI()
        : m_DeviceManager(InitRHIDeviceManager())
    {
        GRHI = this;
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

        m_CfgNumFramesPerFlight = ODD_CONF_OR(uint32_t, "Engine.RHI.NumFramesInFlight", 3u);
        PrepareCommandContexts();

        // Begin recording the first context
        GetCurrentCommandContext().BeginRecording();
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

    void RHI::EndFrameAndSubmit()
    {
        RHICommandContext& cmdCtx = GetCurrentCommandContext();
        cmdCtx.EndRecording();

        cmdCtx.Submit(*m_GraphicsCommandQueue.Get());

        // Set frame in flight index.
        m_CurrentFrameInFlight = (m_CurrentFrameInFlight + 1) % m_CfgNumFramesPerFlight;
        GetCurrentCommandContext().BeginRecording();
    }

    RHICommandContext& RHI::GetCurrentCommandContext()
    {
        return *m_CommandContexts.at(m_CurrentFrameInFlight).Get();
    }

    RHICommandList& RHI::GetCommandListForGraphics()
    {
        return GetCurrentCommandContext().GetCommandList();
    }

    void RHI::PrepareCommandContexts()
    {
        oddValidate(m_CfgNumFramesPerFlight > 0);
        m_CommandContexts.reserve(m_CfgNumFramesPerFlight);

        ODD_LOG_TRACE("Creating {} in-flight contexts per queue.", m_CfgNumFramesPerFlight);

        for (uint32_t index = 0; index < m_CfgNumFramesPerFlight; ++index)
        {
            m_CommandContexts.push_back(m_Device->CreateCommandContext(RHICommandQueueType::Graphics));
        }
    }

} // namespace Odd