// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_XBOX_PLATFORM_

#include "CIrrDeviceBOX360.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "irros.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include <stdio.h>
#include "COSOperator.h"
#include "irrlicht.h"
//#import <wchar.h>
//#import <time.h>
#include <xtl.h>
namespace irr
{
	namespace video
	{
		IVideoDriver* createXBOX360DirectX9Driver(const core::dimension2d<s32>& screenSize,
		HWND window, u32 bits, bool fullscreen, bool stencilbuffer,
		io::IFileSystem* io, bool pureSoftware, bool highPrecisionFPU,
		bool vsync, bool AntiAlias,irr::video::E_FULLSCREEN_ANTIALIASING_SETTING aa_setting);
	}
} // end namespace irr

static bool firstLaunch = true;

namespace irr
{
//! constructor
CIrrDeviceXBOX360::CIrrDeviceXBOX360(const SIrrlichtCreationParameters& param)
 : CIrrDeviceStub(param), _window(NULL), _active(true), _oglcontext(NULL)//, _cglcontext(NULL)
{
	//struct utsname name;
	//NSString	*path;
		
#ifdef _DEBUG
	setDebugName("CIrrDeviceXBOX360");
#endif
			
	//uname(&name);
	//Operator = new COSOperator(name.version);
	//os::Printer::log(name.version,ELL_INFORMATION);
		
	if(FileSystem)
	{
		FileSystem->grab();
		//FileSystem->changeWorkingDirectoryTo("game://");
		FileSystem->addFolderFileArchive("update:");
		FileSystem->addFolderFileArchive("game:");
		
		FileSystem->drop();
	}
	initKeycodes();
	if (CreationParams.DriverType != video::EDT_NULL)
		createWindow();
	createDriver();
	createGUIAndScene();
}
	
CIrrDeviceXBOX360::~CIrrDeviceXBOX360()
{
}

void CIrrDeviceXBOX360::closeDevice()
{
	_active = false;
}

bool CIrrDeviceXBOX360::createWindow()
{
	
	return (true);
}

void CIrrDeviceXBOX360::setResize(int width,int height)
{
}

void CIrrDeviceXBOX360::createDriver()
{
	switch (CreationParams.DriverType)
	{
		case video::EDT_PS3:
		case video::EDT_OPENGL:
		#ifdef _IRR_COMPILE_WITH_PS3_
			VideoDriver = video::createPS3Driver(CreationParams, FileSystem, this);
		#else
			os::Printer::log("No PS3 support compiled in.", ELL_ERROR);
		#endif
			break;

		case video::EDT_DIRECT3D8:
		case video::EDT_DIRECT3D9:
#ifdef _IRR_XBOX_PLATFORM_
			VideoDriver = video::createXBOX360DirectX9Driver(CreationParams.WindowSize, 0, 
			CreationParams.Bits, CreationParams.Fullscreen, CreationParams.Stencilbuffer, 
			FileSystem, false, CreationParams.HighPrecisionFPU, CreationParams.Vsync, 
			CreationParams.AntiAlias, CreationParams.AntiAliasSetting);
#else
			os::Printer::log("This driver is not available in PS3. Try OpenGL or Software renderer.", ELL_ERROR);
#endif
			break;

		case video::EDT_NULL:
			VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
			break;

		default:
			os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
			break;
	}
}
	
void CIrrDeviceXBOX360::flush()
{

}

bool CIrrDeviceXBOX360::run()
{
	os::Timer::tick();
	return _active;
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceXBOX360::yield()
{
	Sleep(1);
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceXBOX360::sleep(u32 timeMs, bool pauseTimer=false)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();
	
	Sleep(timeMs);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceXBOX360::setWindowCaption(const wchar_t* text)
{

}


bool CIrrDeviceXBOX360::isWindowActive() const
{
	return (true);
}


bool CIrrDeviceXBOX360::isWindowFocused() const
{
	return true;
}


bool CIrrDeviceXBOX360::isWindowMinimized() const
{
	return false;
}


void CIrrDeviceXBOX360::postKeyEvent(void *event,irr::SEvent &ievent,bool pressed)
{
}

void CIrrDeviceXBOX360::postMouseEvent(void *event,irr::SEvent &ievent)
{
}

void CIrrDeviceXBOX360::storeMouseLocation()
{
}

void CIrrDeviceXBOX360::setMouseLocation(int x,int y)
{
}

void CIrrDeviceXBOX360::setCursorVisible(bool visible)
{

}

void CIrrDeviceXBOX360::initKeycodes()
{
}

//! Sets if the window should be resizeable in windowed mode.

void CIrrDeviceXBOX360::setResizeAble(bool resize)
{
	// todo: implement resize
}

bool CIrrDeviceXBOX360::present(video::IImage* surface, void* windowId, core::rect<s32>* src )
{
	return true;// todo: implement 
}

video::IVideoModeList* CIrrDeviceXBOX360::getVideoModeList()
{
		return NULL;
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(const SIrrlichtCreationParameters& param)
{
	CIrrDeviceXBOX360* dev = new CIrrDeviceXBOX360(param);

	if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
	{
		dev->drop();
		dev = 0;
	}

	return dev;
}

}

#endif // _IRR_USE_PS3_DEVICE_

