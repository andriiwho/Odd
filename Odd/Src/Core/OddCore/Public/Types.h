#pragma once

#include <cstdint>
#include <cstddef>

#include "OddMalloc.h"
#include "Macros.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <list>

namespace Odd
{
    using TypeOfNull = decltype(nullptr);

    template <typename T, typename Allocator = DefaultSTDAllocator<T>>
    using Vector = std::vector<T, Allocator>;

    template <typename CharType, typename CharTraits = std::char_traits<CharType>, typename Allocator = DefaultSTDAllocator<CharType>>
    using String = std::basic_string<CharType, CharTraits, Allocator>;

    template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename Allocator = DefaultSTDAllocator<std::pair<const Key, T>>>
    using HashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

    template <typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = DefaultSTDAllocator<std::pair<const Key, T>>>
    using SortedHashMap = std::map<Key, T, Compare, Allocator>;

    template <typename T, typename Allocator = DefaultSTDAllocator<T>>
    using LinkedList = std::list<T, Allocator>;

    template <typename T>
    using SharedPtr = std::shared_ptr<T>;

    template <typename T>
    struct DefaultSTDDeleter
    {
        void operator()(T* ptr) const
        {
            OddDelete(ptr);
        }
    };

    template <typename T, typename Deleter = DefaultSTDDeleter<T>>
    using UniquePtr = std::unique_ptr<T, Deleter>;
} // namespace Odd

// Functions
namespace Odd
{
    template <typename T, typename... Args>
    SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::allocate_shared<T>(DefaultSTDAllocator{}, std::forward<Args>(args)...);
    }

    template <typename T, typename Allocator = DefaultSTDAllocator<T>, typename... Args>
    SharedPtr<T> MakeSharedWithAllocator(const Allocator& allocator, Args&&... args)
    {
        return std::allocate_shared<T>(allocator, std::forward<Args>(args)...);
    }

    template <typename T, typename Allocator = DefaultSTDAllocator<T>, typename... Args>
    SharedPtr<T> MakeSharedWithAllocator(Allocator&& allocator, Args&&... args)
    {
        return std::allocate_shared<T>(allocator, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    UniquePtr<T> MakeUnique(Args&&... args)
    {
        ODD_BEGIN_CRASH_ON_EXCEPTIONS

        void* pMemory = OddMalloc(sizeof(T));
        if (!pMemory)
        {
            throw std::bad_alloc();
        }
        T* pObj = new (pMemory) T(std::forward<Args>(args)...);
        return UniquePtr<T>(pObj);

        ODD_END_CRASH_ON_EXCEPTIONS
    }
} // namespace Odd