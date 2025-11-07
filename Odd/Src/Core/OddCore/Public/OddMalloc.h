#pragma once

#include "Types.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace Odd
{
    // ============================================================================
    // Configuration Constants
    // ============================================================================

    // Page sizes
    constexpr size_t SMALL_PAGE_SIZE = 4096;   // 4 KB
    constexpr size_t MEDIUM_PAGE_SIZE = 65536; // 64 KB
    constexpr size_t LARGE_THRESHOLD = 65536;  // Allocations > 64 KB are "large"

    // Size classes for small allocations (powers of 2)
    // 16, 32, 64, 128, 256, 512, 1024, 2048
    constexpr size_t MIN_ALLOCATION_SIZE = 16;
    constexpr size_t MAX_SMALL_SIZE = 2048;

    // Size classes for medium allocations
    // 4096, 8192, 16384, 32768
    constexpr size_t MIN_MEDIUM_SIZE = 4096;
    constexpr size_t MAX_MEDIUM_SIZE = 32768;

    // Number of size classes
    constexpr size_t NUM_SMALL_SIZE_CLASSES = 8;  // 16 -> 2048
    constexpr size_t NUM_MEDIUM_SIZE_CLASSES = 4; // 4K -> 32K
    constexpr size_t NUM_SIZE_CLASSES = NUM_SMALL_SIZE_CLASSES + NUM_MEDIUM_SIZE_CLASSES;

    // Alignment
    constexpr size_t ALLOCATION_ALIGNMENT = 16;

    // ============================================================================
    // Forward Declarations
    // ============================================================================

    struct PageMetadata;
    struct LargeAllocationHeader;
    class MemoryPool;

    // ============================================================================
    // Free List Node
    // ============================================================================

    // When a chunk is free, we store a pointer to the next free chunk in the chunk itself
    struct FreeListNode
    {
        FreeListNode* Next;
    };

    // ============================================================================
    // Allocation Header
    // ============================================================================

    // Header stored before each small/medium allocation for O(1) free
    struct AllocationHeader
    {
        PageMetadata* OwningPage; // Which page owns this allocation
        uint32_t      Magic;      // Magic number for validation (0xDEADBEEF in debug)
        uint32_t      Padding;    // Keep 16-byte alignment
    };

    // ============================================================================
    // Page Metadata
    // ============================================================================

    // Metadata for each page - stored WITH the page memory as a header
    struct PageMetadata
    {
        void*         PageMemory;      // Pointer to the actual page memory (after this header)
        FreeListNode* FreeList;        // Head of free list for this page
        PageMetadata* NextPage;        // Next page in the active size class list
        PageMetadata* PrevPage;        // Previous page in the active size class list
        PageMetadata* NextFreePage;    // Next page in the free page list (when deallocated)
        uint16_t      AllocationCount; // Number of active allocations
        uint16_t      TotalChunks;     // Total number of chunks in this page
        uint8_t       SizeClassIndex;  // Which size class this page belongs to
        uint8_t       IsActive;        // Whether this page is currently in use
        uint16_t      PageSize;        // Size of this page (SMALL_PAGE_SIZE or MEDIUM_PAGE_SIZE)
    };

    // ============================================================================
    // Large Allocation Header
    // ============================================================================

    // Header for large allocations (>32KB) - stored before the allocation
    // These are allocated directly from the OS
    struct LargeAllocationHeader
    {
        size_t                 Size;  // Size of the allocation (excluding header)
        uint32_t               Magic; // Magic number for validation (0xCAFEBABE)
        uint32_t               Padding;
        LargeAllocationHeader* Next; // Next large allocation in the list
        LargeAllocationHeader* Prev; // Previous large allocation in the list
    };

    // ============================================================================
    // Memory Pool
    // ============================================================================

    class MemoryPool
    {
    public:
        MemoryPool();
        ~MemoryPool();

        // Initialize the pool
        bool Initialize();

        // Shutdown and free all memory
        void Shutdown();

        // Core allocation functions
        void* Allocate(size_t size);
        void  Free(void* ptr);
        void* Reallocate(void* ptr, size_t newSize);

        // Query functions
        size_t GetTotalAllocated() const { return m_TotalAllocated; }
        size_t GetTotalUsed() const { return m_TotalUsed; }
        size_t GetNumPages() const { return m_NumPages; }
        size_t GetNumLargeAllocations() const { return m_NumLargeAllocations; }

        // Debug functions
        void PrintStats() const;

    private:
        // Internal allocation functions for different size categories
        void* AllocateSmall(size_t size);
        void* AllocateMedium(size_t size);
        void* AllocateLarge(size_t size);

        // Internal deallocation functions
        void FreeSmall(void* ptr, PageMetadata* pageMeta);
        void FreeMedium(void* ptr, PageMetadata* pageMeta);
        void FreeLarge(void* ptr);

        // Page management
        PageMetadata* AllocatePage(uint8_t sizeClassIndex);
        void          DeallocatePage(PageMetadata* pageMeta);
        PageMetadata* TryReusePage(uint8_t sizeClassIndex);
        void          InitializePage(PageMetadata* pageMeta, size_t chunkSize, size_t pageSize, uint16_t numChunks);

        // Helper functions
        uint8_t GetSizeClassIndex(size_t size) const;
        size_t  GetSizeForClass(uint8_t sizeClassIndex) const;
        bool    IsLargeAllocation(void* ptr) const;

        // Active page lists for each size class
        std::array<PageMetadata*, NUM_SIZE_CLASSES> m_SizeClassLists;

        // Free page lists for reuse (one per size class)
        std::array<PageMetadata*, NUM_SIZE_CLASSES> m_FreePageLists;

        // Large allocations (allocated directly from OS)
        LargeAllocationHeader* m_LargeAllocations;

        // Statistics
        size_t m_TotalAllocated;
        size_t m_TotalUsed;
        size_t m_NumPages;
        size_t m_NumLargeAllocations;

        // Initialization flag
        bool m_IsInitialized;
    };

    // ============================================================================
    // Global Memory Pool Functions
    // ============================================================================

    // Initialize the global memory pool
    bool InitializeMemoryPool();

    // Shutdown the global memory pool
    void ShutdownMemoryPool();

    // Global allocation functions
    void* OddMalloc(size_t size);
    void  OddFree(void* ptr);
    void* OddRealloc(void* ptr, size_t newSize);

    // Get the global memory pool instance
    MemoryPool* GetGlobalMemoryPool();

} // namespace Odd
