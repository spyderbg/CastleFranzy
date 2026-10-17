
#include "stdafx.h"

// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COpenGLDriver.h"
// needed here also because of the create methods' parameters
#include "CNullDriver.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLTexture.h"
#include "CCommonGLMaterialRenderer.h"
#include "COpenGLShaderMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"
#include "COpenGLNormalMapRenderer.h"
#include "COpenGLParallaxMapRenderer.h"
#include "CImage.h"
#include "irros.h"

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
COpenGLDriver::COpenGLDriver(const core::dimension2d<s32>& screenSize,
							 HWND window, bool stencilBuffer,
							 io::IFileSystem* io,
							 bool antiAlias)
	: CCommonGLDriver(screenSize, window, stencilBuffer, io, antiAlias)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
}

//! inits the open gl driver
bool COpenGLDriver::initWindowsDriver(const core::dimension2d<s32>& screenSize,
									  HWND window, 
									  u32 bits, 
									  bool vsync, 
									  bool stencilBuffer)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,				// Version Number
		PFD_DRAW_TO_WINDOW |		// Format Must Support Window
		PFD_SUPPORT_OPENGL |		// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,		// Must Support Double Buffering
		PFD_TYPE_RGBA,			// Request An RGBA Format
		bits,				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,		// Color Bits Ignored
		0,				// No Alpha Buffer
		0,				// Shift Bit Ignored
		0,				// No Accumulation Buffer
		0, 0, 0, 0,			// Accumulation Bits Ignored
		24,				// Z-Buffer (Depth Buffer)
		stencilBuffer ? 1 : 0,		// Stencil Buffer Depth
		0,				// No Auxiliary Buffer
		PFD_MAIN_PLANE,			// Main Drawing Layer
		0,				// Reserved
		0, 0, 0				// Layer Masks Ignored
	};

	for (u32 i=0; i<5; ++i)
	{
		if (i == 1)
		{
			if (stencilBuffer)
			{
				os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
				stencilBuffer = false;
				pfd.cStencilBits = 0;
			}
			else
				continue;
		}
		else
		if (i == 2)
		{
			pfd.cDepthBits = 24;
		}
		if (i == 3)
		{
			if (bits!=16)
				pfd.cDepthBits = 16;
			else
				continue;
		}
		else
		if (i == 4)
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			return false;
		}

		// get hdc
		if (!(HDc=GetDC(window)))
		{
			os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
			continue;
		}

		GLuint PixelFormat;

		// choose pixelformat
		if (!(PixelFormat = ChoosePixelFormat(HDc, &pfd)))
		{
			os::Printer::log("Cannot find a suitable pixelformat.", ELL_ERROR);
			continue;
		}

		// set pixel format
		if(!SetPixelFormat(HDc, PixelFormat, &pfd))
		{
			os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
			continue;
		}

		// create rendering context
		if (!(HRc=wglCreateContext(HDc)))
		{
			os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
			continue;
		}

		// activate rendering context
		if(!wglMakeCurrent(HDc, HRc))
		{
			os::Printer::log("Cannot activate GL rendering context", ELL_ERROR);
			continue;
		}

		break;
	}

	if (HDc)
	{
		int pf = GetPixelFormat(HDc);
		DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		if (pfd.cAlphaBits != 0)
		{
			if (pfd.cRedBits == 8)
			{
				ColorFormat = ECF_A8R8G8B8;
			}
			else
				ColorFormat = ECF_A1R5G5B5;
		}
		else
		{
			if (pfd.cRedBits == 8)
				ColorFormat = ECF_R8G8B8;
			else
				ColorFormat = ECF_R5G6B5;
		}
	}

	// set vsync
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vsync ? 1 : 0);

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
#ifdef _IRR_USE_OSX_DEVICE_
//! Windows constructor and init code
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
							 io::IFileSystem* io,
							 CIrrDeviceMacOSX *device)
	: CCommonGLDriver( params, io, device)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
}

#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_LINUX_DEVICE_
//! Linux constructor and init code
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
							 io::IFileSystem* io)
	: CCommonGLDriver( params, io)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
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
COpenGLDriver::COpenGLDriver(const SIrrlichtCreationParameters& params,
							 io::IFileSystem* io)
	: CCommonGLDriver( params, io)
{
	#ifdef _DEBUG
	setDebugName("COpenGLDriver");
	#endif
}

#endif // _IRR_USE_SDL_DEVICE_


// -----------------------------------------------------------------------
//! destructor
// -----------------------------------------------------------------------
COpenGLDriver::~COpenGLDriver()
{
	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.

	deleteAllTextures();

#ifdef _IRR_USE_WINDOWS_DEVICE_
	if (HRc)
	{
		if (!wglMakeCurrent(0, 0))
			os::Printer::log("Release of dc and rc failed.", ELL_WARNING);

		if (!wglDeleteContext(HRc))
			os::Printer::log("Release of rendering context failed.", ELL_WARNING);
	}

	if (HDc)
		ReleaseDC(Window, HDc);
#endif
}

// -----------------------------------------------------------------------
// Methodes
// -----------------------------------------------------------------------

E_DRIVER_TYPE
COpenGLDriver::getDriverType() const
{
	return EDT_OPENGL;
}

void
COpenGLDriver::createMaterialRenderers()
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

	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = irrnew COpenGLNormalMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = irrnew COpenGLNormalMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = irrnew COpenGLNormalMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	// add parallax map renderers
	renderer = irrnew COpenGLParallaxMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = irrnew COpenGLParallaxMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = irrnew COpenGLParallaxMapRenderer((COpenGLDriver*)this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
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

video::ITexture*
COpenGLDriver::createDeviceDependentTexture(IImage* surface, const char* name)
{
	return irrnew COpenGLTexture(surface, name, this);
}

//! returns a device dependent texture from a software surface (IImage)
video::ITexture*
COpenGLDriver::createDeviceDependentRTTexture(const core::dimension2d<s32>& size, const char* name, bool colorTexture, bool depthTexture)
{
	return irrnew COpenGLTexture(size, name, this, false, colorTexture, depthTexture);
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! returns a device dependant texture from a "native" format in a file
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture*
COpenGLDriver::createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
														  const c8* hashName,
														  bool /*refData*/)
{
	COpenGLTexture* texture = irrnew COpenGLTexture(file, hashName, this);
	if (texture && !texture->isValid())
	{
		delete texture;
		texture = 0;
	}
	return texture;
}
#endif

//! Sets a vertex shader constant.
void
COpenGLDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_vertex_program
	for (s32 i=0; i<constantAmount; ++i)
		programLocalParameter4fv(GL_VERTEX_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a pixel shader constant.
void
COpenGLDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
#ifdef GL_ARB_fragment_program
	for (s32 i=0; i<constantAmount; ++i)
		programLocalParameter4fv(GL_FRAGMENT_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a constant for the vertex shader based on a name.
bool
COpenGLDriver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(name, floats, count);
}

//! Sets a constant for the pixel shader based on a name.
bool
COpenGLDriver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32
COpenGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	COpenGLShaderMaterialRenderer* r = irrnew COpenGLShaderMaterialRenderer(
		this, nr, vertexShaderProgram, pixelShaderProgram,
		callback, getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}


//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32
COpenGLDriver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	u32 vertexAttributeMask,
	s32 userData)
{
	s32 nr = -1;

	COpenGLSLMaterialRenderer* r = irrnew COpenGLSLMaterialRenderer(
		this, nr, vertexShaderProgram, vertexShaderEntryPointName,
		vsCompileTarget, pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
		callback,getMaterialRenderer(baseMaterial), vertexAttributeMask, userData);

	r->drop();
	return nr;
}

IGPUProgrammingServices*
COpenGLDriver::getGPUProgrammingServices()
{
	return this;
}

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void
COpenGLDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
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
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
		GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

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

			activeStencilFace(GL_BACK);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP_EXT);
			else
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			activeStencilFace(GL_FRONT);
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

			activeStencilFace(GL_BACK);
			if (FeatureAvailable[IRR_EXT_stencil_wrap])
				glStencilOp(GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);
			else
				glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			glStencilMask(~0);
			glStencilFunc(GL_ALWAYS, 0, ~0);

			activeStencilFace(GL_FRONT);
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

#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
			glCullFace(GL_BACK);
#else
			glCullFace(GL_FRONT);
#endif

			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glDrawArrays(GL_TRIANGLES,0,count);

#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
			glCullFace(GL_FRONT);
#else
			glCullFace(GL_BACK);
#endif
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else
		{
			// ZFAIL Method

			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
			glCullFace(GL_FRONT);
#else
			glCullFace(GL_BACK);
#endif
			glDrawArrays(GL_TRIANGLES,0,count);

			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
			glCullFace(GL_BACK);
#else
			glCullFace(GL_FRONT);
#endif
			glDrawArrays(GL_TRIANGLES,0,count);
		}
	}

#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
			glCullFace(GL_BACK);
#else
			glCullFace(GL_FRONT);
#endif

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif

	glDisableClientState(GL_VERTEX_ARRAY); //not stored on stack
	glPopAttrib();
}


void
COpenGLDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
	video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	// store attributes
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT );
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

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
	//glMatrixMode(GL_MODELVIEW);
	_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	struct
	{
		GLubyte Color[4];
		GLfloat Pos[3];
	} vertices[] = {
		{{rightDownEdge.R, rightDownEdge.G, rightDownEdge.B, rightDownEdge.A},
		 {1.1f,-1.1f,0.9f}},
		{{rightUpEdge.R, rightUpEdge.G, rightUpEdge.B, rightUpEdge.A},
		 {1.1f, 1.1f,0.9f}},
		{{leftUpEdge.R, leftUpEdge.G, leftUpEdge.B, leftUpEdge.A},
		 {-1.1f, 1.1f,0.9f}},
		{{leftDownEdge.R, leftDownEdge.G, leftDownEdge.B, leftDownEdge.A},
		 {-1.1f,-1.1f,0.9f}}
	};

	setupArrayEnables(EVA_POSITION | EVA_COLOR0);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertices[0]), &vertices[0].Color[0]);
	glVertexPointer(3, GL_FLOAT, sizeof(vertices[0]), &vertices[0].Pos[0]);
	glDrawArrays(GL_QUADS, 0, 4);

	if (clearStencilBuffer)
		glClear(GL_STENCIL_BUFFER_BIT);

	// restore settings
	glPopMatrix();
	glPopAttrib();
}

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_)

namespace irr
{
namespace video
{

// -----------------------------------
// WINDOWS VERSION
// -----------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
IVideoDriver*
createOpenGLDriver(const core::dimension2d<s32>& screenSize,
				   HWND window,
				   u32 bits,
				   bool stencilBuffer,
				   io::IFileSystem* io,
				   bool vsync,
				   bool antiAlias)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl =  irrnew COpenGLDriver(screenSize, window, stencilBuffer, io, antiAlias);
	if ( !ogl->initDriver(screenSize, window, bits, vsync, stencilBuffer) )
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif // _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_WINDOWS_DEVICE_

// -----------------------------------
// MACOSX VERSION
// -----------------------------------
#if defined(_IRR_USE_OSX_DEVICE_)
IVideoDriver*
createOpenGLDriver(const SIrrlichtCreationParameters& params,
				   io::IFileSystem* io,
				   CIrrDeviceMacOSX *device)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl = irrnew COpenGLDriver(params, io, device);
	if( !ogl->initDriver(params.WindowSize, params.Vsync, params.Stencilbuffer) )
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_OSX_DEVICE_

// -----------------------------------
// X11/SDL VERSION
// -----------------------------------
#if defined(_IRR_USE_LINUX_DEVICE_) || defined(_IRR_USE_SDL_DEVICE_)
IVideoDriver*
createOpenGLDriver(const SIrrlichtCreationParameters& params,
				   io::IFileSystem* io)
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	COpenGLDriver* ogl = irrnew COpenGLDriver(params, io);
	if( !ogl->initDriver(params.WindowSize, params.Vsync, params.Stencilbuffer) )
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
#else
	return 0;
#endif //  _IRR_COMPILE_WITH_OPENGL_
}
#endif // _IRR_USE_LINUX_DEVICE_


} // end namespace
} // end namespace

#endif

