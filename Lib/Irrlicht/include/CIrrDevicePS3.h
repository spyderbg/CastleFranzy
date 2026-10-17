// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_IRR_DEVICE_PS3_H_INCLUDED__
#define __C_IRR_DEVICE_PS3_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_PS3_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "IGUIEnvironment.h"
#include "ICursorControl.h"
#include "SIrrCreationParameters.h"

#include <map>

namespace irr
{
	class CIrrDevicePS3 : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDevicePS3(const SIrrlichtCreationParameters& params);

		//! destructor
		virtual ~CIrrDevicePS3();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run();

		//! Cause the device to temporarily pause execution and let other processes to run
		// This should bring down processor usage without major performance loss for Irrlicht
		virtual void yield();

		//! Pause execution and let other processes to run for a specified amount of time.
		virtual void sleep(u32 timeMs, bool pauseTimer);

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text);

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const;

		//! Checks if the Irrlicht window has focus
		virtual bool isWindowFocused() const;

		//! Checks if the Irrlicht window is minimized
		virtual bool isWindowMinimized() const;

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0 );

		//! notifies the device that it should close itself
		virtual void closeDevice();

		//! Sets if the window should be resizeable in windowed mode.
		virtual void setResizeAble(bool resize);

		//! \return Returns a pointer to a list with all video modes
		//! supported by the gfx adapter.
		virtual video::IVideoModeList* getVideoModeList();

		virtual void flush();
#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
		virtual bool flushVRM();
#endif
        //void setMouseLocation(int x,int y);
		//void setResize(int width,int height);
		//void setCursorVisible(bool visible);

	private:

		//! create the driver
		void createDriver();
		bool createWindow();
		void initKeycodes() {}
		//void storeMouseLocation();
		//void postMouseEvent(void *event,irr::SEvent &ievent);
		//void postKeyEvent(void *event,irr::SEvent &ievent,bool pressed);

		void			*_window;
		//CGLContextObj		_cglcontext;
		void			*_oglcontext;
		int			_width;
		int			_height;
		std::map<int,int>	_keycodes;
		int			_screenWidth;
		int			_screenHeight;
		bool			_active;
	};
#ifdef _IRR_COMPILE_WITH_THREAD_FLUSH
    void pushDelyFlush(IReferenceCounted* object);
    bool isNeedDelyFlush();
#endif

} // end namespace irr

#endif // _IRR_USE_PS3_DEVICE_
#endif // __C_IRR_DEVICE_PS3_H_INCLUDED__

