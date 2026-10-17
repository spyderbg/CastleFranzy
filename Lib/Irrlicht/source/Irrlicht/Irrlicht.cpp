#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

static const char* const copyright = "Irrlicht Engine (c) 2002-2008 Nikolaus Gebhardt";
	#ifdef _IRR_XBOX_PLATFORM_
		#include <xtl.h>
	#endif

#ifdef _IRR_WINDOWS_
	#include <windows.h>
	#if defined(_DEBUG) && !defined(__GNUWIN32__) && !defined(_WIN32_WCE)
		#include <crtdbg.h>
	#endif // _DEBUG
#endif

#ifdef _IRR_XBOX_PLATFORM_
	#include <xtl.h>
#endif

#include "irrlicht.h"

inline bool IsPowerOf2(int value)
{
	return value && !(value & (value - 1));
}

template <typename T>
inline T RoundUp(T x, unsigned int base)
{
	_IRR_DEBUG_BREAK_IF(!IsPowerOf2(base));

	return static_cast<T>( (x + (base - 1)) & ~(base - 1) );
}

#undef irrnew

void* IrrAlloc(size_t size, irr::MemHint hint);
void IrrFree(void* p);

void* operator new(size_t size, irr::MemHint hint) {
	return IrrAlloc(size, hint);
}
void* operator new[](size_t size, irr::MemHint hint) {
	return IrrAlloc(size, hint);
}
void operator delete(void* p, irr::MemHint hint) {
	IrrFree(p);
}
void operator delete[](void* p, irr::MemHint hint) {
	IrrFree(p);
}

#if 0
#if defined(_IRR_COMPILE_WITH_PS3_)
void *operator new(std::size_t size) throw(std::bad_alloc)
{
	return IrrAlloc(size, irr::MEMHINT_NONE);
}
void *operator new(std::size_t size, const std::nothrow_t&) throw()
{
	return IrrAlloc(size, irr::MEMHINT_NONE);
}
void *operator new[](std::size_t size) throw(std::bad_alloc)
{
	return IrrAlloc(size, irr::MEMHINT_NONE);
}
void *operator new[](std::size_t size, const std::nothrow_t&) throw()
{
	return IrrAlloc(size, irr::MEMHINT_NONE);
}
void operator delete  (void *p) throw(){
	IrrFree(p);
}
void operator delete[](void *p) throw(){
	IrrFree(p);
}
void operator delete  (void *p, const std::nothrow_t&) throw(){
	IrrFree(p);
}
void operator delete[](void *p, const std::nothrow_t&) throw(){
	IrrFree(p);
}
void operator delete  (void *p, void *) throw(){
	IrrFree(p);
}
void operator delete[](void *p, void *) throw(){
	IrrFree(p);
}
#endif
#endif

extern "C" void *irr_malloc(size_t size)
{
	return IrrAlloc(size, irr::MEMHINT_NONE);
}

extern "C" void *irr_free(void* p)
{
	IrrFree(p);
	return 0;
}

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line);

void* operator new(size_t size, irr::MemHint hint, int block, const char* filename, int line) {
	return IrrAlloc(size, hint, block, filename, line);
}
void* operator new[](size_t size, irr::MemHint hint, int block, const char* filename, int line) {
	return IrrAlloc(size, hint, block, filename, line);
}
void operator delete(void* p, irr::MemHint hint, int block, const char* filename, int line) {
	IrrFree(p);
}
void operator delete[](void* p, irr::MemHint hint, int block, const char* filename, int line) {
	IrrFree(p);
}


#if defined(_IRR_STATIC_LIB_) && !defined(_IRR_WII_EMULATION_) && !defined(_IRR_USE_DEFAULT_ALLOCATOR_)

//both these functions need to be implemented game side

//void* IrrAlloc(size_t size, irr::MemHint hint) 
//void IrrFree(void* p, irr::MemHint hint)
//void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line) ;

#else

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line)
{
	return IrrAlloc(size, hint);
}

void* IrrAlloc(size_t size, irr::MemHint hint) 
{
	return malloc(size);
}

void IrrFree(void* p)
{
	free(p);
}

#endif


namespace irr
{
	//! stub for calling createDeviceEx
	IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDevice(video::E_DRIVER_TYPE driverType,
			const core::dimension2d<s32>& windowSize,
			u32 bits, bool fullscreen,
			bool stencilbuffer, bool vsync, IEventReceiver* res)
	{
		SIrrlichtCreationParameters p;
		p.DriverType = driverType;
		p.WindowSize = windowSize;
		p.Bits = bits;
		p.Fullscreen = fullscreen;
		p.Stencilbuffer = stencilbuffer;
		p.Vsync = vsync;
		p.EventReceiver = res;

		return createDeviceEx(p);
	}

namespace core
{
	const matrix4 IdentityMatrix(matrix4::EM4CONST_IDENTITY);
}
} // end namespace irr


#if (defined(_IRR_WINDOWS_API_)||defined(_IRR_XBOX_PLATFORM_)) && !defined(_IRR_STATIC_LIB_) 

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
	// _crtBreakAlloc = 139;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/*#if defined(_DEBUG) && !defined(__GNUWIN32__) && !defined(__BORLANDC__) && !defined (_WIN32_WCE)
				_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
			#endif*/
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#endif // defined(_IRR_WINDOWS_)

