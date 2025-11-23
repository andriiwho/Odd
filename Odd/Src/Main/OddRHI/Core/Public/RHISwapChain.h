#pragma once

#include "RHIDeviceChild.h"
#include "RHICommandQueue.h"
#include "OddPlatformWindow.h"
#include "WeakObjPtr.h"

namespace Odd
{
    inline constexpr uint32_t DEFAULT_SWAPCHAIN_MIN_IMAGE_NUM = 3;

    struct SwapChainCreateInfo
    {
        WeakObjPtr<IWindow>         AssociatedWindow{};
        WeakObjPtr<RHICommandQueue> CommandQueue{};
        bool                        EnableVSync{};
        uint32_t                    MinImageCount{ DEFAULT_SWAPCHAIN_MIN_IMAGE_NUM };
    };

    class RHISwapChain : public RHIDeviceChild
    {
    public:
        RHISwapChain(RHIDevice* parent, const SwapChainCreateInfo& createInfo);

        inline WeakObjPtr<IWindow>        GetAssociatedWindow() const { return m_CreateInfo.AssociatedWindow; }
        inline const SwapChainCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

        virtual void Present() = 0;

    private:
        SwapChainCreateInfo m_CreateInfo{};
    };
} // namespace Odd