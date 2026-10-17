// This is a Demo of the Irrlicht Engine (c) 2005-2008 by N.Gebhardt.
// This file is not documented.

#include <irrlicht.h>
#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif

#include <stdio.h>

#include "CMainMenu.h"
#include "CDemo.h"

using namespace irr;
#if defined(SN_TARGET_PS3)//used for DirectX9 Driver on Windows and XBOX 360 test
#define MEMORY_MALLOC(mm, s) mm = malloc(s)
#define MEMORY_FREE(mm) free(mm)
#define MEMSET memset
void* IrrAlloc(size_t size, irr::MemHint hint)
{
	if (size == 0)
	{
		return NULL;
	}
	else
	{
		void* mm = NULL;
		MEMORY_MALLOC(mm, size);

		MEMSET(mm, 0, size);
		return mm;
	}
}
void IrrFree(void* ptr)
{
	if (ptr)
	{
		MEMORY_FREE(ptr);
	}
}
void IrrFree(void* ptr, irr::MemHint hint)
{
	if (ptr)
	{
		MEMORY_FREE(ptr);
	}
}
void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line)
{
	if (size == 0)
	{
		return NULL;
	}
	else
	{
		void* mm = NULL;
		MEMORY_MALLOC(mm, size);

		MEMSET(mm, 0, size);
		return mm;
	}
}
#endif
#ifdef _WIN32

#pragma comment(lib, "Irrlicht.lib")
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char* argv[])
#endif
{
	bool fullscreen = false;
	bool music = true;
	bool shadows = false;
	bool additive = false;
	bool vsync = false;

#ifndef _IRR_WINDOWS_
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
#elif defined(_IRR_PS3_PLATFORM_)
    video::E_DRIVER_TYPE driverType = video::EDT_PS3;
    fullscreen = true;
#else
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
#endif

	CMainMenu menu;

//#ifndef _DEBUG
	if (menu.run(fullscreen, music, shadows, additive, vsync, driverType))
//#endif
	{
		CDemo demo(fullscreen, music, shadows, additive, vsync, driverType);
		demo.run();
	}

	return 0;
}

