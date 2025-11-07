#include "Main.h"
#include "LuaRuntime.h"
#include "Logging.h"

namespace Odd
{
    // Globals
    static LuaRuntime* GLuaRuntime;

    static void CoreSetup()
    {
        // Initialize Logging
        InitializeLogging();
        ODD_LOG_INFO("Logging initialized.");


        // Initialize LuaJIT
        GLuaRuntime = MakeObject<LuaRuntime>();
        oddValidate(GLuaRuntime != nullptr);
        ODD_LOG_INFO("Lua Runtime initialized.");

        ODD_LOG_INFO("Core initialized.");
    }

    static void CoreShutdown()
    {
        ODD_LOG_INFO("Starting core shutdown...");
        if (GLuaRuntime)
        {
            GLuaRuntime->Release();
            GLuaRuntime = nullptr;
            ODD_LOG_INFO("Lua runtime shut down.");
        }

        ODD_LOG_INFO("Shutting down logging. This is the last message from me. Bye.");
        ShutdownLogging();
    }

    void Main::Launch(std::span<const char* const> commandLine)
    {
        CoreSetup();

        CoreShutdown();
    }
} // namespace Odd