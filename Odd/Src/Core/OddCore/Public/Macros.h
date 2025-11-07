#pragma once

// Macros for C++ casts, because default ones are really tedious to write.

#define scast(T, value) static_cast<T>(value)
#define rcast(T, value) reinterpret_cast<T>(value)
#define dcast(T, value) dynamic_cast<T>(value)
#define cncast(T, value) const_cast<T>(value)
#define c_cast(T, value) (T)(value)
