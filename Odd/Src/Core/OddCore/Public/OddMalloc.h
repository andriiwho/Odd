#pragma once

#include "Types.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_set>

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
    // Configuration Structures
    // ============================================================================

    // Memory pool configuration
    struct MemoryPoolConfig
    {
        // Page sizes (can be customized)
        size_t SmallPageSize = SMALL_PAGE_SIZE;
        size_t MediumPageSize = MEDIUM_PAGE_SIZE;

        // Maximum memory limit (0 = unlimited)
        size_t MaxMemoryBytes = 0;

        // Error callback (called on allocation failures)
        void (*ErrorCallback)(const char* message) = nullptr;

        // Enable/disable features
        bool EnableStatistics = true;
        bool EnableThreadSafety = true;

        // Default configuration
        static MemoryPoolConfig Default()
        {
            return MemoryPoolConfig{};
        }
    };

    // ============================================================================
    // Error Codes
    // ============================================================================

    enum class MemoryError
    {
        None = 0,
        OutOfMemory,
        InvalidPointer,
        InvalidSize,
        MaxMemoryExceeded,
        NotInitialized,
        AlreadyInitialized,
        CorruptedHeader,
        DoubleFree
    };

    // ============================================================================
    // Statistics Structure
    // ============================================================================

    struct MemoryPoolStats
    {
        // Overall statistics
        size_t TotalAllocated;      // Total bytes allocated from OS
        size_t TotalUsed;           // Total bytes in use by user
        size_t TotalFreed;          // Total bytes freed (cumulative)
        size_t PeakMemoryUsage;     // Highest memory usage achieved
        size_t NumPages;            // Active pages
        size_t NumFreePages;        // Pages in free lists
        size_t NumLargeAllocations; // Active large allocations

        // Per-size-class statistics
        size_t AllocationsByClass[NUM_SIZE_CLASSES];
        size_t FreesByClass[NUM_SIZE_CLASSES];
        size_t ActiveAllocsByClass[NUM_SIZE_CLASSES];

        // Frequency counters
        size_t TotalAllocationCount;
        size_t TotalFreeCount;

        // Fragmentation metric (0.0 = no fragmentation, 1.0 = high fragmentation)
        float FragmentationRatio;
    };

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

        // Initialize the pool with optional configuration
        bool Initialize(const MemoryPoolConfig& config = MemoryPoolConfig::Default());

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

        // Statistics functions
        MemoryPoolStats GetStats() const;
        void            PrintStats() const;

        // Error handling
        MemoryError GetLastError() const { return m_LastError; }
        const char* GetErrorString(MemoryError error) const;

        // Configuration
        const MemoryPoolConfig& GetConfig() const { return m_Config; }

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

        // Error handling
        void SetError(MemoryError error, const char* message = nullptr);

        // Statistics helpers
        void UpdatePeakMemory();
        void UpdateFragmentation();

        // Active page lists for each size class
        std::array<PageMetadata*, NUM_SIZE_CLASSES> m_SizeClassLists;

        // Free page lists for reuse (one per size class)
        std::array<PageMetadata*, NUM_SIZE_CLASSES> m_FreePageLists;

        // Large allocations (allocated directly from OS)
        LargeAllocationHeader* m_LargeAllocations;

        // Fast lookup for large allocations (O(1) instead of O(n))
        std::unordered_set<void*> m_LargeAllocationSet;

        // Thread safety - mutexes for each size class (fine-grained locking)
        std::array<std::mutex, NUM_SIZE_CLASSES> m_SizeClassMutexes;
        std::mutex                               m_LargeMutex;
        std::mutex                               m_StatsMutex;

        // Statistics
        size_t                               m_TotalAllocated;
        size_t                               m_TotalUsed;
        size_t                               m_TotalFreed;
        size_t                               m_PeakMemoryUsage;
        size_t                               m_NumPages;
        size_t                               m_NumLargeAllocations;
        std::array<size_t, NUM_SIZE_CLASSES> m_AllocationsByClass;
        std::array<size_t, NUM_SIZE_CLASSES> m_FreesByClass;
        size_t                               m_TotalAllocationCount;
        size_t                               m_TotalFreeCount;

        // Configuration
        MemoryPoolConfig m_Config;

        // Error tracking
        MemoryError m_LastError;

        // Initialization flag
        bool m_IsInitialized;
    };

    // ============================================================================
    // Global Memory Pool Functions
    // ============================================================================

    // Initialize the global memory pool with optional configuration
    bool InitializeMemoryPool(const MemoryPoolConfig& config = MemoryPoolConfig::Default());

    // Shutdown the global memory pool
    void ShutdownMemoryPool();

    // Global allocation functions
    void* OddMalloc(size_t size);
    void  OddFree(void* ptr);
    void* OddRealloc(void* ptr, size_t newSize);

    // Get the global memory pool instance
    MemoryPool* GetGlobalMemoryPool();

    // Get statistics from global pool
    MemoryPoolStats GetGlobalMemoryStats();

    // Print statistics from global pool
    void PrintGlobalMemoryStats();

    template <typename T, typename... Args>
    T* OddNew(Args&&... args)
    {
        void* pMemory = OddMalloc(sizeof(T));
        if (!pMemory)
        {
            return nullptr;
        }
        return new (pMemory) T(std::forward<Args>(args)...);
    }

    template <typename T>
    void OddDelete(T* pObj)
    {
        if (pObj)
        {
            pObj->~T();
            OddFree(pObj);
        }
    }
} // namespace Odd
