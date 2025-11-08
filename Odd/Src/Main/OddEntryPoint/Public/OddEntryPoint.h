#pragma once

#include "OddCore.h"

#include <span>

namespace Odd
{
    class EntryPoint
    {
    public:
        EntryPoint(std::span<const char* const> commandLineArgs);
        ~EntryPoint();

        void Main();
    };
} // namespace Odd