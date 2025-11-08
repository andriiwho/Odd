#pragma once

#include "OddCore.h"
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
        RootObj();
        virtual ~RootObj() = default;

        RootObj(const RootObj&) = delete;
        RootObj& operator=(const RootObj&) = delete;

        RootObj(RootObj&&) noexcept = delete;
        RootObj& operator=(RootObj&&) noexcept = delete;

        size_t AddRef() const;
        size_t Release() const;
        size_t GetRefCount() const;

        inline Internal::RootObjectID GetRootObjectID() const { return m_RootObjectID; }

    private:
        mutable std::atomic_size_t m_RefCount;
        Internal::RootObjectID     m_RootObjectID;
    };

    namespace Internal
    {
        void   DeleteRootObj(void* pObj);
        size_t RootObjAddRef(void* pObj);
        size_t RootObjRelease(void* pObj);
        size_t RootObjGetRefCount(void* pObj);

        extern void InitializeRootObjectSystem();
        extern void ShutdownRootObjectSystem();
    } // namespace Internal

    template <std::derived_from<RootObj> T>
    inline T* MakeObject()
    {
        return OddNew<T>();
    }
} // namespace Odd