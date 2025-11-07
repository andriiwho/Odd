#pragma once

#include "OddCore.h"
#include "RootObj.h"

#include <span>

namespace Odd
{
    class Main : public RootObj
    {
    public:
        void Launch(std::span<const char* const> commandLine);
    };
} // namespace Odd