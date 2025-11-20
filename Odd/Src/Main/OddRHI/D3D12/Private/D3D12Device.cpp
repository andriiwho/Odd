#include "D3D12Device.h"
#include "OddCore.h"
#include "D3D12CommandQueue.h"

namespace Odd::D3D12
{
    namespace Config
    {
        static bool GCfgDebug = false;
        static bool GCfgValidateGPU = false;
        static bool GCfgSyncQueueValidation = false;
    } // namespace Config

    D3D12Device::D3D12Device()
    {
        Config::GCfgDebug = ODD_CONF_OR(bool, "Engine.RHI.Debug.Enable", false);
        Config::GCfgValidateGPU = ODD_CONF_OR(bool, "Engine.RHI.Debug.GPU", false);
        Config::GCfgSyncQueueValidation = ODD_CONF_OR(bool, "Engine.RHI.Debug.SyncQueueValidation", false);

        InitFactory();
        if (Config::GCfgDebug)
        {
            InitDebugInterface();
        }
        SelectAdapter();
        CreateDevice();
    }

    D3D12Device::~D3D12Device()
    {
    }

    void D3D12Device::InitFactory()
    {
        ODD_LOG_TRACE("Initializing DXGI factory");

        UINT dxgiFacFlags = 0;
        if (Config::GCfgDebug)
            dxgiFacFlags |= DXGI_CREATE_FACTORY_DEBUG;

        oddHrValidate(CreateDXGIFactory2(dxgiFacFlags, IID_PPV_ARGS(&m_Factory)));

        ODD_LOG_TRACE("DXGI factory created.");
    }

    void D3D12Device::InitDebugInterface()
    {
        ODD_LOG_TRACE("Configuring D3D12 debug interface");

        ComPtr<ID3D12Debug4> debugInterface;
        oddHrValidate(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));

        if (debugInterface)
        {
            debugInterface->EnableDebugLayer();
            ODD_LOG_TRACE(" - Debug layer enabled.");

            if (Config::GCfgValidateGPU)
            {
                debugInterface->SetEnableGPUBasedValidation(true);
                ODD_LOG_TRACE(" - GPU based validation enabled.");
            }

            if (Config::GCfgSyncQueueValidation)
            {
                debugInterface->SetEnableSynchronizedCommandQueueValidation(true);
                ODD_LOG_TRACE(" - Synchronized command queue validation enabled.");
            }
        }
    }

    void D3D12Device::SelectAdapter()
    {
        ODD_LOG_TRACE("Querying DXGI adapter...");
        oddHrValidate(m_Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_Adapter)));
        if (m_Adapter)
        {
            DXGI_ADAPTER_DESC desc;
            oddHrValidate(m_Adapter->GetDesc(&desc));

            char deviceNameBuffer[std::size(desc.Description)] = { 0 };
            wcstombs(deviceNameBuffer, desc.Description, std::size(deviceNameBuffer));
            ODD_LOG_TRACE("Selected DXGI adapter: {}", deviceNameBuffer);
        }
    }

    void D3D12Device::CreateDevice()
    {
        ODD_LOG_TRACE("Creating D3D12 device...");
        oddHrValidate(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)));
        ODD_LOG_TRACE("D3D12 device created.");
    }

    RHICommandQueue* D3D12Device::CreateCommandQueue(RHICommandQueueType type)
    {
        return CreateChildObject<D3D12CommandQueue>(this, type);
    }

} // namespace Odd::D3D12