#pragma once

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

namespace Odd
{
    void InitializeLogging();
    void ShutdownLogging();

    spdlog::logger* GetGlobalLogger();
} // namespace Odd

#ifndef ODD_DISABLE_LOGGING
    #define ODD_LOG_TRACE(...) Odd::GetGlobalLogger()->trace(__VA_ARGS__)
    #define ODD_LOG_DEBUG(...) Odd::GetGlobalLogger()->debug(__VA_ARGS__)
    #define ODD_LOG_INFO(...) Odd::GetGlobalLogger()->info(__VA_ARGS__)
    #define ODD_LOG_WARNING(...) Odd::GetGlobalLogger()->warn(__VA_ARGS__)
    #define ODD_LOG_ERROR(...) Odd::GetGlobalLogger()->error(__VA_ARGS__)
    #define ODD_LOG_CRITICAL(...) Odd::GetGlobalLogger()->critical(__VA_ARGS__)
#else
    #define ODD_LOG_TRACE(...)
    #define ODD_LOG_DEBUG(...)
    #define ODD_LOG_INFO(...)
    #define ODD_LOG_WARNING(...)
    #define ODD_LOG_ERROR(...)
    #define ODD_LOG_CRITICAL(...)
#endif