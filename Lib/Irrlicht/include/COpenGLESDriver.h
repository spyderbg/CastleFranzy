// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_OPEN_GL_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_ES_

#include "CCommonGLDriver.h"

namespace irr
{
namespace video
{
class COpenGLESTexture;

class COpenGLESDriver : public CCommonGLDriver
{
public:

#ifdef _IRR_WINDOWS_API_
	//! win32 constructor
	COpenGLESDriver(const core::dimension2d<s32>& screenSize,
					HWND window,
					bool stencilBuffer,
					io::IFileSystem* io,
					bool antiAlias);
	
	virtual bool initWindowsDriver(const core::dimension2d<s32>& screenSize, 
								   HWND window,
								   u32 bits,
								   bool vsync, 
								   bool stencilBuffer);
#endif

#if defined(_IRR_USE_LINUX_DEVICE_) || defined(_IRR_USE_SDL_DEVICE_)
	COpenGLESDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io);
#endif

#ifdef _IRR_USE_IPHONEOS_DEVICE_
	COpenGLESDriver(const SIrrlichtCreationParameters& params,
					io::IFileSystem* io,
					CIrrDeviceIPhoneOS *device);
#endif

	//! destructor
	virtual ~COpenGLESDriver();

	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	virtual void createMaterialRenderers();

	virtual bool swapBuffers(int clearMask);

	//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
	//! this: First, draw all geometry. Then use this method, to draw the shadow
	//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
	virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail);

	//! Fills the stencil shadow with color. After the shadow volume has been drawn
	//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
	//! to draw the color of the shadow.
	virtual void drawStencilShadow(bool clearStencilBuffer=false,
								   video::SColor leftUpEdge = video::SColor(0,0,0,0),
								   video::SColor rightUpEdge = video::SColor(0,0,0,0),
								   video::SColor leftDownEdge = video::SColor(0,0,0,0),
								   video::SColor rightDownEdge = video::SColor(0,0,0,0));

private:
	//! returns a device dependent texture from a software surface (IImage)
	virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

	//! returns a device dependent RenderTarget texture from a software surface (IImage)
	virtual video::ITexture* createDeviceDependentRTTexture(const core::dimension2d<s32>& size, 
															const char* name, 
															bool colorTexture, 
															bool depthTexture);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! returns a device dependant texture from a "native" format in a file
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
																		const c8* hashName = 0,
																		bool refData = false);
#endif

#if defined(_IRR_WINDOWS_API_)
	EGLDisplay 	GLDisplay;
	EGLConfig 	GLConfig[10];
	EGLSurface 	GLSurface;
	EGLContext	GLContext;
#endif
};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ES_
#endif
