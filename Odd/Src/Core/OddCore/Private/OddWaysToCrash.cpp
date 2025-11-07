#include "OddWaysToCrash.h"

#include <cstdio>
#include <cstdlib>

namespace Odd
{
    [[noreturn]] void ReportAssertionFailure(
        const char* condition,
        const char* file,
        int         line,
        const char* message)
    {
        if (message)
        {
            std::fprintf(stderr, "\n=== ASSERTION FAILED ===\n");
            std::fprintf(stderr, "Condition: %s\n", condition);
            std::fprintf(stderr, "File: %s\n", file);
            std::fprintf(stderr, "Line: %d\n", line);
            std::fprintf(stderr, "Message: %s\n", message);
            std::fprintf(stderr, "========================\n\n");
        }
        else
        {
            std::fprintf(stderr, "\n=== ASSERTION FAILED ===\n");
            std::fprintf(stderr, "Condition: %s\n", condition);
            std::fprintf(stderr, "File: %s\n", file);
            std::fprintf(stderr, "Line: %d\n", line);
            std::fprintf(stderr, "========================\n\n");
        }

        std::fflush(stderr);

        // Trigger a crash
        std::abort();
    }
} // namespace Odd
