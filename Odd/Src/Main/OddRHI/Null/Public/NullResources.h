#pragma once

#include "NullPrefix.h"
#include "RHIResources.h"

namespace Odd
{
	class NullBuffer : public RHIBuffer
	{
    public:
        NullBuffer(RHIDevice* device, const RHIBufferCreateInfo& createInfo);
	};
}