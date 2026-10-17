#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CCommonGLDriver.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || \
	defined(_IRR_COMPILE_WITH_OPENGL_ES_) || \
	defined(_IRR_COMPILE_WITH_PS3_)

#include "CCommonGLTexture.h"
#include "CCommonGLMaterialRenderer.h"
#include "CImage.h"
#include "SMaterial.h"
#include "irros.h"

#include "IBatchList.h"
#include "CBatchListCompileData.h"

#ifdef _IRR_USE_SDL_DEVICE_
#include <SDL/SDL.h>
#endif

namespace irr
{

extern int AllocCount;

namespace video
{

namespace
{

const GLenum VertexComponentTypeMap[] =
{
	GL_BYTE,			//CT_BYTE = 0,
	GL_UNSIGNED_BYTE,	//CT_UNSIGNED_BYTE = 1,
	GL_SHORT,			//CT_SHORT = 2,
	GL_UNSIGNED_SHORT,	//CT_UNSIGNED_SHORT = 3,
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	GL_FLOAT,			//CT_INT = 4,
	GL_FLOAT,			//CT_UNSIGNED_INT = 5,
#else
	GL_INT,				//CT_INT = 4,
	GL_UNSIGNED_INT,	//CT_UNSIGNED_INT = 5,
#endif
	GL_FLOAT,			//CT_FLOAT = 6,
};

static const GLenum ZBufferFuncMap[] =
{
	GL_NEVER,    // ECFN_NEVER
	GL_LESS,     // ECFN_LESS
	GL_EQUAL,    // ECFN_EQUAL
	GL_LEQUAL,   // ECFN_LEQUAL
	GL_GREATER,  // ECFN_GREATER
	GL_NOTEQUAL, // ECFN_NOTEQUAL
	GL_GEQUAL,   // ECFN_GEQUAL
	GL_ALWAYS    // ECFN_ALWAYS
};

const GLenum HWMappingMap[] =
{
	0,               // EHM_NEVER, corresponds to nothing
	GL_STATIC_DRAW,  // EHM_STATIC
	GL_DYNAMIC_DRAW, // EHM_DYNAMIC
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	0,				 // EHM_STREAM not supprted
#else
	GL_STREAM_DRAW,  // EHM_STREAM
#endif
};

const GLenum IndexTypeMap[] =
{
	GL_UNSIGNED_SHORT, // EIT_16BIT
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	GL_FLOAT,		   // EIT_32BIT
#else
	GL_UNSIGNED_INT    // EIT_32BIT
#endif
};

const u32 IndexTypeSize[] =
{
	2, // EIT_16BIT
	4  // EIT_32BIT
};

// small helper function to create vertex buffer object adress offsets
static inline u8*
buffer_offset(const long offset)
{
	return ((u8*)0 + offset);
}

// small helper function to create vertex buffer object adress offsets
static inline u8*
buffer_offset(const void* p1, const void* p2)
{
	long int offset = (long)p2 - (long)p1;

	return buffer_offset(offset);
}

}

const GLenum CCommonGLDriver::MaterialColorParamMap[] =
{
	GL_AMBIENT,  // EMCP_AMBIENT
	GL_DIFFUSE,  // EMCP_DIFFUSE
	GL_SPECULAR, // EMCP_SPECULAR
	GL_EMISSION, // EMCP_EMISSION
};

#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
// Initial values are taken from OpenGL specifications

CCommonGLDriver::SShadowState::STexEnv::STexEnv()
	: Mode(GL_MODULATE)
	, CombineRGB(GL_MODULATE)
	, CombineAlpha(GL_MODULATE)
	, RGBScale(1.0f)
	, AlphaScale(1.0f)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	Source[0].RGB = Source[0].Alpha = GL_TEXTURE;
	Source[0].OperandRGB = GL_SRC_COLOR;
	Source[0].OperandAlpha = GL_SRC_ALPHA;

	Source[1].RGB = Source[1].Alpha = GL_PREVIOUS;
	Source[1].OperandRGB = GL_SRC_COLOR;
	Source[1].OperandAlpha = GL_SRC_ALPHA;

	Source[2].RGB = Source[2].Alpha = GL_CONSTANT;
	Source[2].OperandRGB = GL_SRC_ALPHA;
	Source[2].OperandAlpha = GL_SRC_ALPHA;
#endif
}

CCommonGLDriver::SShadowState::SMaterial::SMaterial()
	: Shininess(0.0f)
#ifdef GL_EXT_separate_specular_color
	, ColorControl(GL_SINGLE_COLOR)
#endif
{
	// Ambient
	Color[0].set(255, 51, 51, 51);    // {1.0, 0.2, 0.2, 0.2}
	// Diffuse
	Color[1].set(255, 204, 204, 204); // {1.0, 0.8, 0.8, 0.8}
	// Specular
	Color[2].set(255, 0, 0, 0);       // {1.0, 0.0, 0.0, 0.0}
	// Emission
	Color[3].set(255, 0, 0, 0);       // {1.0, 0.0, 0.0, 0.0}
}

CCommonGLDriver::SShadowState::SShadowState()
	: ArrayBuffer(0)
	, ElementArrayBuffer(0)
	, ColorMaterialEnabled(false)
	, DepthMask(true)
{
}
#endif

E_DRIVER_TYPE
CCommonGLDriver::SBinding::getDriverType() const
{
	return EDT_OPENGL_FAMILY;
}

// -----------------------------------------------------------------------
// WINDOWS CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_WINDOWS_DEVICE_
//! Windows constructor and init code
CCommonGLDriver::CCommonGLDriver(const core::dimension2d<s32>& screenSize,
							 HWND window, bool stencilBuffer,
							 io::IFileSystem* io,
							 bool antiAlias)
	: CNullDriver(io, screenSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentOrientation(EOO_0)
	, CurrentOrientation3D(EOO_0)
	, CurrentRenderMode(ERM_NONE)
	, ResetRenderStates(true)
	, AntiAlias(antiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
//	, IsTextureMatrixIdentity(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, HDc(0)
	, Window(window)
	, HRc(0)
{

}

#endif //IRR_USE_WINDOWS_DEVICE_

// -----------------------------------------------------------------------
// MacOSX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_OSX_DEVICE_
//! Windows constructor and init code
CCommonGLDriver::CCommonGLDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io,
								 CIrrDeviceMacOSX *device)
	: CNullDriver(io, params.WindowSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentRenderMode(ERM_NONE)
	, CurrentOrientation(EOO_0)
	, ResetRenderStates(true)
	, AntiAlias(params.AntiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
//	, IsTextureMatrixIdentity(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, ColorFormat(ECF_R8G8B8)
	, MacDevice(device)
{

}

#endif

// -----------------------------------------------------------------------
// iPhone CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_IPHONEOS_DEVICE_
//! iPhone constructor and init code
CCommonGLDriver::CCommonGLDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io,
								 CIrrDeviceIPhoneOS *device)
	: CNullDriver(io, params.WindowSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentRenderMode(ERM_NONE)
	, CurrentOrientation(EOO_0)
	, ResetRenderStates(true)
	, AntiAlias(params.AntiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, ColorFormat(ECF_R8G8B8)
	, IphoneDevice(device)
{

}

#endif


#ifdef _IRR_COMPILE_WITH_PS3_
//! Windows constructor and init code
CCommonGLDriver::CCommonGLDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io, 
								 CIrrDevicePS3 *device)
	: CNullDriver(io, params.WindowSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentRenderMode(ERM_NONE)
	, CurrentOrientation(EOO_0)
	, ResetRenderStates(true)
	, AntiAlias(params.AntiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, ColorFormat(ECF_R8G8B8)
{

}
#endif

// -----------------------------------------------------------------------
// LINUX CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_LINUX_DEVICE_
//! Linux constructor and init code
CCommonGLDriver::CCommonGLDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io)
	: CNullDriver(io, params.WindowSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentRenderMode(ERM_NONE)
	, CurrentOrientation(EOO_0)
	, CurrentOrientation3D(EOO_0)
	, ResetRenderStates(true)
	, AntiAlias(params.AntiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
//	, IsTextureMatrixIdentity(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, ColorFormat(ECF_R8G8B8)
{

}

#endif // _IRR_USE_LINUX_DEVICE_


// -----------------------------------------------------------------------
// SDL CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_SDL_DEVICE_
//! SDL constructor and init code
CCommonGLDriver::CCommonGLDriver(const SIrrlichtCreationParameters& params,
								 io::IFileSystem* io)
	: CNullDriver(io, params.WindowSize)
	, CCommonGLExtensionHandler()
	, MatricesDirtyFlag(0)
	, CurrentRenderMode(ERM_NONE)
	, CurrentRenderMode(ERM_NONE)
	, ResetRenderStates(true)
	, AntiAlias(params.AntiAlias)
	, WasDynamicBatchEnabledBefore2D(false)
	, IsFlushingDynamicBatch(false)
	, LastRenderWasDynamicBatch(false)
	, DynamicBatch(0)
	, CompileData(0)
	, CurrentBatchId(-1)
//	, IsTextureMatrixIdentity(-1)
	, RenderTargetTexture(0)
	, LastSetLight(-1)
	, CurrentRendertargetSize(0,0)
	, ColorFormat(ECF_R8G8B8)
{

}
#endif

//! destructor
CCommonGLDriver::~CCommonGLDriver()
{
	if (DynamicBatch)
	{
		DynamicBatch->drop();
		DynamicBatch = 0;
	}

	deleteMaterialRenders();
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

//! Returns true if the init is sucessfull, fals on any error
bool
CCommonGLDriver::initDriver(const core::dimension2d<s32>& screenSize, 
#ifdef _IRR_WINDOWS_API_
							HWND window,
							u32 bits, 
#endif
							bool vsync, 
							bool stencilBuffer)
{
#if defined(_IRR_USE_WINDOWS_DEVICE_)
	if(!initWindowsDriver(screenSize, window, bits, vsync, stencilBuffer))
		return false;
#elif defined(_IRR_USE_LINUX_DEVICE_)
	// set vsync
	#ifdef GLX_SGI_swap_control
		#ifdef _IRR_OPENGL_USE_EXTPOINTER_
			if (vsync && glxSwapIntervalSGI)
				glxSwapIntervalSGI(1);
		#else
			if (vsync)
				glXSwapIntervalSGI(1);
		#endif
		testGlErrorParanoid();
	#endif
#elif defined(_IRR_COMPILE_WITH_PS3_)
	if (vsync)
		glEnable(GL_VSYNC_SCE);
	else
		glDisable(GL_VSYNC_SCE);
#endif

	if(!genericDriverInit(screenSize, stencilBuffer))
		return false;

	testGlErrorDebug();
	return true;
}

//! Returns true if the init is sucessfull, fals on any error
bool
CCommonGLDriver::genericDriverInit(const core::dimension2d<s32>& screenSize,
								   bool stencilBuffer)
{
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	Name=L"";
	Name.append(glGetString(GL_VERSION));
#else
	Name=L"OpenGL ";
	Name.append(glGetString(GL_VERSION));
	s32 pos=Name.findNext(L' ', 7);
	if (pos != -1)
		Name=Name.subString(0, pos);
#endif
	printVersion();

	// print renderer information
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	if (renderer && vendor)
	{
		os::Printer::log(reinterpret_cast<const c8*>(renderer), reinterpret_cast<const c8*>(vendor), ELL_INFORMATION);
		vendorName = reinterpret_cast<const c8*>(vendor);
	}

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		CurrentTexture[i] = 0;
	}
	// load extensions
	initExtensions(stencilBuffer);
	testGlErrorParanoid();

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	if (queryFeature(EVDF_ARB_GLSL))
	{
		char buf[32];
		const u32 maj = ShaderLanguageVersion/100;
		snprintf(buf, 32, "%u.%u", maj, ShaderLanguageVersion-maj*100);
		os::Printer::log("GLSL version", buf, ELL_INFORMATION);
	}
	else
		os::Printer::log("GLSL not available.", ELL_INFORMATION);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	testGlErrorParanoid();
#elif defined(_IRR_COMPILE_WITH_PS3_)
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	testGlErrorParanoid();
#endif

	ClearColor = SColor(0, 0, 0, 0);
	ClearDepth = 1.0f;
#ifdef _IRR_HAS_STENCIL_BUFFER_
	ClearStencil = 0;
#endif

	// Reset The Current Viewport
	glViewport(0, 0, screenSize.Width, screenSize.Height);
	testGlErrorParanoid();

// This needs an SMaterial flag to enable/disable later on, but should become default sometimes
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	testGlErrorParanoid();
#endif

	setAmbientLight(SColorf(0.0f,0.0f,0.0f,0.0f));
	testGlErrorParanoid();
#ifdef GL_EXT_separate_specular_color
	if (FeatureAvailable[IRR_EXT_separate_specular_color])
	{
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		testGlErrorParanoid();
	}
#endif
	
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_ ) && !defined(_IRR_COMPILE_WITH_PS3_)
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	testGlErrorParanoid();
#endif

// This is a fast replacement for NORMALIZE_NORMALS
//	if ((Version>101) || FeatureAvailable[IRR_EXT_rescale_normal])
//		glEnable(GL_RESCALE_NORMAL_EXT);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthFunc(GL_LEQUAL);
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
	glFrontFace( GL_CCW );
#else
	glFrontFace( GL_CW );
#endif
	testGlErrorParanoid();

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	if (AntiAlias)
	{
		if (MultiSamplingExtension)
			glEnable(GL_MULTISAMPLE_ARB);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		testGlErrorParanoid();
	}
#endif

// currently disabled, because often in software, and thus very slow
//	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
//	glEnable(GL_POINT_SMOOTH);

	UserClipPlane.reallocate(MaxUserClipPlanes);
	UserClipPlaneEnabled.reallocate(MaxUserClipPlanes);
	for (u32 i = 0; i < MaxUserClipPlanes; ++i)
	{
		UserClipPlane.push_back(core::plane3df());
		UserClipPlaneEnabled.push_back(false);
	}

	// Allocate the dynamic batch
 	DynamicBatch = irrnew scene::CBatchBuffer(this);
	MaxDynamicBatchSegmentSize = _IRR_MAX_DYNAMIC_BATCH_SEGMENT_SIZE_;
	ActualMaxDynamicBatchSegmentSize = MaxDynamicBatchSegmentSize;
	SavedMaxDynamicBatchSegmentSize = MaxDynamicBatchSegmentSize;
	SavedDynamicBatchVertexCapacity = 0;
	SavedDynamicBatchIndexCapacity = 0;
	CurrentDynamicBatchVertexCapacity = _IRR_DEFAULT_DYNAMIC_BATCH_MAX_VERTEX_BUFFER_SIZE_;
	CurrentDynamicBatchIndexCapacity = _IRR_DEFAULT_DYNAMIC_BATCH_MAX_INDEX_BUFFER_SIZE_;
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	IsDynamicBatchUsingProcessBuffer = true;
#endif
	//setOption(EVDO_BATCHING, true);

	// Init the structure for the 2D Quad processing
	QuadComponents.Position = &QuadBuffer[0].Pos;
	QuadComponents.PositionStride = sizeof(QuadVertex);
	QuadComponents.PositionType = S3DVertexComponentArrays::ECT_FLOAT;
	QuadComponents.TexCoord[0].Coord = &QuadBuffer[0].TexCoord;
	QuadComponents.TexCoord[0].Stride = sizeof(QuadVertex);
	QuadComponents.TexCoord[0].Type = S3DVertexComponentArrays::ECT_FLOAT;
	QuadComponents.Color0 = &QuadBuffer[0].Color;
	QuadComponents.Color0Stride = sizeof(QuadVertex);
	QuadComponents.Color0Type = S3DVertexComponentArrays::ECT_UNSIGNED_BYTE;

	// create material renderers
	createMaterialRenderers();

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	testGlErrorParanoid();

	// set the renderstates
	setRenderStates3DMode();

	glAlphaFunc(GL_GREATER, 0.5f);
	testGlErrorParanoid();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0,0), core::vector2df(0,1.0f), core::vector2df(1.0f,-1.0f));

	// init soft tex gen
	TexGenEnabled = 0;
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		TexGenType[i] = ETGT_NONE;
	}

	// Position array always enabled
	glEnableClientState(GL_VERTEX_ARRAY);
	testGlErrorParanoid();
	LastRequiredVertexAttributes = getCurrentRequiredAttributes();

	// Ensure default matrix mode
	glMatrixMode(GL_MODELVIEW);

	testGlErrorDebug();

	// Ensure buffer is cleared in case of a first swap before rendering the
	// first frame
	clearBuffers(EFB_COLOR);

	return true;
}

void CCommonGLDriver::setDynamicBatchCapatity(u32 vertexCapacity,
							                  u32 indexCapacity)
{
	CurrentDynamicBatchVertexCapacity = vertexCapacity;
	CurrentDynamicBatchIndexCapacity = indexCapacity;
}

void
CCommonGLDriver::allocateDynamicBatch(
	u32 vertexCapacity,
	u32 indexCapacity
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	, bool useProcessBuffer
#endif
)
{
	if (vertexCapacity)
	{
		vertexCapacity
			= core::min_(vertexCapacity,
						 u16(-1) * S3DVertexComponentArrays::MAX_STRIDE);
#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
		void* buf = allocProcessBuffer(vertexCapacity);
#else
		void* buf = NULL;
		if (useProcessBuffer)
		{
			buf = allocProcessBuffer(vertexCapacity);
		}
		else
		{
			buf = irrnew u8 [vertexCapacity];
		}
#endif
		_IRR_DEBUG_BREAK_IF(buf == NULL);
		DynamicBatch->setVertexBuffer(buf, vertexCapacity);
	}
	if (indexCapacity)
	{
#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
		void* buf = allocProcessBuffer(indexCapacity);
#else
		void* buf = NULL;
		if (useProcessBuffer)
		{
			buf = allocProcessBuffer(indexCapacity);
		}
		else
		{
			buf = irrnew u8 [indexCapacity];
		}
#endif
		_IRR_DEBUG_BREAK_IF(buf == NULL);
		DynamicBatch->setIndexBuffer(buf, indexCapacity);
	}
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	IsDynamicBatchUsingProcessBuffer = useProcessBuffer;
#endif
}

void
CCommonGLDriver::releaseDynamicBatch(u32* oldVertexCapacity,
									 u32* oldIndexCapacity)
{
 	void* buf = DynamicBatch->getVertexBuffer(oldVertexCapacity);
	if (buf)
	{
#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
		irr::releaseProcessBuffer(buf);
#else
		if (IsDynamicBatchUsingProcessBuffer)
		{
			irr::releaseProcessBuffer(buf);
		}
		else
		{
			delete[] reinterpret_cast<u8*>(buf);
		}
#endif
		DynamicBatch->setVertexBuffer(0, 0);
	}

 	buf = DynamicBatch->getIndexBuffer(oldIndexCapacity);
	if (buf)
	{
#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
		irr::releaseProcessBuffer(buf);
#else
		if (IsDynamicBatchUsingProcessBuffer)
		{
			irr::releaseProcessBuffer(buf);
		}
		else
		{
			delete[] reinterpret_cast<u8*>(buf);
		}
#endif
		DynamicBatch->setIndexBuffer(0, 0);
	}
}

bool
CCommonGLDriver::beginScene()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	CNullDriver::beginScene();

	allocateDynamicBatch(CurrentDynamicBatchVertexCapacity,
						 CurrentDynamicBatchIndexCapacity);

	return true;
}


bool
CCommonGLDriver::endScene()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);

	flush();

	// make sure the Scene didn't throw any GL error
	testGLError();

	glFlush();

	CNullDriver::endScene();

	releaseDynamicBatch();

	return true;
}

bool
CCommonGLDriver::swapBuffers(int clearMask)
{
	bool success = false;
#ifdef _IRR_USE_WINDOWS_DEVICE_
#    ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	success = SwapBuffers(HDc) == TRUE;
#    endif
#elif defined(_IRR_COMPILE_WITH_PS3_)
	psglSwap();
	success = true;
#elif defined(_IRR_USE_IPHONEOS_DEVICE_)
	IphoneDevice->flush();
	success = true;
#elif defined(_IRR_USE_LINUX_DEVICE_)
	glXSwapBuffers((Display*)ExposedData.OpenGLLinux.X11Display, Drawable);
	success = true;
#elif defined(_IRR_USE_OSX_DEVICE_) 
	MacDevice->flush();
	success = true;
#elif defined(_IRR_USE_SDL_DEVICE_)
	SDL_GL_SwapBuffers();
	success = true;
#endif

	if (clearMask != 0)
	{
		clearBuffers(clearMask);
	}

	return success;
}

void
CCommonGLDriver::clearBuffers(int clearMask)
{
	flush();

	GLbitfield mask = 0;
	if (clearMask & EFB_COLOR)
	{
		const f32 inv = 1.0f / 255.0f;
		glClearColor(ClearColor.R * inv,
					 ClearColor.G * inv,
					 ClearColor.B * inv,
					 ClearColor.A * inv);
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (clearMask & EFB_DEPTH)
	{
		setDepthMask(true);
		LastMaterial.setFlag(EMF_ZWRITE_ENABLE, true);
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
		glClearDepthf(ClearDepth);
#else
		glClearDepth(ClearDepth);
#endif
		mask |= GL_DEPTH_BUFFER_BIT;
	}

#ifdef _IRR_HAS_STENCIL_BUFFER_
	if (clearMask & EFB_STENCIL)
	{
		glClearStencil(ClearStencil);
		mask |= GL_STENCIL_BUFFER_BIT;
	}
#endif

	if (mask != 0)
	{
		glClear(mask);
	}

	testGlErrorParanoid();
}

void
CCommonGLDriver::setClearColor(const SColor& color)
{
	ClearColor = color;
}

SColor
CCommonGLDriver::getClearColor() const
{
	return ClearColor;
}

void
CCommonGLDriver::setClearDepth(float depth)
{
	ClearDepth = depth;
}

float
CCommonGLDriver::getClearDepth() const
{
	return ClearDepth;
}

#ifdef _IRR_HAS_STENCIL_BUFFER_
void
CCommonGLDriver::setClearStencil(int stencil)
{
	ClearStencil = stencil;
}

int
CCommonGLDriver::getClearStencil() const
{
	return ClearStencil;
}
#endif

namespace
{

const u32 Scene2DAttributes = EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;

}

bool
CCommonGLDriver::beginScene2D()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	_IRR_DEBUG_BREAK_IF(CurrentDynamicBatchVertexCapacity > 0
						&& DynamicBatch->getVertexBuffer() == NULL);

	// Make sure to flush the 3D dynamic batch buffer
	flush();
	testGlErrorParanoid();

	// Keep the current batching state and force it to true for 2D
	WasDynamicBatchEnabledBefore2D = getOption(EVDO_BATCHING);
	if(!WasDynamicBatchEnabledBefore2D)
	{
		CNullDriver::setOption(EVDO_BATCHING, true);
	}

	// Backup the 3D Matrices to be able to restore them after 2D is over
	ProjectionMatrixBackup = getTransform(ETS_PROJECTION);
	WorldMatrixBackup = getTransform(ETS_WORLD);
	ViewMatrixBackup = getTransform(ETS_VIEW);

	setTransform(ETS_VIEW, core::IdentityMatrix);
	setTransform(ETS_WORLD, core::IdentityMatrix);
	setTransform(ETS_TEXTURE_0, core::IdentityMatrix);
	setTransform(ETS_TEXTURE_1, core::IdentityMatrix);

	// Tell our driver that all the following calls will be for 2D drawing
	CurrentRenderMode = ERM_2D;

	// Setup base material for 2D
	SMaterial mat;
	mat.setMaterialType(EMT_SOLID);
	mat.setFlag(EMF_ZBUFFER, false);
	mat.setFlag(EMF_LIGHTING, false);
	mat.setFlag(EMF_BACK_FACE_CULLING, false);
	set2DMaterial(mat);
	setBasicRenderStates(mat, LastMaterial, true);
	testGlErrorParanoid();
	
	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	core::matrix4 projMatrix(core::matrix4::EM4CONST_NOTHING);
	projMatrix.buildProjectionMatrixOrtho(0.0f,
										  f32(renderTargetSize.Width),
										  f32(renderTargetSize.Height),
										  0.0f,
										  -1.0f,
										  1.0f);
	// "post multiply" projMatrix by a translation matrix of (0.375, 0.375)
	// see http://glprogramming.com/red/appendixg.html#name1
	//projMatrix[12] += -projMatrix[0] * 0.5f;
	//projMatrix[13] +=  projMatrix[5] * 0.375f;
	setTransform(ETS_PROJECTION, projMatrix);

	return true;
}


bool
CCommonGLDriver::endScene2D()
{
	testGlErrorParanoid();
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
#if !defined(_IRR_COMPILE_WITH_PS3_)
	_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif

	// Flush the current 2D dynamic buffer before returning to 3D
	flush();
	testGlErrorParanoid();

	// Restore batching state
	if(!WasDynamicBatchEnabledBefore2D)
	{
		CNullDriver::setOption(EVDO_BATCHING, false);
	}

	// return to 3D mode
	CurrentRenderMode = ERM_3D;
	ResetRenderStates = true;

	// Reset the matrices to the 3D state
	setTransform(ETS_WORLD, WorldMatrixBackup);
	setTransform(ETS_VIEW, ViewMatrixBackup);
	setTransform(ETS_PROJECTION, ProjectionMatrixBackup);

	return true;
}


//! Returns the transformation set by setTransform
const core::matrix4&
CCommonGLDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}


//! sets transformation
void
CCommonGLDriver::setTransform(E_TRANSFORMATION_STATE state,
							  const core::matrix4& mat)
{

	switch(state)
	{
		case ETS_VIEW:
			flush();
			MatricesDirtyFlag |= ETSDF_VIEW;
			Matrices[ETS_VIEW] = mat;
			break;
		case ETS_WORLD:
		{
			// in 2D mode, due to batching implementation, world matrix state is unaffected
			Matrices[ETS_WORLD] = mat;
			if (CurrentRenderMode != ERM_2D)
			{
				MatricesDirtyFlag |= ETSDF_WORLD;
			}
		}
		break;

		case ETS_PROJECTION:
		{			
			flush();
			Matrices[ETS_PROJECTION] = mat;
			MatricesDirtyFlag |= ETSDF_PROJECTION;
		}
		break;

		case ETS_TEXTURE_0:
		case ETS_TEXTURE_1:
		case ETS_TEXTURE_2:
		case ETS_TEXTURE_3:
		{
			Matrices[state] = mat;
			const u16 i = state - ETS_TEXTURE_0;
			const SMaterial& material = getCurrentMaterial();
			const bool isRTT = material.getTexture(i) && material.getTexture(i)->isRenderTarget();
			if (mat.isIdentity() && !isRTT)
			{
				Matrices[state].setDefinitelyIdentityMatrix(true);
			}
			else
			{
				Matrices[state].setDefinitelyIdentityMatrix(false);
			}
			MatricesDirtyFlag |= 1 << state;
		}
		break;

		default:
			break;
	}
	testGlErrorParanoid();
}

bool
CCommonGLDriver::updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	if (!FeatureAvailable[IRR_ARB_vertex_buffer_object])
		return false;
#endif

#if defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const u32 vertexCount = mb->getVertexCount();
	const E_VERTEX_TYPE vType = mb->getVertexType();
	
	const S3DVertexComponentArrays* components = NULL;
	SScopedProcessArray<SColor> colorBuffer;
	if (vType == EVT_COMPONENT_ARRAYS)
	{
		components = reinterpret_cast<const S3DVertexComponentArrays*>(mb->getVertices());
	}
	else
	{
		convertVertexType(const_cast<void*>(mb->getVertices()),
						  0,
						  vertexCount,
						  vType,
						  colorBuffer);
		components = &VertexComponents;
	}

	const void* vertices = components->Position;
	const u32 vertexSize = components->PositionStride;

	//get or create buffer
	bool newBuffer = false;
	if (!HWBuffer->vbo_verticesID)
	{
		genBuffers(1, &HWBuffer->vbo_verticesID);
		if (!HWBuffer->vbo_verticesID)
		{
			return false;
		}
		newBuffer = true;
	}
	else if (HWBuffer->vbo_verticesSize < vertexCount * vertexSize)
	{
		newBuffer = true;
	}

	bindArrayBuffer(HWBuffer->vbo_verticesID);

	//copy data to graphics card
	glGetError(); // clear error storage

	if (!newBuffer)
	{
		bufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * vertexSize, vertices);
	}
	else
	{
		HWBuffer->vbo_verticesSize = vertexCount * vertexSize;
		bufferData(GL_ARRAY_BUFFER,
						HWBuffer->vbo_verticesSize,
						vertices,
						HWMappingMap[HWBuffer->Mapped_Vertex]);
	}

	bindArrayBuffer(0);

	return (glGetError() == GL_NO_ERROR);
#else
	return false;
#endif
}


bool
CCommonGLDriver::updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer)
{
	if (!HWBuffer)
		return false;

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	if (!FeatureAvailable[IRR_ARB_vertex_buffer_object])
		return false;
#endif

#if defined(GL_ARB_vertex_buffer_object) ||  defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;

	const void* indices = mb->getIndices();
	u32 indexCount = mb->getIndexCount();

	u32 indexSize = IndexTypeSize[mb->getIndexType()];

	//get or create buffer
	bool newBuffer = false;
	if (!HWBuffer->vbo_indicesID)
	{
		genBuffers(1, &HWBuffer->vbo_indicesID);
		if (!HWBuffer->vbo_indicesID)
		{
			return false;
		}
		newBuffer = true;
	}
	else if (HWBuffer->vbo_indicesSize < indexCount * indexSize)
	{
		newBuffer = true;
	}

	bindElementArrayBuffer(HWBuffer->vbo_indicesID);

	//copy data to graphics card
	glGetError(); // clear error storage
	if (!newBuffer)
	{
		bufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * indexSize, indices);
	}
	else
	{
		HWBuffer->vbo_indicesSize = indexCount * indexSize;
		bufferData(GL_ELEMENT_ARRAY_BUFFER,
				   HWBuffer->vbo_indicesSize,
				   indices,
				   HWMappingMap[HWBuffer->Mapped_Index]);
	}

	bindElementArrayBuffer(0);

	return (glGetError() == GL_NO_ERROR);
#else
	return false;
#endif
}


//! updates hardware buffer if needed
bool
CCommonGLDriver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Vertex != HWBuffer->MeshBuffer->getChangedID_Vertex()
			|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_verticesID)
		{

			HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();

			if (!updateVertexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
				return false;
		}
	}
	testGlErrorParanoid();

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Index != HWBuffer->MeshBuffer->getChangedID_Index()
			|| !((SHWBufferLink_opengl*)HWBuffer)->vbo_indicesID)
		{

			HWBuffer->ChangedID_Index = HWBuffer->MeshBuffer->getChangedID_Index();

			if (!updateIndexHardwareBuffer((SHWBufferLink_opengl*)HWBuffer))
				return false;
		}
	}

	testGlErrorDebug();
	return true;
}


//! Create hardware buffer from meshbuffer
CCommonGLDriver::SHWBufferLink*
CCommonGLDriver::createHardwareBuffer(const scene::IMeshBuffer* mb)
{
#if defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	if (!mb || (mb->getHardwareMappingHint_Index()==scene::EHM_NEVER && mb->getHardwareMappingHint_Vertex()==scene::EHM_NEVER))
		return 0;

	SHWBufferLink_opengl *HWBuffer=irrnew SHWBufferLink_opengl(mb);

	//add to map
	HWBufferMap.insert(HWBuffer->MeshBuffer, HWBuffer);

	HWBuffer->ChangedID_Vertex=HWBuffer->MeshBuffer->getChangedID_Vertex();
	HWBuffer->ChangedID_Index=HWBuffer->MeshBuffer->getChangedID_Index();
	HWBuffer->Mapped_Vertex=mb->getHardwareMappingHint_Vertex();
	HWBuffer->Mapped_Index=mb->getHardwareMappingHint_Index();
	HWBuffer->LastUsed=0;
	HWBuffer->vbo_verticesID=0;
	HWBuffer->vbo_indicesID=0;
	HWBuffer->vbo_verticesSize=0;
	HWBuffer->vbo_indicesSize=0;

	if (!updateHardwareBuffer(HWBuffer))
	{
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	testGlErrorParanoid();
	return HWBuffer;
#else
	return 0;
#endif
}


void
CCommonGLDriver::deleteHardwareBuffer(SHWBufferLink* _HWBuffer)
{
	if (!_HWBuffer) return;

#if defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	SHWBufferLink_opengl *HWBuffer=(SHWBufferLink_opengl*)_HWBuffer;
	if (HWBuffer->vbo_verticesID)
	{
		deleteBuffers(1, &HWBuffer->vbo_verticesID);
		HWBuffer->vbo_verticesID=0;
	}
	if (HWBuffer->vbo_indicesID)
	{
		deleteBuffers(1, &HWBuffer->vbo_indicesID);
		HWBuffer->vbo_indicesID=0;
	}
	testGlErrorParanoid();
#endif

	CNullDriver::deleteHardwareBuffer(_HWBuffer);

}


//! Draw hardware buffer
void
CCommonGLDriver::drawHardwareBuffer(SHWBufferLink* _HWBuffer)
{
	if (!_HWBuffer)
		return;

	SHWBufferLink_opengl* HWBuffer = (SHWBufferLink_opengl*)_HWBuffer;

	updateHardwareBuffer(HWBuffer); //check if update is needed

	HWBuffer->LastUsed = 0; //reset count

#if defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;

	const void* vertices = mb->getVertices();
	const void* indexList = mb->getIndices();
	bool vboFlag = false;

	if (HWBuffer->Mapped_Vertex != scene::EHM_NEVER)
	{
		bindArrayBuffer(HWBuffer->vbo_verticesID);
		vboFlag = true;
		//vertices = 0;
	}

	if (HWBuffer->Mapped_Index != scene::EHM_NEVER)
	{
		bindElementArrayBuffer(HWBuffer->vbo_indicesID);
		indexList = 0;
	}

	drawVertexPrimitiveList(vertices,
							indexList,
							0,
							mb->getVertexCount(),
							mb->getIndexCount() / 3,
							mb->getVertexType(),
							mb->getPrimitiveType(),
							mb->getIndexType(),
							vboFlag);

	if (HWBuffer->Mapped_Vertex != scene::EHM_NEVER)
		bindArrayBuffer(0);

	if (HWBuffer->Mapped_Index != scene::EHM_NEVER)
		bindElementArrayBuffer(0);

	testGlErrorParanoid();
#endif
}

void
CCommonGLDriver::drawMeshBuffer(const scene::IMeshBuffer* mb)
{
	if (!mb)
		return;

	//IVertexBuffer and IIndexBuffer later
	SHWBufferLink* HWBuffer = CompileData ? NULL : getBufferLink(mb);

	if (HWBuffer)
	{
		drawHardwareBuffer(HWBuffer);
	}
	else
	{
		IDriverBinding** binding = &getBinding(mb);
		drawVertexPrimitiveList(mb->getVertices(),
								mb->getIndices(),
								mb->getVertexIndexStart(),
								mb->getVertexIndexEnd(),
								mb->getIndexCount() / 3,
								mb->getVertexType(),
								mb->getPrimitiveType(),
								mb->getIndexType(),
								binding);
		if (CompileData)
		{
			scene::SCompileInfo info;
			info.SourceBuffer = mb;
			CompileData->getBatchList()->setSegmentCompileInfo(
				CurrentBatchId,
				CurrentSegmentId,
				info
			);
		}
	}
}

void
CCommonGLDriver::drawVertexPrimitiveList(const void* vertices,
										 const void* indexList,
										 u32 startIndex,
										 u32 endIndex,
										 u32 primitiveCount,
										 E_VERTEX_TYPE vType,
										 scene::E_PRIMITIVE_TYPE pType,
										 E_INDEX_TYPE iType,
										 IDriverBinding** binding)
{
	const bool isBatchingEnabled = getOption(EVDO_BATCHING);
	if (isBatchingEnabled
		&& endIndex - startIndex < ActualMaxDynamicBatchSegmentSize
		&& pType != scene::EPT_POINTS
		&& pType != scene::EPT_POINT_SPRITES
		&& pType != scene::EPT_LINES
		&& pType != scene::EPT_LINE_STRIP
		&& iType == EIT_16BIT)
	{
		if (!DynamicBatch->hasEnoughSpace(endIndex - startIndex,
										  primitiveCount * 3))
		{
			flush();
		}
		// convert to components everything
		SScopedProcessArray<SColor> colorBuffer;
		if(vType != EVT_COMPONENT_ARRAYS)
		{
			convertVertexType(const_cast<void*>(vertices),
							  startIndex,
							  endIndex,
							  vType,
							  colorBuffer);
			vertices = &VertexComponents;
		}
		const S3DVertexComponentArrays* components
			= reinterpret_cast<const S3DVertexComponentArrays*>(vertices);
		u32 vOffset, iOffset;
		DynamicBatch->append(*components,
							 reinterpret_cast<const u16*>(indexList),
							 startIndex,
							 endIndex,
							 primitiveCount,
							 pType,
							 &vOffset,
							 &iOffset);
		if (CompileData)
		{
			if (CurrentBatchId == u32(-1))
			{
				CurrentBatchId = CompileData->getBatchList()->addBatch();
			}
			_IRR_DEBUG_BREAK_IF(
				CompileData->getBatchList()->getSegmentCount(CurrentBatchId) == 0
				&& (vOffset != 0 || iOffset != 0)
			);
			CurrentSegmentId = CompileData->getBatchList()->addSegment(
				CurrentBatchId,
				vOffset,
				vOffset + (endIndex - startIndex),
				iOffset,
				iOffset + primitiveCount * 3
			);
			if (binding != NULL)
			{
				SBinding* b = ensureBinding(binding);
				b->setInBatch(CompileData->getBatchList(),
							  CurrentBatchId,
							  CurrentSegmentId);
			}
		}
	}
	else if (CompileData == 0)
	{
		if (isBatchingEnabled && !getOption(EVDO_RETAIN_BATCH_ON_MISS))
		{
			flush();
		}
		drawVertexPrimitiveList(vertices,
								indexList,
								startIndex,
								endIndex,
								primitiveCount,
								vType,
								pType,
								iType,
								false);
	}

	testGlErrorParanoid();
}

bool
CCommonGLDriver::convertVertexType(void* vertices,
								   u32 startIndex,
								   u32 endIndex,
								   E_VERTEX_TYPE type,
								   SScopedProcessArray<SColor>& colorBuffer)
{
	_IRR_DEBUG_BREAK_IF(type > EVT_TANGENTS);

	// safe const cast here because we just read; we would actually need
	// a const version of S3DVertexComponentArrays
	VertexComponents.assign(const_cast<void*>(vertices), type);

#if defined(_IRR_SCOLOR_USE_ARGB_)
	// convert colors to gl color format.
	colorBuffer.reset(endIndex);
	if (colorBuffer.get() == NULL)
	{
		os::Printer::log("color buffer too large", ELL_ERROR);
		return false;
	}

	const u8* p = (reinterpret_cast<const u8*>(vertices)
				   + startIndex * VertexStrides[type]);
	for (u32 i = startIndex; i < endIndex; ++i)
	{
		// all supported non-component vertex types inherit from S3DVertex
		reinterpret_cast<const S3DVertex*>(p)->Color.toRGBA8(
			reinterpret_cast<u8*>(colorBuffer.get() + i)
		);
		p += VertexStrides[type];
	}
	VertexComponents.Color0 = colorBuffer.get();
	VertexComponents.Color0Stride = sizeof(SColor);
#endif
	return true;
}

u32
CCommonGLDriver::setupArrays(u32 requiredAttribs,
							 const S3DVertexComponentArrays* components,
							 bool fVBOFlag,
							 bool* quitInTextureMode)
{
	_IRR_DEBUG_BREAK_IF(components->Position == NULL && !fVBOFlag);
	testGlErrorParanoid();

	if (fVBOFlag)
	{
		// setup component pointer with offsets (it's not worth to compute
		// offsets conditionally on required attributes: few if's vs few integer
		// additions/subtractions, better save the branching

		VertexComponents = *components;
		VertexComponents.Normal = reinterpret_cast<core::vector3df*>(
			buffer_offset(components->Position,
						  components->Normal)
		);
		// it might be worth it for texcoords though, as we might skip more in
		// one shot
		u32 texMask = requiredAttribs & EVA_TEXCOORDS_MASK;
		for (u32 i = 0; texMask; ++i)
		{
			u32 unitMask = EVA_TEXCOORD0 << i;
			//if (texMask & unitMask) // but not this one
			{
				VertexComponents.TexCoord[i].Coord = reinterpret_cast<core::vector2df*>(
					buffer_offset(components->Position,
								  components->TexCoord[i].Coord)
				);
			}
			texMask &= ~unitMask;
		}
		VertexComponents.Color0
			= reinterpret_cast<SColor*>(buffer_offset(components->Position,
													  components->Color0));
		VertexComponents.Color1
			= reinterpret_cast<SColor*>(buffer_offset(components->Position,
													  components->Color1));
		VertexComponents.Position = 0;

		components = &VertexComponents;
		testGlErrorParanoid();
	}

	u32 changeMask = requiredAttribs ^ LastRequiredVertexAttributes;

	// Setup vertex array
	if (components->PositionType != S3DVertexComponentArrays::ECT_FLOAT)
	{
#if !defined(_IRR_COMPILE_WITH_PS3_)
		_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif
		glPushMatrix();
		const core::vector3df& scale = *components->getPositionScale();
		const core::vector3df& offset = *components->getPositionOffset();
		const GLfloat transform[] = 
			{ scale.X, 0,0,0,
			  0, scale.Y, 0,0,
			  0, 0, scale.Z,0,
			  offset.X, offset.Y, offset.Z, 1 };
		glMultMatrixf(transform);
	}
	glVertexPointer(3, 
					VertexComponentTypeMap[components->PositionType], 
					components->PositionStride, 
					components->Position);
	testGlErrorParanoid();
		
	// Setup normal array
	if (requiredAttribs & EVA_NORMAL)
	{
		if (components->Normal)
		{
			if (changeMask & EVA_NORMAL)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
			}
			glNormalPointer(VertexComponentTypeMap[components->NormalType], 
							components->NormalStride, 
							components->Normal);
			testGlErrorParanoid();
		}
		else
		{
			// should we _IRR_DEBUG_BREAK_IF(...) instead (or in addition)?
			os::Printer::log("missing normal component", ELL_WARNING);

			if (!(changeMask & EVA_NORMAL))
			{
				glDisableClientState(GL_NORMAL_ARRAY);
			}
			glNormal3f(0.0f, 0.0f, 1.0f);
			testGlErrorParanoid();
			requiredAttribs &= ~EVA_NORMAL;
		}
	}
	else if (changeMask & EVA_NORMAL)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
		testGlErrorParanoid();
	}

	bool inModelViewMatrixMode = true;
	u32 texCoords = requiredAttribs & EVA_TEXCOORDS_MASK;
	u32 texCoordsChangeMask = changeMask & EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texCoords | texCoordsChangeMask; ++i)
	{
		const u32 unitMask = EVA_TEXCOORD0 << i;
		texCoords &= ~unitMask;
		if (requiredAttribs & unitMask)
		{
			const S3DVertexComponentArrays::STexCoordComponent& texCoord
				= components->TexCoord[i];
			setClientActiveTexture(i);
			if (texCoord.Coord)
			{
				if (changeMask & unitMask)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					testGlErrorParanoid();
				}

				if (texCoord.Type != S3DVertexComponentArrays::ECT_FLOAT)
				{
					if (inModelViewMatrixMode)
					{
						glMatrixMode(GL_TEXTURE);
						inModelViewMatrixMode = false;
					}
#if !defined(_IRR_COMPILE_WITH_PS3_)
					_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_TEXTURE);
#endif
					glPushMatrix();
					const core::vector3df& scale = *components->getTexCoordScale(i);
					const core::vector3df& offset = *components->getTexCoordOffset(i);
					const GLfloat transform[] = 
						{ scale.X, 0, 0, 0,
						  0, scale.Y, 0, 0,
						  0, 0, 1, 0,
						  offset.X, offset.Y, 0, 1 };
					glMultMatrixf(transform);
					testGlErrorParanoid();
				}
				glTexCoordPointer(2, 
								  VertexComponentTypeMap[texCoord.Type],
								  texCoord.Stride, 
								  texCoord.Coord);
				testGlErrorParanoid();
			}
			else
			{
				// should we _IRR_DEBUG_BREAK_IF(...) instead (or in addition)?
				os::Printer::log("missing tex coord component", ELL_WARNING);

				if (!(changeMask & unitMask))
				{
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)	
				glTexCoord2f(0.0f, 0.0f);
				testGlErrorParanoid();
#endif
				requiredAttribs &= ~unitMask;
			}
		}
		else if (changeMask & unitMask)
		{
			setClientActiveTexture(i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			testGlErrorParanoid();
		}

		texCoordsChangeMask &= ~unitMask;
	}
	if (quitInTextureMode)
	{
		*quitInTextureMode = !inModelViewMatrixMode;
	}

	// Setup normal array
	if (requiredAttribs & EVA_COLOR0)
	{
		const SMaterial& material = getCurrentMaterial();
		if (components->Color0
			&& (!material.getFlag(EMF_LIGHTING)
				|| material.isPerVertexMaterialColorRequested()))
		{
			if (changeMask & EVA_COLOR0)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				testGlErrorParanoid();
			}
//			if (Material.getFlag(EMF_LIGHTING))
//			{
//				glEnable(GL_COLOR_MATERIAL);
//			}
			
			// On the iPhone this is required... even with color array.
			glColor4ub( 0xFF,0xFF,0xFF,0xFF );
			glColorPointer(4,
						   VertexComponentTypeMap[components->Color0Type], 
						   components->Color0Stride,
						   components->Color0);	
			testGlErrorParanoid();		
		}
		else
		{
//			if (!Material.getFlag(EMF_LIGHTING))
//			{
//				glDisable(GL_COLOR_MATERIAL);
//			}
			if (!(changeMask & EVA_COLOR0))
			{
				glDisableClientState(GL_COLOR_ARRAY);
				testGlErrorParanoid();	
			}
			
			SColor color = material.getDiffuseColor();
			glColor4ub( color.R, color.G, color.B, color.A );
			testGlErrorParanoid();	
			requiredAttribs &= ~EVA_COLOR0;
		}
	}
	else if (changeMask & EVA_COLOR0)
	{
//		if (!Material.getFlag(EMF_LIGHTING))
//		{
//			glDisable(GL_COLOR_MATERIAL);
//		}
		glDisableClientState(GL_COLOR_ARRAY);
		testGlErrorParanoid();	
	}

	// TODO: secondary color (components->Color1)
	
	testGlErrorDebug();	
	
	return requiredAttribs;
}

void
CCommonGLDriver::unsetupArrays(u32 requiredAttribs,
							   const S3DVertexComponentArrays* components,
							   bool wasInTextureMatrixMode)
{
	// Undo matrices changes in reverse order, this reduces the number of
	// glMatrixMode calls.
	u32 requiredTexCoords = requiredAttribs & EVA_TEXCOORDS_MASK;
	for (u32 i = 0; requiredTexCoords; ++i)
	{
		const u32 unitMask = EVA_TEXCOORD0 << i;
		if ((requiredTexCoords & unitMask)
			&& components->TexCoord[i].Type != S3DVertexComponentArrays::ECT_FLOAT)
		{
			setClientActiveTexture(i);
			testGlErrorParanoid();	
#if !defined(_IRR_COMPILE_WITH_PS3_)
			_IRR_DEBUG_BREAK_IF(!wasInTextureMatrixMode || getOGLInteger(GL_MATRIX_MODE) != GL_TEXTURE);
#else
			_IRR_DEBUG_BREAK_IF(!wasInTextureMatrixMode);
#endif
			glPopMatrix();
		}
		requiredTexCoords &= ~unitMask;
	}
	if (wasInTextureMatrixMode)
	{
#if !defined(_IRR_COMPILE_WITH_PS3_)
		_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_TEXTURE);
#endif
		glMatrixMode(GL_MODELVIEW);
	}
	if (components->PositionType != S3DVertexComponentArrays::ECT_FLOAT)
	{
#if !defined(_IRR_COMPILE_WITH_PS3_)
		_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif
		glPopMatrix();
	}

	// Save required attributes settings for next time
	LastRequiredVertexAttributes = requiredAttribs;

#if !defined(_IRR_COMPILE_WITH_PS3_)
	_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif
}

void CCommonGLDriver::setupArrayEnables(u32 requiredAttribs)
{
	if (LastRequiredVertexAttributes != requiredAttribs)
	{
		u32 changeMask = LastRequiredVertexAttributes ^ requiredAttribs;

		if (changeMask & EVA_NORMAL)
		{
			if (requiredAttribs & EVA_NORMAL)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				testGlErrorParanoid();	
			}
			else
			{
				glDisableClientState(GL_NORMAL_ARRAY);
				testGlErrorParanoid();	
			}
		}

		if (changeMask & EVA_COLOR0)
		{
			if (requiredAttribs & EVA_COLOR0)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				testGlErrorParanoid();	
			}
			else
			{
				glDisableClientState(GL_COLOR_ARRAY);
				testGlErrorParanoid();	
			}
		}
		// TODO: EVA_COLOR1 with separate color extension

		u32 texMask = LastRequiredVertexAttributes & EVA_TEXCOORDS_MASK;
		u32 texChangedMask = changeMask & EVA_TEXCOORDS_MASK;
		for (u32 i = 0; texMask | texChangedMask; ++i)
		{
			u32 unitMask = EVA_TEXCOORD0 << i;
			if (changeMask & unitMask)
			{
				setClientActiveTexture(i);
				if (texMask & unitMask)
				{
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					testGlErrorParanoid();	
				}
				else
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					testGlErrorParanoid();	
				}
			}
			texMask &= ~unitMask;
			texChangedMask &= ~unitMask;
		}

		LastRequiredVertexAttributes = requiredAttribs;
	}
}

void
CCommonGLDriver::softTexGen(u32 texGenEnabled,
							SScopedProcessArray<core::vector2df> texGenBuffer[MATERIAL_MAX_TEXTURES],
							S3DVertexComponentArrays* components,
							u32 startIndex,
							u32 endIndex)
{
	// TODO: cache view*world
	core::matrix4 modelview(core::matrix4::EM4CONST_NOTHING);
	Matrices[ETS_VIEW].mult34(Matrices[ETS_WORLD], modelview);
	// TODO: try to detect scaling and compute this if there is:
	//core::matrix4 invXPose(modelview,
	//					   core::matrix4::EM4CONST_INVERSE_TRANSPOSED);

	for (int texGenUnit = 0; texGenEnabled; ++texGenUnit)
	{
		const int unitMask = (1 << texGenUnit);
		if (texGenEnabled & unitMask)
		{
			// remove from mask now in case of errors later in the loop
			texGenEnabled &= ~unitMask;

			texGenBuffer[texGenUnit].reset(endIndex);
			if (!texGenBuffer[texGenUnit].get())
			{
				os::Printer::log("CCommonGLDriver::softTexGen",
								 "failed to allocate process buffer for tex "
								 "gen",
								 ELL_ERROR);
				continue;
			}

			switch (TexGenType[texGenUnit])
			{
				case ETGT_SPHERE_MAP :
				{
					if (components->PositionType
						!= S3DVertexComponentArrays::ECT_FLOAT)
					{
						os::Printer::log(
							"CCommonGLDriver::softTexGen",
							"software texgen from non float position not "
							"supported",
							ELL_ERROR
						);
					}
					else if (components->Normal == NULL)
					{
						os::Printer::log(
							"COpenGLESDriver::softTexGen",
							"requesting sphere map tex gen with no normals",
							ELL_ERROR
						);
					}
					else if (components->NormalType
							 != S3DVertexComponentArrays::ECT_FLOAT)
					{
						os::Printer::log(
							"CCommonGLDriver::softTexGen",
							"software texgen from non float normals not "
							"supported",
							ELL_ERROR
						);
					}
					else
					{
						// TODO: check if it would be better to parse indices
						computeSphereMapTexCoords(modelview,
												  0,
												  startIndex,
												  endIndex,
												  components->Position,
												  components->PositionStride,
												  components->Normal,
												  components->NormalStride,
												  texGenBuffer[texGenUnit].get(),
												  sizeof(core::vector2df),
												  getOrientation3D());

						// Update components
						_IRR_DEBUG_BREAK_IF(texGenUnit >= S3DVertexComponentArrays::MAX_TEXTURES);
						S3DVertexComponentArrays::STexCoordComponent& texCoord
							= components->TexCoord[texGenUnit];
						texCoord.Type = S3DVertexComponentArrays::ECT_FLOAT;
						texCoord.Coord = texGenBuffer[texGenUnit].get();
						texCoord.Stride = sizeof(core::vector2df);
					}
				}
				break;

				case ETGT_SIMPLE_SPHERE_MAP :
				{
					if (components->Normal == NULL)
					{
						os::Printer::log(
							"COpenGLESDriver::softTexGen",
							"requesting sphere map tex gen with no normals",
							ELL_ERROR
						);
					}
					else if (components->NormalType
							 != S3DVertexComponentArrays::ECT_FLOAT)
					{
						os::Printer::log(
							"CCommonGLDriver::softTexGen",
							"software texgen from non float normals not "
							"supported",
							ELL_ERROR
						);
					}
					else
					{
						// TODO: check if it would be better to parse indices
						computeSimpleSphereMapTexCoords(modelview,
														startIndex,
														endIndex,
														components->Normal,
														components->NormalStride,
														texGenBuffer[texGenUnit].get(),
														sizeof(core::vector2df),
														false,
														getOrientation3D());
						
						// Update components
						_IRR_DEBUG_BREAK_IF(texGenUnit >= S3DVertexComponentArrays::MAX_TEXTURES);
						S3DVertexComponentArrays::STexCoordComponent& texCoord
							= components->TexCoord[texGenUnit];
						texCoord.Type = S3DVertexComponentArrays::ECT_FLOAT;
						texCoord.Coord = texGenBuffer[texGenUnit].get();
						texCoord.Stride = sizeof(core::vector2df);
					}
				}
				break;

				// add more as required
			}
		}
	}
}

//! draws a vertex primitive list
void
CCommonGLDriver::drawVertexPrimitiveList(const void* vertices,
										 const void* indexList,
										 u32 startIndex,
										 u32 endIndex,
										 u32 primitiveCount,
										 E_VERTEX_TYPE vType,
										 scene::E_PRIMITIVE_TYPE pType,
										 E_INDEX_TYPE iType,
										 bool fVBOFlag)
{
	testGlErrorParanoid();	
	applyMatricesChanges(ResetRenderStates);

	if (!primitiveCount || startIndex == endIndex)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, indexList, startIndex, endIndex, primitiveCount, vType, pType, iType);

	// convert to components everything
	SScopedProcessArray<SColor> colorBuffer;
	if(vType != EVT_COMPONENT_ARRAYS)
	{
		// VBOs are now implemented via S3DVertexComponentArrays
		_IRR_DEBUG_BREAK_IF(fVBOFlag);
		convertVertexType(const_cast<void*>(vertices), startIndex, endIndex, vType, colorBuffer);
		vertices = &VertexComponents;
	}

	if(CurrentRenderMode == ERM_2D)
	{	
#ifdef _IRR_WITH_FRAME_STATISTICS_
		// decreased the 3D DrawCalls and increase the 
		// DrawCalls2D this is in fact a 2D call
		++DrawCalls2D;
		--DrawCalls;
#endif

		setRenderStates2DMode();
	}
	else
	{
		setRenderStates3DMode();
	}
	testGlErrorParanoid();

	//
	// setup arrays
	//

	_IRR_DEBUG_BREAK_IF(vertices == NULL);
	const S3DVertexComponentArrays* components
		= reinterpret_cast<const S3DVertexComponentArrays*>(vertices);

	u32 requiredAttribs = getCurrentRequiredAttributes();
	// no normals and texcoords for point sprites
	if ((pType == scene::EPT_POINTS) || (pType == scene::EPT_POINT_SPRITES))
	{
		requiredAttribs &= ~(EVA_NORMAL | EVA_TEXCOORDS_MASK);
	}

	_IRR_DEBUG_BREAK_IF(components->Position == NULL && !fVBOFlag);

	// check for texture coordinates generation

	SScopedProcessArray<core::vector2df> texGenBuffer[MATERIAL_MAX_TEXTURES];
	// Tex gen only for required coordinates
	u32 texGenEnabled = TexGenEnabled & getTexCoordMask(requiredAttribs);
	if (texGenEnabled)
	{
		if (fVBOFlag)
		{
			os::Printer::log("CCommonGLDriver::drawVertexPrimitiveList",
							 "software texgen from VBO not supported",
							 ELL_WARNING);
		}
		else
		{
			VertexComponents = *components;
			components = &VertexComponents;
			softTexGen(texGenEnabled,
					   texGenBuffer,
					   &VertexComponents,
					   startIndex,
					   endIndex);
		}
	}
	testGlErrorParanoid();
	// register arrays to GL

	bool wasInTextureMatrixMode;
	requiredAttribs = setupArrays(requiredAttribs,
								  components,
								  fVBOFlag,
								  &wasInTextureMatrixMode);
	testGlErrorParanoid();

	//
	// Draw
	//

// 	GLboolean colorMaterialEnabled = glIsEnabled(GL_COLOR_MATERIAL);
// 	GLfloat ambient[4];
// 	GLfloat diffuse[4];
// 	GLfloat specular[4];
// 	GLfloat emission[4];
// 	glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
// 	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
// 	glGetMaterialfv(GL_FRONT, GL_SPECULAR, specular);
// 	glGetMaterialfv(GL_FRONT, GL_EMISSION, emission);
// 	GLint env;
// 	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &env);
// 	GLint combine_rgb, combine_alpha;
// 	glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_RGB, &combine_rgb);
// 	glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, &combine_alpha);

// 	GLboolean blending = glIsEnabled(GL_BLEND);
// 	GLint blend_src_rgb, blend_dst_rgb;
// 	glGetIntegerv(GL_BLEND_SRC_RGB, &blend_src_rgb);
// 	glGetIntegerv(GL_BLEND_DST_RGB, &blend_dst_rgb);

// 	GLboolean alphatest = glIsEnabled(GL_ALPHA_TEST);
// 	GLfloat alpha_ref;
// 	GLint alpha_func;
// 	glGetFloatv(GL_ALPHA_TEST_REF, &alpha_ref);
// 	glGetIntegerv(GL_ALPHA_TEST_FUNC, &alpha_func);

	const GLenum indexSize = IndexTypeMap[iType];
	const SMaterial& material = getCurrentMaterial();

	switch (pType)
	{
		case scene::EPT_POINTS:
		case scene::EPT_POINT_SPRITES:
		{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#if defined(GL_ARB_point_sprite) || defined(GL_OES_point_sprite)
			if (pType==scene::EPT_POINT_SPRITES && (FeatureAvailable[IRR_ARB_point_sprite] || FeatureAvailable[IRR_OES_point_sprite]))
				glEnable(GL_POINT_SPRITE);
	#endif
			float quadratic[] = {0.0f, 0.0f, 10.01f};
			pointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
			float maxParticleSize=1.0f;
			glGetFloatv(GL_POINT_SIZE_MAX, &maxParticleSize);
//			maxParticleSize=maxParticleSize<Material.Thickness?maxParticleSize:Material.getThickness();
//			pointParameterf(GL_POINT_SIZE_MAX, maxParticleSize);
//			pointParameterf(GL_POINT_SIZE_MIN, Material.getThickness());
			pointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
			glPointSize(getCurrentMaterial().getThickness());
	#if defined(GL_ARB_point_sprite) || defined(GL_OES_point_sprite)
			if (pType==scene::EPT_POINT_SPRITES && (FeatureAvailable[IRR_ARB_point_sprite] || FeatureAvailable[IRR_OES_point_sprite]))
				glTexEnvf(GL_POINT_SPRITE,GL_COORD_REPLACE, GL_TRUE);
	#endif
			glDrawArrays(GL_POINTS, 0, primitiveCount);
	#if defined(GL_ARB_point_sprite) || defined(GL_OES_point_sprite)
			if (pType==scene::EPT_POINT_SPRITES && (FeatureAvailable[IRR_ARB_point_sprite] || FeatureAvailable[IRR_OES_point_sprite]))
			{
				glDisable(GL_POINT_SPRITE);
				glTexEnvf(GL_POINT_SPRITE,GL_COORD_REPLACE, GL_FALSE);
			}
	#endif
			testGlErrorParanoid();
#endif
		}
			break;
		case scene::EPT_LINE_STRIP:
			glDrawElements(GL_LINE_STRIP, primitiveCount+1, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_LINES:
			glDrawElements(GL_LINES, primitiveCount*2, indexSize, indexList);
			testGlErrorParanoid();
			break;
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_)
		case scene::EPT_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, primitiveCount+2, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount+2, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_TRIANGLES:
			glDrawElements(GL_TRIANGLES, primitiveCount*3, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_QUAD_STRIP:
			glDrawElements(GL_QUAD_STRIP, primitiveCount*2+2, indexSize, indexList);
			testGlErrorParanoid();
			break;
		case scene::EPT_QUADS:
			glDrawElements(GL_QUADS, primitiveCount*4, indexSize, indexList);
			testGlErrorParanoid();
			break;
	#if defined(_IRR_COMPILE_WITH_PS3_)
		case scene::EPT_POLYGON:
			glDrawElements(GL_TRIANGLE_FAN, primitiveCount, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;	
	#else
		case scene::EPT_POLYGON:
			glDrawElements(GL_POLYGON, primitiveCount, indexSize, indexList);
			testGlErrorParanoid();
			break;
	#endif
#else
		//note that PolygonMode in OpenGL ES is not directly supported. The following provides
		//a workaround for PolygonMode(GL_LINES) or PolygonMode(GL_POINTS). However, the wireframe
		//mode, which corresponds to GL_LINES, is inefficiently emulated because a large number
		//of function calls are needed.
		//
		//QUAD_STRIP, QUADS and POLYGON are also not directly supported. We use triangle strips,
		//triangle loops to render them.
		case scene::EPT_TRIANGLE_STRIP:
			if(material.getFlag(EMF_WIREFRAME))
			{
				for(u32 i = 0; i<primitiveCount; i++)
				{
					glDrawElements(GL_LINE_LOOP, 3, indexSize, ((u16*)indexList)+i);
					testGlErrorParanoid();
				}
			}
			else
				glDrawElements(material.getFlag(EMF_POINTCLOUD) ? GL_POINTS : GL_TRIANGLE_STRIP, primitiveCount+2, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;
		case scene::EPT_TRIANGLE_FAN:
			if(material.getFlag(EMF_WIREFRAME))
			{
				//draw the outer line loop
				glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, ((u16*)indexList));
				testGlErrorParanoid();
		
				//draw inner lines
				u16 lineIndex[2];
				lineIndex[0] = ((u16*)indexList)[0];
				for(u32 i = 2; i <= primitiveCount; i++)
				{
					lineIndex[1] = ((u16*)indexList)[i];
					glDrawElements(GL_LINES, 2, indexSize, lineIndex);
					testGlErrorParanoid();
				}
			}
			glDrawElements(material.getFlag(EMF_POINTCLOUD) ? GL_POINTS : GL_TRIANGLE_FAN, primitiveCount+2, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;
		case scene::EPT_TRIANGLES:
			if(material.getFlag(EMF_WIREFRAME))
			{
				for(u32 i = 0; i < primitiveCount; i++)
				{
					glDrawElements(GL_LINE_LOOP, 3, indexSize, ((u16*)indexList)+(i*3));
					testGlErrorParanoid();
				}
			}
			else
 				glDrawElements(material.getFlag(EMF_POINTCLOUD) ? GL_POINTS : GL_TRIANGLES, primitiveCount*3, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;
		case scene::EPT_QUAD_STRIP:
			if(material.getFlag(EMF_WIREFRAME))
			{
				u16 lineIndex[4];
				for(u32 i = 0; i < primitiveCount; i++)
				{
					lineIndex[0] = ((u16*)indexList)[i*2];
					lineIndex[1] = ((u16*)indexList)[i*2+1];
					lineIndex[2] = ((u16*)indexList)[i*2+3];
					lineIndex[3] = ((u16*)indexList)[i*2+2];
					glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, lineIndex);
				}
			}
			else
				glDrawElements(material.getFlag(EMF_POINTCLOUD) ? GL_POINTS : GL_TRIANGLE_STRIP, primitiveCount*2+2, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;
		case scene::EPT_QUADS:
			if(material.getFlag(EMF_WIREFRAME))
			{
				for(u32 i = 0; i < primitiveCount; i++)
					glDrawElements(GL_LINE_LOOP, 4, indexSize, ((u16*)indexList)+i*4);
			}
			else if(material.getFlag(EMF_POINTCLOUD))
				glDrawElements(GL_POINTS, primitiveCount*4, indexSize, ((u16*)indexList));
			else
			{
                for(u32 i = 0; i < primitiveCount; i++)
					glDrawElements(GL_TRIANGLE_FAN, 4, indexSize, ((u16*)indexList)+i*4);
			}
			testGlErrorParanoid();
			break;
		case scene::EPT_POLYGON:
			if(material.getFlag(EMF_WIREFRAME))
			{
				for(u32 i = 0; i < primitiveCount; i++)
					glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, ((u16*)indexList));
			}
			else
				glDrawElements(material.getFlag(EMF_POINTCLOUD) ? GL_POINTS : GL_TRIANGLE_FAN, primitiveCount, indexSize, ((u16*)indexList));
			testGlErrorParanoid();
			break;	
#endif
	}

	testGlErrorDebug();

	unsetupArrays(requiredAttribs, components, wasInTextureMatrixMode);
}

void
CCommonGLDriver::drawQuads(const core::rect<s32>& npos,
						   const core::rect<f32>& tcoords,
						   const SColor* colors)
{
 	//lower right, upper right, lower left, upper left
	QuadBuffer[0].set(npos.LowerRightCorner.X, npos.LowerRightCorner.Y, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y, colors[2]);
	QuadBuffer[1].set(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y,  tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y,  colors[3]);
	QuadBuffer[2].set(npos.UpperLeftCorner.X,  npos.LowerRightCorner.Y, tcoords.UpperLeftCorner.X,  tcoords.LowerRightCorner.Y, colors[1]);
	QuadBuffer[3].set(npos.UpperLeftCorner.X,  npos.UpperLeftCorner.Y,  tcoords.UpperLeftCorner.X,  tcoords.UpperLeftCorner.Y,  colors[0]);

#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
 	//lower right, upper right, lower left; lower left, upper right, upper left
	static const u16 indices[] = { 0, 1, 2, 
		                           2, 1, 3 };
#else
	// lower left, upper left, lower right; lower right, upper left, upper right
	static const u16 indices[] = { 2, 3, 0, 
					               0, 3, 1 };
#endif

	drawVertexPrimitiveList(&QuadComponents, indices, 0, 6, 2, EVT_COMPONENT_ARRAYS, scene::EPT_TRIANGLES, EIT_16BIT);
}

//! draws a 2d image, using a color and the alpha channel of the texture if
//! desired. The image is drawn at pos, clipped against clipRect (if != 0).
//! Only the subtexture defined by sourceRect is used.
//! @deprecated 
void
CCommonGLDriver::draw2DImage(const video::ITexture* texture,
							 const core::position2d<s32>& pos,
							 const core::rect<s32>& sourceRect,
							 const core::rect<s32>* clipRect, 
							 const SColor color,
							 bool useAlphaChannelOfTexture)
{
	DEPRECATED("This method is deprecated, you should set your materials/texture with set2DTexture/set2DUseVertexAlpha and draw with draw2DRectangle(destRect, sourceRect, colors, clipRect = 0)", "4 juin 2009");
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	// Setup the 2D Material
	set2DTexture(texture,useAlphaChannelOfTexture);
	set2DUseVertexAlpha(color.A < 255);

	// Calc the position of the target
	core::rect<s32> poss(pos, core::abs(sourceRect.getSize()));
#ifdef _IRR_ENABLE_DRAW2DIMAGE_MANUAL_VIEWPORT_CLIPPING_
	// This part is not even used in the clip...
	if (!clip(poss, tcoords, core::rect<s32>(core::position2d<s32>(0,0),
											 getCurrentRenderTargetSize())))
	{
		return;
	}
#endif

	// Setup the color array
	SColor colorsArray[4] = {color, color, color, color };

	draw2DRectangle(poss, sourceRect, colorsArray, clipRect);
}


//! The same, but with a four element array of colors, one for each vertex
//! @deprecated 
void
CCommonGLDriver::draw2DImage(const video::ITexture* texture,
							 const core::rect<s32>& destRect,
							 const core::rect<s32>& sourceRect,
							 const core::rect<s32>* clipRect,
							 const video::SColor* const colors,
							 bool useAlphaChannelOfTexture)
{
	DEPRECATED("This method is deprecated, you should set your materials/texture with set2DTexture/set2DUseVertexAlpha and draw with draw2DRectangle(destRect, sourceRect, colors, clipRect = 0)", "4 juin 2009");
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	if (!texture)
		return;

	// Setup the 2D Material
	set2DTexture(texture,useAlphaChannelOfTexture);
	set2DUseVertexAlpha(true);
	if(colors)
	{ 
		draw2DRectangle(destRect, sourceRect, colors, clipRect);
	}
	else
	{
		const SColor colorsArray[4] = { SColor(0xff,0xff,0xff,0xff),
									    SColor(0xff,0xff,0xff,0xff),
									    SColor(0xff,0xff,0xff,0xff),
									    SColor(0xff,0xff,0xff,0xff) };
		draw2DRectangle(destRect, sourceRect, colorsArray, clipRect);
	}
}

//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired. The images are drawn beginning at pos and concatenated
//! in one line. All drawings are clipped against clipRect (if != 0).
//! The subtextures are defined by the array of sourceRects and are chosen
//! by the indices given.
//! @deprecated 
void
CCommonGLDriver::draw2DImage(const video::ITexture* texture,
							 const core::position2d<s32>& pos,
							 const core::array<core::rect<s32> >& sourceRects,
							 const core::array<s32>& indices,
							 const core::rect<s32>* clipRect, 
							 const SColor color,
							 bool useAlphaChannelOfTexture)
{
	DEPRECATED("This method is deprecated, you should set your materials/texture with set2DTexture/set2DUseVertexAlpha and draw with draw2DRectangle(destRect, sourceRect, colors, clipRect = 0)", "4 juin 2009");
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	if (!texture)
		return;

	// Setup the 2D Material
	set2DTexture(texture,useAlphaChannelOfTexture);
	set2DUseVertexAlpha(color.A < 255);

	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::position2d<s32> targetPos(pos);
	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);

	SColor colorsArray[4] = {color, color, color, color };

	for (u32 i = 0; i < indices.size(); ++i)
	{
		const s32 currentIndex = indices[i];

		if (!sourceRects[currentIndex].isValid())
			break;

		core::dimension2d<s32> dims = core::abs(sourceRects[currentIndex].getSize());
		core::rect<s32> poss(targetPos, dims);

		draw2DRectangle(poss, sourceRects[currentIndex], colorsArray, clipRect);
		targetPos.X += dims.Width;
	}
}

//! draw a 2d rectangle
//! @deprecated 
void
CCommonGLDriver::draw2DRectangle(SColor color,
								 const core::rect<s32>& position,
								 const core::rect<s32>* clip)
{
	DEPRECATED("This method is deprecated, you should set your materials/texture with set2DTexture/set2DUseVertexAlpha and draw with draw2DRectangle(destRect, sourceRect, colors, clipRect = 0)", "4 juin 2009");
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	// Setup the 2D Material, removing the texture
	set2DTexture(0,false);
	set2DUseVertexAlpha(color.A < 255);

	SColor colorsArray[4] = {color, color, color, color };
	const core::rect<s32> source;
	draw2DRectangle(position, source, colorsArray, clip);
}


//! draw an 2d rectangle
//! @deprecated 
void
CCommonGLDriver::draw2DRectangle(const core::rect<s32>& position,
								 SColor colorLeftUp,
								 SColor colorRightUp,
								 SColor colorLeftDown,
								 SColor colorRightDown,
								 const core::rect<s32>* clip)
{
	DEPRECATED("This method is deprecated, you should set your materials/texture with set2DTexture/set2DUseVertexAlpha and draw with draw2DRectangle(destRect, sourceRect, colors, clipRect = 0)", "4 juin 2009");
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	set2DTexture(0,false);
	set2DUseVertexAlpha((colorLeftUp.A < 255 ||
						 colorRightUp.A < 255 ||
						 colorLeftDown.A < 255 ||
						 colorRightDown.A < 255));

	SColor colorsArray[4] = {colorLeftUp, colorRightUp, colorRightDown, colorLeftDown };
	const core::rect<s32> source;
	draw2DRectangle(position, source, colorsArray, clip);
}

//! Draws a 2d quad, using a color array for each vertex's color and the current 
//! 2D material texture and alpha params. The image is drawn at destRect, clipped 
//! against clipRect (if != 0).  The current 2D Material is used, so make sure to 
//! set/unset your texture first.
//! Only the subtexture defined by sourceRect is used.
void
CCommonGLDriver::draw2DRectangle(const core::rect<s32>& destRect,
							     const core::rect<s32>& sourceRect,
							     const SColor colors[4],
							     const core::rect<s32>* clipRect)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	ITexture* texture = getCurrentMaterial().getTexture(0);
	if(texture)
	{
		// texcoords need to be flipped horizontally for RTTs
		const bool isRTT = texture->isRenderTarget();

		// find the texture's pixel ratio for textcoords
		const core::dimension2d<s32>& ss = texture->getOriginalSize();
		const f32 invW = 1.f / static_cast<f32>(ss.Width);
		const f32 invH = 1.f / static_cast<f32>(ss.Height);

		core::rect<f32> tcoords((f32)sourceRect.UpperLeftCorner.X * invW,
								(f32)sourceRect.UpperLeftCorner.Y * invH,
								(f32)sourceRect.LowerRightCorner.X * invW,
								(f32)sourceRect.LowerRightCorner.Y * invH);

		if (isRTT)
		{
			tcoords.UpperLeftCorner.Y = 1.0f - tcoords.UpperLeftCorner.Y;
			tcoords.LowerRightCorner.Y = 1.0f - tcoords.LowerRightCorner.Y;
		}

		core::rect<s32> pos( destRect );
		if ( !clipRect || clip(pos, tcoords, *clipRect) )
		{
			drawQuads(pos, tcoords, colors);
		}
	}
	else
	{
		// Clip the destination if needed
		core::rect<s32> poss( destRect );
		if ( clipRect )
			poss.clipAgainst(*clipRect);
		
		drawQuads(poss, core::rect<f32>(), colors);
	}
}


//! Draws a 2d line. This 2D call is not buffered, so it will flush the last batch. Use with care...
void
CCommonGLDriver::draw2DLine(const core::position2d<s32>& start,
							const core::position2d<s32>& end,
							SColor color)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	// Unset all materials stuff (will flush if needed)
	set2DTexture(0, false);
	set2DUseVertexAlpha(color.A < 255);

	// Make sure 2D states are applied
	setRenderStates2DMode();

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	// Make sure the matrices are good befor drawing
	applyMatricesChanges(false);

	setupArrayEnables(EVA_POSITION);
	glColor4ub(color.R, color.G, color.B, color.A);
	GLfloat vertices[] = {(f32)start.X, (f32)start.Y, 0.0f, (f32)end.X, (f32)end.Y, 0.0f};
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glDrawArrays(GL_LINES, 0, 2);

	testGlErrorParanoid();
}


bool
CCommonGLDriver::setTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= MaxTextureUnits)
		return false;

	if (CurrentTexture[stage] == texture)
	{
		// will still must check if the texture's state is dirty in case only
		// parameters have changed
		if (texture && texture->isDirty())
		{
			// should not happen due to a condition farther below
			_IRR_DEBUG_BREAK_IF(texture->getDriverType() != getDriverType());
			static_cast<const CCommonGLTexture*>(texture)->updateParameters();
		}
		return true;
	}

	setActiveTexture(stage);

	CurrentTexture[stage] = texture;

	if (!texture)
	{
		glDisable(GL_TEXTURE_2D);
		return true;
	}
	else
	{
		if (texture->getDriverType() != getDriverType())
		{
			CurrentTexture[stage] = NULL;
			glDisable(GL_TEXTURE_2D);
			os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
			return false;
		}

#ifdef _IRR_WITH_FRAME_STATISTICS_
		++TextureBindings;
#endif

		glEnable(GL_TEXTURE_2D);

#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) && defined(GL_EXT_texture_lod_bias)
		setTexEnvLodBias(texture->getLODBias());
#endif
		static_cast<const CCommonGLTexture*>(texture)->bind();
	}

	testGlErrorParanoid();
	return true;
}


//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
//! Returns whether disabling was successful or not.
bool
CCommonGLDriver::disableTextures(u32 fromStage)
{
	bool result=true;
	for (u32 i=fromStage; i<MaxTextureUnits; ++i)
		result &= setTexture(i, 0);
	return result;
}


//! creates a matrix in supplied GLfloat array to pass to OpenGL
inline
void
CCommonGLDriver::createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m)
{
	memcpy(gl_matrix, m.pointer(), 16 * sizeof(f32));
}


//! creates a opengltexturematrix from a D3D style texture matrix
inline
void
CCommonGLDriver::createGLTextureMatrix(GLfloat *o, const core::matrix4& m)
{
	o[0] = m[0];
	o[1] = m[1];
	o[2] = 0.f;
	o[3] = 0.f;

	o[4] = m[4];
	o[5] = m[5];
	o[6] = 0.f;
	o[7] = 0.f;

	o[8] = 0.f;
	o[9] = 0.f;
	o[10] = 1.f;
	o[11] = 0.f;

	o[12] = m[8];
	o[13] = m[9];
	o[14] = 0.f;
	o[15] = 1.f;
}

//! Sets a material. All 3d drawing functions draw geometry now
//! using this material.
//! \param material: Material to be used from now on.
void
CCommonGLDriver::setMaterial(const SMaterial& material)
{
	// Make sure we're not in 2D mode and set the material as 2D material change must use set2DMaterial()
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	setCurrentMaterial(material);
}


//! Set the current 2D Material to be used on the next 2D calls
void 
CCommonGLDriver::set2DMaterial(SMaterial& material)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	

	// Make sure it's a real 2D material before comparing it to the current
	if(material.getFlag((E_MATERIAL_FLAG)(EMF_ZBUFFER | EMF_ZWRITE_ENABLE | EMF_LIGHTING | EMF_BACK_FACE_CULLING)))
	{
		material.setMaterialType( material.getFlag(EMF_2D_USE_TEXTURE_ALPHA) || 
								  material.getFlag(EMF_2D_USE_VERTEX_ALPHA) 
										? EMT_2D_ALPHA 
										: EMT_SOLID);
		material.setFlag(EMF_ZBUFFER, false)
			    .setFlag(EMF_ZWRITE_ENABLE, false)
			    .setFlag(EMF_LIGHTING, false)
				.setFlag(EMF_BACK_FACE_CULLING, false);
	}

	if (!getCurrentMaterial().matches(material))
	{
		flush();
		setCurrentMaterial(material);
	}
}


//! Do the actual material change from the setMaterial and set2DMaterial
void 
CCommonGLDriver::setCurrentMaterial(const SMaterial& material)
{	
	if (getOption(EVDO_BATCHING) && !material.matches(getCurrentMaterial()))
	{
		flush();
	}
	DynamicBatch->setMaterial(material);

	ActualMaxDynamicBatchSegmentSize = core::min_(MaxDynamicBatchSegmentSize, 
		DynamicBatch->getMaxVertexCount());

	u32 texMask = getCurrentRequiredAttributes() & EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texMask; ++i)
	{
		u32 unitMask = EVA_TEXCOORD0 << i;
		if (texMask
			&& (material.hasTextureMatrix(i)
				|| !Matrices[ETS_TEXTURE_0 + i].getDefinitelyIdentityMatrix()
				|| (material.getTexture(i)
					&& material.getTexture(i)->isRenderTarget())))
		{
			
			setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + i),
						 material.getTextureMatrix(i));
		}
		texMask &= ~unitMask;
	}
}

//! Change the texture of the current 2D Material
void 
CCommonGLDriver::set2DTexture(const ITexture* tex, bool useTextureAlpha)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	SMaterial& material = DynamicBatch->getMaterial();
	if( tex != material.getTexture(0) ||
		material.getFlag(EMF_2D_USE_TEXTURE_ALPHA) != useTextureAlpha )
	{
		if (getOption(EVDO_BATCHING))
		{
			flush();
		}

		if (material.getMaterialType() != EMT_A8I8_GRADIENT)
			material.setMaterialType( useTextureAlpha || material.getFlag(EMF_2D_USE_VERTEX_ALPHA)  ? EMT_2D_ALPHA : EMT_SOLID);
		material.setFlag(EMF_2D_USE_TEXTURE_ALPHA, useTextureAlpha);
		material.setTexture(0,const_cast<ITexture*>(tex));
		
		DynamicBatch->commitMaterialChanges();
	}

}

//! Change the EMF_2D_USE_VERTEX_ALPHA flag for the current 2D material
void 
CCommonGLDriver::set2DUseVertexAlpha(bool value)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	SMaterial& material = DynamicBatch->getMaterial();
	if(material.getFlag(EMF_2D_USE_VERTEX_ALPHA) != value)
	{
		if (getOption(EVDO_BATCHING))
		{
			flush();
		}

		if (material.getMaterialType() != EMT_A8I8_GRADIENT)
			material.setMaterialType( value || material.getFlag(EMF_2D_USE_TEXTURE_ALPHA) ? EMT_2D_ALPHA : EMT_SOLID);
		material.setFlag(EMF_2D_USE_VERTEX_ALPHA, value);

		DynamicBatch->commitMaterialChanges();
	}
}


//! prints error if an error happened.
bool
CCommonGLDriver::testGLError()
{
#ifdef _DEBUG
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	case GL_TABLE_TOO_LARGE:
		os::Printer::log("GL_TABLE_TOO_LARGE", ELL_ERROR); break;
#if defined(GL_EXT_framebuffer_object)
	case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
		os::Printer::log("GL_INVALID_FRAMEBUFFER_OPERATION", ELL_ERROR); break;
#endif
#endif
	};
	return true;
#else
	return false;
#endif
}


//! sets the needed renderstates
void
CCommonGLDriver::setRenderStates3DMode()
{
	if (CurrentRenderMode != ERM_3D)
	{
		// Reset Texture Stages
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

		MatricesDirtyFlag |= ETSDF_WORLD | ETSDF_VIEW | ETSDF_PROJECTION;
		testGlErrorParanoid();

		ResetRenderStates = true;
	}

	const SMaterial& material = getCurrentMaterial();
	if (ResetRenderStates || LastMaterial != material)
	{
		// unset old material

		if (LastMaterial.getMaterialType() != material.getMaterialType() &&
			static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
		}

		// set new material.
		if (static_cast<u32>(material.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[material.getMaterialType()].Renderer->onSetMaterial(material, 
																				  LastMaterial, 
																				  ResetRenderStates, 
																				  this);
		}

		LastMaterial = material;
		ResetRenderStates = false;
	}

	if (static_cast<u32>(material.getMaterialType()) < MaterialRenderers.size())
		MaterialRenderers[material.getMaterialType()].Renderer->onRender(this, video::EVT_STANDARD);

	CurrentRenderMode = ERM_3D;
}


void
CCommonGLDriver::setRenderStates2DMode()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	const SMaterial& material = getCurrentMaterial();
	if (ResetRenderStates || LastMaterial != material)
	{
		// unset old material

		if (LastMaterial.getMaterialType() != material.getMaterialType() &&
			static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
		}

		// set new material.
		if (static_cast<u32>(material.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[material.getMaterialType()].Renderer->onSetMaterial(material, 
																				  LastMaterial, 
																				  ResetRenderStates, 
																				  this);
		}

		LastMaterial = material;
		ResetRenderStates = false;
	}

	if (static_cast<u32>(material.getMaterialType()) < MaterialRenderers.size())
		MaterialRenderers[material.getMaterialType()].Renderer->onRender(this, video::EVT_STANDARD);
}


void 
CCommonGLDriver::applyMatricesChanges(bool resetAllRenderStates)
{
	// If we're switching from batching mode, modelview matrix must be restored
	if (LastRenderWasDynamicBatch != IsFlushingDynamicBatch)
	{
		if (IsFlushingDynamicBatch)
		{
			// Reset View matrice as we don't need World in batching mode
			MatricesDirtyFlag |= ETSDF_VIEW;
		}
		else
		{
			// If world is identity, we can keep the current ModelView matrix 
			// as it's already set to view
			if (!Matrices[ETS_WORLD].getDefinitelyIdentityMatrix())
			{
				MatricesDirtyFlag |= ETSDF_VIEW | ETSDF_WORLD;
			}
		}
	}
	LastRenderWasDynamicBatch = IsFlushingDynamicBatch;
	
	// Only update if we reset all renderstates or if a matrice is dirty
	if (resetAllRenderStates || MatricesDirtyFlag & ETSDF_ALL_MATRICES)
	{
#if !defined(_IRR_COMPILE_WITH_PS3_)
		_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif

		if (resetAllRenderStates 
			|| MatricesDirtyFlag & ETSDF_VIEW 
			|| ( MatricesDirtyFlag & ETSDF_WORLD && !IsFlushingDynamicBatch) )
		{
			// OpenGL only has a model matrix, view and world is not existent. 
			// so lets fake these two.
			if (IsFlushingDynamicBatch)
			{
				glLoadMatrixf(Matrices[ETS_VIEW].pointer());
			}
			else
			{
				// Try to avoid matrix multiplication if possible
				if (!Matrices[ETS_VIEW].getDefinitelyIdentityMatrix() && 
					!Matrices[ETS_WORLD].getDefinitelyIdentityMatrix())
				{
					glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
				}
				else if (Matrices[ETS_WORLD].getDefinitelyIdentityMatrix())
				{
					glLoadMatrixf(Matrices[ETS_VIEW].pointer());
				}
				else
				{
					glLoadMatrixf(Matrices[ETS_WORLD].pointer());
				}
			}
			MatricesDirtyFlag &= ~(ETSDF_VIEW | ETSDF_WORLD);

			// we have to update the clip planes to the latest view matrix
			for (u32 i=0; i<MaxUserClipPlanes; ++i)
				if (UserClipPlaneEnabled[i])
					uploadClipPlane(i);
		}

		if (resetAllRenderStates || MatricesDirtyFlag & ETSDF_PROJECTION)
		{
			
 			core::matrix4 &glmat = Matrices[ETS_PROJECTION];
			
			// Correct the screen/scene orientation if we are rendering 2D
			if (CurrentRenderMode == ERM_2D && CurrentOrientation != EOO_0)
			{
				glmat.setTranslation(core::vector3df(-1,1,0));
				const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
				
				core::matrix4 r;
				switch(CurrentOrientation)
				{
					case EOO_0:
						break;
					case EOO_180:
						r.setTranslation(core::vector3df((float)renderTargetSize.Width,(float)renderTargetSize.Height,0));
						r.setRotationDegrees(core::vector3df(0,0,180));
						glmat *= r;
						break;
					case EOO_90:
						r.setTranslation(core::vector3df(0,(float)renderTargetSize.Height,0));
						r.setRotationDegrees(core::vector3df(0,0,-90));
						glmat *= r;
						break;
					case EOO_270:
						r.setTranslation(core::vector3df((float)renderTargetSize.Width,0,0));
						r.setRotationDegrees(core::vector3df(0,0,90));
						glmat *= r;
						break;
				}
			}	

			core::matrix4 glmat2 = glmat;
			if (glmat2[11] != 0)
			{
				float n = - glmat2[14] / glmat2[10];
				float f = n / (glmat2[10] + 1);
				glmat2[10] += n * glmat2[10] / f;
				glmat2[14] *= 2;
			}
			
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(glmat.pointer());
			glMatrixMode(GL_MODELVIEW);
			
			MatricesDirtyFlag &= ~ETSDF_PROJECTION;
		}

		if (IsFlushingDynamicBatch && (resetAllRenderStates || !(MatricesDirtyFlag & ETSDF_TEXTURE_BATCHING)))
		{
			glMatrixMode(GL_TEXTURE);
			// going in batching mode, set all texture matrices to identity
			for (u32 i=0; i < MaxTextureUnits; ++i)
			{
				setActiveTexture(i);
				glLoadIdentity();
			}
			MatricesDirtyFlag |= ETSDF_TEXTURE_BATCHING;
			MatricesDirtyFlag &= ~ETSDF_TEXTURES;
			glMatrixMode(GL_MODELVIEW);
		}
		else if (!IsFlushingDynamicBatch)
		{
			// Returning from batching, set all texture matrices as dirty
			if (resetAllRenderStates || MatricesDirtyFlag & ETSDF_TEXTURE_BATCHING)
			{
				MatricesDirtyFlag |= ETSDF_TEXTURES;
				MatricesDirtyFlag &= ~ETSDF_TEXTURE_BATCHING;
			}

			if (MatricesDirtyFlag & ETSDF_TEXTURES)
			{
				glMatrixMode(GL_TEXTURE);
				for (u32 i=0; i < MaxTextureUnits; ++i)
				{
					u32 state = ETS_TEXTURE_0+i;
					if (MatricesDirtyFlag & (1 << (state)))
					{
						const core::matrix4& mat = Matrices[state];
						const SMaterial& material = getCurrentMaterial();
						const bool isRTT = material.getTexture(i) && material.getTexture(i)->isRenderTarget();

						setActiveTexture(i);

						if (mat.getDefinitelyIdentityMatrix())
						{
							glLoadIdentity();
						}
						else
						{
							GLfloat glmat[16];
							if (isRTT)
								createGLTextureMatrix(glmat, mat * TextureFlipMatrix);
							else
								createGLTextureMatrix(glmat, mat);
						
							glLoadMatrixf(glmat);
						}
						
						MatricesDirtyFlag &= ~(1 << (ETS_TEXTURE_0+i));
					}
				}
				glMatrixMode(GL_MODELVIEW);
				MatricesDirtyFlag &= ~ETSDF_TEXTURES;
			}
		}
#if !defined(_IRR_COMPILE_WITH_PS3_)
		_IRR_DEBUG_BREAK_IF(getOGLInteger(GL_MATRIX_MODE) != GL_MODELVIEW);
#endif
	}
}

//! Can be called by an IMaterialRenderer to make its work easier.
void
CCommonGLDriver::setBasicRenderStates(const SMaterial& material, 
									  const SMaterial& lastmaterial,
									  bool resetAllRenderStates)
{
// 	if (resetAllRenderStates ||
// 		lastmaterial.getAmbientColor() != material.getAmbientColor() ||
// 		lastmaterial.getDiffuseColor() != material.getDiffuseColor() ||
// 		lastmaterial.getSpecularColor() != material.getSpecularColor() ||
// 		lastmaterial.getEmissiveColor() != material.getEmissiveColor() ||
// 		lastmaterial.getShininess() != material.getShininess())
// 	{
// 		setMaterialColor(material);
// 	}

	if(resetAllRenderStates || material.getFlag(EMF_LIGHTING))
	{
		setMaterialColor(EMCP_AMBIENT, material.getAmbientColor());
		if (material.getFlag(EMF_PER_VERTEX_MATERIAL_COLOR))
		{
			setColorMaterialEnable(true);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
			// committing to glMaterial will be done when quitting
			// GL_COLOR_MATERIAL mode
			ShadowState.Material.Color[EMCP_DIFFUSE] = material.getDiffuseColor();
#endif
		}
		else
		{
			setMaterialColor(EMCP_DIFFUSE, material.getDiffuseColor());
			setColorMaterialEnable(false);
		}
		//setColorMaterialEnable(material.isDiffuseBatchable());
// 		if (lastmaterial.getDiffuseColor() != material.getDiffuseColor())
// 		{
// 			if (material.isDiffuseBatchable())
// 			{
// 				setBaseColor(material.getDiffuseColor());
// 				//setColorMaterialEnable(true);
// 			}
// 			else
// 			{
// 				setMaterialColor(GL_DIFFUSE, material.getDiffuseColor());
// 				//setColorMaterialEnable(false);
// 			}
// 		}

		setMaterialShininess(material.getShininess());
		// disable Specular colors if no shininess is set
		if (material.getShininess() != 0.0f)
		{
#ifdef GL_EXT_separate_specular_color
			setLightModelColorControl(GL_SEPARATE_SPECULAR_COLOR);
#endif
			setMaterialColor(EMCP_SPECULAR, material.getSpecularColor());
			testGlErrorParanoid();
		}
#ifdef GL_EXT_separate_specular_color
		else
		{
			setLightModelColorControl(GL_SINGLE_COLOR);
			testGlErrorParanoid();
		}
#endif

		setMaterialColor(EMCP_EMISSION, material.getEmissiveColor());
	}

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	// fillmode
	if (resetAllRenderStates || (lastmaterial.getFlag(EMF_WIREFRAME) != material.getFlag(EMF_WIREFRAME))
		|| (lastmaterial.getFlag(EMF_POINTCLOUD) != material.getFlag(EMF_POINTCLOUD)))
		glPolygonMode(GL_FRONT_AND_BACK, material.getFlag(EMF_WIREFRAME) ? GL_LINE : material.getFlag(EMF_POINTCLOUD) ? GL_POINT : GL_FILL);
#endif

	// shademode
	if (resetAllRenderStates || (lastmaterial.getFlag(EMF_GOURAUD_SHADING) != material.getFlag(EMF_GOURAUD_SHADING)))
	{
		if (material.getFlag(EMF_GOURAUD_SHADING))
			glShadeModel(GL_SMOOTH);
		else
			glShadeModel(GL_FLAT);
		testGlErrorParanoid();
	}

	// lighting
	if (resetAllRenderStates || (lastmaterial.getFlag(EMF_LIGHTING) != material.getFlag(EMF_LIGHTING)))
	{
		if (material.getFlag(EMF_LIGHTING))
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
		testGlErrorParanoid();
	}

	// zbuffer
	if (resetAllRenderStates || lastmaterial.getFlag(EMF_ZBUFFER) != material.getFlag(EMF_ZBUFFER))
	{
		if (material.getFlag(EMF_ZBUFFER))
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
		testGlErrorParanoid();
	}
	if (resetAllRenderStates || material.getZBufferFunc() != lastmaterial.getZBufferFunc())
	{
		glDepthFunc(ZBufferFuncMap[material.getZBufferFunc()]);
	}

	// zwrite
	IMaterialRenderer* mr = getMaterialRenderer(material.getMaterialType());
	setDepthMask(material.getFlag(EMF_ZWRITE_ENABLE)
				 && (!(mr && mr->isTransparent()) || getOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT)));
	testGlErrorParanoid();

	// back face culling
	if (resetAllRenderStates
		|| (lastmaterial.getFlag(EMF_FRONT_FACE_CULLING) != material.getFlag(EMF_FRONT_FACE_CULLING))
		|| (lastmaterial.getFlag(EMF_BACK_FACE_CULLING) != material.getFlag(EMF_BACK_FACE_CULLING)))
	{
		if (material.getFlag(EMF_FRONT_FACE_CULLING) && (material.getFlag(EMF_BACK_FACE_CULLING)))
		{
			glCullFace(GL_FRONT_AND_BACK);
			glEnable(GL_CULL_FACE);
		}
		else if (material.getFlag(EMF_BACK_FACE_CULLING))
		{
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
		}
		else if (material.getFlag(EMF_FRONT_FACE_CULLING))
		{
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
		testGlErrorParanoid();
	}

	// fog
	if (resetAllRenderStates || lastmaterial.getFlag(EMF_FOG_ENABLE) != material.getFlag(EMF_FOG_ENABLE))
	{
		if (material.getFlag(EMF_FOG_ENABLE))
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
		testGlErrorParanoid();
	}

	// normalization
	if (resetAllRenderStates || lastmaterial.getFlag(EMF_NORMALIZE_NORMALS) != material.getFlag(EMF_NORMALIZE_NORMALS))
	{
		if (material.getFlag(EMF_NORMALIZE_NORMALS))
			glEnable(GL_NORMALIZE);
		else
			glDisable(GL_NORMALIZE);
		testGlErrorParanoid();
	}

	// thickness
	if (resetAllRenderStates || lastmaterial.getThickness() != material.getThickness())
	{
		glPointSize(material.getThickness());
		glLineWidth(material.getThickness());
		testGlErrorParanoid();
	}



	// be sure to leave in texture stage 0
	setActiveTexture(0); // is it really useful now?
}


//! \return Returns the name of the video driver.
const wchar_t*
CCommonGLDriver::getName() const
{
	return Name.c_str();
}


//! deletes all dynamic lights there are
void
CCommonGLDriver::deleteAllDynamicLights()
{
	for (s32 i=0; i<LastSetLight+1; ++i)
		glDisable(GL_LIGHT0 + i);
	testGlErrorParanoid();

	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
void
CCommonGLDriver::addDynamicLight(const SLight& light)
{
	if (LastSetLight == MaxLights-1)
		return;

	// We need to set Wolrd to identity and apply the changes now!
	setTransform(ETS_WORLD, core::matrix4());
	applyMatricesChanges(false);

	++LastSetLight;
	CNullDriver::addDynamicLight(light);

	s32 lidx = GL_LIGHT0 + LastSetLight;
	GLfloat data[4];

	switch (light.Type)
	{
	case video::ELT_SPOT:
		data[0] = light.Direction.X;
		data[1] = light.Direction.Y;
		data[2] = light.Direction.Z;
		data[3] = 0.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, light.Falloff);
		glLightf(lidx, GL_SPOT_CUTOFF, light.OuterCone);
	break;
	case video::ELT_POINT:
		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	case video::ELT_DIRECTIONAL:
		// set direction
#if defined(_IRR_COMPILE_WITH_PS3_)
		data[0] = -light.Direction.X;
		data[1] = -light.Direction.Y;
		data[2] = -light.Direction.Z;
#else
		data[0] = light.Direction.X;
		data[1] = light.Direction.Y;
		data[2] = light.Direction.Z;
#endif
		data[3] = 0.0f; // 0.0f for directional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
	break;
	}

	// set diffuse color
	data[0] = light.DiffuseColor.R;
	data[1] = light.DiffuseColor.G;
	data[2] = light.DiffuseColor.B;
	data[3] = light.DiffuseColor.A;
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	data[0] = light.SpecularColor.R;
	data[1] = light.SpecularColor.G;
	data[2] = light.SpecularColor.B;
	data[3] = light.SpecularColor.A;
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	data[0] = light.AmbientColor.R;
	data[1] = light.AmbientColor.G;
	data[2] = light.AmbientColor.B;
	data[3] = light.AmbientColor.A;
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linear * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, light.Attenuation.X);
	glLightf(lidx, GL_LINEAR_ATTENUATION, light.Attenuation.Y);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, light.Attenuation.Z);

	glEnable(lidx);

	testGlErrorParanoid();
}


//! returns the maximal amount of dynamic lights the device can handle
u32
CCommonGLDriver::getMaximalDynamicLightAmount() const
{
	return MaxLights;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void
CCommonGLDriver::setAmbientLight(const SColorf& color)
{
	AmbientLight = color;
	GLfloat data[4] = {color.R, color.G, color.B, color.A};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
	testGlErrorParanoid();
}


void
CCommonGLDriver::setLogicalScreenSize(const core::dimension2d<s32>& size)
{
	flush();
	LogicalScreenSize = size.Width > 0 && size.Height > 0 ? size : core::dimension2d<s32>();
	setViewPort(core::rect<s32>(core::position2d<s32>(0,0), getScreenSize()));
}

const core::dimension2d<s32>&
CCommonGLDriver::getScreenSize() const
{
	return LogicalScreenSize.Width > 0 ? LogicalScreenSize : ScreenSize;
}


// this code was sent in by Oliver Klems, thank you! (I modified the glViewport
// method just a bit.
void
CCommonGLDriver::setViewPort(const core::rect<s32>& area)
{
	flush();
	core::rect<s32> vp = area;
	const core::dimension2d<s32>& logical = getCurrentRenderTargetSize();
	core::rect<s32> rendert(0,0, logical.Width, logical.Height);
	vp.clipAgainst(rendert);

	// Render targets retain their own pixel coordinates. Only the window scales.
	const core::dimension2d<s32>& physical = RenderTargetTexture ? logical : ScreenSize;
	if (vp.getHeight()>0 && vp.getWidth()>0)
	{
		const s32 left = vp.UpperLeftCorner.X * physical.Width / logical.Width;
		const s32 right = vp.LowerRightCorner.X * physical.Width / logical.Width;
		const s32 top = vp.UpperLeftCorner.Y * physical.Height / logical.Height;
		const s32 bottom = vp.LowerRightCorner.Y * physical.Height / logical.Height;
		glViewport(left, physical.Height - bottom, right - left, bottom - top);
	}

	// Projection, hit testing and camera shake continue to use game coordinates.
	ViewPort = vp;
	testGlErrorParanoid();
}


//! Sets the fog mode.
void
CCommonGLDriver::setFog(SColor c, 
						bool linearFog, 
						f32 start,
						f32 end, 
						f32 density, 
						bool pixelFog, 
						bool rangeFog)
{
	CNullDriver::setFog(c, linearFog, start, end, density, pixelFog, rangeFog);
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glFogf(GL_FOG_MODE, GLfloat(linearFog ? GL_LINEAR : GL_EXP));
#else
	glFogi(GL_FOG_MODE, linearFog ? GL_LINEAR : GL_EXP);
	#ifdef GL_EXT_fog_coord
	if (FeatureAvailable[IRR_EXT_fog_coord])
		glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
	#endif
#endif

	if(linearFog)
	{
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
	}
	else
		glFogf(GL_FOG_DENSITY, density);

	if (pixelFog)
		glHint(GL_FOG_HINT, GL_NICEST);
	else
		glHint(GL_FOG_HINT, GL_FASTEST);

	SColorf color(c);
	GLfloat data[4] = {color.R, color.G, color.B, color.A};
	glFogfv(GL_FOG_COLOR, data);
	testGlErrorParanoid();
}



//! Draws a 3d line.
void
CCommonGLDriver::draw3DLine(const core::vector3df& start,
							const core::vector3df& end,
							SColor color)
{
	flush();
	setRenderStates3DMode();

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif

	// Make sure the matrices are good befor drawing
	applyMatricesChanges(false);

	setupArrayEnables(EVA_POSITION);
	glColor4ub( color.R, color.G, color.B, color.A );
	GLfloat vertices[] = {start.X, start.Y, start.Z, end.X, end.Y, end.Z};
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glDrawArrays(GL_LINES, 0, 2);
	testGlErrorParanoid();
}



//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void
CCommonGLDriver::OnResize(const core::dimension2d<s32>& size)
{
	CNullDriver::OnResize(size);
	setViewPort(core::rect<s32>(core::position2d<s32>(0,0), getScreenSize()));
}


//! Returns type of video driver
E_DRIVER_TYPE
CCommonGLDriver::getDriverType() const
{
	return EDT_OPENGL_FAMILY;
}


//! returns color format
ECOLOR_FORMAT
CCommonGLDriver::getColorFormat() const
{
	return ColorFormat;
}


//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver*
CCommonGLDriver::getVideoDriver()
{
	return this;
}

ITexture*
CCommonGLDriver::addRenderTargetTexture(const core::dimension2d<s32>& size,
										const c8* name,
										ECOLOR_FORMAT format)
{
	return addRenderTargetTexture(size, name, true, false);
}

ITexture*
CCommonGLDriver::addRenderTargetTexture(const core::dimension2d<s32>& size,
										const c8* name,
										bool colorTexture,
										bool depthTexture)
{
	//disable mip-mapping
	bool generateMipLevels = getOption(EVDO_CREATE_TEXTURE_MIPMAPS);
	setOption(EVDO_CREATE_TEXTURE_MIPMAPS, false);

	video::ITexture* rtt = 0;
	if (name==0)
		name="rt";
#if defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object) || defined(_IRR_COMPILE_WITH_PS3_)
	// if driver supports FrameBufferObjects, use them
	if (queryFeature(EVDF_FRAMEBUFFER_OBJECT))
	{
		rtt = createDeviceDependentRTTexture(size, name, colorTexture, depthTexture);
		addTexture(rtt);
		rtt->drop();
		testGlErrorParanoid();
	}
	else
#endif
	{
		rtt = addTexture(size, name, ECF_A8R8G8B8);
		if (rtt)
		{			
			rtt->grab();
			static_cast<video::CCommonGLTexture*>(rtt)->setIsRenderTarget(true);
		}
	}

	//restore mip-mapping
	setOption(EVDO_CREATE_TEXTURE_MIPMAPS, generateMipLevels);

	return rtt;
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32
CCommonGLDriver::getMaximalPrimitiveCount() const
{
	return 65535;// TODO: Fix all loaders to auto-split and then return the correct value: MaxIndices;
}


//! checks triangle count and print warning if wrong
bool
CCommonGLDriver::setRenderTarget(video::ITexture* texture,
								 int clearMask)
{
	flush();

	// check for right driver type

	if (texture && texture->getDriverType() != getDriverType())
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back

	setTexture(0, 0);
	ResetRenderStates=true;
	if (RenderTargetTexture!=0)
	{
		RenderTargetTexture->unbindRTT();
	}

	if (texture)
	{
		// we want to set a new target. so do this.
		RenderTargetTexture = static_cast<CCommonGLTexture*>(texture);
		RenderTargetTexture->bindRTT();
		RenderTargetTexture->setIsRenderTarget(true);
		CurrentRendertargetSize = texture->getSize();
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
		glViewport(0,0,CurrentRendertargetSize.Width,CurrentRendertargetSize.Height);
#    if defined(_IRR_COMPILE_WITH_PS3_)
		glScissor(0,0,ScreenSize.Width,ScreenSize.Height);
#    endif
#endif
	}
	else
	{
		glViewport(0,0,ScreenSize.Width,ScreenSize.Height);
#if defined(_IRR_COMPILE_WITH_PS3_)
		glScissor(0,0,ScreenSize.Width,ScreenSize.Height);
#endif
		RenderTargetTexture = 0;
		CurrentRendertargetSize = core::dimension2d<s32>(0,0);
	}
	testGlErrorParanoid();

	if (clearMask != 0)
	{
		clearBuffers(clearMask);
	}

	return true;
}

// returns the current size of the screen or rendertarget
const core::dimension2d<s32>&
CCommonGLDriver::getCurrentRenderTargetSize() const
{
	if ( CurrentRendertargetSize.Width == 0 )
		return getScreenSize();
	else
		return CurrentRendertargetSize;
}

//! Returns an image created from the last rendered frame.
IImage*
CCommonGLDriver::createScreenShot()
{
	flush();

	IImage* newImage = irrnew CImage(ECF_R8G8B8, ScreenSize);

	u8* pPixels = static_cast<u8*>(newImage->lock());
	if (!pPixels)
	{
		newImage->drop();
		return 0;
	}

	// allows to read pixels in top-to-bottom order
#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
	{
		glPixelStorei(GL_PACK_INVERT_MESA, GL_TRUE);
		testGlErrorParanoid();
	}
#endif

	// We want to read the front buffer to get the latest render finished.
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	glReadBuffer(GL_FRONT);
	testGlErrorParanoid();
#endif	
	glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, GL_RGB, GL_UNSIGNED_BYTE, pPixels);
	testGlErrorParanoid();
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	glReadBuffer(GL_BACK);
	testGlErrorParanoid();
#endif


#ifdef GL_MESA_pack_invert
	if (FeatureAvailable[IRR_MESA_pack_invert])
	{
		glPixelStorei(GL_PACK_INVERT_MESA, GL_FALSE);
		testGlErrorParanoid();
	}
	else
#endif
	{
		// opengl images are horizontally flipped, so we have to fix that here.
		const s32 pitch=newImage->getPitch();
		u8* p2 = pPixels + (ScreenSize.Height - 1) * pitch;
		SScopedProcessBuffer tmpBuffer(pitch);
		//u8* tmpBuffer = irrnew u8[pitch];
		for (s32 i=0; i < ScreenSize.Height; i += 2)
		{
			memcpy(tmpBuffer.get(), pPixels, pitch);
			memcpy(pPixels, p2, pitch);
			memcpy(p2, tmpBuffer.get(), pitch);
			pPixels += pitch;
			p2 -= pitch;
		}
		//delete [] tmpBuffer;
	}

	newImage->unlock();

	if (testGLError())
	{
		newImage->drop();
		return 0;
	}

	return newImage;
}


//! Set/unset a clipping plane.
//! There are at least 6 clipping planes available for the user to set at will.
//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
//! \param plane: The plane itself.
//! \param enable: If true, enable the clipping plane else disable it.
bool
CCommonGLDriver::setClipPlane(u32 index,
							  const core::plane3df& plane,
							  bool enable)
{
	if (index >= MaxUserClipPlanes)
		return false;

	UserClipPlane[index]=plane;
	enableClipPlane(index, enable);
	return true;
}


void
CCommonGLDriver::uploadClipPlane(u32 index)
{
	// opengl needs an array of doubles for the plane equation
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	float clip_plane[4];
#else
	double clip_plane[4];
#endif
	clip_plane[0] = UserClipPlane[index].Normal.X;
	clip_plane[1] = UserClipPlane[index].Normal.Y;
	clip_plane[2] = UserClipPlane[index].Normal.Z;
	clip_plane[3] = UserClipPlane[index].D;
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glClipPlanef(GL_CLIP_PLANE0 + index, clip_plane);
#else
	glClipPlane(GL_CLIP_PLANE0 + index, clip_plane);
#endif
	testGlErrorParanoid();
}


//! Enable/disable a clipping plane.
//! There are at least 6 clipping planes available for the user to set at will.
//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
//! \param enable: If true, enable the clipping plane else disable it.
void
CCommonGLDriver::enableClipPlane(u32 index, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return;
	flush();
	if (enable)
	{
		if (!UserClipPlaneEnabled[index])
		{
			uploadClipPlane(index);
			glEnable(GL_CLIP_PLANE0 + index);
		}
	}
	else
		glDisable(GL_CLIP_PLANE0 + index);

	testGlErrorParanoid();

	UserClipPlaneEnabled[index]=enable;
}

void
CCommonGLDriver::setOrthoOrientation(E_ORTHO_ORIENTATION newOri)
{	
	CurrentOrientation = newOri;
}
	
E_ORTHO_ORIENTATION
CCommonGLDriver::getOrthoOrientation()
{
	return CurrentOrientation;
}

void
CCommonGLDriver::setOrientation3D(E_ORIENTATION newOri)
{
    CurrentOrientation3D = newOri;
}

E_ORIENTATION
CCommonGLDriver::getOrientation3D()
{
    return CurrentOrientation3D;    
}


//set scissor test
//! \param scissorRect: the scissor rect, upperleft cornor is 0,0
void
CCommonGLDriver::setScissor(const core::rect<s32>& scissorRect)
{
	flush();

	glEnable(GL_SCISSOR_TEST);
	const core::dimension2d<s32>& rts = getCurrentRenderTargetSize();
	s32 rw = rts.Width;
	s32 rh = rts.Height;
	E_ORIENTATION orit = getOrientation3D();
	s32 x = scissorRect.UpperLeftCorner.X;
	s32 y = scissorRect.UpperLeftCorner.Y;
	s32 w = scissorRect.getWidth();
	s32 h = scissorRect.getHeight();
	if(orit == EOO_90)
	{
		s32 t = x;
		x = y;
		y = t;
		
		t = w;
		w = h;
		h = t;
	}
	else if (orit == EOO_270)
	{
		s32 t = x;
		x = rw - y - h;
		y = rh - t - w;
		
		t = w;
		w = h;
		h = t;
	}
	else if (orit == EOO_180)
	{
		//y = y + h;
		x = rw - x - w;
	}
	else
	{
		y = rh - y - h;
	}
	if (!RenderTargetTexture)
	{
		const s32 right = (x + w) * ScreenSize.Width / rw;
		const s32 top = (y + h) * ScreenSize.Height / rh;
		x = x * ScreenSize.Width / rw;
		y = y * ScreenSize.Height / rh;
		w = right - x;
		h = top - y;
	}
	glScissor(x, y, w, h);
	testGlErrorParanoid();
}

//close scissor test
void
CCommonGLDriver::resetScissor()
{
	glDisable(GL_SCISSOR_TEST);
}

//! Set alpha/color mask to write in color buffer.
void
CCommonGLDriver::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	flush();
	glColorMask( red, green, blue, alpha );
	testGlErrorParanoid();
}

void
CCommonGLDriver::setDepthRange(float n, float f)
{
	flush();
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PSGL_) || defined(_IRR_COMPILE_WITH_PS3_)
	glDepthRangef(n, f);
#else
	glDepthRange(n, f);
#endif
}

//!
void
CCommonGLDriver::flush()
{
	testGlErrorParanoid();
	if (DynamicBatch->getIndexCount())
	{
		if (CompileData == 0)
		{
			// temporary inefficient solution for matrices
			testGlErrorParanoid();

			IsFlushingDynamicBatch = true;
			bool batchingEnabled = getOption(EVDO_BATCHING);
			if (batchingEnabled)
			{
				CNullDriver::setOption(EVDO_BATCHING, false);
			}

			drawMeshBuffer(DynamicBatch);
			DynamicBatch->clear();

			if (batchingEnabled)
			{
				CNullDriver::setOption(EVDO_BATCHING, true);
			}
			IsFlushingDynamicBatch=false;

			testGlErrorParanoid();
		}
		else
		{
			// update batch list
			if (CompileData->getBatchList()->getSegmentCount(CurrentBatchId) > 0)
			{
				scene::CBatchBuffer* buf
					= irrnew scene::CBatchBuffer(DynamicBatch);
				CompileData->getBatchList()->setBuffer(CurrentBatchId, buf);
				buf->drop();
				// prepare next batch
				CurrentBatchId = u32(-1);
			}
			DynamicBatch->clear();
		}
	}
}

void
CCommonGLDriver::setMaxBatchSegmentSize(u32 size)
{
	MaxDynamicBatchSegmentSize = size;
}

void
CCommonGLDriver::beginCompile(ICompileData* compileData)
{
	_IRR_DEBUG_BREAK_IF(compileData == NULL);
	_IRR_DEBUG_BREAK_IF(compileData->getType() != ECDT_BATCH_LIST);
	_IRR_DEBUG_BREAK_IF(
		static_cast<CBatchListCompileData*>(compileData)->getBatchList() == NULL
	);
	flush();

	// Force batching on
	SavedBatchingOption = getOption(EVDO_BATCHING);
	setOption(EVDO_BATCHING, true);

	// setup the batch list
	CompileData = static_cast<CBatchListCompileData*>(compileData);

	// prepare first batch
	CurrentBatchId = u32(-1);

	// ensure big enough buffers
	releaseDynamicBatch(&SavedDynamicBatchVertexCapacity,
						&SavedDynamicBatchIndexCapacity);
	allocateDynamicBatch(
		CompileData->getMaxVertexBufferSize(),
		CompileData->getMaxIndexBufferSize()
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
		, false
#endif
	);
	SavedMaxDynamicBatchSegmentSize = MaxDynamicBatchSegmentSize;
	setMaxBatchSegmentSize(DynamicBatch->getMaxVertexBufferSize()
						   / sizeof(core::vector3df));
}

void
CCommonGLDriver::endCompile()
{
	_IRR_DEBUG_BREAK_IF(CompileData == NULL);
	flush();

	// last batch should always contains nothing, because an empty batch is
	// always add in flush() to prepare the next one
	_IRR_DEBUG_BREAK_IF(CurrentBatchId != u32(-1));
	CompileData = NULL;

	// Revert changes
	releaseDynamicBatch();
	allocateDynamicBatch(SavedDynamicBatchVertexCapacity,
						 SavedDynamicBatchIndexCapacity);
	setMaxBatchSegmentSize(SavedMaxDynamicBatchSegmentSize);
	setOption(EVDO_BATCHING, SavedBatchingOption);
}

void
CCommonGLDriver::setOption(u32 option, bool value)
{
	CNullDriver::setOption(option, value);

	// Force batching on when compiling
	if ((option & EVDO_BATCHING) && !value)
	{
		// if switching batching off, we must flush
		flush();
		if (CompileData != NULL)
		{
			OptionFlags |= EVDO_BATCHING;
		}

		testGlErrorDebug();
	}
}

#ifdef _DEBUG
extern u8* LastProcessBufferAllocationRealStart;
extern u8* LastProcessBufferAllocationRealEnd;
#endif

namespace
{

void
getBatchBuffers(u32 vertexStart,
				u32 attributes,
				S3DVertexComponentArrays& components,
				CCommonGLDriver::SBinding* b)
{
	b->clearProcessBuffer();

	scene::CBatchBuffer* batchBuffer
		= b->getBatchList()->getBuffer(b->getBatch());
	u16 batchVertexStart, batchVertexEnd;
	b->getBatchList()->getSegmentVertexRange(b->getBatch(),
											 b->getSegment(),
											 batchVertexStart,
											 batchVertexEnd);
	S3DVertexComponentArrays* batchComponents
		= reinterpret_cast<S3DVertexComponentArrays*>(
			batchBuffer->getVertices()
		);
#ifdef _DEBUG
	LastProcessBufferAllocationRealStart
		= (reinterpret_cast<u8*>(batchComponents->Position)
		   + batchComponents->PositionStride * batchVertexStart);
	LastProcessBufferAllocationRealEnd
		= (LastProcessBufferAllocationRealStart
		   + (batchVertexEnd - batchVertexStart) * batchComponents->PositionStride);
#endif
	u32 offset
		= batchComponents->PositionStride * (batchVertexStart - vertexStart);
	switch (attributes)
	{
		case EVA_POSITION :
		{
			components.Position
				= core::stepPointer(batchComponents->Position, offset);
			components.PositionStride = batchComponents->PositionStride;
		}
		break;

		case EVA_POSITION | EVA_NORMAL :
		{
			components.Position
				= core::stepPointer(batchComponents->Position, offset);
			components.PositionStride = batchComponents->PositionStride;
			components.Normal
				= core::stepPointer(batchComponents->Normal, offset);
			components.NormalStride = batchComponents->NormalStride;
		}
		break;

		default:
		{
			if (attributes & EVA_POSITION)
			{
				components.Position
					= core::stepPointer(batchComponents->Position, offset);
				components.PositionStride = batchComponents->PositionStride;
			}
			if (attributes & EVA_NORMAL)
			{
				components.Normal
					= core::stepPointer(batchComponents->Normal, offset);
				components.NormalStride = batchComponents->NormalStride;
			}
			if (attributes & EVA_COLOR0)
			{
				components.Color0
					= core::stepPointer(batchComponents->Color0, offset);
				components.Color0Stride = batchComponents->Color0Stride;
			}
			u32 texMask = attributes & EVA_TEXCOORDS_MASK;
			for (u32 i = 0; texMask; ++i)
			{
				u32 unitMask = EVA_TEXCOORD0 << i;
				texMask &= ~unitMask;
				if (attributes & texMask)
				{
					components.TexCoord[i].Coord
						= core::stepPointer(batchComponents->TexCoord[i].Coord,
											offset);
					components.TexCoord[i].Stride
						= batchComponents->TexCoord[i].Stride;
				}
			}
		}
	}
}

}

E_DRIVER_ALLOCATION_RESULT
CCommonGLDriver::getProcessBuffer(u32 vertexStart,
								  u32 vertexEnd,
								  u32 attributes,
								  E_PROCESS_BUFFER_TYPE type,
								  S3DVertexComponentArrays& components,
								  IDriverBinding** binding,
								  bool allocate)
{
	E_DRIVER_ALLOCATION_RESULT result = EDAR_SUCCESS;
	switch (type)
	{
		case EPBT_DYNAMIC :
		{
			if (binding && *binding)
			{
				SBinding* b = static_cast<SBinding*>(*binding);
				if (b->ProcessBuffer != NULL || b->getBatchList() != NULL)
				{
					result = EDAR_NOT_FOUND;
					break;
				}
			}
			u32 stride = 0;
			void* buf = allocateProcessBuffer(
				vertexStart,
				vertexEnd,
				attributes,
				components,
				SBinding::getProcessBufferHeapAllocator(),
				stride
			);
			if (buf == NULL)
			{
				result = EDAR_FAILED;
			}
		}
		break;

		case EPBT_STATIC :
		{
			SBinding* b = ensureBinding(binding);
			_IRR_DEBUG_BREAK_IF(b == NULL);

			if (b->getBatchList())
			{
				getBatchBuffers(vertexStart, attributes, components, b);
#ifdef _DEBUG
				u16 batchVertexStart, batchVertexEnd;
				b->getBatchList()->getSegmentVertexRange(b->getBatch(),
														 b->getSegment(),
														 batchVertexStart,
														 batchVertexEnd);
				_IRR_DEBUG_BREAK_IF((s32)(vertexEnd - vertexStart)
									> (s32)(batchVertexEnd - batchVertexStart));
#endif
				result = EDAR_SUCCESS_AND_SKIP_RENDERING;
			}
			else
			{
				result = b->getProcessBuffer(vertexStart,
											 vertexEnd,
											 attributes,
											 components,
											 allocate);
			}
		}
		break;
	}
	return result;
}

void
CCommonGLDriver::releaseProcessBuffer(E_PROCESS_BUFFER_TYPE type,
									  void* bindingOrDynamicBuffer,
									  u32 vertexStart,
									  u32 stride)
{
	_IRR_DEBUG_BREAK_IF(bindingOrDynamicBuffer == NULL);
	switch (type)
	{
		case EPBT_DYNAMIC :
		{
			irr::releaseProcessBuffer(
				reinterpret_cast<u8*>(bindingOrDynamicBuffer)
				+ vertexStart * stride
			);
#ifdef _DEBUG
			LastProcessBufferAllocationRealStart
				= LastProcessBufferAllocationRealEnd = NULL;
#endif
		}
		break;

		case EPBT_STATIC :
		{
			SBinding* b = static_cast<SBinding*>(bindingOrDynamicBuffer);
			b->clearProcessBuffer();
#ifdef _DEBUG
			LastProcessBufferAllocationRealStart
				= LastProcessBufferAllocationRealEnd = NULL;
#endif
		}
		break;
	}
}

CCommonGLDriver::SBinding*
CCommonGLDriver::ensureBinding(IDriverBinding** binding)
{
	_IRR_DEBUG_BREAK_IF(binding == NULL);
	if (*binding == NULL)
	{
		*binding = irrnew SBinding(this);
	}
	return static_cast<SBinding*>(*binding);
}

// --------------------------------------------------------------------------------------------
// Driver dependent method, default implementation does nothing
// --------------------------------------------------------------------------------------------

void
CCommonGLDriver::setVertexShaderConstant(const f32* data,
										 s32 startRegister,
										 s32 constantAmount) 
{
	os::Printer::log("Error: CCommonGLDriver::setVertexShaderConstant() not supported.");
}

void
CCommonGLDriver::setPixelShaderConstant(const f32* data,
										s32 startRegister,
										s32 constantAmount) 
{
	os::Printer::log("Error: CCommonGLDriver::setPixelShaderConstant() not supported.");
}

bool
CCommonGLDriver::setVertexShaderConstant(const c8* name,
										 const f32* floats,
										 int count) 
{
	os::Printer::log("Error: CCommonGLDriver::setVertexShaderConstant() not supported.");
	return false;
}

bool
CCommonGLDriver::setPixelShaderConstant(const c8* name,
										const f32* floats,
										int count) 
{
	os::Printer::log("Error: CCommonGLDriver::setPixelShaderConstant() not supported.");
	return false;
}

s32
CCommonGLDriver::addShaderMaterial(const c8* vertexShaderProgram,
								   const c8* pixelShaderProgram,
								   IShaderConstantSetCallBack* callback,
								   E_MATERIAL_TYPE baseMaterial,
								   s32 userData)
{
	os::Printer::log("Error: CCommonGLDriver::addShaderMaterial() not supported.");
	return 0;
}

s32
CCommonGLDriver::addHighLevelShaderMaterial(const c8* vertexShaderProgram,
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
	os::Printer::log("Error: CCommonGLDriver::addHighLevelShaderMaterial() not supported.");
	return 0;
}

IGPUProgrammingServices*
CCommonGLDriver::getGPUProgrammingServices()
{
	os::Printer::log("Error: CCommonGLDriver::getGPUProgrammingServices() not supported.");
	return 0;
}

IDriverBinding* CCommonGLDriver::createBinding()
{
	return irrnew SBinding(this);
}

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
