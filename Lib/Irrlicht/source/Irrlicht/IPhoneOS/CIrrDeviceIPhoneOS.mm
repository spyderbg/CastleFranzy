// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_IPHONEOS_DEVICE_

//#import <Cocoa/Cocoa.h>
//#import <OpenGL/gl.h>
//#import <Carbon/Carbon.h>

#include "CIrrDeviceIPhoneOS.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "irros.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include <stdio.h>
#include <sys/utsname.h>
#include "COSOperator.h"
#include "irrlicht.h"
#import <wchar.h>
#import <time.h>
#import "AppDelegate.h"

// some macros to make code more readable.
#define GetModeWidth(mode) GetDictionaryLong((mode), kCGDisplayWidth)
#define GetModeHeight(mode) GetDictionaryLong((mode), kCGDisplayHeight)
#define GetModeRefreshRate(mode) GetDictionaryLong((mode), kCGDisplayRefreshRate)
#define GetModeBitsPerPixel(mode) GetDictionaryLong((mode), kCGDisplayBitsPerPixel)

#define GetModeSafeForHardware(mode) GetDictionaryBoolean((mode), kCGDisplayModeIsSafeForHardware)
#define GetModeStretched(mode) GetDictionaryBoolean((mode), kCGDisplayModeIsStretched)

//------------------------------------------------------------------------------------------
Boolean GetDictionaryBoolean(CFDictionaryRef theDict, const void* key) 
{
	// get a boolean from the dictionary
	Boolean value = false;
	CFBooleanRef boolRef;
	boolRef = (CFBooleanRef)CFDictionaryGetValue(theDict, key);
	if (boolRef != NULL)
		value = CFBooleanGetValue(boolRef); 	
	return value;
}
//------------------------------------------------------------------------------------------
long GetDictionaryLong(CFDictionaryRef theDict, const void* key) 
{
	// get a long from the dictionary
	long value = 0;
	CFNumberRef numRef;
	numRef = (CFNumberRef)CFDictionaryGetValue(theDict, key); 
	if (numRef != NULL)
		CFNumberGetValue(numRef, kCFNumberLongType, &value); 	
	return value;
}

namespace irr
{
namespace video
{

IVideoDriver* createOpenGLES1Driver(const SIrrlichtCreationParameters& param,
									io::IFileSystem* io,
									CIrrDeviceIPhoneOS *device);

}
} // end namespace irr

static bool firstLaunch = true;

namespace irr
{
//! constructor
CIrrDeviceIPhoneOS::CIrrDeviceIPhoneOS(const SIrrlichtCreationParameters& param)
 : CIrrDeviceStub(param), _window(NULL), _active(true), _oglcontext(NULL)//, _cglcontext(NULL)
	,_keepRunning(true)
{
	struct utsname name;
	NSString	*path;
		
#ifdef _DEBUG
	setDebugName("CIrrDeviceMacOSX");
#endif
		
	/*if (firstLaunch)
	{
		firstLaunch = false;
			
		[[NSAutoreleasePool alloc] init];
		[NSApplication sharedApplication];
		[NSApp setDelegate:[[[AppDelegate alloc] initWithDevice:this] autorelease]];
		[NSBundle loadNibNamed:@"MainMenu" owner:[NSApp delegate]];
		[NSApp finishLaunching];
			
		path = [[[NSBundle mainBundle] bundlePath] stringByDeletingLastPathComponent];
		chdir([path cString]);
	}*/
		
	uname(&name);
	Operator = new COSOperator(name.version);
	os::Printer::log(name.version,ELL_INFORMATION);
		
	initKeycodes();
	if (CreationParams.DriverType != video::EDT_NULL)
		createWindow();
	CursorControl = new CCursorControl(CreationParams.WindowSize, this);
	createDriver();
	createGUIAndScene();
}
	
CIrrDeviceIPhoneOS::~CIrrDeviceIPhoneOS()
{
}

void CIrrDeviceIPhoneOS::closeDevice()
{
	_keepRunning = false;
}

bool CIrrDeviceIPhoneOS::createWindow()
{
	
	return (true);
}

void CIrrDeviceIPhoneOS::setResize(int width,int height)
{
}

void CIrrDeviceIPhoneOS::createDriver()
{
	switch (CreationParams.DriverType)
	{
		case video::EDT_OPENGL:
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
			VideoDriver = video::createOpenGLES1Driver(CreationParams, FileSystem, this);
#else
			os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
#endif
			break;

		case video::EDT_DIRECT3D8:
		case video::EDT_DIRECT3D9:
			os::Printer::log("This driver is not available in OSX. Try OpenGL or Software renderer.", ELL_ERROR);
			break;

		case video::EDT_NULL:
			VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
			break;

		default:
			os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
			break;
	}
}
	
void CIrrDeviceIPhoneOS::flush()
{

}

bool CIrrDeviceIPhoneOS::run()
{
	os::Timer::tick();
	return (_keepRunning);
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceIPhoneOS::yield()
{
// 	// TODO: Does this work or maybe is there a better way?
// 	struct timespec ts = {0,0};
// 	nanosleep(&ts, NULL);

	sched_yield();
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceIPhoneOS::sleep(u32 timeMs, bool pauseTimer=false)
{
	// TODO: Does this work or maybe is there a better way?

	bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceIPhoneOS::setWindowCaption(const wchar_t* text)
{

}


bool CIrrDeviceIPhoneOS::isWindowActive() const
{
	return (true);
}


bool CIrrDeviceIPhoneOS::isWindowFocused() const
{
	// TODO: return proper value
	return true;
}


bool CIrrDeviceIPhoneOS::isWindowMinimized() const
{
	// TODO: return proper value
	return false;
}


void CIrrDeviceIPhoneOS::postKeyEvent(void *event,irr::SEvent &ievent,bool pressed)
{
}

void CIrrDeviceIPhoneOS::postMouseEvent(void *event,irr::SEvent &ievent)
{
}

void CIrrDeviceIPhoneOS::storeMouseLocation()
{
}

void CIrrDeviceIPhoneOS::setMouseLocation(int x,int y)
{
}

void CIrrDeviceIPhoneOS::setCursorVisible(bool visible)
{

}

void CIrrDeviceIPhoneOS::initKeycodes()
{
}

//! Sets if the window should be resizeable in windowed mode.

void CIrrDeviceIPhoneOS::setResizeAble(bool resize)
{
	// todo: implement resize
}

bool CIrrDeviceIPhoneOS::present(video::IImage* surface, void* windowId, core::rect<s32>* src )
{
	return true;// todo: implement 
}

video::IVideoModeList* CIrrDeviceIPhoneOS::getVideoModeList()
{
		return NULL;
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(const SIrrlichtCreationParameters& param)
{
	CIrrDeviceIPhoneOS* dev = new CIrrDeviceIPhoneOS(param);

	if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
	{
		dev->drop();
		dev = 0;
	}

	return dev;
}

}

#endif // _IRR_USE_IPHONEOS_DEVICE_

