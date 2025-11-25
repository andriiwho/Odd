#pragma once

#include <cstdint>
#include <cstddef>

#include "OddMalloc.h"
#include "Macros.h"

#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <map>
#include <list>

namespace Odd
{
    using float32_t = float;
    using float64_t = double;

    using TypeOfNull = decltype(nullptr);

    template <typename T, typename Allocator = DefaultSTDAllocator<T>>
    using Vector = std::vector<T, Allocator>;

    template <typename CharType, typename CharTraits = std::char_traits<CharType>, typename Allocator = DefaultSTDAllocator<CharType>>
    using BasicString = std::basic_string<CharType, CharTraits, Allocator>;

    using String = BasicString<char>;
    using WideString = BasicString<wchar_t>;
    inline String ToString(const std::string& str)
    {
        String outString;
        outString.assign_range(str);
        return outString;
    }

    inline WideString StringToWide(const String& str)
    {
        wchar_t* buffer = OddCalloc<wchar_t>(str.size());
        mbstowcs(buffer, str.c_str(), str.size());
        WideString out(buffer);
        OddFree(buffer);
        return out;
    }

    using StringView = std::string_view;

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
        return std::allocate_shared<T>(DefaultSTDAllocator<T>{}, std::forward<Args>(args)...);
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