// Copyright (C) 2002-2009 Gameloft
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_WII_H_INCLUDED__
#define __C_IRR_DEVICE_WII_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_WII_DEVICE_

#include "irrTypes.h"

struct MEMAllocator;

namespace irr
{
namespace os
{

struct EHeap
{
	enum Enum
	{
		Fast = 0,
		Slow = 1,
		Count = 2,
		Default = Fast,
		Mem1 = Fast,
		Mem2 = Slow,
	};
};

void MemMgrInit();

class CWiiMemMgr
{
public:
	static void Init();
	static void * Alloc(u32 size);
	static void * Alloc(u32 size, EHeap::Enum heap); 
	static void Free(void* ptr);
	static void Free(void* ptr, EHeap::Enum heap);
	static void * Realloc(void* ptr, u32 size);
	static void * Realloc(void* ptr, u32 size, EHeap::Enum heap);
    static MEMAllocator* GetAllocator(u32 idx);

protected:
	static MEMAllocator* GetAllocatorFromPtr(void* block);
	static MEMAllocator s_allocators[EHeap::Count];

};

}; // namespace os
}; // namespace irr

#endif // _IRR_USE_WII_DEVICE_

#endif // __C_IRR_DEVICE_WII_H_INCLUDED__
