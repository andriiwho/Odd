#pragma once

#include "OddCore.h"
#include <d3d12.h>
#include <d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include "wrl.h"

namespace Odd::D3D12
{
    using Microsoft::WRL::ComPtr;
}

#define oddHrValidate(hr) oddValidate(!FAILED(hr))
#define oddHrAssert(hr) oddAssert(!FAILED(hr))