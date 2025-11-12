#pragma once

#include "Types.h"
#include "ObjPtr.h"

namespace Odd
{
    namespace Internal
    {
        enum class RootObjectID : uint64_t;
    }

    class RootObj;

    class WeakObjPtrBase
    {
    public:
        WeakObjPtrBase() = default;
        WeakObjPtrBase(const ObjPtr<RootObj>& ptr);
        WeakObjPtrBase(RootObj* ptr);

        WeakObjPtrBase(const WeakObjPtrBase&) = default;
        WeakObjPtrBase& operator=(const WeakObjPtrBase&) = default;

        WeakObjPtrBase(WeakObjPtrBase&&) noexcept = default;
        WeakObjPtrBase& operator=(WeakObjPtrBase&&) noexcept = default;

        RootObj* Get() const;
        bool     IsValid() const;

        inline RootObj* operator->() const { return Get(); }

    private:
        Internal::RootObjectID m_Id{};
    };

    template <typename T>
    class WeakObjPtr : public WeakObjPtrBase
    {
    public:
        using WeakObjPtrBase::WeakObjPtrBase;

        WeakObjPtr(T* value)
            : WeakObjPtrBase(value)
        {
        }

        WeakObjPtr(const ObjPtr<T>& value)
            : WeakObjPtr(value.Get())
        {
        }

        inline T* Get() const
        {
            return scast(T*, WeakObjPtrBase::Get());
        }

        inline T* operator->() const
        {
            return scast(T*, WeakObjPtrBase::operator->());
        }
    };
} // namespace Odd