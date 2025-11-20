#pragma once

#include "RHIDeviceManager.h"
#include "RHICommandQueue.h"
#include "WeakObjPtr.h"

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

    private:
        UniquePtr<RHIDeviceManager> m_DeviceManager{};

        WeakObjPtr<RHIDevice>       m_Device{};
        WeakObjPtr<RHICommandQueue> m_GraphicsCommandQueue{};
    };
} // namespace Odd