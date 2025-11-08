#include "OddEntryPoint.h"

int main(int argc, char const* const* const argv)
{
    using namespace Odd;
    
    EntryPoint entryPoint(ContainerHelpers::MakeSpan(argv, argc));
    entryPoint.Main();

    return 0;
}