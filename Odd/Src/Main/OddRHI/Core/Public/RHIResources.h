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

    struct BufferCreateInfo
    {
    };

    class RHIBuffer : public RHIResource
    {
    public:
        RHIBuffer(RHIDevice* device, const BufferCreateInfo& createInfo);

        inline const BufferCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

    private:
        BufferCreateInfo m_CreateInfo{};
    };

    struct ImageCreateInfo
    {
    };

    class RHIImage : public RHIResource
    {
    public:
        RHIImage(RHIDevice* device, const ImageCreateInfo& createInfo);

        inline const ImageCreateInfo& GetCreateInfo() const { return m_CreateInfo; }

    private:
        ImageCreateInfo m_CreateInfo{};
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