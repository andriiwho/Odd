#include "D3D12Resources.h"
#include "D3D12Device.h"

namespace Odd::D3D12
{
    D3D12Buffer::D3D12Buffer(D3D12Device* device, const RHIBufferCreateInfo& createInfo)
        : ODD_NULL_PREFIX(Buffer)(device, createInfo)
        , m_D3D12Device(device)
    {
        // Calculate resource flags for a buffer
        D3D12_RESOURCE_FLAGS bufferFlags{};
        if (!createInfo.Flags.ShaderResource)
            bufferFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        if (createInfo.Flags.UnorderedAccess)
            bufferFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        D3D12_RESOURCE_DESC desc{
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = 0,
            .Width = scast(UINT64, createInfo.SizeInBytes),
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc = { 1, 0 },
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = bufferFlags
        };

        D3D12_HEAP_TYPE heapType{};
        if (createInfo.Flags.Staging)
            heapType = D3D12_HEAP_TYPE_UPLOAD;
        else if (createInfo.Flags.Readback)
            heapType = D3D12_HEAP_TYPE_READBACK;
        else
            heapType = D3D12_HEAP_TYPE_DEFAULT;

        D3D12MA::ALLOCATION_DESC allocDesc{
            .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
            .HeapType = heapType,
            .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
            .CustomPool = nullptr,
            .pPrivateData = nullptr,
        };
        if (auto pAlloc = device->GetAllocatorNativeHandle())
        {
            oddHrValidate(pAlloc->CreateResource(&allocDesc,
                &desc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                &m_Allocation,
                IID_PPV_ARGS(&m_Resource)));
        }
    }
} // namespace Odd::D3D12