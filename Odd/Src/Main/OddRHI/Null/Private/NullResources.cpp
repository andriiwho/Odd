#include "NullResources.h"

namespace Odd
{

    NullBuffer::NullBuffer(RHIDevice* device, const RHIBufferCreateInfo& createInfo)
        : RHIBuffer(device, createInfo)
    {
    }

    void* NullBuffer::Map(bool enableRead /*= false*/)
    {
        if (m_MappedData.IsValid())
        {
            return m_MappedData.Memory;
        }

        m_MappedData = ScopedAlloc(GetCreateInfo().SizeInBytes);
        return m_MappedData.Memory;
    }

    void NullBuffer::Unmap()
    {
        if (m_MappedData.IsValid())
        {
            m_MappedData.Reset();
        }
    }

} // namespace Odd