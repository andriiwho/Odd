#include "OddEntryPoint.h"

#include "PlatformMacros.h"

#if ODD_PLATFORM_WINDOWS
extern "C" 
{
    __declspec(dllexport) extern const uint32_t D3D12SDKVersion = 618;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}
#endif

int main(int argc, char const* const* const argv)
{
    using namespace Odd;
    
    EntryPoint entryPoint(ContainerHelpers::MakeSpan(argv, argc));
    entryPoint.Main();

    return 0;
}