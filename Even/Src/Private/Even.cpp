#include "OddCore.h"
#include "Main/Main.h"
#include "ObjPtr.h"

int main(int argc, char const* const* const argv)
{
    using namespace Odd;
    oddValidate(InitializeMemoryPool());
    {
        ObjPtr mainObj{ MakeObject<Main>() };
        oddValidate(mainObj != nullptr);
        mainObj->Launch(std::span<const char* const>(argv, argc));
    }
    ShutdownMemoryPool();
    return 0;
}