#pragma once

#include <stdexcept>

#include "OddWaysToCrash.h"

// Macros for C++ casts, because default ones are really tedious to write.

#define scast(T, value) static_cast<T>(value)
#define rcast(T, value) reinterpret_cast<T>(value)
#define dcast(T, value) dynamic_cast<T>(value)
#define cncast(T, value) const_cast<T>(value)
#define c_cast(T, value) (T)(value)

#define ODD_BEGIN_CRASH_ON_EXCEPTIONS \
    try                               \
    {

#define ODD_END_CRASH_ON_EXCEPTIONS                              \
    }                                                            \
    catch (const std::exception& e)                              \
    {                                                            \
        oddValidateMsg(false, "Exception caught: {}", e.what()); \
    }

#define ODD_NO_COPY_CONSTRUCTORS(clsType) \
    clsType(const clsType&) = delete;     \
    clsType& operator=(const clsType&) = delete

#define ODD_NO_MOVE_CONSTRUCTORS(clsType) \
    clsType(clsType&&) noexcept = delete; \
    clsType& operator=(clsType&&) noexcept = delete

#define ODD_NO_MOVE_COPY_CONSTRUCTORS(clsType) \
    ODD_NO_COPY_CONSTRUCTORS(clsType);         \
    ODD_NO_MOVE_CONSTRUCTORS(clsType)