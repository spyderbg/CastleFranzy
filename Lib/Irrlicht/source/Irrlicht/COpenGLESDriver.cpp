#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COpenGLESDriver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"

#include <sstream>

#ifdef _IRR_COMPILE_WITH_OPENGL_ES_

// do the obvious to enable stats in release also
#if 1//def _DEBUG
#    define DEBUG_STATISTICS
#endif

#include "COpenGLESTexture.h"
#include "CCommonGLMaterialRenderer.h"
#include "COpenGLESShaderMaterialRenderer.h"
#include "COpenGLESSLMaterialRenderer.h"
#include "COpenGLESNormalMapRenderer.h"
#include "COpenGLESParallaxMapRenderer.h"
#include "CImage.h"
#include "irros.h"
#include "ISceneManager.h"

#ifdef _IRR_USE_SDL_DEVICE_
#include <SDL/SDL.h>
#endif


namespace irr
{
namespace video
{

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
//! Windows constructor and init code
COpenGLESDriver::COpenGLESDriver(const core::dimension2d<s32>& screenSize,
								 HWND window, 
								 bool stencilBuffer,
								 io::IFileSystem* io, 
								 bool antiAlias)
	: CCommonGLDriver(screenSize, window, stencilBuffer, io,  antiAlias)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESDriver");
	#endif
}

//! inits the open gl driver
bool
COpenGLESDriver::initWindowsDriver(const core::dimension2d<s32>& screenSize, 
								   HWND window, 
								   u32 bits, 
								   bool vsync, 
								   bool stencilBuffer)
{
	//---eglGetDisplay
	GLDisplay = eglGetDisplay((NativeDisplayType)EGL_DEFAULT_DISPLAY);
	int error = eglGetError();
	if(error != EGL_SUCCESS)
		return false;

	//---eglInitialize
	EGLint major = 0;
	EGLint minor = 0;
	EGLBoolean result = eglInitialize(GLDisplay, &major, &minor);
	error = eglGetError();
	if(error != EGL_SUCCESS)
	{
		os::Printer::log("eglInitialize", ELL_ERROR);
		return false;
	}

	//---eglGetConfigs / eglChooseConfig
	EGLint numConfigs = 10;
	EGLint attribs[] = 
	{
		EGL_DEPTH_SIZE, 16,
		EGL_NATIVE_RENDERABLE, EGL_FALSE,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6, 
		EGL_BLUE_SIZE, 5, 
		EGL_ALPHA_SIZE, 0, 
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLint nconfig;
	result = eglChooseConfig(GLDisplay, attribs, &GLConfig[0], numConfigs, &nconfig);
	error = eglGetError();
	if(error != EGL_SUCCESS)
	{
		os::Printer::log("eglChooseConfig", ELL_ERROR);
		return false;
	}

	//---eglCreateWindowSurface
	GLSurface = eglCreateWindowSurface( GLDisplay, GLConfig[0], (NativeWindowType)window, NULL);
	error = eglGetError();
	if(error != EGL_SUCCESS)
	{
		os::Printer::log("eglCreateWindowSurface", ELL_ERROR);
		return false;
	}

	//---eglCreateContext
	GLContext = eglCreateContext(GLDisplay, GLConfig[0], 0, attribs);
	error = eglGetError();
	if(error != EGL_SUCCESS)
	{
		os::Printer::log("eglCreateContext", ELL_ERROR);
		return false;
	}

	//---eglMakeCurrent
	result = eglMakeCurrent(GLDisplay, GLSurface, GLSurface, GLContext);
	if(!result)
	{
		error = eglGetError();
		if(error != EGL_SUCCESS)
		{
			os::Printer::log("eglMakeCurrent", ELL_ERROR);
			return false;
		}
	}
	
	// set vsync
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (eglSwapInterval)
		eglSwapInterval(GLDisplay, vsync ? 1 : 0);
#endif

	// set exposed data
	ExposedData.OpenGLWin32.HDc = HDc;
	ExposedData.OpenGLWin32.HRc = HRc;
	ExposedData.OpenGLWin32.HWnd = Window;

	return true;
}

#endif //IRR_USE_WINDOWS_DEVICE_

// -----------------------------------------------------------------------
// MacOSX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_IPHONEOS_DEVICE_
//! Windows constructor and init code
COpenGLESDriver::COpenGLESDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io, 
								 CIrrDeviceIPhoneOS *device)
	: CCommonGLDriver(params, io, device)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESDriver");
	#endif
}

#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_LINUX_DEVICE_
//! Linux constructor and init code
COpenGLESDriver::COpenGLESDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io)
	: COpenGLESDriver( params, io)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESDriver");
	#endif
	ExposedData.OpenGLLinux.X11Context = glXGetCurrentContext();
	ExposedData.OpenGLLinux.X11Display = glXGetCurrentDisplay();
	ExposedData.OpenGLLinux.X11Window = (unsigned long)params.WindowId;
	Drawable = glXGetCurrentDrawable();
}

#endif // _IRR_USE_LINUX_DEVICE_


// -----------------------------------------------------------------------
// SDL CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_SDL_DEVICE_
//! SDL constructor and init code
COpenGLESDriver::COpenGLESDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io)
	: COpenGLESDriver(	params, io)
{
	#ifdef _DEBUG
	setDebugName("COpenGLESDriver");
	#endif

	genericDriverInit(params.WindowSize, params.Stencilbuffer);
}

#endif // _IRR_USE_SDL_DEVICE_


//! destructor
COpenGLESDriver::~COpenGLESDriver()
{
	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.

	deleteAllTextures();

#ifdef _IRR_USE_WINDOWS_DEVICE_
	if (HRc)
	{
		EGLBoolean result;
		GLenum error;

		result = eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		error = eglGetError();
		if (result != EGL_TRUE)
			os::Printer::log("eglMakeCurrent failed.", ELL_WARNING);

		if (GLContext)
		{
			result = eglDestroyContext(GLDisplay, GLContext);
			error = eglGetError();
			if (result != EGL_TRUE)
				os::Printer::log("eglDestroyContext failed.", ELL_WARNING);
			GLContext = 0;
		}

		if (GLSurface)
		{
			result = eglDestroySurface(GLDisplay, GLSurface);
			error = eglGetError();
			if (result != EGL_TRUE)
				os::Printer::log("eglDestroySurface failed.", ELL_WARNING);
			GLSurface = 0;
		}

		if (GLDisplay)
		{
			result = eglTerminate(GLDisplay);
			error = eglGetError();
			if (result != EGL_TRUE)
				os::Printer::log("eglTerminate failed.", ELL_WARNING);
			GLDisplay = 0;
		}
	}

	if (HDc)
		ReleaseDC(Window, HDc);
#endif
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

E_DRIVER_TYPE
COpenGLESDriver::getDriverType() const
{
	return EDT_OPENGLES1;
}

void
COpenGLESDriver::createMaterialRenderers()
{
	// create OpenGL material renderers

	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_SOLID_2_LAYER(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_SOLID_2_LAYER_DECAL(this));

	// add the same renderer for all lightmap types
	CCommonGLMaterialRenderer_LIGHTMAP* lmr = irrnew CCommonGLMaterialRenderer_LIGHTMAP(this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_DETAIL_MAP(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_SPHERE_MAP(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_REFLECTION_2_LAYER(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_ADD_COLOR(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));

	CCommonGLMaterialRenderer_UNSUPPORTED* renderer = irrnew CCommonGLMaterialRenderer_UNSUPPORTED(this);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	renderer->drop();

	// add basic 1 texture blending
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_ONETEXTURE_BLEND(this));
	
	// add transpareny texture
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(this));

	// add 2D material renderer
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_2D_ALPHA(this));

	// add custom material
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_A8I8_GRADIENT(this));
}

//! returns true if the swap has been made
bool
COpenGLESDriver::swapBuffers(int clearMask)
{
	bool success = false;
#if defined(_IRR_USE_WINDOWS_DEVICE_)
	success = eglSwapBuffers(GLDisplay, GLSurface) == EGL_TRUE;
#elif defined(_IRR_USE_IPHONEOS_DEVICE_)
	IphoneDevice->flush();
	success = true;
#endif

	if (clearMask != 0)
	{
		clearBuffers(clearMask);
	}

	return success;
}

//! returns a device dependent texture from a software surface (IImage)
video::ITexture*
COpenGLESDriver::createDeviceDependentTexture(IImage* surface, const char* name)
{
	return irrnew COpenGLESTexture(surface, name, this);
}

//! returns a device dependent texture from a software surface (IImage)
video::ITexture*
COpenGLESDriver::createDeviceDependentRTTexture(const core::dimension2d<s32>& size, 
												const char* name, 
												bool colorTexture, 
												bool depthTexture)
{
	return irrnew COpenGLESTexture(size, name, this);
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! returns a device dependant texture from a "native" format in a file
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture*
COpenGLESDriver::createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
															const c8* hashName,
															bool /*refData*/)
{
	COpenGLESTexture* texture = irrnew COpenGLESTexture(file, hashName, this);
	if (texture && !texture->isValid())
	{
		delete texture;
		texture = 0;
	}
	return texture;
}
#endif


//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void
COpenGLESDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	if (!StencilBuffer || !count)
		return;

	// unset last 3d material
	{
		const SMaterial& material = getCurrentMaterial();
		if (CurrentRenderMode == ERM_3D &&
			static_cast<u32>(material.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[material.getMaterialType()].Renderer->onUnsetMaterial();
			ResetRenderStates = true;
		}
	}

	// store current OpenGL state
	GLboolean bLighting = glIsEnabled(GL_LIGHTING);
	GLboolean bFog      = glIsEnabled(GL_FOG);
	GLboolean bStencil  = glIsEnabled(GL_STENCIL_TEST);
	GLboolean bCull     = glIsEnabled(GL_CULL_FACE);
	GLboolean bDepthMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &bDepthMask);
	GLboolean bColorMask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, bColorMask);
	GLint depthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	GLint stencilFunc, stencilRef, stencilMask;
	glGetIntegerv(GL_STENCIL_FUNC, &stencilFunc);
	glGetIntegerv(GL_STENCIL_REF, &stencilRef);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencilMask);
	GLint stencilFail, stencilZFail, stencilZPass;
	glGetIntegerv(GL_STENCIL_FAIL, &stencilFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,&stencilZFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,&stencilZPass);
	GLint cullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE); // no depth buffer writing
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE ); // no color buffer drawing
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(core::vector3df),&triangles[0]);
	glStencilMask(~0);
	glStencilFunc(GL_ALWAYS, 0, ~0);

	// The first parts are not correctly working, yet.
#if 0
#ifdef GL_EXT_stencil_two_side
	if (FeatureAvailable[IRR_EXT_stencil_two_side])
	{
		glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
#ifdef GL_NV_depth_clamp
		if (FeatureAvailable[IRR_NV_depth_clamp])
			glEnable(GL_DEPTH_CLAMP_NV);
#endif
		glDisable(GL_CULL_FACE);
		if (!zfail)
		{
			// ZPASS Method

			extGlActiveStencilFace(GL_BACK);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT);
			else
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			extGlActiveStencilFace(GL_FRONT);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP_EXT);
			else
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else
		{
			// ZFAIL Method

			extGlActiveStencilFace(GL_BACK);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
			else
				glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			extGlActiveStencilFace(GL_FRONT);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP);
			else
				glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			glDrawArrays(GL_TRIANGLES,0,count);
		}
	}
	else
#endif
	if (FeatureAvailable[IRR_ATI_separate_stencil])
	{
		glDisable(GL_CULL_FACE);
		if (!zfail)
		{
			// ZPASS Method

			extGlStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR);
			extGlStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR);
			extGlStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~0);
			glStencilMask(~0);

			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else
		{
			// ZFAIL Method

			extGlStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
			extGlStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
			extGlStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~0);

			glDrawArrays(GL_TRIANGLES,0,count);
		}
	}
	else
#endif
	{
		glEnable(GL_CULL_FACE);
		if (!zfail)
		{
			// ZPASS Method

			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glDrawArrays(GL_TRIANGLES,0,count);

			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else
		{
			// ZFAIL Method

			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			glCullFace(GL_FRONT);
			glDrawArrays(GL_TRIANGLES,0,count);

			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
			glCullFace(GL_BACK);
			glDrawArrays(GL_TRIANGLES,0,count);
		}
	}

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif

	// Restore Attribs
	if(bLighting) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);

	if(bFog) glEnable(GL_FOG);
	else glDisable(GL_FOG);

	if(bStencil) glEnable(GL_STENCIL_TEST);
	else glDisable(GL_STENCIL_TEST);

	if(bCull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glDepthMask(bDepthMask);
	glDepthFunc(depthFunc);
	glColorMask(bColorMask[0],bColorMask[1],bColorMask[2],bColorMask[3]);
	glStencilOp(stencilFail, stencilZFail, stencilZPass);
	glStencilFunc(stencilFunc,stencilRef,stencilMask);
	glCullFace(cullFaceMode);
}

void
COpenGLESDriver::drawStencilShadow(bool clearStencilBuffer,
								   video::SColor leftUpEdge,
								   video::SColor rightUpEdge,
								   video::SColor leftDownEdge,
								   video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	// store attributes
	GLboolean bLighting = glIsEnabled(GL_LIGHTING);
	GLboolean bFog      = glIsEnabled(GL_FOG);
	GLboolean bStencil  = glIsEnabled(GL_STENCIL_TEST);
	GLboolean bBlend    = glIsEnabled(GL_BLEND);
	GLboolean bDepthMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &bDepthMask);
	GLboolean bColorMask[4];
	glGetBooleanv(GL_COLOR_WRITEMASK, bColorMask);
	GLint depthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	GLint stencilFunc, stencilRef, stencilMask;
	glGetIntegerv(GL_STENCIL_FUNC, &stencilFunc);
	glGetIntegerv(GL_STENCIL_REF, &stencilRef);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencilMask);
	GLint stencilFail, stencilZFail, stencilZPass;
	glGetIntegerv(GL_STENCIL_FAIL, &stencilFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,&stencilZFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,&stencilZPass);
	GLint shadeModel;
	glGetIntegerv(GL_SHADE_MODEL, &shadeModel);
	GLint frontFace;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);
	GLint blendSrc, blendDst;
	glGetIntegerv(GL_BLEND_SRC, &blendSrc);
	glGetIntegerv(GL_BLEND_DST, &blendDst);

	glDisable( GL_LIGHTING );
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);

	glShadeModel( GL_FLAT );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable( GL_STENCIL_TEST );
	glStencilFunc(GL_NOTEQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif

	// draw a shadow rectangle covering the entire screen using stencil buffer
	_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	GLfloat points[3*4] = {
		-1.1f, 1.1f, 0.90f,
		-1.1f,-1.1f,0.90f,
		1.1f, 1.1f,0.90f,
		1.1f,-1.1f,0.90f,
	};
	GLubyte colors[4*4];
	leftUpEdge.toOpenGLColor(&colors[0]);
	leftDownEdge.toOpenGLColor(&colors[4]);
	rightUpEdge.toOpenGLColor(&colors[8]);
	rightDownEdge.toOpenGLColor(&colors[12]);

	glVertexPointer(3, GL_FLOAT, 0, points);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	// restore settings
	glPopMatrix();
	
	if(bLighting) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);

	if(bFog) glEnable(GL_FOG);
	else glDisable(GL_FOG);

	if(bStencil) glEnable(GL_STENCIL_TEST);
	else glDisable(GL_STENCIL_TEST);

	if(bBlend) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
	glDepthMask(bDepthMask);
	glDepthFunc(depthFunc);
	glShadeModel(shadeModel);
	glFrontFace(frontFace);
	glBlendFunc(blendSrc, blendDst);
	glColorMask(bColorMask[0],bColorMask[1],bColorMask[2],bColorMask[3]);
	glStencilOp(stencilFail, stencilZFail, stencilZPass);
	glStencilFunc(stencilFunc,stencilRef,stencilMask);
}

// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
IVideoDriver*
createOpenGLES1Driver(const core::dimension2d<s32>& screenSize,
					  HWND window, 
					  u32 bits, 
					  bool stencilBuffer, 
					  io::IFileSystem* io, 
					  bool vsync, 
					  bool antiAlias)
{
	COpenGLESDriver* driver = irrnew COpenGLESDriver(screenSize, window, stencilBuffer, io, antiAlias);
	if (!driver->initDriver(screenSize, window, bits, false/*vsync*/, stencilBuffer))
	{
		driver->drop();
		driver = 0;
	}
	return driver;
}
#endif // _IRR_USE_WINDOWS_DEVICE_

// -----------------------------------
// IPHONE VERSION
// -----------------------------------
#if defined(_IRR_USE_IPHONEOS_DEVICE_)
IVideoDriver*
createOpenGLES1Driver(const SIrrlichtCreationParameters& params,
					  io::IFileSystem* io, 
					  CIrrDeviceIPhoneOS *device)
{
	COpenGLESDriver *driver = irrnew COpenGLESDriver(params, io, device);
	if( !driver->initDriver(params.WindowSize, params.Vsync, params.Stencilbuffer) )
	{
		driver->drop();
		driver = 0;
	}
	return driver;
}
#endif // _IRR_USE_IPHONEOS_DEVICE_

// -----------------------------------
// X11/SDL VERSION
// -----------------------------------
#if defined(_IRR_USE_LINUX_DEVICE_) || defined(_IRR_USE_SDL_DEVICE_)
IVideoDriver*
createOpenGLES1Driver(const SIrrlichtCreationParameters& params,
					  io::IFileSystem* io)
{
	
	COpenGLESDriver *driver = irrnew COpenGLESDriver(params, io);
	if( !driver->initDriver(params.WindowSize, params.Vsync, params.Stencilbuffer) )
	{
		driver->drop();
		driver = 0;
	}
	return driver;
}
#endif // _IRR_USE_LINUX_DEVICE_

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_ES_
