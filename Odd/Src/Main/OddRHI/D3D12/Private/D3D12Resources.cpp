#include "D3D12Resources.h"
#include "D3D12Device.h"

namespace Odd::D3D12
{
    D3D12Buffer::D3D12Buffer(D3D12Device* device, const RHIBufferCreateInfo& createInfo)
        : ODD_NULL_PREFIX(Buffer)(device, createInfo)
        , m_D3D12Device(device)
    {
#ifndef ODD_DISTRIBUTION_BUILD
        // Flags validation
        if (createInfo.Flags.Staging)
            oddValidateMsg(!createInfo.Flags.UnorderedAccess, "Upload buffers cannot have UAV access.");
#endif

        // Calculate resource flags for a buffer
        D3D12_RESOURCE_FLAGS bufferFlags{};
        if (!createInfo.Flags.ShaderResource)
            bufferFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        if (createInfo.Flags.UnorderedAccess)
            bufferFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        UINT64 alignedBufferWidth = scast(UINT64, createInfo.SizeInBytes);
        if (createInfo.Flags.ConstantBuffer)
            alignedBufferWidth = (alignedBufferWidth + 255) & ~255;

        D3D12_RESOURCE_DESC desc{
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = 0,
            .Width = alignedBufferWidth,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc = { 1, 0 },
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags = bufferFlags
        };

        D3D12_HEAP_TYPE       heapType{};
        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
        if (createInfo.Flags.Staging)
        {
            heapType = D3D12_HEAP_TYPE_UPLOAD;
            initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
        }
        else if (createInfo.Flags.Readback)
        {
            heapType = D3D12_HEAP_TYPE_READBACK;
            initialState = D3D12_RESOURCE_STATE_COPY_DEST;
        }
        else
        {
            heapType = D3D12_HEAP_TYPE_DEFAULT;
        }

        D3D12MA::ALLOCATION_DESC allocDesc{
            .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
            .HeapType = heapType,
            .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
            .CustomPool = nullptr,
            .pPrivateData = nullptr,
        };
        auto pAlloc = device->GetAllocatorNativeHandle();
        oddAssertMsg(pAlloc != nullptr, "Allocator is null");

        oddHrValidate(pAlloc->CreateResource(&allocDesc,
            &desc,
            initialState, // Use calculated state
            nullptr,
            &m_Allocation,
            IID_PPV_ARGS(&m_Resource)));

#ifndef ODD_DISTRIBUTION_BUILD
        if (m_Resource && !createInfo.DebugName.empty())
        {
            WideString wName = StringToWide(createInfo.DebugName);
            oddHrValidate(m_Resource->SetName(wName.c_str()));
        }
#endif
    }

    void* D3D12Buffer::Map(bool enableRead)
    {
#ifndef ODD_DISTRIBUTION_BUILD
        if (GetCreateInfo().Flags.Staging == true && enableRead)
        {
            ODD_LOG_WARNING("Mapping upload buffer for reading may cause performance issues.");
        }
#endif

        if (m_MappedData)
        {
            // If different values of enableRead from previous call, need to remap the buffer.
            if (m_CachedEnableRead != enableRead)
            {
                Unmap();
            }
            else
            {
                return m_MappedData;
            }
        }

        // We only need to pass this in if the reading is disabled.
        // Passing these values means that the resource is write-only.
        // If enableRead is specified to true then pass nullptr to read the entire resource.
        D3D12_RANGE readRange = {
            .Begin = 0,
            .End = 0,
        };
        oddHrValidate(m_Resource->Map(0, enableRead ? nullptr : &readRange, &m_MappedData));
        m_CachedEnableRead = enableRead;
        return m_MappedData;
    }

    void D3D12Buffer::Unmap()
    {
        if (m_MappedData)
        {
            m_Resource->Unmap(0, nullptr);
            m_MappedData = nullptr;
        }
    }

} // namespace Odd::D3D12