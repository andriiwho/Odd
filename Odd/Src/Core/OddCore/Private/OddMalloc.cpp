#include "OddMalloc.h"
#include "Macros.h"
#include "OddWaysToCrash.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>
#include <algorithm>
#include <cstdio>

namespace Odd
{
    // ============================================================================
    // Magic Numbers for Validation
    constexpr uint32_t ALLOCATION_MAGIC = 0xDEADBEEF;
    constexpr uint32_t LARGE_ALLOC_MAGIC = 0xCAFEBABE;

    // ============================================================================
    // Global Memory Pool Instance
    static MemoryPool* GGlobalMemoryPool = nullptr;

    // Helper Functions
    // ============================================================================
    static inline size_t RoundUpPowerOf2(size_t value)
    {
        if (value == 0)
            return 1;

        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value |= value >> 32;
        value++;

        return value;
    }

    // ============================================================================
    // Get the log2 of a power of 2 value
    static inline uint8_t Log2(size_t value)
    {
        uint8_t result = 0;
        while (value >>= 1)
        {
            result++;
        }
        return result;
    }

    // ============================================================================
    static inline size_t AlignSize(size_t size, size_t alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    // Check if a pointer is aligned
    static inline bool IsAligned(void* ptr, size_t alignment)
    {
        return (rcast(uintptr_t, ptr) & (alignment - 1)) == 0;
    }

    // MemoryPool Implementation
    MemoryPool::MemoryPool()
        : m_LargeAllocations(nullptr)
        , m_TotalAllocated(0)
        , m_TotalUsed(0)
        , m_TotalFreed(0)
        , m_PeakMemoryUsage(0)
        , m_NumPages(0)
        , m_NumLargeAllocations(0)
        , m_TotalAllocationCount(0)
        , m_TotalFreeCount(0)
        , m_LastError(MemoryError::None)
        , m_IsInitialized(false)
    {
        m_SizeClassLists.fill(nullptr);
        m_FreePageLists.fill(nullptr);
        m_AllocationsByClass.fill(0);
        m_FreesByClass.fill(0);
    }

    MemoryPool::~MemoryPool()
    {
        if (m_IsInitialized)
        {
            Shutdown();
        }
    }

    bool MemoryPool::Initialize(const MemoryPoolConfig& config)
    {
        if (m_IsInitialized)
        {
            SetError(MemoryError::AlreadyInitialized, "Memory pool already initialized");
            return false;
        }

        // Store configuration
        m_Config = config;

        // Initialize all lists
        m_SizeClassLists.fill(nullptr);
        m_FreePageLists.fill(nullptr);
        m_LargeAllocations = nullptr;
        m_LargeAllocationSet.clear();

        // Reset statistics
        m_TotalAllocated = 0;
        m_TotalUsed = 0;
        m_TotalFreed = 0;
        m_PeakMemoryUsage = 0;
        m_NumPages = 0;
        m_NumLargeAllocations = 0;
        m_AllocationsByClass.fill(0);
        m_FreesByClass.fill(0);
        m_TotalAllocationCount = 0;
        m_TotalFreeCount = 0;

        m_LastError = MemoryError::None;
        m_IsInitialized = true;

        return true;
    }

    void MemoryPool::Shutdown()
    {
        if (!m_IsInitialized)
        {
            return;
        }

        // Lock all mutexes if thread safety is enabled
        if (m_Config.EnableThreadSafety)
        {
            for (auto& mutex : m_SizeClassMutexes)
            {
                mutex.lock();
            }
            m_LargeMutex.lock();
        }

        // Free all large allocations
        LargeAllocationHeader* large = m_LargeAllocations;
        while (large)
        {
            LargeAllocationHeader* next = large->Next;
            std::free(large);
            large = next;
        }

        // Free all active pages
        for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
        {
            PageMetadata* page = m_SizeClassLists[i];
            while (page)
            {
                PageMetadata* next = page->NextPage;
                std::free(page); // Free the entire page including metadata header
                page = next;
            }
        }

        // Free all pages in free lists
        for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
        {
            PageMetadata* page = m_FreePageLists[i];
            while (page)
            {
                PageMetadata* next = page->NextFreePage;
                std::free(page);
                page = next;
            }
        }

        m_SizeClassLists.fill(nullptr);
        m_FreePageLists.fill(nullptr);
        m_LargeAllocations = nullptr;
        m_LargeAllocationSet.clear();

        m_TotalAllocated = 0;
        m_TotalUsed = 0;
        m_TotalFreed = 0;
        m_PeakMemoryUsage = 0;
        m_NumPages = 0;
        m_NumLargeAllocations = 0;

        m_IsInitialized = false;

        // Unlock all mutexes if thread safety is enabled
        if (m_Config.EnableThreadSafety)
        {
            m_LargeMutex.unlock();
            for (auto& mutex : m_SizeClassMutexes)
            {
                mutex.unlock();
            }
        }
    }

    void* MemoryPool::Allocate(size_t size)
    {
        if (!m_IsInitialized)
        {
            SetError(MemoryError::NotInitialized, "Memory pool not initialized");
            return nullptr;
        }

        if (size == 0)
        {
            SetError(MemoryError::InvalidSize, "Cannot allocate zero bytes");
            return nullptr;
        }

        // Check max memory limit
        if (m_Config.MaxMemoryBytes > 0 && m_TotalAllocated + size > m_Config.MaxMemoryBytes)
        {
            SetError(MemoryError::MaxMemoryExceeded, "Max memory limit exceeded");
            return nullptr;
        }

        // Align size to minimum allocation size
        size = (size < MIN_ALLOCATION_SIZE) ? MIN_ALLOCATION_SIZE : size;

        void* ptr = nullptr;

        if (size <= MAX_SMALL_SIZE)
        {
            ptr = AllocateSmall(size);
        }
        else if (size <= MAX_MEDIUM_SIZE)
        {
            ptr = AllocateMedium(size);
        }
        else
        {
            ptr = AllocateLarge(size);
        }

        if (ptr)
        {
            if (m_Config.EnableThreadSafety)
            {
                std::lock_guard<std::mutex> lock(m_StatsMutex);
                m_TotalAllocated += size;
                m_TotalAllocationCount++;
                UpdatePeakMemory();
            }
            else
            {
                m_TotalAllocated += size;
                m_TotalAllocationCount++;
                UpdatePeakMemory();
            }
            m_LastError = MemoryError::None;
        }
        else
        {
            SetError(MemoryError::OutOfMemory, "Allocation failed");
        }

        return ptr;
    }

    void MemoryPool::Free(void* ptr)
    {
        if (!ptr)
        {
            return;
        }

        if (!m_IsInitialized)
        {
            SetError(MemoryError::NotInitialized, "Memory pool not initialized");
            return;
        }

        // Check if this is a large allocation (O(1) lookup now!)
        if (IsLargeAllocation(ptr))
        {
            FreeLarge(ptr);
            if (m_Config.EnableThreadSafety)
            {
                std::lock_guard<std::mutex> lock(m_StatsMutex);
                m_TotalFreeCount++;
            }
            else
            {
                m_TotalFreeCount++;
            }
            return;
        }

        // Small or medium allocation - get header
        AllocationHeader* header = rcast(AllocationHeader*, rcast(uint8_t*, ptr) - sizeof(AllocationHeader));

        // Validate magic number
        if (header->Magic != ALLOCATION_MAGIC)
        {
            SetError(MemoryError::CorruptedHeader, "Corrupted allocation header");
            return;
        }

        PageMetadata* pageMeta = header->OwningPage;
        if (!pageMeta)
        {
            SetError(MemoryError::InvalidPointer, "Invalid page metadata");
            return;
        }

        size_t sizeClass = pageMeta->SizeClassIndex;

        if (sizeClass < NUM_SMALL_SIZE_CLASSES)
        {
            FreeSmall(ptr, pageMeta);
        }
        else
        {
            FreeMedium(ptr, pageMeta);
        }

        if (m_Config.EnableThreadSafety)
        {
            std::lock_guard<std::mutex> lock(m_StatsMutex);
            m_TotalFreeCount++;
            if (sizeClass < NUM_SIZE_CLASSES)
            {
                m_FreesByClass[sizeClass]++;
            }
        }
        else
        {
            m_TotalFreeCount++;
            if (sizeClass < NUM_SIZE_CLASSES)
            {
                m_FreesByClass[sizeClass]++;
            }
        }
    }

    void* MemoryPool::Reallocate(void* ptr, size_t newSize)
    {
        if (!ptr)
        {
            return Allocate(newSize);
        }

        if (newSize == 0)
        {
            Free(ptr);
            return nullptr;
        }

        // Find the old allocation size
        size_t oldSize = 0;

        if (IsLargeAllocation(ptr))
        {
            // Large allocation
            LargeAllocationHeader* header = rcast(LargeAllocationHeader*, rcast(uint8_t*, ptr) - sizeof(LargeAllocationHeader));
            oldSize = header->Size;
        }
        else
        {
            // Small/medium allocation
            AllocationHeader* header = rcast(AllocationHeader*, rcast(uint8_t*, ptr) - sizeof(AllocationHeader));
            oldSize = GetSizeForClass(header->OwningPage->SizeClassIndex);
        }

        // If new size fits in the same size class, just return the same pointer
        if (oldSize > 0 && newSize <= oldSize)
        {
            // Could potentially fit in the same allocation
            size_t newSizeClassSize = RoundUpPowerOf2(newSize);
            if (newSizeClassSize == oldSize || newSize > MAX_MEDIUM_SIZE)
            {
                return ptr;
            }
        }

        // Allocate new memory and copy
        void* newPtr = Allocate(newSize);
        if (newPtr && oldSize > 0)
        {
            size_t copySize = (oldSize < newSize) ? oldSize : newSize;
            std::memcpy(newPtr, ptr, copySize);
        }

        Free(ptr);

        return newPtr;
    }

    void* MemoryPool::AllocateSmall(size_t size)
    {
        // Round up to nearest power of 2
        size_t actualSize = RoundUpPowerOf2(size);
        if (actualSize < MIN_ALLOCATION_SIZE)
        {
            actualSize = MIN_ALLOCATION_SIZE;
        }

        // Get size class index
        uint8_t sizeClass = GetSizeClassIndex(actualSize);

        // Lock this size class if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_SizeClassMutexes[sizeClass]);
        }

        // Find a page with free space
        PageMetadata* page = m_SizeClassLists[sizeClass];

        while (page && !page->FreeList)
        {
            page = page->NextPage;
        }

        // If no page with free space, allocate a new page
        if (!page)
        {
            page = AllocatePage(sizeClass);
            if (!page)
            {
                return nullptr;
            }
        }

        // Pop from free list
        oddAssert(page->FreeList != nullptr);

        FreeListNode* node = page->FreeList;
        page->FreeList = node->Next;
        page->AllocationCount++;

        m_TotalUsed += actualSize;

        // Update statistics
        if (m_Config.EnableStatistics)
        {
            m_AllocationsByClass[sizeClass]++;
        }

        // Set up allocation header
        AllocationHeader* header = rcast(AllocationHeader*, node);
        header->OwningPage = page;
        header->Magic = ALLOCATION_MAGIC;
        header->Padding = 0;

        // Return pointer after header
        return rcast(uint8_t*, header) + sizeof(AllocationHeader);
    }

    void* MemoryPool::AllocateMedium(size_t size)
    {
        // For medium allocations, we use size classes: 4K, 8K, 16K, 32K
        size_t actualSize = RoundUpPowerOf2(size);

        // Clamp to medium range
        if (actualSize < MIN_MEDIUM_SIZE)
        {
            actualSize = MIN_MEDIUM_SIZE;
        }
        else if (actualSize > MAX_MEDIUM_SIZE)
        {
            // Should go to large allocation instead
            return AllocateLarge(size);
        }

        // Get size class index (offset by NUM_SMALL_SIZE_CLASSES)
        uint8_t sizeClass = GetSizeClassIndex(actualSize);

        // Lock this size class if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_SizeClassMutexes[sizeClass]);
        }

        // Find a page with free space
        PageMetadata* page = m_SizeClassLists[sizeClass];

        while (page && !page->FreeList)
        {
            page = page->NextPage;
        }

        // If no page with free space, allocate a new page
        if (!page)
        {
            page = AllocatePage(sizeClass);
            if (!page)
            {
                return nullptr;
            }
        }

        // Pop from free list
        oddAssert(page->FreeList != nullptr);

        FreeListNode* node = page->FreeList;
        page->FreeList = node->Next;
        page->AllocationCount++;

        m_TotalUsed += actualSize;

        // Update statistics
        if (m_Config.EnableStatistics)
        {
            m_AllocationsByClass[sizeClass]++;
        }

        // Set up allocation header
        AllocationHeader* header = rcast(AllocationHeader*, node);
        header->OwningPage = page;
        header->Magic = ALLOCATION_MAGIC;
        header->Padding = 0;

        // Return pointer after header
        return rcast(uint8_t*, header) + sizeof(AllocationHeader);
    }

    void* MemoryPool::AllocateLarge(size_t size)
    {
        // Lock large allocation list if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_LargeMutex);
        }

        // Allocate directly from OS with header
        size_t totalSize = sizeof(LargeAllocationHeader) + size;

        void* memory = std::malloc(totalSize);
        if (!memory)
        {
            return nullptr;
        }

        // Set up header
        LargeAllocationHeader* header = rcast(LargeAllocationHeader*, memory);
        header->Size = size;
        header->Magic = LARGE_ALLOC_MAGIC;
        header->Padding = 0;
        header->Next = m_LargeAllocations;
        header->Prev = nullptr;

        // Link into list
        if (m_LargeAllocations)
        {
            m_LargeAllocations->Prev = header;
        }
        m_LargeAllocations = header;

        m_NumLargeAllocations++;
        m_TotalUsed += size;

        // Get user pointer
        void* userPtr = rcast(uint8_t*, header) + sizeof(LargeAllocationHeader);

        // Add to fast lookup set (O(1) IsLargeAllocation now!)
        m_LargeAllocationSet.insert(userPtr);

        // Return pointer after header
        return userPtr;
    }

    void MemoryPool::FreeSmall(void* ptr, PageMetadata* pageMeta)
    {
        uint8_t sizeClass = pageMeta->SizeClassIndex;

        // Lock this size class if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_SizeClassMutexes[sizeClass]);
        }

        // Get back to the allocation header
        void* allocation = rcast(uint8_t*, ptr) - sizeof(AllocationHeader);

        // Add back to free list
        FreeListNode* node = rcast(FreeListNode*, allocation);
        node->Next = pageMeta->FreeList;
        pageMeta->FreeList = node;

        pageMeta->AllocationCount--;

        size_t chunkSize = GetSizeForClass(pageMeta->SizeClassIndex);
        m_TotalUsed -= chunkSize;
        m_TotalFreed += chunkSize;

        // If page is empty and not the first page in the list, deallocate it
        if (pageMeta->AllocationCount == 0)
        {
            PageMetadata* firstPage = m_SizeClassLists[sizeClass];

            if (pageMeta != firstPage)
            {
                DeallocatePage(pageMeta);
            }
        }
    }

    void MemoryPool::FreeMedium(void* ptr, PageMetadata* pageMeta)
    {
        uint8_t sizeClass = pageMeta->SizeClassIndex;

        // Lock this size class if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_SizeClassMutexes[sizeClass]);
        }

        // Get back to the allocation header
        void* allocation = rcast(uint8_t*, ptr) - sizeof(AllocationHeader);

        // Same logic as FreeSmall
        FreeListNode* node = rcast(FreeListNode*, allocation);
        node->Next = pageMeta->FreeList;
        pageMeta->FreeList = node;

        pageMeta->AllocationCount--;

        size_t chunkSize = GetSizeForClass(pageMeta->SizeClassIndex);
        m_TotalUsed -= chunkSize;
        m_TotalFreed += chunkSize;

        // If page is empty and not the first page in the list, deallocate it
        if (pageMeta->AllocationCount == 0)
        {
            PageMetadata* firstPage = m_SizeClassLists[sizeClass];

            if (pageMeta != firstPage)
            {
                DeallocatePage(pageMeta);
            }
        }
    }

    void MemoryPool::FreeLarge(void* ptr)
    {
        // Lock large allocation list if thread safety is enabled
        std::unique_lock<std::mutex> lock;
        if (m_Config.EnableThreadSafety)
        {
            lock = std::unique_lock<std::mutex>(m_LargeMutex);
        }

        // Remove from fast lookup set
        m_LargeAllocationSet.erase(ptr);

        // Get header
        LargeAllocationHeader* header = rcast(LargeAllocationHeader*, rcast(uint8_t*, ptr) - sizeof(LargeAllocationHeader));

        // Validate magic
        if (header->Magic != LARGE_ALLOC_MAGIC)
        {
            SetError(MemoryError::CorruptedHeader, "Invalid large allocation");
            return;
        }

        // Remove from list
        if (header->Prev)
        {
            header->Prev->Next = header->Next;
        }
        else
        {
            m_LargeAllocations = header->Next;
        }

        if (header->Next)
        {
            header->Next->Prev = header->Prev;
        }

        m_TotalUsed -= header->Size;
        m_TotalFreed += header->Size;
        m_NumLargeAllocations--;

        // Free the OS allocation
        std::free(header);
    }

    PageMetadata* MemoryPool::AllocatePage(uint8_t sizeClassIndex)
    {
        // Validate size class index
        if (sizeClassIndex >= NUM_SIZE_CLASSES)
        {
            return nullptr;
        }

        // Try to reuse a free page first
        PageMetadata* pageMeta = TryReusePage(sizeClassIndex);

        if (!pageMeta)
        {
            // No free page available, allocate new from OS
            size_t chunkSize = GetSizeForClass(sizeClassIndex);
            size_t pageSize = (sizeClassIndex < NUM_SMALL_SIZE_CLASSES) ? m_Config.SmallPageSize : m_Config.MediumPageSize;

            // Account for allocation headers in each chunk
            size_t effectiveChunkSize = chunkSize + sizeof(AllocationHeader);

            // Ensure we don't allocate more than the page can hold
            size_t numChunks = pageSize / effectiveChunkSize;
            if (numChunks == 0)
            {
                return nullptr;
            }

            // Allocate page + metadata as single block from OS
            // Metadata goes at the beginning, page memory follows
            size_t totalSize = sizeof(PageMetadata) + pageSize;
            void*  memory = std::malloc(totalSize);
            if (!memory)
            {
                return nullptr;
            }

            pageMeta = rcast(PageMetadata*, memory);
            pageMeta->PageMemory = rcast(uint8_t*, memory) + sizeof(PageMetadata);
            pageMeta->SizeClassIndex = sizeClassIndex;
            pageMeta->PageSize = scast(uint16_t, pageSize);
            pageMeta->IsActive = 1;

            // Initialize the page with the effective chunk size
            InitializePage(pageMeta, effectiveChunkSize, pageSize, scast(uint16_t, numChunks));

            m_NumPages++;
        }
        else
        {
            // Reusing page - reinitialize it
            size_t chunkSize = GetSizeForClass(sizeClassIndex);
            size_t pageSize = pageMeta->PageSize;
            size_t effectiveChunkSize = chunkSize + sizeof(AllocationHeader);
            size_t numChunks = pageSize / effectiveChunkSize;

            pageMeta->IsActive = 1;
            InitializePage(pageMeta, effectiveChunkSize, pageSize, scast(uint16_t, numChunks));
        }

        // Add to the size class list (at the head)
        pageMeta->NextPage = m_SizeClassLists[sizeClassIndex];
        pageMeta->PrevPage = nullptr;
        pageMeta->NextFreePage = nullptr;

        if (m_SizeClassLists[sizeClassIndex])
        {
            m_SizeClassLists[sizeClassIndex]->PrevPage = pageMeta;
        }

        m_SizeClassLists[sizeClassIndex] = pageMeta;

        return pageMeta;
    }

    PageMetadata* MemoryPool::TryReusePage(uint8_t sizeClassIndex)
    {
        if (sizeClassIndex >= NUM_SIZE_CLASSES)
        {
            return nullptr;
        }

        PageMetadata* freePage = m_FreePageLists[sizeClassIndex];
        if (!freePage)
        {
            return nullptr;
        }

        // Remove from free list
        m_FreePageLists[sizeClassIndex] = freePage->NextFreePage;

        return freePage;
    }

    void MemoryPool::DeallocatePage(PageMetadata* pageMeta)
    {
        oddAssert(pageMeta->AllocationCount == 0);

        // Remove from the active size class list
        if (pageMeta->PrevPage)
        {
            pageMeta->PrevPage->NextPage = pageMeta->NextPage;
        }
        else
        {
            m_SizeClassLists[pageMeta->SizeClassIndex] = pageMeta->NextPage;
        }

        if (pageMeta->NextPage)
        {
            pageMeta->NextPage->PrevPage = pageMeta->PrevPage;
        }

        pageMeta->IsActive = 0;
        m_NumPages--;

        // Add to free page list for reuse
        uint8_t sizeClass = pageMeta->SizeClassIndex;
        pageMeta->NextFreePage = m_FreePageLists[sizeClass];
        pageMeta->NextPage = nullptr;
        pageMeta->PrevPage = nullptr;
        m_FreePageLists[sizeClass] = pageMeta;
    }

    void MemoryPool::InitializePage(PageMetadata* pageMeta, size_t chunkSize, size_t pageSize, uint16_t numChunks)
    {
        pageMeta->AllocationCount = 0;
        pageMeta->TotalChunks = numChunks;
        pageMeta->FreeList = nullptr;
        pageMeta->NextPage = nullptr;
        pageMeta->PrevPage = nullptr;

        // Build the free list by linking all chunks
        uint8_t* current = rcast(uint8_t*, pageMeta->PageMemory);

        for (uint16_t i = 0; i < pageMeta->TotalChunks; i++)
        {
            // Ensure we don't go past the page boundary
            if (current + chunkSize > rcast(uint8_t*, pageMeta->PageMemory) + pageSize)
            {
                // Stop if we would exceed the page
                pageMeta->TotalChunks = i;
                break;
            }

            FreeListNode* node = rcast(FreeListNode*, current);
            node->Next = pageMeta->FreeList;
            pageMeta->FreeList = node;

            current += chunkSize;
        }
    }

    uint8_t MemoryPool::GetSizeClassIndex(size_t size) const
    {
        if (size <= MAX_SMALL_SIZE)
        {
            // Small size classes: 16, 32, 64, 128, 256, 512, 1024, 2048
            // Index = log2(size) - log2(16) = log2(size) - 4
            size_t rounded = RoundUpPowerOf2(size);
            if (rounded < MIN_ALLOCATION_SIZE)
            {
                rounded = MIN_ALLOCATION_SIZE;
            }

            uint8_t index = Log2(rounded) - Log2(MIN_ALLOCATION_SIZE);
            return index;
        }
        else if (size <= MAX_MEDIUM_SIZE)
        {
            // Medium size classes: 4K, 8K, 16K, 32K
            // Index = NUM_SMALL_SIZE_CLASSES + (log2(size) - log2(4096))
            size_t  rounded = RoundUpPowerOf2(size);
            uint8_t index = NUM_SMALL_SIZE_CLASSES + (Log2(rounded) - Log2(MIN_MEDIUM_SIZE));
            return index;
        }
        else
        {
            // This shouldn't happen - large allocations don't use size classes
            // Return an invalid index
            return NUM_SIZE_CLASSES;
        }
    }

    size_t MemoryPool::GetSizeForClass(uint8_t sizeClassIndex) const
    {
        if (sizeClassIndex < NUM_SMALL_SIZE_CLASSES)
        {
            // Small size classes
            return MIN_ALLOCATION_SIZE << sizeClassIndex;
        }
        else
        {
            // Medium size classes
            uint8_t mediumIndex = sizeClassIndex - NUM_SMALL_SIZE_CLASSES;
            return MIN_MEDIUM_SIZE << mediumIndex;
        }
    }

    bool MemoryPool::IsLargeAllocation(void* ptr) const
    {
        // Fast O(1) lookup using hash set!
        if (m_Config.EnableThreadSafety)
        {
            // Need const_cast because unordered_set doesn't have const find in some implementations
            // but logically this is const
            return const_cast<MemoryPool*>(this)->m_LargeAllocationSet.count(ptr) > 0;
        }
        else
        {
            return m_LargeAllocationSet.count(ptr) > 0;
        }
    }

    void MemoryPool::SetError(MemoryError error, const char* message)
    {
        m_LastError = error;
        if (m_Config.ErrorCallback && message)
        {
            m_Config.ErrorCallback(message);
        }
    }

    void MemoryPool::UpdatePeakMemory()
    {
        if (m_TotalUsed > m_PeakMemoryUsage)
        {
            m_PeakMemoryUsage = m_TotalUsed;
        }
    }

    void MemoryPool::UpdateFragmentation()
    {
        // Simple fragmentation metric: ratio of wasted space to total allocated
        // Not implemented yet - would require tracking free space per page
    }

    const char* MemoryPool::GetErrorString(MemoryError error) const
    {
        switch (error)
        {
            case MemoryError::None:
                return "No error";
            case MemoryError::OutOfMemory:
                return "Out of memory";
            case MemoryError::InvalidPointer:
                return "Invalid pointer";
            case MemoryError::InvalidSize:
                return "Invalid size";
            case MemoryError::MaxMemoryExceeded:
                return "Maximum memory limit exceeded";
            case MemoryError::NotInitialized:
                return "Memory pool not initialized";
            case MemoryError::AlreadyInitialized:
                return "Memory pool already initialized";
            case MemoryError::CorruptedHeader:
                return "Corrupted allocation header";
            case MemoryError::DoubleFree:
                return "Double free detected";
            default:
                return "Unknown error";
        }
    }

    MemoryPoolStats MemoryPool::GetStats() const
    {
        MemoryPoolStats stats = {};

        if (m_Config.EnableThreadSafety)
        {
            std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_StatsMutex));

            stats.TotalAllocated = m_TotalAllocated;
            stats.TotalUsed = m_TotalUsed;
            stats.TotalFreed = m_TotalFreed;
            stats.PeakMemoryUsage = m_PeakMemoryUsage;
            stats.NumPages = m_NumPages;
            stats.NumLargeAllocations = m_NumLargeAllocations;
            stats.TotalAllocationCount = m_TotalAllocationCount;
            stats.TotalFreeCount = m_TotalFreeCount;

            // Copy per-class stats
            for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
            {
                stats.AllocationsByClass[i] = m_AllocationsByClass[i];
                stats.FreesByClass[i] = m_FreesByClass[i];
                stats.ActiveAllocsByClass[i] = m_AllocationsByClass[i] - m_FreesByClass[i];
            }
        }
        else
        {
            stats.TotalAllocated = m_TotalAllocated;
            stats.TotalUsed = m_TotalUsed;
            stats.TotalFreed = m_TotalFreed;
            stats.PeakMemoryUsage = m_PeakMemoryUsage;
            stats.NumPages = m_NumPages;
            stats.NumLargeAllocations = m_NumLargeAllocations;
            stats.TotalAllocationCount = m_TotalAllocationCount;
            stats.TotalFreeCount = m_TotalFreeCount;

            // Copy per-class stats
            for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
            {
                stats.AllocationsByClass[i] = m_AllocationsByClass[i];
                stats.FreesByClass[i] = m_FreesByClass[i];
                stats.ActiveAllocsByClass[i] = m_AllocationsByClass[i] - m_FreesByClass[i];
            }
        }

        // Count free pages
        stats.NumFreePages = 0;
        for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
        {
            PageMetadata* page = m_FreePageLists[i];
            while (page)
            {
                stats.NumFreePages++;
                page = page->NextFreePage;
            }
        }

        // Calculate fragmentation (simple metric)
        if (stats.TotalAllocated > 0)
        {
            stats.FragmentationRatio = 1.0f - (scast(float, stats.TotalUsed) / scast(float, stats.TotalAllocated));
        }
        else
        {
            stats.FragmentationRatio = 0.0f;
        }

        return stats;
    }

    void MemoryPool::PrintStats() const
    {
        MemoryPoolStats stats = GetStats();

        printf("=== Memory Pool Statistics ===\n");
        printf("Total Allocated:   %zu bytes (%.2f MB)\n", stats.TotalAllocated, stats.TotalAllocated / 1024.0 / 1024.0);
        printf("Total Used:        %zu bytes (%.2f MB)\n", stats.TotalUsed, stats.TotalUsed / 1024.0 / 1024.0);
        printf("Total Freed:       %zu bytes (%.2f MB)\n", stats.TotalFreed, stats.TotalFreed / 1024.0 / 1024.0);
        printf("Peak Memory Usage: %zu bytes (%.2f MB)\n", stats.PeakMemoryUsage, stats.PeakMemoryUsage / 1024.0 / 1024.0);
        printf("Active Pages:      %zu\n", stats.NumPages);
        printf("Free Pages:        %zu\n", stats.NumFreePages);
        printf("Large Allocations: %zu\n", stats.NumLargeAllocations);
        printf("Total Alloc Count: %zu\n", stats.TotalAllocationCount);
        printf("Total Free Count:  %zu\n", stats.TotalFreeCount);
        printf("Fragmentation:     %.2f%%\n", stats.FragmentationRatio * 100.0f);
        printf("\n");

        printf("Per-Size-Class Statistics:\n");
        printf("Size Class | Allocations | Frees | Active\n");
        printf("-----------|-------------|-------|-------\n");

        for (size_t i = 0; i < NUM_SIZE_CLASSES; i++)
        {
            size_t sizeClass = GetSizeForClass(scast(uint8_t, i));
            printf("%10zu | %11zu | %5zu | %6zu\n", sizeClass, stats.AllocationsByClass[i], stats.FreesByClass[i], stats.ActiveAllocsByClass[i]);
        }

        printf("==============================\n");
    }

    // ============================================================================
    // Global Memory Pool Functions
    // ============================================================================

    bool InitializeMemoryPool(const MemoryPoolConfig& config)
    {
        if (GGlobalMemoryPool)
        {
            return false;
        }

        GGlobalMemoryPool = new MemoryPool();
        if (!GGlobalMemoryPool)
        {
            return false;
        }

        if (!GGlobalMemoryPool->Initialize(config))
        {
            delete GGlobalMemoryPool;
            GGlobalMemoryPool = nullptr;
            return false;
        }

        return true;
    }

    void ShutdownMemoryPool()
    {
        if (GGlobalMemoryPool)
        {
            GGlobalMemoryPool->Shutdown();
            delete GGlobalMemoryPool;
            GGlobalMemoryPool = nullptr;
        }
    }

    void* OddMalloc(size_t size)
    {
        if (!GGlobalMemoryPool)
        {
            return nullptr;
        }

        return GGlobalMemoryPool->Allocate(size);
    }

    void OddFree(void* ptr)
    {
        if (!GGlobalMemoryPool)
        {
            return;
        }

        GGlobalMemoryPool->Free(ptr);
    }

    void* OddRealloc(void* ptr, size_t newSize)
    {
        if (!GGlobalMemoryPool)
        {
            return nullptr;
        }

        return GGlobalMemoryPool->Reallocate(ptr, newSize);
    }

    MemoryPool* GetGlobalMemoryPool()
    {
        return GGlobalMemoryPool;
    }

    MemoryPoolStats GetGlobalMemoryStats()
    {
        if (!GGlobalMemoryPool)
        {
            return MemoryPoolStats{};
        }

        return GGlobalMemoryPool->GetStats();
    }

    void PrintGlobalMemoryStats()
    {
        if (GGlobalMemoryPool)
        {
            GGlobalMemoryPool->PrintStats();
        }
    }

    LinearAllocator::LinearAllocator(size_t size)
        : m_Capacity(size)
    {
        if (m_Capacity > 0)
        {
            m_Memory = scast(uint8_t*, OddMalloc(m_Capacity));
            oddAssert(m_Memory != nullptr);
        }
    }

    void* LinearAllocator::Allocate(size_t size)
    {
        oddAssert((m_Size + size) <= m_Capacity);
        void* outMemory = m_Memory + m_Size;
        m_Size += size;
        return outMemory;
    }

    void LinearAllocator::Reset()
    {
        m_Size = 0;
    }

    void LinearAllocator::ResetAndResize(size_t size)
    {
        m_Size = 0;
        if (size == 0 || size <= m_Capacity)
        {
            return;
        }

        m_Memory = scast(uint8_t*, OddRealloc(m_Memory, size));
        if (m_Memory)
        {
            std::memset(m_Memory, 0, m_Capacity);
        }
        m_Capacity = size;
    }

    LinearAllocator::~LinearAllocator()
    {
        OddFree(m_Memory);
    }

} // namespace Odd
