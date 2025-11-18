#include "RootObj.h"
#include "RootObjRegistry.h"

namespace Odd
{
    RootObj::RootObj()
        : m_RefCount(1)
        , m_RootObjectID(Internal::GRootObjRegistry->RegisterRootObj(this))
    {
    }

    RootObj::~RootObj()
    {
        // Ensure children are properly handled on destruction
        // At this point, children should already be marked expired by MarkChildrenExpired
        // or by the parent's Release/ForceExpire calls, but we ensure cleanup here as well
        
        // Remove from parent if we're still attached
        if (m_Parent)
        {
            m_Parent->Detach(this);
        }
        
        // Children should already be cleared by MarkChildrenExpired,
        // but we clear again as a safety measure
        m_Children.clear();
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
        MarkChildrenExpired();
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

    void RootObj::Attach(RootObj* child)
    {
        if (!child)
            return;

        // Prevent self-attachment
        if (child == this)
            return;

        // Prevent circular references: check if 'this' is a descendant of 'child'
        // If we attach 'child' to 'this', but 'this' is already a descendant of 'child',
        // we would create a cycle
        if (child->IsParentOf(this))
            return;

        // Remove child from its previous parent
        if (child->m_Parent)
        {
            child->m_Parent->Detach(child);
        }

        // Add to this object's children
        m_Children.push_back(child);
        child->m_Parent = this;

        // This is needed in order to ensure ownership.
        child->ForceRefCount(1);
    }

    void RootObj::Detach(RootObj* obj)
    {
        if (!obj)
            return;

#ifndef NDEBUG
        oddValidate(IsParentOf(obj));
#endif

        // Find and remove the child from the vector
        auto it = std::find(m_Children.begin(), m_Children.end(), obj);
        if (it != m_Children.end())
        {
            m_Children.erase(it);
            obj->m_Parent = nullptr;
        }
    }

    RootObj* RootObj::GetRoot() const
    {
        if (!m_Parent)
            return cncast(RootObj*, this);

        return m_Parent->GetRoot();
    }

    RootObj* RootObj::GetLastChild() const
    {
        if (m_Children.empty())
            return cncast(RootObj*, this);

        return m_Children.back()->GetLastChild();
    }

    bool RootObj::IsParentOf(RootObj* obj) const
    {
        if (!obj)
            return false;

        // Check immediate children
        for (RootObj* child : m_Children)
        {
            if (child == obj)
                return true;

            // Recursively check child's descendants
            if (child->IsParentOf(obj))
                return true;
        }

        return false;
    }

    void RootObj::MarkChildrenExpired()
    {
        // Mark all children expired recursively (depth-first, post-order)
        // This ensures children are expired before their parents
        for (RootObj* child : m_Children)
        {
            if (child)
            {
                // First recursively expire all descendants
                child->MarkChildrenExpired();
                // Then expire the child itself
                child->ForceExpire();
            }
        }
        
        // Clear the children vector after marking them expired
        // The actual deletion will happen in FlushExpiredRootObjects
        m_Children.clear();
    }

    extern void Internal::DeleteRootObj(void* pObj)
    {
        if (RootObj* pRootObj = scast(RootObj*, pObj))
        {
            // This function is called to manually delete a root object.
            // We force expire it, which will:
            // 1. Set ref count to 0
            // 2. Mark all children as expired recursively
            // 3. Register this object in the expired list
            pRootObj->ForceExpire();
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