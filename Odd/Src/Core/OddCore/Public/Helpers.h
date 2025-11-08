#pragma once

#include <span>
#include "Macros.h"

namespace Odd
{
	namespace ContainerHelpers
	{
		template<typename T>
		inline constexpr std::span<T> MakeSpan(T* ptr, size_t count)
		{
			return std::span<T>(ptr, count);
        }
	}
}