#pragma once

#include "OddCore.h"
#include <d3d12.h>
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include "wrl.h"

#include "RHITypes.h"

namespace Odd::D3D12
{
    using Microsoft::WRL::ComPtr;

    inline D3D12_RESOURCE_STATES ToD3D12ResouceStates(RHIResourceState state)
    {
        switch (state)
        {
            case RHIResourceState::Common:
                return D3D12_RESOURCE_STATE_COMMON;
            case RHIResourceState::VertexOrConstantBuffer:
                return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            case RHIResourceState::IndexBuffer:
                return D3D12_RESOURCE_STATE_INDEX_BUFFER;
            case RHIResourceState::RenderTarget:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case RHIResourceState::UnorderedAccess:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case RHIResourceState::DepthWrite:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case RHIResourceState::DepthRead:
                return D3D12_RESOURCE_STATE_DEPTH_READ;
            case RHIResourceState::NonPixelShaderResource:
                return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            case RHIResourceState::PixelShaderResource:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            case RHIResourceState::StreamOut:
                return D3D12_RESOURCE_STATE_STREAM_OUT;
            case RHIResourceState::IndirectArgument:
                return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
            case RHIResourceState::CopyDest:
                return D3D12_RESOURCE_STATE_COPY_DEST;
            case RHIResourceState::CopySource:
                return D3D12_RESOURCE_STATE_COPY_SOURCE;
            case RHIResourceState::ResolveDest:
                return D3D12_RESOURCE_STATE_RESOLVE_DEST;
            case RHIResourceState::ResolveSource:
                return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
            case RHIResourceState::RaytracingAccelerationStructure:
                return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
            case RHIResourceState::ShadingRateSource:
                return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
            case RHIResourceState::GenericRead:
                return D3D12_RESOURCE_STATE_GENERIC_READ;
            case RHIResourceState::AllShaderResource:
                return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            case RHIResourceState::Present:
                return D3D12_RESOURCE_STATE_PRESENT;
            case RHIResourceState::Predication:
                return D3D12_RESOURCE_STATE_PREDICATION;
            case RHIResourceState::VideoDecodeRead:
                return D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
            case RHIResourceState::VideoDecodeWrite:
                return D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
            case RHIResourceState::VideoProcessRead:
                return D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
            case RHIResourceState::VideoProcessWrite:
                return D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
            case RHIResourceState::VideoEncodeRead:
                return D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
            case RHIResourceState::VideoEncodeWrite:
                return D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
            default:
                break;
        }
        return D3D12_RESOURCE_STATE_COMMON;
    }
} // namespace Odd::D3D12

#define oddHrValidate(hr) oddValidate(!FAILED(hr))
#define oddHrAssert(hr) oddAssert(!FAILED(hr))