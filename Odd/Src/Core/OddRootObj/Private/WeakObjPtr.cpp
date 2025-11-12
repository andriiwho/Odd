#include "WeakObjPtr.h"
#include "RootObj.h"
#include "RootObjRegistry.h"

namespace Odd
{

    WeakObjPtrBase::WeakObjPtrBase(RootObj* ptr)
        : m_Id(ptr ? ptr->GetRootObjectID() : Internal::RootObjectID::Invalid)
    {
    }

    WeakObjPtrBase::WeakObjPtrBase(const ObjPtr<RootObj>& ptr)
        : WeakObjPtrBase(ptr.Get())
    {
    }

    RootObj* WeakObjPtrBase::Get() const
    {
        return Internal::GRootObjRegistry->TryGet(m_Id);
    }

    bool WeakObjPtrBase::IsValid() const
    {
        return Get();
    }

}