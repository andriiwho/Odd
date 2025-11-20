#pragma once

#include "OddD3D12.h"
#include "RHICommandQueue.h"

namespace Odd::D3D12
{
    inline constexpr D3D12_COMMAND_LIST_TYPE ToD3D12CommandQueueType(RHICommandQueueType type)
    {
        switch (type)
        {
            case RHICommandQueueType::Graphics:
                return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case RHICommandQueueType::Compute:
                return D3D12_COMMAND_LIST_TYPE_COMPUTE;
            case RHICommandQueueType::Transfer:
                return D3D12_COMMAND_LIST_TYPE_COPY;
            default:
                break;
        }

        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
} // namespace Odd::D3D12