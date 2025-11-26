#pragma once

#include "RHICommandContext.h"

namespace Odd
{
    class RHIResource;

    class RHICommand_TransitionResource : public RHICommand
    {
    public:
        RHIResource* Resource;
        RHIResourceTransitionInfo TransitionInfo;

        virtual void ExecuteAndClear(RHICommandContext& cmdCtx)
        {
            cmdCtx.CMDTransitionResource(Resource, TransitionInfo);
        }
    };
}