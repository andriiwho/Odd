#pragma once

#include "OddCore.h"
#include "RHIDeviceChild.h"
#include "WeakObjPtr.h"

namespace Odd
{
    class RHIResource : public RHIDeviceChild
    {
    public:
        RHIResource(RHIDevice* device, RHIDeviceChildType type);
    };

    // Resource views must be children of resources, not device.
    class RHIResourceView : public RHIResource
    {
    public:
        RHIResourceView(RHIResource* resource, RHIDeviceChildType type);

        inline RHIResource* GetViewedResource() const
        {
            if (m_ViewedResource.IsValid())
            {
                return m_ViewedResource.Get();
            }
            return nullptr;
        }

    private:
        WeakObjPtr<RHIResource> m_ViewedResource;
    };

    struct RHIBufferCreateInfo
    {
        size_t SizeInBytes{};
        struct BufferFlags
        {
            bool ShaderResource : 1 = false;
            bool UnorderedAccess : 1 = false;
            bool Staging : 1 = false;
            bool Readback : 1 = false;
        } Flags;
    };

    class RHIBuffer : public RHIResource
    {
    public:
        RHIBuffer(RHIDevice* device, const RHIBufferCreateInfo& createInfo);

        inline const RHIBufferCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

    private:
        RHIBufferCreateInfo m_CreateInfo{};
    };

    struct RHIImageCreateInfo
    {
    };

    class RHIImage : public RHIResource
    {
    public:
        RHIImage(RHIDevice* device, const RHIImageCreateInfo& createInfo);

        inline const RHIImageCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

    private:
        RHIImageCreateInfo m_CreateInfo{};
    };

    class RHIImageView : public RHIResourceView
    {
    public:
        RHIImageView(RHIImage* image, RHIDeviceChildType type);

        inline RHIImage* GetViewedImage() const
        {
            return scast(RHIImage*, GetViewedResource());
        }
    };
} // namespace Odd