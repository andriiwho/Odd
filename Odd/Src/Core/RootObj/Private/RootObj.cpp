#include "RootObj.h"
#include "RootObjRegistry.h"

namespace Odd
{
    RootObj::RootObj()
        : m_RefCount(1)
        , m_RootObjectID(Internal::GRootObjRegistry->RegisterRootObj(this))
    {
    }

    size_t RootObj::AddRef() const
    {
        return m_RefCount.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    size_t RootObj::Release() const
    {
        const size_t newCount = m_RefCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (newCount == 0)
        {
            Internal::GRootObjRegistry->MarkRootObjectExpired(m_RootObjectID);
        }
        return newCount;
    }

    size_t RootObj::GetRefCount() const
    {
        return m_RefCount.load(std::memory_order_relaxed);
    }

    extern void Internal::DeleteRootObj(void* pObj)
    {
        if (RootObj* pRootObj = scast(RootObj*, pObj))
        {
            // For now just use delete
            // OddDelete(pRootObj);
            Internal::GRootObjRegistry->MarkRootObjectExpired(pRootObj->GetRootObjectID());
        }
    }

    size_t Internal::RootObjAddRef(void* pObj)
    {
        if (RootObj* pRootObj = scast(RootObj*, pObj))
        {
            return pRootObj->AddRef();
        }

        return 0;
    }

    size_t Internal::RootObjRelease(void* pObj)
    {
        if (RootObj* pRootObj = scast(RootObj*, pObj))
        {
            return pRootObj->Release();
        }

        return 0;
    }

    size_t Internal::RootObjGetRefCount(void* pObj)
    {
        if (RootObj* pRootObj = scast(RootObj*, pObj))
        {
            return pRootObj->GetRefCount();
        }

        return 0;
    }

    extern void Internal::InitializeRootObjectSystem()
    {
        RootObjRegistry::Init();
    }

    extern void Internal::ShutdownRootObjectSystem()
    {
        RootObjRegistry::Shutdown();
    }

} // namespace Odd