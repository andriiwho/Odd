#include "RootObj.h"
#include "RootObjRegistry.h"

namespace Odd
{
    RootObj::RootObj()
        : m_RefCount(1)
        , m_RootObjectID(Internal::GRootObjRegistry->RegisterRootObj(this))
    {
    }

    size_t RootObj::AddRef()
    {
        return m_RefCount.fetch_add(1, std::memory_order::relaxed) + 1;
    }

    size_t RootObj::Release()
    {
        const size_t newCount = m_RefCount.fetch_sub(1, std::memory_order::acq_rel) - 1;
        if (newCount == 0)
        {
            MarkChildrenExpired();
            Internal::GRootObjRegistry->MarkRootObjectExpired(m_RootObjectID);
        }
        return newCount;
    }

    void RootObj::ForceExpire()
    {
        m_RefCount.store(0, std::memory_order::release);
        Internal::GRootObjRegistry->MarkRootObjectExpired(m_RootObjectID);
    }

    size_t RootObj::GetRefCount() const
    {
        return m_RefCount.load(std::memory_order::relaxed);
    }

    size_t RootObj::ForceRefCount(size_t newCount)
    {
        m_RefCount.store(newCount, std::memory_order::relaxed);
        return GetRefCount();
    }

    bool RootObj::Expired() const
    {
        return GetRefCount() == 0;
    }

    void RootObj::Attach(RootObj* next)
    {
        if (m_Next != nullptr)
        {
            m_Next->Attach(next);
            return;
        }

        if (next)
        {
            m_Next = next;
            if (auto prev = next->m_Prev)
            {
                prev->Detach(next);
            }
            next->m_Prev = this;

            // This is needed in order to ensure ownership.
            next->ForceRefCount(1);
        }
    }

    void RootObj::Detach(RootObj* obj)
    {
        if (m_Next == nullptr)
            return;

#ifndef NDEBUG
        oddValidate(IsParentOf(obj));
#endif

        RootObj* next = obj->m_Next;
        RootObj* prev = obj->m_Prev;

        if (next)
        {
            next->m_Prev = prev;
        }
        if (prev)
        {
            prev->m_Next = next;
        }
    }

    RootObj* RootObj::GetRoot() const
    {
        if (!m_Prev)
            return cncast(RootObj*, this);

        return m_Prev->GetRoot();
    }

    RootObj* RootObj::GetLastChild() const
    {
        if (!m_Next)
            return cncast(RootObj*, this);

        return m_Next->GetLastChild();
    }

    bool RootObj::IsParentOf(RootObj* obj) const
    {
        // Get the root
        if (!m_Next)
            return false;

        RootObj* current = m_Next;
        while (true)
        {
            if (current == obj)
                return true;
            else if (current->m_Next)
            {
                current = current->m_Next;
            }
            else
                return false;
        }
        return false;
    }

    void RootObj::MarkChildrenExpired()
    {
        if (!m_Next)
            return;

        RootObj* current = GetLastChild();
        while (true)
        {
            // We mark self expired manually from the latest child up to the head.
            if (current == this)
                break;

            current->ForceExpire();

            if (current->m_Prev)
                current = current->m_Prev;
        }
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

    extern void Internal::FlushExpiredRootObjects()
    {
        GRootObjRegistry->FlushExpiredRootObjects();
    }

    extern void Internal::ShutdownRootObjectSystem()
    {
        RootObjRegistry::Shutdown();
    }

} // namespace Odd