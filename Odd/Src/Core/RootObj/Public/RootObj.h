#pragma once

#include "OddCore.h"
#include <concepts>

#include <atomic>
#include <new>
#include <memory>
#include <cassert>

namespace Odd
{
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


	private:
		mutable std::atomic_size_t m_RefCount;
	};

	namespace Internal
	{
		void DeleteRootObj(void* pObj);
		size_t RootObjAddRef(void* pObj);
		size_t RootObjRelease(void* pObj);
		size_t RootObjGetRefCount(void* pObj);
	}

	template<std::derived_from<RootObj> T>
	inline T* MakeObject()
	{
        void* pMemory = OddMalloc(sizeof(T));
        assert(pMemory != nullptr && "Failed to allocate RootObject");
        return new (pMemory) T();
	}
}