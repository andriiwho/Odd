#include "Logging.h"
#include "OddMalloc.h"
#include "OddWaysToCrash.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Odd
{
    static spdlog::logger* GLogger;

    void InitializeLogging()
    {
        oddValidateMsg(GLogger == nullptr, "Another instance of GLogger already exists.");
        GLogger = OddNew<spdlog::logger>("Odd");
        oddValidate(GLogger != nullptr);

        // We don't use our own allocator here, though it's probably ok for now.
        auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sink->set_level(spdlog::level::trace);
        sink->set_pattern("[%H:%M:%S] [%^%l%$] %v");
        GLogger->sinks().push_back(sink);

        GLogger->set_level(spdlog::level::trace);
        GLogger->set_pattern("[%H:%M:%S] (%n) %^[%l]%$: %v");
        GLogger->info("Logging initialized.");
    }

    void ShutdownLogging()
    {
        GLogger->info("Shutting down logging.");
        if (GLogger)
            OddDelete(GLogger);

        GLogger = nullptr;
    }

    spdlog::logger* GetGlobalLogger()
    {
        oddValidateMsg(GLogger, "Did you call Initialize logging?");
        return GLogger;
    }

} // namespace Odd