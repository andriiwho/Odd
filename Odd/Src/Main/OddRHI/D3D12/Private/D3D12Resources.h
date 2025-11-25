#pragma once

#include "NullResources.h"
#include "OddD3D12.h"
#include "D3D12MemAlloc.h"

namespace Odd::D3D12
{
    class D3D12Device;

    class D3D12Buffer final : public ODD_NULL_PREFIX(Buffer)
    {
    public:
        D3D12Buffer(D3D12Device* device, const RHIBufferCreateInfo& createInfo);

        inline ID3D12Resource1* GetResourceNativeHandle() const { return m_Resource.Get(); }

        void* Map(bool enableRead = false);
        void  Unmap();

    private:
        D3D12Device* m_D3D12Device{};
        ComPtr<D3D12MA::Allocation> m_Allocation{};
        ComPtr<ID3D12Resource1>     m_Resource{};
        
        void* m_MappedData{};
        bool  m_CachedEnableRead : 1 = false;
    };
} // namespace Odd::D3D12