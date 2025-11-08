#include "OddPlatformCore.h"
#include "OddWaysToCrash.h"

namespace Odd
{
    IPlatform* GPlatform = nullptr;

    IPlatform::IPlatform()
    {
    }

    IPlatform::~IPlatform()
    {
    }

    namespace Internal
    {
        extern IPlatform* InitializePlatform_Internal();
        extern void       ShutdownPlatform_Internal(IPlatform* platform);
    } // namespace Internal

    void InitializePlatform()
    {
        GPlatform = Internal::InitializePlatform_Internal();
    }

    void ShutdownPlatform()
    {
        Internal::ShutdownPlatform_Internal(GPlatform);
        GPlatform = nullptr;
    }

} // namespace Odd