#pragma once

#include "RootObj.h"

#include "RHICommandQueue.h"

namespace Odd
{
    class RHIDevice : public RootObj
    {
    public:
        virtual RHICommandQueue* CreateCommandQueue(RHICommandQueueType type) = 0;
    };
} // namespace Odd