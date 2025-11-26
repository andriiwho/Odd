#pragma once

#include "OddCore.h"

namespace Odd
{
    // Lots of these directly correspond to D3D12_RESOURCE_STATE.
    // Though these are also suitable for vulkan backend.
    // TODO(andriiwho): Revisit if needs any changes
    enum class RHIResourceState
    {
        Common,
        VertexOrConstantBuffer,
        IndexBuffer,
        RenderTarget,
        UnorderedAccess,
        DepthWrite,
        DepthRead,
        NonPixelShaderResource,
        PixelShaderResource,
        StreamOut,
        IndirectArgument,
        CopyDest,
        CopySource,
        ResolveDest,
        ResolveSource,
        RaytracingAccelerationStructure,
        ShadingRateSource,
        GenericRead,
        AllShaderResource,
        Present,
        Predication,
        VideoDecodeRead,
        VideoDecodeWrite,
        VideoProcessRead,
        VideoProcessWrite,
        VideoEncodeRead,
        VideoEncodeWrite
    };

    struct RHIResourceTransitionInfo
    {
        RHIResourceState StateFrom;
        RHIResourceState StateTo;
    };
} // namespace Odd