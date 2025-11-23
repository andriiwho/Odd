#include "D3D12CommandContext.h"
#include "D3D12Device.h"
#include "D3D12Types.h"

namespace Odd::D3D12
{
    D3D12CommandContext::D3D12CommandContext(RHIDevice* device, RHICommandQueueType queueType /*= RHICommandQueueType::Graphics*/)
        : ODD_NULL_PREFIX(CommandContext)(device, queueType)
        , m_D3D12Device(dcast(D3D12Device*, device))
    {
        oddValidate(m_D3D12Device != nullptr);
        if (auto pNativeDevice = m_D3D12Device->GetDeviceNativeHandle())
        {
            oddHrValidate(pNativeDevice->CreateCommandAllocator(ToD3D12CommandQueueType(queueType), IID_PPV_ARGS(&m_CommandAllocator)));
            oddHrValidate(
                pNativeDevice->CreateCommandList1(0,
                    ToD3D12CommandQueueType(queueType),
                    D3D12_COMMAND_LIST_FLAGS::D3D12_COMMAND_LIST_FLAG_NONE,
                    IID_PPV_ARGS(&m_CommandList)));
        }
    }

} // namespace Odd::D3D12