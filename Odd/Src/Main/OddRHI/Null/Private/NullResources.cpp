#include "NullResources.h"

namespace Odd
{

    NullBuffer::NullBuffer(RHIDevice* device, const RHIBufferCreateInfo& createInfo)
        : RHIBuffer(device, createInfo)
    {
    }

}