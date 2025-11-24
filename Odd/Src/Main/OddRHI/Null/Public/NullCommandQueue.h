#pragma once

#include "NullPrefix.h"
#include "RHICommandQueue.h"

namespace Odd
{
	class NullCommandQueue : public RHICommandQueue
	{
    public:
        NullCommandQueue(RHIDevice* device, RHICommandQueueType type);

		virtual void Flush() override;
	};
}