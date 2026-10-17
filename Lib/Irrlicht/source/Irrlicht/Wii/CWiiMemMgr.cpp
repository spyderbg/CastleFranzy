#include "stdafx.h"

#include "CWiiMemMgr.h"

#if 0
extern "C" void *irr_malloc(size_t size)
{
#ifdef _WII
	return irr::os::CWiiMemMgr::Alloc(size, irr::os::EHeap::Mem2);
#else
	return new char[size];
#endif
}

extern "C" void irr_free(void *ptr)
{
#ifdef _WII
	irr::os::CWiiMemMgr::Free(ptr, irr::os::EHeap::Mem2);
#else
	delete[] ptr;
#endif
}
#endif


#if 0
void IrrFree(void* ptr)
{
	delete(ptr);
}

void* IrrAlloc(unsigned long size, irr::MemHint irrMemHint)
{
	 return operator irrnew(size);
}

void* IrrAlloc(unsigned long size, irr::MemHint irrMemHint, int, const char*, int)
{
	return operator irrnew(size);
}

#endif

#if 0 //def _WII

#include <revolution/mem.h>
#include <string.h>

#ifndef min
#define min(x, y)	((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y)	((x) > (y) ? (x) : (y))
#endif


void * operator new(size_t size)
{
	return irr::os::CWiiMemMgr::Alloc(size);
}

void operator delete(void *ptr) throw()
{
	return irr::os::CWiiMemMgr::Free(ptr);
}

void * operator new[](size_t size)
{
	return irr::os::CWiiMemMgr::Alloc(size);
}

void operator delete[](void *ptr) throw()
{
	return irr::os::CWiiMemMgr::Free(ptr);
}



namespace irr
{
namespace os
{

MEMAllocator CWiiMemMgr::s_allocators[EHeap::Count];

#ifdef DEBUG
    #define ALLOC_FLAGS (MEM_HEAP_OPT_0_CLEAR | MEM_HEAP_OPT_THREAD_SAFE | MEM_HEAP_OPT_DEBUG_FILL)
#else
	#define ALLOC_FLAGS (MEM_HEAP_OPT_0_CLEAR | MEM_HEAP_OPT_THREAD_SAFE)
#endif

void MemMgrInit()
{
	CWiiMemMgr::Init();
}

void 
CWiiMemMgr::Init()
{
	static bool bInitialized = false;
	if(!bInitialized)
	{
		OSInit();

		u32 arenaSize1 = (u32)OSGetMEM1ArenaHi() - (u32)OSGetMEM1ArenaLo();

		arenaSize1 = arenaSize1 * 23 / 24 ;

		void* heap1 = OSAllocFromMEM1ArenaLo(arenaSize1, 32);
		MEMHeapHandle hHeap1 = MEMCreateExpHeapEx(
                            (char*)heap1,
                            arenaSize1,
                            ALLOC_FLAGS);
		MEMInitAllocatorForExpHeap(&s_allocators[EHeap::Fast], hHeap1, 32);

		u32 arenaSize2 = (u32)OSGetMEM2ArenaHi() - (u32)OSGetMEM2ArenaLo();
		void* heap2 = OSAllocFromMEM2ArenaLo(arenaSize2, 32);
		MEMHeapHandle hHeap2 = MEMCreateExpHeapEx(
                            (char*)heap2,
                            arenaSize2,
                            ALLOC_FLAGS);
		MEMInitAllocatorForExpHeap(&s_allocators[EHeap::Slow], hHeap2, 32);

		bInitialized = true;
	}
}

u32 g_Allocated = 0;

MEMAllocator* 
CWiiMemMgr::GetAllocatorFromPtr(void* block)
{
	MEMHeapHandle hHeap = MEMFindContainHeap(block);

	for(int i = 0; i < EHeap::Count; ++i)
	{
		MEMAllocator* alloc = &s_allocators[i];
		if(alloc->pHeap == hHeap)
			return alloc;
	}

	ASSERT(false);

	return &s_allocators[0];
}

MEMAllocator* 
CWiiMemMgr::GetAllocator (u32 idx)
{
    return &s_allocators[idx];
}

void *
CWiiMemMgr::Alloc(u32 size) 
{
	Init();
	void* block = MEMAllocFromAllocator(&s_allocators[EHeap::Default], size);
	if(block)
	{
		g_Allocated += size;
	}
	else
	{
		block = MEMAllocFromAllocator(&s_allocators[EHeap::Slow], size);
	}
	return block;
}

void *
CWiiMemMgr::Alloc(u32 size, EHeap::Enum heap) 
{
	Init();
	void* block = MEMAllocFromAllocator(&s_allocators[heap], size);
	if(block)
	{
		g_Allocated += size;
	}
	return block;
}

void 
CWiiMemMgr::Free(void* ptr)
{
	if(ptr)
	{
		u32 oldSize = MEMGetSizeForMBlockExpHeap(ptr);
		
		g_Allocated -= oldSize;
		MEMFreeToAllocator(GetAllocatorFromPtr(ptr), ptr);
	}
}

void 
CWiiMemMgr::Free(void* ptr, EHeap::Enum heap)
{
	if(ptr)
	{
		u32 oldSize = MEMGetSizeForMBlockExpHeap(ptr);
		g_Allocated -= oldSize;
		MEMFreeToAllocator(GetAllocatorFromPtr(ptr), ptr);
	}
}

void *
CWiiMemMgr::Realloc(void* ptr, u32 size)
{
	void* block = CWiiMemMgr::Alloc(size);

	if (ptr != NULL)
	{
		u32 oldSize = MEMGetSizeForMBlockExpHeap(ptr);
		memcpy(block, ptr, min(oldSize, size));
		Free(ptr);
	}

	return block;
}

void *
CWiiMemMgr::Realloc(void* ptr, u32 size, EHeap::Enum heap)
{
	void* block = CWiiMemMgr::Alloc(size, heap);

	if (ptr != NULL)
	{
		u32 oldSize = MEMGetSizeForMBlockExpHeap(ptr);
		memcpy(block, ptr, min(oldSize, size));
		Free(ptr);
	}

	return block;
}

} // namespace os
} // namespace irr

#endif
