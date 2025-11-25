#pragma once

#include "NullPrefix.h"
#include "RHICommandContext.h"

namespace Odd
{
	class NullCommandContext : public RHICommandContext
	{
    public:
        NullCommandContext(RHIDevice* device, RHICommandQueueType queueType = RHICommandQueueType::Graphics);

		virtual void BeginRecording() override;
        virtual void EndRecording() override;
        virtual void Submit(RHICommandQueue& commandQueue) override;
	};
}