#include "RHIResources.h"

namespace Odd
{
    RHIResource::RHIResource(RHIDevice* device, RHIDeviceChildType type)
        : RHIDeviceChild(device, type)
    {
    }

    RHIResourceView::RHIResourceView(RHIResource* resource, RHIDeviceChildType type)
        : RHIResource(resource ? resource->GetDevice() : nullptr, type)
    {
#ifndef ODD_DISTRIBUTION_BUILD
        // Validate that the parent is RHIResource, not RHIDevice.
        EnsureParentType<RHIResource>();
#endif
    }

    RHIBuffer::RHIBuffer(RHIDevice* device, const BufferCreateInfo& createInfo)
        : RHIResource(device, RHIDeviceChildType::Buffer)
        , m_CreateInfo(createInfo)
    {
    }

    RHIImage::RHIImage(RHIDevice* device, const ImageCreateInfo& createInfo)
        : RHIResource(device, RHIDeviceChildType::Image)
        , m_CreateInfo(createInfo)
    {
    }

    RHIImageView::RHIImageView(RHIImage* image, RHIDeviceChildType type)
        : RHIResourceView(image, type)
    {
    }

} // namespace Odd