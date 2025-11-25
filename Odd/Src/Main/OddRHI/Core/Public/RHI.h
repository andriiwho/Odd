#pragma once

#include "RHIDeviceManager.h"
#include "RHICommandQueue.h"
#include "WeakObjPtr.h"
#include "RHICommandContext.h"

namespace Odd
{
    class RHI
    {
    public:
        RHI();
        ~RHI();

        inline RHIDeviceManager* GetDeviceManager() const noexcept { return m_DeviceManager.get(); }
        inline RHIDevice*        GetDevice() const noexcept { return m_Device.Get(); }
        inline RHICommandQueue*  GetGraphicsQueue() const noexcept { return m_GraphicsCommandQueue.Get(); }

        void CreateSwapChain(WeakObjPtr<IWindow> window);
        void PresentSwapChain(WeakObjPtr<IWindow> window);
        void PresentAll();

        void EndFrameAndSubmit();

        RHICommandList&    GetCommandListForGraphics();

    private:
        UniquePtr<RHIDeviceManager> m_DeviceManager{};

        WeakObjPtr<RHIDevice>                                     m_Device{};
        WeakObjPtr<RHICommandQueue>                               m_GraphicsCommandQueue{};
        Vector<WeakObjPtr<RHISwapChain>>                          m_SwapChains;
        HashMap<Internal::RootObjectID, WeakObjPtr<RHISwapChain>> m_SwapChainsPerWindow;

        Vector<WeakObjPtr<RHICommandContext>> m_CommandContexts;
        uint32_t                              m_CfgNumFramesPerFlight = 3;
        uint32_t                              m_CurrentFrameInFlight{};

        void PrepareCommandContexts();
        RHICommandContext& GetCurrentCommandContext();
    };

    extern RHI* GRHI;
} // namespace Odd