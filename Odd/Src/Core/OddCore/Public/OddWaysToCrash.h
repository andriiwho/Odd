#pragma once

#include <format>
#include <source_location>

namespace Odd
{
    // Internal function to handle assertion failures
    [[noreturn]] void ReportAssertionFailure(
        const char* condition,
        const char* file,
        int         line,
        const char* message = nullptr);
} // namespace Odd

// oddValidate: Always crashes the program when condition is false, even in release builds
#define oddValidate(condition)                                             \
    do                                                                     \
    {                                                                      \
        if (!(condition)) [[unlikely]]                                     \
        {                                                                  \
            ::Odd::ReportAssertionFailure(#condition, __FILE__, __LINE__); \
        }                                                                  \
    }                                                                      \
    while (false)

// oddValidateMsg: Always crashes with a formatted message when condition is false
#define oddValidateMsg(condition, fmt, ...)                                                 \
    do                                                                                      \
    {                                                                                       \
        if (!(condition)) [[unlikely]]                                                      \
        {                                                                                   \
            auto message = std::format(fmt, ##__VA_ARGS__);                                 \
            ::Odd::ReportAssertionFailure(#condition, __FILE__, __LINE__, message.c_str()); \
        }                                                                                   \
    }                                                                                       \
    while (false)

// oddAssert: Crashes only in debug builds when condition is false
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
    #define oddAssert(condition) oddValidate(condition)
    #define oddAssertMsg(condition, fmt, ...) oddValidateMsg(condition, fmt, ##__VA_ARGS__)
#else
    #define oddAssert(condition) ((void)0)
    #define oddAssertMsg(condition, fmt, ...) ((void)0)
#endif
