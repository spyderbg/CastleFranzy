#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h


#include "CPS3Driver.h"

#ifdef _IRR_COMPILE_WITH_PS3_
#ifdef _DEBUG
#define IRR_ENABLE_TEST_CGERROR_DEBUG          1
#define IRR_ENABLE_TEST_CGERROR_BREAK_ON_ERROR 0
#endif
// needed here also because of the create methods' parameters
#include "CNullDriver.h"

#include "CPS3Texture.h"
#include "CCommonGLMaterialRenderer.h"
#include "CPS3ShaderMaterialRenderer.h"
#include "CPS3SLMaterialRenderer.h"
#include "CPS3NormalMapRenderer.h"
#include "CPS3ParallaxMapRenderer.h"
#include "CImage.h"
#include "irros.h"

namespace irr
{
namespace video
{

// -----------------------------------------------------------------------
// PS3 CONSTRUCTOR
// -----------------------------------------------------------------------
#ifdef _IRR_USE_PS3_DEVICE_
//! Windows constructor and init code
CPS3Driver::CPS3Driver(const SIrrlichtCreationParameters& params,
					   io::IFileSystem* io, 
					   CIrrDevicePS3 *device)
	: CCommonGLDriver( params, io, device)
{
	#ifdef _DEBUG
	setDebugName("CPS3Driver");
	#endif

	genericDriverInit(params.WindowSize, params.Stencilbuffer);
}
#endif
bool CPS3Driver::testCGError()
{
#ifdef _DEBUG
	CGerror ret = cgGetError();
    if(ret)
    {
        const char *str = cgGetErrorString(ret);
        os::Printer::log(str,ELL_ERROR);
	    return true;
    }
#endif
	return false;
}
bool CPS3Driver::testCGErrorDebug()
{
#if IRR_ENABLE_TEST_CGERROR_DEBUG
    bool result = testCGError();
#if IRR_ENABLE_TEST_CGERROR_BREAK_ON_ERROR
    _IRR_DEBUG_BREAK_IF(result) 
#else
    if(result)
        os::Printer::log( "PS3 CG Error Test failed ! (IRR_TEST_CG_ERROR_DEBUG)", ELL_ERROR);
#endif
    return result;
#else
    return false;
#endif
}
//! destructor
CPS3Driver::~CPS3Driver()
{
	cgDestroyContext(CgContext);
}

// -----------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------

E_DRIVER_TYPE
CPS3Driver::getDriverType() const
{
	return EDT_PS3;
}
video::ITexture* CPS3Driver::createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data)
{
    return irrnew CPS3Texture(core::dimension2d<s32>(w,h), GL_ARGB_SCE, name, this);
}

void
CPS3Driver::createMaterialRenderers()
{
	/*cgRTCgcInit();*/

	// create CG Context
	CgContext = cgCreateContext();
	if(CgContext == NULL) {
		os::Printer::log("FATAL ERROR: Unable to create CG context.", ELL_ERROR);
		exit(-1);
	}

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

#ifdef _IRR_COMPILE_WITH_PS3_
	CCommonGLMaterialRenderer_UNSUPPORTED* renderer = irrnew CCommonGLMaterialRenderer_UNSUPPORTED(this);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	addMaterialRenderer(renderer);
	renderer->drop();
#else
	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = irrnew CCommonGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = irrnew CCommonGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = irrnew CCommonGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	// add parallax map renderers
	renderer = irrnew CCommonGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();
	renderer = irrnew CCommonGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();
	renderer = irrnew CCommonGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();
#endif
	// add basic 1 texture blending
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_ONETEXTURE_BLEND(this));
	// add transparency texture X vertex alpha
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(this));

	// add 2D material renderer
	addAndDropMaterialRenderer(irrnew CCommonGLMaterialRenderer_2D_ALPHA(this));
}

//! returns a device dependent texture from a software surface (IImage)
video::ITexture*
CPS3Driver::createDeviceDependentTexture(IImage* surface, const char* name)
{
	return irrnew CPS3Texture(surface, name, this);
}

//! returns a device dependent texture from a RTT surface (IImage)
ITexture*
CPS3Driver::createDeviceDependentRTTexture(const core::dimension2d<s32>& size, 
										   const char* name, 
										   bool colorTexture, 
										   bool depthTexture)
{
	return irrnew CPS3Texture(size, name, this, 0, colorTexture, depthTexture);
}

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void
CPS3Driver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	if (!StencilBuffer || !count)
		return;

	// unset last 3d material
	if (CurrentRenderMode == ERM_3D &&
		static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
	{
		MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
		ResetRenderStates = true;
	}

	// store current OpenGL state
	GLboolean bLighting; 
	glGetBooleanv(GL_LIGHTING, &bLighting);
	GLboolean bFog; 
	glGetBooleanv(GL_FOG, &bFog);
	GLboolean bStencil; 
	glGetBooleanv(GL_STENCIL_TEST, &bStencil);
	GLboolean bCull;     
	glGetBooleanv(GL_CULL_FACE, &bCull);

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

			stencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR);
			stencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR);
			stencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~0);
			glStencilMask(~0);

			glDrawArrays(GL_TRIANGLES,0,count);
		}
		else
		{
			// ZFAIL Method

			stencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
			stencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
			stencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, ~0);

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

	glDisableClientState(GL_VERTEX_ARRAY); //not stored on stack

	if(bLighting) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);

	if(bFog) glEnable(GL_FOG);
	else glDisable(GL_FOG);

	if(bStencil) glEnable(GL_STENCIL_TEST);
	else glDisable(GL_STENCIL_TEST);

	if(bCull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif
}

void
CPS3Driver::drawStencilShadow(bool clearStencilBuffer,
							  video::SColor leftUpEdge,
							  video::SColor rightUpEdge,
							  video::SColor leftDownEdge,
							  video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	disableTextures();

	// store attributes
	GLboolean bLighting; 
	glGetBooleanv(GL_LIGHTING, &bLighting);
	GLboolean bFog; 
	glGetBooleanv(GL_FOG, &bFog);
	GLboolean bStencil; 
	glGetBooleanv(GL_STENCIL_TEST, &bStencil);
	GLboolean bBlend;     
	glGetBooleanv(GL_BLEND, &bBlend);

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

	// draw a shadow rectangle covering the entire screen using stencil buffer
	glMatrixMode(GL_MODELVIEW);
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

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif
}

//! Sets a vertex shader constant.
void
CPS3Driver::setVertexShaderConstant(const f32* data,
									s32 startRegister,
									s32 constantAmount)
{
#ifdef GL_ARB_vertex_program
	for (s32 i=0; i<constantAmount; ++i)
		programLocalParameter4fv(GL_VERTEX_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a pixel shader constant.
void
CPS3Driver::setPixelShaderConstant(const f32* data,
								   s32 startRegister,
								   s32 constantAmount)
{
#ifdef GL_ARB_fragment_program
	for (s32 i=0; i<constantAmount; ++i)
		programLocalParameter4fv(GL_FRAGMENT_PROGRAM_ARB, startRegister+i, &data[i*4]);
#endif
}

//! Sets a constant for the vertex shader based on a name.
bool
CPS3Driver::setVertexShaderConstant(const c8* name,
									const f32* floats,
									int count)
{
	//pass this along, as in GLSL the same routine is used for both vertex and fragment shaders
	return setPixelShaderConstant(name, floats, count);
}

//! Sets a constant for the pixel shader based on a name.
bool
CPS3Driver::setPixelShaderConstant(const c8* name,
								   const f32* floats,
								   int count)
{
	os::Printer::log("Error: Please call services->setPixelShaderConstant(), not VideoDriver->setPixelShaderConstant().");
	return false;
}

//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32
CPS3Driver::addShaderMaterial(const c8* vertexShaderProgram,
							  const c8* pixelShaderProgram,
							  IShaderConstantSetCallBack* callback,
							  E_MATERIAL_TYPE baseMaterial,
							  s32 userData)
{
	s32 nr = -1;
	CPS3ShaderMaterialRenderer* r = irrnew CPS3ShaderMaterialRenderer(
		this, nr, vertexShaderProgram, pixelShaderProgram,
		callback, getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}


//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32
CPS3Driver::addHighLevelShaderMaterial(
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

	CPS3SLMaterialRenderer* r = irrnew CPS3SLMaterialRenderer(
		this, nr, vertexShaderProgram, vertexShaderEntryPointName,
		vsCompileTarget, pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
		callback,getMaterialRenderer(baseMaterial), vertexAttributeMask, userData);

	r->drop();
	return nr;
}

//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices*
CPS3Driver::getGPUProgrammingServices()
{
	return this;
}

} // end namespace
} // end namespace

namespace irr
{
namespace video
{

// -----------------------------------
// PS3 VERSION
// -----------------------------------
IVideoDriver*
createPS3Driver(const SIrrlichtCreationParameters& params,
				io::IFileSystem* io, 
				CIrrDevicePS3 *device)
{
	CPS3Driver *driver = irrnew CPS3Driver(params, io, device);
	if( !driver->initDriver(params.WindowSize, params.Vsync, params.Stencilbuffer) )
	{
		driver->drop();
		driver = 0;
	}
	return driver;
}

} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_PS3_
