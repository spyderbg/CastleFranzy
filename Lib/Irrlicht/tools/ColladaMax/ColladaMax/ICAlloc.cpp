#include "stdafx.h"
#undef snprintf
#include "irrtypes.h"

void* IrrAlloc(size_t size, irr::MemHint hint) 
{
	return malloc(size);
}

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line)
{
	return IrrAlloc(size, hint);
}

void IrrFree(void* p)
{
	free(p);
}

//namespace irr
//{
//namespace ps
//{
//	void* PS_NEW(size_t size)
//	{
//		return malloc( size );
//	}
//
//	void PS_FREE(void* p)
//	{
//		free( p );
//	}
//}
//}