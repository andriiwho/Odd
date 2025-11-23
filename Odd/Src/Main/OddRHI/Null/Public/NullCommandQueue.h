#pragma once

#include "RHICommandQueue.h"

namespace Odd
{
	class NullCommandQueue : public RHICommandQueue
	{
    public:
        NullCommandQueue(RHIDevice* device, RHICommandQueueType type);
	};
}