#include "Logging.h"
#include "OddMalloc.h"
#include "OddWaysToCrash.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _MSC_VER
    #include <spdlog/sinks/msvc_sink.h>
    #define LOGGING_USE_MSVC_SINK
#endif

namespace Odd
{
    static spdlog::logger* GLogger;

    void InitializeLogging()
    {
        oddValidateMsg(GLogger == nullptr, "Another instance of GLogger already exists.");
        GLogger = OddNew<spdlog::logger>("Odd");
        oddValidate(GLogger != nullptr);

        auto configureSinkAndPush = [](spdlog::sink_ptr sink) {
            sink->set_level(spdlog::level::trace);
            sink->set_pattern("[%H:%M:%S] [%^%l%$] %v");
            GLogger->sinks().push_back(sink);
        };

        // We don't use our own allocator here, though it's probably ok for now.
        configureSinkAndPush(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

#ifdef LOGGING_USE_MSVC_SINK
        configureSinkAndPush(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

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