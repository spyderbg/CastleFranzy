#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceWii.h"

#ifdef _IRR_USE_WII_DEVICE_

#include "screenshot.h"
#include "ISceneManager.h"
#include "IEventReceiver.h"
#include "IFileSystem.h"
#include "IGUIEnvironment.h"
#include "irros.h"
#include "IrrCompileConfig.h"
#include "CTimer.h"
#include "CLogger.h"
#include "irrString.h"
#include "SIrrCreationParameters.h"

#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/gx.h> // GXPokeARGB
//#include <nw4r/math/types.h>

//const int SCRNSHOT_SHARE_MEMORY_SIZE = 8 * 1024; //0x2000

namespace irr
{

#define PAL_XFB_HEIGHT 560

GXRenderModeObj Pal560to480Aa =
{
    VI_TVMODE_PAL_INT,							// viTVmode
    640,										// fbWidth
    242,										// efbHeight
    PAL_XFB_HEIGHT,								// xfbHeight
    (VI_MAX_WIDTH_PAL - 640) >> 1,				// viXOrigin
    (VI_MAX_HEIGHT_PAL - PAL_XFB_HEIGHT) >> 1,	// viYOrigin
    640,										// viWidth
    PAL_XFB_HEIGHT,								// viHeight
    VI_XFBMODE_DF,								// xFBmode
    GX_FALSE,									// field_rendering
    GX_TRUE,									// aa
	// sample_pattern[12][2]
	3, 2, 9, 6, 3, 10, 3, 2, 9, 6, 3, 10,
	9, 2, 3, 6, 9, 10, 9, 2, 3, 6, 9, 10,
	// vfilter[7]
    4, 8, 12, 16, 12, 8, 4
};

GXRenderModeObj Pal560to480Int =
{
    VI_TVMODE_PAL_INT,							// viTVmode
    640,										// fbWidth
    480,										// efbHeight
    PAL_XFB_HEIGHT,								// xfbHeight
    (VI_MAX_WIDTH_PAL - 640) >> 1,				// viXOrigin
    (VI_MAX_HEIGHT_PAL - PAL_XFB_HEIGHT) >> 1,	// viYOrigin
    640,										// viWidth
    PAL_XFB_HEIGHT,								// viHeight
    VI_XFBMODE_DF,								// xFBmode
    GX_FALSE,									// field_rendering
    GX_FALSE,									// aa
	// sample_pattern[12][2]
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	// vfilter[7]
    0, 0, 21, 22, 21, 0, 0
};

#define VI_WIDTH_USED			670
#define VI_WIDTH_WIDE_USED		700
#define VI_WIDTH_WIDE_PAL_USED	700

#define SAFE_FRAME_W_PERCENT	0.844
#define SAFE_FRAME_H_PERCENT    0.808

#define SCREEN_HEIGHT			480
#define SCREEN_WIDTH			640

const u32 k_2PassRendering_copy_overlap = 2;
const int DEFAULT_GX_FIFO_SIZE = (256*1024);

//static u8 s_garbage[640 * k_2PassRendering_copy_overlap * VI_DISPLAY_PIX_SZ] ATTRIBUTE_ALIGN(32);

//! constructor
CIrrDeviceWii::CIrrDeviceWii(const SIrrlichtCreationParameters& parameters)
	: CIrrDeviceStub(parameters)
	, DriverType(parameters.DriverType)
	, Vsync(parameters.Vsync)
	, FifoBuf(0)
	, FifoBufObj(0)
	, Fb1(0) 
	, Fb2(0)
	, Currentfb(0)
	, CurrentRenderModeObj(0)
	, RenderModeObj(0)
	, RenderModeObjAA(0)
	, IsScreenCaptureEnabled(true)
	, ScreenshotWorkingBuffer(0)
{
	VIInit();
	SetVIMode();
	SetVIScreen();

#if ENABLE_AA //&& !defined(WIN32)
	SetAAMode(true);
	SetFrameBuffer(true);
#else
	SetAAMode(false);
	SetFrameBuffer(false);
#endif
	
	StartVI();

	// create driver
	createDriver(core::dimension2d<s32>(640, 480), Vsync);

	if (!VideoDriver)
		return;

	createGUIAndScene();
}


CIrrDeviceWii::~CIrrDeviceWii()
{
}

void CIrrDeviceWii::setScreenCapture(bool activate) 
{ 
	IsScreenCaptureEnabled = activate;
	
	if(IsScreenCaptureEnabled) {
	} else {
	}
}

//! returns the video driver
video::IVideoDriver* CIrrDeviceWii::getVideoDriver()
{
	return VideoDriver;
}



//! return file system
io::IFileSystem* CIrrDeviceWii::getFileSystem()
{
	return FileSystem;
}



//! returns the gui environment
gui::IGUIEnvironment* CIrrDeviceWii::getGUIEnvironment()
{
	return GUIEnvironment;
}



//! returns the scene manager
scene::ISceneManager* CIrrDeviceWii::getSceneManager()
{
	return SceneManager;
}


//! \return Returns a pointer to the ITimer object. With it the
//! current Time can be received.
ITimer* CIrrDeviceWii::getTimer()
{
	return Timer;
}


//! Returns the version of the engine. 
const char* CIrrDeviceWii::getVersion() const
{
	return IRRLICHT_SDK_VERSION;
}

//! \return Returns a pointer to the mouse cursor control interface.
gui::ICursorControl* CIrrDeviceWii::getCursorControl()
{
	return CursorControl;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
video::IVideoModeList* CIrrDeviceWii::getVideoModeList()
{
	return &VideoModeList;
}


//! send the event to the right receiver
bool CIrrDeviceWii::postEventFromUser(const SEvent& event)
{
	bool absorbed = false;

	if (UserReceiver)
		absorbed = UserReceiver->OnEvent(event);

	if (!absorbed && GUIEnvironment)
		absorbed = GUIEnvironment->postEventFromUser(event);

	scene::ISceneManager* inputReceiver = InputReceivingSceneManager;
	if (!inputReceiver)
		inputReceiver = SceneManager;

	if (!absorbed && inputReceiver)
		absorbed = inputReceiver->postEventFromUser(event);

	return absorbed;
}


//! Sets a new event receiver to receive events
void CIrrDeviceWii::setEventReceiver(IEventReceiver* receiver)
{
	UserReceiver = receiver;
	Logger->setReceiver(receiver);
	if (GUIEnvironment)
		GUIEnvironment->setUserEventReceiver(receiver);
}


//! Returns poinhter to the current event receiver. Returns 0 if there is none.
IEventReceiver* CIrrDeviceWii::getEventReceiver()
{
	return UserReceiver;
}


//! \return Returns a pointer to the logger.
ILogger* CIrrDeviceWii::getLogger()
{
	return Logger;
}


//! Returns the operation system opertator object.
IOSOperator* CIrrDeviceWii::getOSOperator()
{
	return Operator;
}


//! Sets the input receiving scene manager. 
void CIrrDeviceWii::setInputReceivingSceneManager(scene::ISceneManager* sceneManager)
{
	if (InputReceivingSceneManager)
		InputReceivingSceneManager->drop();

	InputReceivingSceneManager = sceneManager;

	if (InputReceivingSceneManager)
		InputReceivingSceneManager->grab();
}

//! presents a surface in the client area
bool CIrrDeviceWii::present(video::IImage* image, void * windowId, core::rect<s32>* src )
{
	for(int i = 0; i < 480; i++)
	{
		for(int j = 0; j < 640; j++)
		{
			video::SColor clr = image->getPixel(j, i);
			GXPokeARGB(j, i, clr.toARGB8());
		}
	}	
	swapBuffer();
	UNIMPLEMENTED(__FUNCTION__);
	return true;
}

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWii::run()
{
	os::Timer::tick();
	UNIMPLEMENTED(__FUNCTION__);
	return true;
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceWii::yield()
{
	UNIMPLEMENTED(__FUNCTION__);
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceWii::sleep(u32 timeMs, bool pauseTimer)
{
	bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();
	
	UNIMPLEMENTED("Sleep(timeMs)");
	//Sleep(timeMs);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

//! notifies the device that it should close itself
void CIrrDeviceWii::closeDevice()
{
	UNIMPLEMENTED(__FUNCTION__);
}

//! create the driver
void CIrrDeviceWii::createDriver(const core::dimension2d<s32>& windowSize,
					bool vsync)
					
{
	switch(DriverType)
	{
	case video::EDT_WII:
		VideoDriver = video::createWiiDriver(FileSystem, windowSize, this);
		break;

	case video::EDT_OPENGL:
	#ifdef _IRR_COMPILE_WITH_OPENGL_
		if (Context)
			VideoDriver = video::createOpenGLDriver(windowSize, true, StencilBuffer, FileSystem, vsync, AntiAlias);
	#else
		os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
	#endif
		break;

	case video::EDT_DIRECT3D8:
	case video::EDT_DIRECT3D9:
		os::Printer::log("Direct3D drivers are not available on the Wii. Try Wii renderer.",
			ELL_ERROR);
		break;

	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, windowSize);
		break;

	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}


	//VideoDriver = video::createWiiDriver(FileSystem, windowSize);
	if (!VideoDriver)
	{
		os::Printer::log("Could not create video driver.", ELL_ERROR);
	}
}

void CIrrDeviceWii::SetVIMode()
{
	u8 mode = SCGetProgressiveMode();
	IsProgressiveScan = (SC_PROGRESSIVE_MODE_ON == mode);

	bool bHighDefCable = (VIGetDTVStatus() == 1);
	
	if(IsProgressiveScan && bHighDefCable)
	{
		IsProgressiveScan = true;
		//TRACE("PROGRESSIVE SCAN ENABLED");
	}
	else
	{
		IsProgressiveScan = false;
		//TRACE("PROGRESSIVE SCAN DISABLED");
	}
	
	switch (VIGetTvFormat())
    {
		case VI_NTSC:
		{
			RenderModeObj = (IsProgressiveScan ? &GXNtsc480Prog : &GXNtsc480Int);
			RenderModeObjAA = (IsProgressiveScan ? &GXNtsc480ProgAa : &GXNtsc480IntAa);			
			Init2PassRenderingConfig();
        	
        	if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
        	{	
	        	RenderModeObj->viWidth = VI_WIDTH_WIDE_USED;
        	}
        	else
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_USED;
        	}
        	
        	RenderModeObj->viXOrigin = (VI_MAX_WIDTH_NTSC - RenderModeObj->viWidth) / 2;
			
        	RenderModeObjAA->viWidth = RenderModeObj->viWidth;
	        RenderModeObjAA->viXOrigin = RenderModeObj->viXOrigin;

			VIMaxWidth = VI_MAX_WIDTH_NTSC;
			VIMaxHeight = VI_MAX_HEIGHT_NTSC;

			//ConstantFps::GetInstance()->SetTargetFps(TARGET_FPS_NTSC);
        	
        }	break;
        	
      	case VI_EURGB60:
		{
			RenderModeObj = (IsProgressiveScan ? &GXEurgb60Hz480Prog : &GXEurgb60Hz480Int);
			RenderModeObjAA = (IsProgressiveScan ? &GXEurgb60Hz480ProgAa : &GXEurgb60Hz480IntAa);			
			Init2PassRenderingConfig();

        	if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_WIDE_USED;
        	}
        	else
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_USED;
        	}
        	
	        RenderModeObj->viXOrigin = (VI_MAX_WIDTH_EURGB60 - RenderModeObj->viWidth) / 2;
			
        	RenderModeObjAA->viWidth = RenderModeObj->viWidth;
	        RenderModeObjAA->viXOrigin = RenderModeObj->viXOrigin;

			VIMaxWidth = VI_MAX_WIDTH_EURGB60;
			VIMaxHeight = VI_MAX_HEIGHT_EURGB60;

			//ConstantFps::GetInstance()->SetTargetFps(TARGET_FPS_EURGB60);

        }	break;
        	
       	case VI_PAL:
		{
			RenderModeObj = &Pal560to480Int;
			RenderModeObjAA = &Pal560to480Aa; 		
			Init2PassRenderingConfig();
			
        	if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_WIDE_USED;
        	}
        	else
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_USED;
        	}
        	
        	RenderModeObj->viXOrigin = (VI_MAX_WIDTH_PAL - RenderModeObj->viWidth) / 2;
			
        	RenderModeObjAA->viWidth = RenderModeObj->viWidth;
	        RenderModeObjAA->viXOrigin = RenderModeObj->viXOrigin;

			VIMaxWidth = VI_MAX_WIDTH_PAL;
			VIMaxHeight = VI_MAX_HEIGHT_PAL;

			//ConstantFps::GetInstance()->SetTargetFps(TARGET_FPS_PAL);
			
        }	break;
		
      	case VI_MPAL:
		{
			RenderModeObj = (IsProgressiveScan ? &GXMpal480Prog : &GXMpal480Int);
			RenderModeObjAA = (IsProgressiveScan ? &GXMpal480ProgAa : &GXMpal480IntAa);			
			Init2PassRenderingConfig();
			
        	if (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_WIDE_USED;
        	}
        	else
        	{
	        	RenderModeObj->viWidth = VI_WIDTH_USED;
        	}
        	
        	RenderModeObj->viXOrigin = (VI_MAX_WIDTH_MPAL - RenderModeObj->viWidth) / 2;
			
        	RenderModeObjAA->viWidth = RenderModeObj->viWidth;
	        RenderModeObjAA->viXOrigin = RenderModeObj->viXOrigin;

			VIMaxWidth = VI_MAX_WIDTH_MPAL;
			VIMaxHeight = VI_MAX_HEIGHT_MPAL;

			//ConstantFps::GetInstance()->SetTargetFps(TARGET_FPS_MPAL);
        	
        }	break;
        	
      	default:
        	OSHalt("SystemInit: invalid TV format\n");
        	break;
    }

	SCREENSHOTSetRenderModeObj(RenderModeObj);

    IsScanChanged = true;

}

void CIrrDeviceWii::SetVIScreen()
{
	u8 aspectRatio = SCGetAspectRatio();
	
	IsWidescreenMode = (aspectRatio == SC_ASPECT_RATIO_16x9);
	if(IsWidescreenMode)
	{
		//TRACE("Game is running in 16/9 mode");
	}
	else
	{
		//TRACE("Game is running in 4/3 mode");
	}
}

void CIrrDeviceWii::SetFrameBuffer(bool AAMode)
{		
	CurrentRenderModeObj = AAMode ? RenderModeObjAA : RenderModeObj;
	CurrentAAMode = NewAAMode = AAMode;


	ASSERT(CurrentRenderModeObj != 0);
		
	//Setup Fifo and frame buffers.
	s32 fbSize = VIPadFrameBufferWidth(CurrentRenderModeObj->fbWidth) * CurrentRenderModeObj->xfbHeight * 
        (u32)VI_DISPLAY_PIX_SZ;

    if(FifoBuf)
	{
		delete [] FifoBuf;
	}
	FifoBuf = irrnewh (MEMHINT_ALIGN32|MEMHINT_FAST) char[DEFAULT_GX_FIFO_SIZE];
	
    if(Fb1)
	{
		delete [] Fb1;
	}
	Fb1 = irrnewh (MEMHINT_ALIGN32|MEMHINT_FAST) char[fbSize];
    if(Fb2)
	{
		delete [] Fb2;
	}
	Fb2 = irrnewh (MEMHINT_ALIGN32|MEMHINT_FAST) char[fbSize];


	
	
#if ENABLEBLOOM 
    if(Bloomfb)
	{
		delete [] (unsigned char*)Bloomfb;
	}
	Bloomfb = (void*)irrnew unsigned char[fbSize];


	//Setup texture buffers
	u32 texBufferSize = GXGetTexBufferSize((u16)CurrentRenderModeObj->fbWidth, (u16)CurrentRenderModeObj->xfbHeight, GX_TF_RGBA8, GX_FALSE, 0);
    if(Bloomtex)
	{
		delete [] (unsigned char*)Bloomtex;
	}
	Bloomtex = irrnew unsigned char[texBufferSize];
    if(Bloomtex2)
	{
		delete [] (unsigned char*)Bloomtex2;
	}
	Bloomtex2 = irrnew unsigned char[texBufferSize];
#endif

    if(ScreenshotWorkingBuffer)
	{
		delete [] ScreenshotWorkingBuffer;
	}
	ScreenshotWorkingBuffer = irrnew char[SCRNSHOT_SHARE_MEMORY_SIZE];

#if ENABLE_SCR_PRINTF
	InitDbgFont();
#endif


	FifoBufObj = GXInit(FifoBuf, DEFAULT_GX_FIFO_SIZE);
	
	IsFirstFrame = GX_TRUE;
	u16     xfbHeight;

    /*----------------------------------------------------------------*
     *  GX configuration by a render mode obj                         *
     *----------------------------------------------------------------*/
    // These are necessary function calls to take a render mode
    // object and set up relevant GX configuration.

 //   GXSetViewport(0.0F, 0.0F, (f32)RenderModeObj->fbWidth, (f32)RenderModeObj->efbHeight, 
//                  0.0F, 1.0F);
                  
 //   GXSetScissor(0, 0, (u32)RenderModeObj->fbWidth, (u32)RenderModeObj->efbHeight);

    GXSetDispCopySrc(0, 0, CurrentRenderModeObj->fbWidth, CurrentRenderModeObj->efbHeight);
    GXSetCopyFilter(CurrentRenderModeObj->aa, CurrentRenderModeObj->sample_pattern, GX_TRUE, CurrentRenderModeObj->vfilter);
//    GXSetDispCopyGamma(GX_GM_1_0);

	if (CurrentRenderModeObj->aa)
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    // Note that following is an appropriate setting for full-frame AA mode.
    // You should use "xfbHeight" instead of "efbHeight" to specify actual
    // view size. Since this library doesn't support such special case, please
    // call these in each application to override the normal setting.
    YScale = GXGetYScaleFactor(RenderModeObj->efbHeight, RenderModeObj->xfbHeight);
    xfbHeight = (u16)GXSetDispCopyYScale(YScale);
    GXSetDispCopyDst(CurrentRenderModeObj->fbWidth, xfbHeight);

	GXColor clearColor = {0, 0, 0, 0};
    GXSetCopyClear(clearColor, GX_MAX_Z24);
    // Clear embedded framebuffer for the first frame
    GXCopyDisp(Fb2, GX_TRUE);	
    Currentfb = Fb1;

	UpdateSafeFrameBorder();
}

void CIrrDeviceWii::StartVI()
{
	// To avoid distorted image
	VISetBlack(TRUE);
	
	// Configure VI with given render mode
	VIConfigure(CurrentRenderModeObj);
    
   	// Need to "flush" so that the VI changes made so far take effetc
   	VIFlush();
   	VIWaitForRetrace();

    // Because of hardware restriction, we need to wait one more
    // field to make sure mode is safely changed when we change
    // INT->DS or DS->INT. (VIInit() sets INT mode as a default)
    if(IsScanChanged)
    {
    	VIWaitForRetrace();
    	IsScanChanged = false;
    }  
}

void CIrrDeviceWii::Init2PassRenderingConfig()
{
	ASSERT(RenderModeObjAA && RenderModeObjAA->aa);
	
	//u32 half_ht = (RenderModeObjAA->xfbHeight / 2);
	u32 half_ht = (RenderModeObjAA->efbHeight - k_2PassRendering_copy_overlap);
	
	CopyWidth = RenderModeObjAA->fbWidth;
	
	Path1CopyStartY = 0;
	Path1CopyHeight = half_ht + 2;
	
	Path2CopyStartY = half_ht - 2;
	Path2CopyHeight = half_ht + 2;

	Path1ScissorY = 0;
	Path2ScissorY = half_ht-2;

	// Calculate copy parameters for bottom half of the screen
	Path2copyLines = (RenderModeObjAA->efbHeight - k_2PassRendering_copy_overlap);
	u16 xfbHalfHeight = RenderModeObjAA->xfbHeight / 2;
	BufferOffset = VIPadFrameBufferWidth(RenderModeObjAA->fbWidth) * xfbHalfHeight * VI_DISPLAY_PIX_SZ;
}

void CIrrDeviceWii::GetScreenSize(float& w, float& h) const
{
	// to avoid black gap 
	//f32 width = (IsWidescreenMode ? CurrentRenderModeObj->viWidth * 1.20f : CurrentRenderModeObj->viWidth);
	// f32 width = (IsWidescreenMode ? CurrentRenderModeObj->fbWidth * 1.20f : CurrentRenderModeObj->fbWidth);
	w = RenderModeObj->fbWidth;
	//if ( IsWidescreenMode ) { screenSize.x *= 1.333333; }
	h = RenderModeObj->efbHeight;	//efb from AA is half
}

void CIrrDeviceWii::UpdateSafeFrameBorder()
{
	float w, h;
	GetScreenSize(w, h);

	float safeFrameW = SAFE_FRAME_W_PERCENT * VIMaxWidth;
	float safeFrameH = SAFE_FRAME_H_PERCENT * VIMaxHeight;

	SafeFrameBorderWidth = (w / 2) * (1.0 - (float)safeFrameW / (float)CurrentRenderModeObj->viWidth);
	SafeFrameBorderHeight = (h / 2) * (1.0 - (float)safeFrameH / (float)CurrentRenderModeObj->viHeight);
}

void CIrrDeviceWii::swapBuffer(bool clearBuffer)
{
   	//GXDrawDone(); 

	// Set Z/Color update to make sure eFB will be cleared at GXCopyDisp.
	// (If you want to control these modes by yourself in your application,
	//  please comment out this part.)
	GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
	GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
	GXSetColorUpdate(GX_ENABLE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
	GXSetCullMode(GX_CULL_FRONT);
	// Issue display copy command

	if (IsScreenCaptureEnabled)
	{
		u32 fbSize = VIPadFrameBufferWidth(CurrentRenderModeObj->fbWidth) * CurrentRenderModeObj->xfbHeight * 
        (u32)VI_DISPLAY_PIX_SZ;

		SCREENSHOTService(Currentfb, 
						  ScreenshotWorkingBuffer, 
						  SCRNSHOT_SHARE_MEMORY_SIZE);
		
	}

	GXCopyDisp(Currentfb, clearBuffer);
	// Wait until everything is drawn and copied into XFB.
	GXDrawDone(); 

	// Display the buffer which was just filled by GXCopyDisplay
    VISetNextFrameBuffer(Currentfb);

    // If this is the first frame, turn off VIBlack
    if(IsFirstFrame)
    {
        VISetBlack(GX_FALSE); 
        IsFirstFrame = GX_FALSE;
    }
    
    // Tell VI device driver to write the current VI settings so far
    VIFlush();
    
    // Wait for vertical retrace.
	if(Vsync)
	{
		VIWaitForRetrace();
	}

    // Swap buffers
    if(Currentfb == Fb1)
        Currentfb = Fb2;
    else
        Currentfb = Fb1;
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(
	const SIrrlichtCreationParameters& parameters)
{
	CIrrDeviceWii* dev = irrnew CIrrDeviceWii(parameters);
	/*
		parameters.DriverType,
		parameters.WindowSize,
		parameters.Bits,
		parameters.Fullscreen,
		parameters.Stencilbuffer,
		parameters.Vsync,
		parameters.AntiAlias,
		parameters.HighPrecisionFPU,
		parameters.EventReceiver,
		reinterpret_cast<HWND>(parameters.WindowId),
		);*/

	if (dev && !dev->getVideoDriver() && parameters.DriverType != video::EDT_NULL)
	{
		dev->closeDevice(); // destroy window
		dev->run(); // consume quit message
		dev->drop();
		dev = 0;
	}

	return dev;
}


} // end namespace irr


#endif // _IRR_USE_WII_DEVIC_E
