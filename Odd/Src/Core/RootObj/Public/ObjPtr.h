#pragma once

#include "OddCore.h"

namespace Odd
{
    namespace Internal
    {
        extern size_t RootObjAddRef(void* pObj);
        extern size_t RootObjRelease(void* pObj);
        extern size_t RootObjGetRefCount(void* pObj);
    } // namespace Internal

    // To allow forward referencing we should not use RootObj class here at all.
    template <class T>
    class ObjPtr
    {
    public:
        ObjPtr() = default;
        ObjPtr(TypeOfNull)
            : m_Obj(nullptr) {}
        ~ObjPtr()
        {
            Release();
        }

        // Assign a pointer. Does not increase ref count.
        explicit ObjPtr(T* pObj)
            : m_Obj(pObj)
        {
        }

        // Copy the underlying pointer of other to this. Increases the ref count.
        ObjPtr(const ObjPtr<T>& other)
            : m_Obj(other.Get())
        {
            AddRef();
        }

        ObjPtr& operator=(const ObjPtr<T>& other)
        {
            if (this != &other)
            {
                if (m_Obj)
                    Release();

                m_Obj = other.Get();
                AddRef();
            }
            return *this;
        }

        template <typename U>
        ObjPtr(const ObjPtr<U>& other)
            : m_Obj(other.Get())
        {
            AddRef();
        }

        template <typename U>
        ObjPtr& operator=(const ObjPtr<U>& other)
        {
            if (m_Obj != other.Get())
            {
                if (m_Obj)
                    Release();

                m_Obj = other.Get();
                AddRef();
            }
            return *this;
        }

        ObjPtr(ObjPtr<T>&& other) noexcept
            : m_Obj(other.GetAndDetach())
        {
        }

        ObjPtr& operator=(ObjPtr<T>&& other) noexcept
        {
            if (this != &other)
            {
                if (m_Obj)
                    Release();

                m_Obj = other.GetAndDetach();
            }
            return *this;
        }

        template <typename U>
        ObjPtr(ObjPtr<U>&& other) noexcept
            : m_Obj(other.GetAndDetach())
        {
        }

        template <typename U>
        ObjPtr& operator=(ObjPtr<U>&& other) noexcept
        {
            if (m_Obj != other.Get())
            {
                if (m_Obj)
                    Release();

                m_Obj = other.GetAndDetach();
            }
            return *this;
        }

        [[nodiscard]] T* Get() const noexcept { return m_Obj; }

        size_t AddRef() const
        {
            if (!m_Obj)
                return 0;
            return Internal::RootObjAddRef(m_Obj);
        }

        size_t Release() const
        {
            if (!m_Obj)
                return 0;
            return Internal::RootObjRelease(m_Obj);
        }

        [[nodiscard]] size_t GetRefCount() const noexcept
        {
            if (!m_Obj)
                return 0;
            return Internal::RootObjGetRefCount(m_Obj);
        }

        // Sets underlying object to nullptr. Returns old object.
        [[nodiscard]] T* GetAndDetach() noexcept
        {
            T* pOld = m_Obj;
            m_Obj = nullptr;
            return pOld;
        }

        // Attach a pointer without increasing ref count (takes ownership)
        void Attach(T* pObj) noexcept
        {
            if (m_Obj)
                Release();
            m_Obj = pObj;
        }

        // Release current object and optionally assign a new one (increases ref count if not null)
        void Reset(T* pObj = nullptr)
        {
            if (m_Obj)
                Release();

            m_Obj = pObj;
            if (m_Obj)
                AddRef();
        }

        // Swap with another ObjPtr
        void Swap(ObjPtr<T>& other) noexcept
        {
            T* temp = m_Obj;
            m_Obj = other.m_Obj;
            other.m_Obj = temp;
        }

        T* operator->() const noexcept
        {
            return m_Obj;
        }

        T& operator*() noexcept
        {
            return *m_Obj;
        }

        const T& operator*() const noexcept
        {
            return *m_Obj;
        }

        explicit operator bool() const noexcept
        {
            return m_Obj != nullptr;
        }

        // Comparison operators
        bool operator==(const ObjPtr<T>& other) const noexcept
        {
            return m_Obj == other.m_Obj;
        }

        bool operator!=(const ObjPtr<T>& other) const noexcept
        {
            return m_Obj != other.m_Obj;
        }

        bool operator==(const T* pObj) const noexcept
        {
            return m_Obj == pObj;
        }

        bool operator!=(const T* pObj) const noexcept
        {
            return m_Obj != pObj;
        }

        bool operator==(TypeOfNull) const noexcept
        {
            return m_Obj == nullptr;
        }

        bool operator!=(TypeOfNull) const noexcept
        {
            return m_Obj != nullptr;
        }

        // Spaceship operator for C++20/23
        auto operator<=>(const ObjPtr<T>& other) const noexcept
        {
            return m_Obj <=> other.m_Obj;
        }

        auto operator<=>(const T* pObj) const noexcept
        {
            return m_Obj <=> pObj;
        }

        // Friend declarations for cross-template access
        template <typename U>
        friend class ObjPtr;

    private:
        T* m_Obj{ nullptr };
    };

    // Non-member swap for ADL
    template <class T>
    void swap(ObjPtr<T>& lhs, ObjPtr<T>& rhs) noexcept
    {
        lhs.Swap(rhs);
    }
} // namespace Odd