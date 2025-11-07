#include "OddCore.h"
#include "RootObj.h"

class SomeObject : public Odd::RootObj
{
public:
    int32_t X;
};

int main()
{
    bool success = Odd::InitializeMemoryPool();
    assert(success);

    SomeObject* pObj = Odd::MakeObject<SomeObject>();
    pObj->X = 15;

	pObj->Release();
	return 0;
}