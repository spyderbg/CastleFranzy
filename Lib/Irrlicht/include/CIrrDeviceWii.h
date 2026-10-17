// Copyright (C) 2002-2009 Gameloft
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_WII_H_INCLUDED__
#define __C_IRR_DEVICE_WII_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_WII_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include <revolution/gx.h>

namespace irr
{
	class CIrrDeviceWii;
	namespace video
	{
		IVideoDriver* createWiiDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize, CIrrDeviceWii *pDevice);
	}

} // end namespace irr

namespace irr
{
	// lots of prototypes:
	class ILogger;
	class CLogger;

	//! Stub for an Irrlicht Device implementation
	class CIrrDeviceWii : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceWii(const SIrrlichtCreationParameters& parameters);

		//! destructor
		virtual ~CIrrDeviceWii();

		//! returns the video driver
		virtual video::IVideoDriver* getVideoDriver();

		//! return file system
		virtual io::IFileSystem* getFileSystem();

		//! returns the gui environment
		virtual gui::IGUIEnvironment* getGUIEnvironment();

		//! returns the scene manager
		virtual scene::ISceneManager* getSceneManager();

		//! \return Returns a pointer to the mouse cursor control interface.
		virtual gui::ICursorControl* getCursorControl();

		//! \return Returns a pointer to a list with all video modes supported
		//! by the gfx adapter.
		virtual video::IVideoModeList* getVideoModeList();

		//! \return Returns a pointer to the ITimer object. With it the
		//! current Time can be received.
		virtual ITimer* getTimer();

		//! Returns the version of the engine. 
		virtual const char* getVersion() const;

		//! send the event to the right receiver
		virtual bool postEventFromUser(const SEvent& event);

		//! Sets a new event receiver to receive events
		virtual void setEventReceiver(IEventReceiver* receiver);

		//! Returns poinhter to the current event receiver. Returns 0 if there is none.
		virtual IEventReceiver* getEventReceiver();

		//! Sets the input receiving scene manager. 
		/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
		virtual void setInputReceivingSceneManager(scene::ISceneManager* sceneManager);

		//! \return Returns a pointer to the logger.
		virtual ILogger* getLogger();

		//! Returns the operation system opertator object.
		virtual IOSOperator* getOSOperator();

		//! presents a surface in the client area
		virtual bool present(video::IImage* image, void * , core::rect<s32>* src );

		//! runs the device. Returns false if device wants to be deleted
		bool run();

		//! Pause the current process for the minimum time allowed only to allow other processes to execute
		void yield();

		//! Pause execution and let other processes to run for a specified amount of time.
		void sleep(u32 timeMs, bool pauseTimer);

		//! Sets the caption of the window.
		/** \param text: New text of the window caption. */
		virtual void setWindowCaption(const wchar_t* text) {};

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const { return true; }

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizeAble(bool resize) {};

		virtual bool isWindowFocused(void) const { return false; }

		virtual bool isWindowMinimized(void) const { return false; }
		
		virtual void setScreenCapture(bool activate);

		const GXRenderModeObj* getRenderModeObj() const
		{
			return CurrentRenderModeObj;
		}
		
		void* GetCurrentFrameBuffer() {
			return Currentfb;
		}

	protected:
		//! create the driver
		void createDriver(const core::dimension2d<s32>& windowSize,
					bool vsync);

		video::E_DRIVER_TYPE DriverType;

		void Init2PassRenderingConfig();

		void StartVI();
		void SetVIMode();
		void SetVIScreen();
		void SetAAMode(bool activateAA){NewAAMode = activateAA;}
		bool GetAAMode()const {return NewAAMode;}
		void SetFrameBuffer(bool AAMode);
		

		void GetScreenSize(float& w, float& h) const;

		void UpdateSafeFrameBorder();

		
		bool             Vsync;
		char*            FifoBuf;
		GXFifoObj*       FifoBufObj;
		char*            Fb1;
		char*            Fb2;
		char*            Currentfb;
		char*			 ScreenshotWorkingBuffer;

		bool				IsScreenCaptureEnabled;
		bool				IsFirstFrame;
		bool				NewAAMode;
		bool				IsProgressiveScan;
		bool				IsScanChanged;
		bool				IsWidescreenMode;
		bool				CurrentAAMode;

		GXRenderModeObj*	CurrentRenderModeObj;
		GXRenderModeObj*	RenderModeObj;
		GXRenderModeObj*	RenderModeObjAA;
		u32					VIMaxWidth;
		u32					VIMaxHeight;
		
		//for 2 pass rendering
		u32				CopyWidth;
		u32				Path1CopyHeight;
		u32				Path1CopyStartY;
		u32				Path2CopyHeight;
		u32				Path2CopyStartY;

		u32				Path1ScissorY;
		u32				Path2ScissorY;
		u32				BufferOffset;
		u32				Path2copyLines;

		f32				YScale;

		f32 SafeFrameBorderWidth, SafeFrameBorderHeight;

	public:

		void swapBuffer(bool clearBuffer = true);
	};

} // end namespace irr

#endif // _IRR_USE_WII_DEVICE_

#endif

