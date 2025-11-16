#pragma once

#include "OddCore.h"
#include "RootObjMacros.h"
#include <concepts>

#include <atomic>
#include <new>
#include <memory>
#include <cassert>

namespace Odd
{
    namespace Internal
    {
        enum class RootObjectID : uint64_t;
    }

    class RootObj
    {
    public:
        ODD_ROOT_OBJECT(RootObj)

        RootObj();
        virtual ~RootObj() = default;

        RootObj(const RootObj&) = delete;
        RootObj& operator=(const RootObj&) = delete;

        RootObj(RootObj&&) noexcept = delete;
        RootObj& operator=(RootObj&&) noexcept = delete;

        size_t AddRef();
        size_t Release();
        void   ForceExpire();
        size_t GetRefCount() const;
        size_t ForceRefCount(size_t newCount);

        /**
         * Check if the object is expired (i.e., ref count is zero).
         * This is a lightweight check but it doesn't guarantee that the object is still valid.
         * It's also unsafe to call after flushing the expired root objects.
         * If you want more safe way of checking if the object is valid @see WeakObjPtr<T>
         */
        [[deprecated("Use WeakObjPtr for checking expiration semantics")]]
        bool Expired() const;

        inline Internal::RootObjectID GetRootObjectID() const { return m_RootObjectID; }

        template <typename T, typename... Args>
        T*       CreateChildObject(Args&&... args);
        void     Attach(RootObj* next);
        void     Detach(RootObj* obj);

        RootObj* GetRoot() const;
        RootObj* GetLastChild() const;

        bool     IsParentOf(RootObj* obj) const;

    private:
        void MarkChildrenExpired();

        mutable std::atomic_size_t m_RefCount;
        Internal::RootObjectID     m_RootObjectID;

        // Child/Parent relationship system.
        RootObj* m_Next{};
        RootObj* m_Prev{};
    };

    namespace Internal
    {
        void   DeleteRootObj(void* pObj);
        size_t RootObjAddRef(void* pObj);
        size_t RootObjRelease(void* pObj);
        size_t RootObjGetRefCount(void* pObj);

        extern void InitializeRootObjectSystem();
        extern void FlushExpiredRootObjects();
        extern void ShutdownRootObjectSystem();
    } // namespace Internal

    template <std::derived_from<RootObj> T, typename... Args>
    inline T* MakeObject(Args&&... inArgs)
    {
        return OddNew<T>(std::forward<Args>(inArgs)...);
    }

    template <typename T, typename... Args>
    T* RootObj::CreateChildObject(Args&&... args)
    {
        static_assert(std::is_base_of_v<RootObj, T>);

        if (m_Next != nullptr)
        {
            return m_Next->CreateChildObject<T>(std::forward<Args>(args)...);
        }

        T* outChild = MakeObject<T>(std::forward<Args>(args)...);
        if (outChild != nullptr)
        {
            outChild->m_Prev = this;
            m_Next = outChild;
        }
        return outChild;
    }
} // namespace Odd