
#include "stdafx.h"

// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_PS3_DEVICE_
#include <cell/sysmodule.h>
#include <sysutil/sysutil_common.h>
#include <sys/timer.h>
#include <sys/ppu_thread.h>
#include "CIrrDevicePS3.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "irros.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include <stdio.h>
#include "COSOperator.h"
#include "irrlicht.h"
#import <wchar.h>
#import <time.h>
#include "SingleProducerConsumer.h"
#include <PSGL/psgl.h>

//#define ENABLE_PS3_MOUSE 1

#if ENABLE_PS3_MOUSE
#include <cell/mouse.h>
static CellMouseData s_lastMouseData;
static int mousePosX = 0;
static int mousePosY = 0;
#endif //ENABLE_PS3_MOUSE

static PSGLdevice* device = NULL;

namespace irr
{
	namespace video
	{
		IVideoDriver* createPS3Driver(const SIrrlichtCreationParameters& param, io::IFileSystem* io, CIrrDevicePS3 *device);
	}
} // end namespace irr

static bool firstLaunch = true;

namespace irr
{

//! constructor
CIrrDevicePS3::CIrrDevicePS3(const SIrrlichtCreationParameters& param)
 : CIrrDeviceStub(param), _window(NULL), _active(true), _oglcontext(NULL)//, _cglcontext(NULL)
{
	//struct utsname name;
	//NSString	*path;
		
#ifdef _DEBUG
	setDebugName("CIrrDevicePS3");
#endif
			
	//uname(&name);
	//Operator = new COSOperator(name.version);
	//os::Printer::log(name.version,ELL_INFORMATION);
		
	initKeycodes();
	if (CreationParams.DriverType != video::EDT_NULL)
		createWindow();

	GLuint width, height;

	psglGetDeviceDimensions(device, &width, &height);

	CreationParams.WindowSize.Width = width;
	CreationParams.WindowSize.Height = height;

	createDriver();
	createGUIAndScene();

#if ENABLE_PS3_MOUSE
	cellMouseInit(1);
	memset(&s_lastMouseData,0,sizeof(s_lastMouseData));
#endif //ENABLE_PS3_MOUSE
}
	
CIrrDevicePS3::~CIrrDevicePS3()
{
#if ENABLE_PS3_MOUSE
	cellMouseEnd();
#endif //ENABLE_PS3_MOUSE
}

void CIrrDevicePS3::closeDevice()
{
}

bool CIrrDevicePS3::createWindow()
{
	
	return (true);
}

//void CIrrDevicePS3::setResize(int width,int height)
//{
//}

void CIrrDevicePS3::createDriver()
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
			os::Printer::log("This driver is not available in PS3. Try OpenGL or Software renderer.", ELL_ERROR);
			break;

		case video::EDT_NULL:
			VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
			break;

		default:
			os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
			break;
	}
}
void CIrrDevicePS3::flush()
{
}
#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
core::SingleProducerConsumer<IReferenceCounted*> _delyFlushList;	
bool CIrrDevicePS3::flushVRM()
{
    IReferenceCounted** ppObject;
    bool ret=false;
    while(ppObject=_delyFlushList.ReadLock())
    {
        (*ppObject)->flushVRM();
        (*ppObject)->drop();
        *ppObject = 0;
        _delyFlushList.ReadUnlock();
        ret = true;
    }
    return ret;
}

void pushDelyFlush(IReferenceCounted* object)
{
    if(object)
    {
        IReferenceCounted** ppObject = _delyFlushList.WriteLock();
        *ppObject = object;
        object->grab();
        _delyFlushList.WriteUnlock();
        int size;
        while((size =_delyFlushList.Size()) > 1)
            sys_timer_usleep(size*(usecond_t)10000);
    }
}
static sys_ppu_thread_t _mainThread=0;
bool isNeedDelyFlush()
{
    sys_ppu_thread_t thread_id=0;
    int ret = sys_ppu_thread_get_id(&thread_id);
    if(ret!=CELL_OK)
    {
        os::Printer::log("Error: sys_ppu_thread_get_id", ELL_ERROR);
    }
    return _mainThread != thread_id;
}
#endif 
static bool _request_exit = false;
static void cellSysutilCallback(uint64_t status, uint64_t param, void * userdata)
{
    (void) param;
	(void) userdata;
	if(status == CELL_SYSUTIL_REQUEST_EXITGAME)
	{
        _request_exit = true;
    }
    else if (status == CELL_SYSUTIL_SYSTEM_MENU_CLOSE)
    {
    }
    else if (status == CELL_SYSUTIL_SYSTEM_MENU_OPEN)
    {
    }
}

bool CIrrDevicePS3::run()
{
	os::Timer::tick();
    int ret = cellSysutilCheckCallback();
    if (ret != CELL_OK)
    {
        os::Printer::log("Error: cellSysutilCheckCallback", ELL_ERROR);
        return false;
    }

#if ENABLE_PS3_MOUSE
	CellMouseInfo mouseInfo;
	if ( cellMouseGetInfo(&mouseInfo) == CELL_OK )
	{
		if ( ( mouseInfo.info != CELL_MOUSE_INFO_INTERCEPTED ) // mouse used by gpad
			&& mouseInfo.status[0] ) // connected
		{

			CellMouseDataList mouseDataList;
			if ( cellMouseGetDataList(0, &mouseDataList ) == CELL_MOUSE_OK )
			{
				for ( unsigned i = 0 ; i < mouseDataList.list_num ; i++ )
				{
					CellMouseData& mouseData = mouseDataList.list[ i ];
					if ( memcmp( &mouseData, &s_lastMouseData, sizeof(CellMouseData) ) != 0 )
					{
						mousePosX += mouseData.x_axis;
						mousePosY += mouseData.y_axis;

						irr::SEvent event;
						event.EventType = irr::EET_MOUSE_INPUT_EVENT;
						event.ControllerIndex = 0;	
						event.MouseInput.X = mousePosX;
						event.MouseInput.Y = mousePosY;
						
						if ( mouseData.x_axis || mouseData.y_axis )
						{
							event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
							postEventFromUser(event);
						}

						int buttondiff = mouseData.buttons ^ s_lastMouseData.buttons;
						
						int buttonLeft = 1;
						if ( buttondiff & buttonLeft )
						{
							event.MouseInput.Event = ( mouseData.buttons & buttonLeft ) ? irr::EMIE_LMOUSE_PRESSED_DOWN : irr::EMIE_LMOUSE_LEFT_UP ;
							//event.MouseInput.X = mouseData.buttons;
							//event.MouseInput.Y = 0;
							postEventFromUser(event);
						}
						
						int buttonRight = 1 << 1;
						if ( buttondiff & buttonRight )
						{
							event.MouseInput.Event = ( mouseData.buttons & buttonRight ) ? irr::EMIE_RMOUSE_PRESSED_DOWN : irr::EMIE_RMOUSE_LEFT_UP ;
							//event.MouseInput.X = mouseData.buttons;
							//event.MouseInput.Y = 0;
							postEventFromUser(event);
						}
					}
					s_lastMouseData = mouseData;
				}
			}
		}
	}
#endif //ENABLE_PS3_MOUSE

    return !_request_exit;

}
//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDevicePS3::yield()
{
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDevicePS3::sleep(u32 timeMs, bool pauseTimer=false)
{
}

void CIrrDevicePS3::setWindowCaption(const wchar_t* text)
{

}


bool CIrrDevicePS3::isWindowActive() const
{
	return (true);
}


bool CIrrDevicePS3::isWindowFocused() const
{
	// TODO: return proper value
	return true;
}


bool CIrrDevicePS3::isWindowMinimized() const
{
	// TODO: return proper value
	return false;
}


//void CIrrDevicePS3::postKeyEvent(void *event,irr::SEvent &ievent,bool pressed)
//{
//}
//
//void CIrrDevicePS3::postMouseEvent(void *event,irr::SEvent &ievent)
//{
//}
//
//void CIrrDevicePS3::storeMouseLocation()
//{
//}
//
//void CIrrDevicePS3::setMouseLocation(int x,int y)
//{
//}
//
//void CIrrDevicePS3::setCursorVisible(bool visible)
//{
//
//}
//
//void CIrrDevicePS3::initKeycodes()
//{
//}

//! Sets if the window should be resizeable in windowed mode.

void CIrrDevicePS3::setResizeAble(bool resize)
{
	// todo: implement resize
}

bool CIrrDevicePS3::present(video::IImage* surface, void* windowId, core::rect<s32>* src )
{
	return true;// todo: implement 
}

video::IVideoModeList* CIrrDevicePS3::getVideoModeList()
{
		return NULL;
}
static void loadModules()
{
	//int ret = cellSysutilRegisterCallback(SYS_CALLBACK_INDEX_0, cellSysutilCallback, NULL);
	int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_GCM_SYS);
    if (ret != CELL_OK) _IRR_THROW ("GCM SYS Fail.\n");

    ret = cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
    if (ret != CELL_OK) _IRR_THROW ("FS Fail.\n");

    ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_GAME);
    if (ret != CELL_OK) _IRR_THROW ("SYSUTIL_GAME Fail.\n");

    ret = cellSysmoduleLoadModule(CELL_SYSMODULE_USBD);
    if (ret != CELL_OK) _IRR_THROW ("USBD Fail.\n");

    ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP);
    if (ret != CELL_OK) _IRR_THROW ("NP Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SAIL);
	if (ret != CELL_OK) _IRR_THROW ("SAIL Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_RTC);
	if (ret != CELL_OK) _IRR_THROW ("RTC Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
	if (ret != CELL_OK) _IRR_THROW ("NET Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_NETCTL);
	if (ret != CELL_OK) _IRR_THROW ("NETCTL Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP2);
	if (ret != CELL_OK) _IRR_THROW ("SYSUTIL_NP2 Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_USERINFO);
	if (ret != CELL_OK) _IRR_THROW ("SYSUTIL_USERINFO Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
	if (ret != CELL_OK) _IRR_THROW ("IO Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_RESC);
	if (ret != CELL_OK) _IRR_THROW ("RESC Fail.\n");
	
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_AUDIO);
	if (ret != CELL_OK) _IRR_THROW ("RESC Fail.\n");
	
	ret = cellSysmoduleLoadModule( CELL_SYSMODULE_L10N );
	if ( ret != CELL_OK ) _IRR_THROW ("L10N Fail.\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
	if(ret != CELL_OK)
		_IRR_THROW("failed:cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC) return 0x%X\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SPURS);
	if(ret != CELL_OK)
		_IRR_THROW("failed:cellSysmoduleLoadModule(CELL_SYSMODULE_SPURS) return 0x%X\n");

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_RESC);
	if(ret != CELL_OK)
		_IRR_THROW("failed:cellSysmoduleLoadModule(CELL_SYSMODULE_RESC) return 0x%X\n");
}

#include <sys/spu_initialize.h>
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <GLES/glext.h>
GLuint	_screenScanMode = 0;

static void initPSGL(const SIrrlichtCreationParameters& device_param)
{
    sys_spu_initialize(6, 1);

	PSGLinitOptions initOpts={
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
		maxSPUs: 1,
		initializeSPUs: false,
				// We're not specifying values for these options, the code is only here
				// to alleviate compiler warnings.
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,
		  // M-O: For some reason, using 128mb below give us GL_OUT_OF_MEMORY erros...
		  // so setting to default "0" for now seems to fix it.
		hostMemorySize: device_param.HostMemorySize
		//RESERVED_FRAME_SIZE
	};
//	psglAllocTraceFuncInit();
    psglInit(&initOpts);

	CellVideoOutState videoState;
	cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState);
	
	PSGLdeviceParameters params;
	memset(&params,0,sizeof(PSGLdeviceParameters));
	params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT | 
		PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT | 
		PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE | 
		PSGL_DEVICE_PARAMETERS_BUFFERING_MODE; 
	params.bufferingMode = device_param.BufferingMode == irr::video::EBM_DOUBLE ? PSGL_BUFFERING_MODE_DOUBLE : PSGL_BUFFERING_MODE_TRIPLE;
	params.colorFormat = GL_ARGB_SCE; 
	params.depthFormat = device_param.ZBufferBits == 16 ? GL_DEPTH_COMPONENT16 : GL_DEPTH_COMPONENT24;

	switch(device_param.AntiAliasSetting)
	{
	case irr::video::EAA_2X:
		params.multisamplingMode = GL_MULTISAMPLING_2X_DIAGONAL_CENTERED_SCE;
		break;
	case irr::video::EAA_4X:
		params.multisamplingMode = GL_MULTISAMPLING_4X_SQUARE_CENTERED_SCE;
		break;
	case irr::video::EAA_4XR:
		params.multisamplingMode = GL_MULTISAMPLING_4X_SQUARE_ROTATED_SCE;
		break;
	default:
		params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;
		break;
	}

	// libresc to give support to SD screens
	if (videoState.displayMode.resolutionId == CELL_VIDEO_OUT_RESOLUTION_480)
	{
		params.enable |= PSGL_DEVICE_PARAMETERS_RESC_RATIO_MODE;
		params.enable |= PSGL_DEVICE_PARAMETERS_RESC_RENDER_WIDTH_HEIGHT;
		params.rescRatioMode = RESC_RATIO_MODE_FULLSCREEN;
		params.rescInterlaceMode = RESC_INTERLACE_MODE_INTERLACE_FILTER;
		if (videoState.displayMode.aspect == CELL_VIDEO_OUT_ASPECT_16_9)
		{
			// define a 16:9 framebuffer
			params.renderWidth = 864;
			params.renderHeight  = 486;
		}
		else
		{
			// define a 4:3 framebuffer
			params.renderWidth = 800;
			params.renderHeight  = 600;
		}
	}

	//scan mode, interlace or progress
	if (videoState.displayMode.scanMode == CELL_VIDEO_OUT_SCAN_MODE_INTERLACE)
	{
		_screenScanMode = 0;
	}
	else
	{
		_screenScanMode = 1;
	}

	device = psglCreateDeviceExtended(&params); 

	if ( !device ) _IRR_THROW("!! Failed to init the device \n!! Exiting Program \n"); 

    PSGLcontext *pContext=psglCreateContext();
    if (pContext==NULL) _IRR_THROW("Error creating PSGL context\n");
    psglMakeCurrent(pContext, device);
    psglResetCurrentContext();

	// enable vsync
	if (device_param.Vsync)
	{
		glEnable(GL_VSYNC_SCE);
	}
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(const SIrrlichtCreationParameters& param)
{
    int ret;
    loadModules();
    initPSGL(param);
#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
    ret = sys_ppu_thread_get_id(&_mainThread);
    if(ret!=CELL_OK)
    {
        os::Printer::log("Error: sys_ppu_thread_get_id", ELL_ERROR);
        return NULL;
    }
#endif
    ret = cellSysutilRegisterCallback(0, cellSysutilCallback, NULL);
    if(ret!=CELL_OK)
    {
        os::Printer::log("Error: cellSysutilRegisterCallback", ELL_ERROR);
        return NULL;
    }
	CIrrDevicePS3* dev = irrnew CIrrDevicePS3(param);

	if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
	{
		dev->drop();
		dev = 0;
	}
	return dev;
}

}

#endif // _IRR_USE_PS3_DEVICE_

