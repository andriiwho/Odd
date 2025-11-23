#pragma once

#include "OddCore.h"
#include "RootObj.h"
#include "ObjPtr.h"
#include "OddPlatformWindow.h"
#include "RHIDevice.h"
#include "WeakObjPtr.h"

namespace Odd
{
    class RHIDeviceManager
    {
    public:
        RHIDeviceManager();
        virtual ~RHIDeviceManager() = default;

        // No copy
        RHIDeviceManager(const RHIDeviceManager&) = delete;
        RHIDeviceManager& operator=(const RHIDeviceManager&) = delete;

        // No move
        RHIDeviceManager(RHIDeviceManager&&) noexcept = delete;
        RHIDeviceManager& operator=(RHIDeviceManager&&) noexcept = delete;

        virtual void CreateWindowAndDevice(const PlatformWindowCreateInfo& createInfo) = 0;

        RHIDevice&          GetDevice() { return *Device; }
        const RHIDevice&    GetDevice() const { return *Device; }
        WeakObjPtr<IWindow> GetMainWindow() const { return MainWindow; }

    protected:
        ObjPtr<RHIDevice>   Device{};
        WeakObjPtr<IWindow> MainWindow{};
    };

    extern RHIDeviceManager* GRHIDeviceManager;

    RHIDeviceManager* InitRHIDeviceManager();

} // namespace Odd