// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_PS3_H_INCLUDED__
#define __C_VIDEO_PS3_H_INCLUDED__

#include "CCommonGLDriver.h"

#ifdef _IRR_COMPILE_WITH_PS3_


#include "CIrrDevicePS3.h"
#include "IMaterialRendererServices.h"
#include "CCommonGLExtensionHandler.h"
#include "SIrrCreationParameters.h"
#include "CNullDriver.h"


namespace irr
{
namespace video
{
class CPS3Texture;

class CPS3Driver : public CCommonGLDriver
{
public:

#ifdef _IRR_USE_PS3_DEVICE_
	CPS3Driver(const SIrrlichtCreationParameters& params,
			io::IFileSystem* io, CIrrDevicePS3 *device);
#endif

	//! destructor
	virtual ~CPS3Driver();

	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	virtual void createMaterialRenderers();

    virtual video::ITexture* createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data);

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

	//! Sets a vertex shader constant.
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a pixel shader constant.
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a constant for the vertex shader based on a name.
	virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);

	//! Sets a constant for the pixel shader based on a name.
	virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);

	//! Returns Gc Context used for program shaders
	CGcontext getCgContext() { return CgContext; }

	//! Adds a new material renderer to the VideoDriver, using
	//! extGLGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status)
	//! pixel and/or vertex shaders to render geometry.
	virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
		IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData);

	//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
	virtual s32 addHighLevelShaderMaterial(const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget, const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget, IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial,
		u32 vertexAttributeMask, s32 userData);

	//! Returns pointer to the IGPUProgrammingServices interface.
	virtual IGPUProgrammingServices* getGPUProgrammingServices();
	static bool testCGErrorDebug();
    static bool testCGError();
private:
	//! returns a device dependent texture from a software surface (IImage)
	virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

		//! returns a device dependent texture from a software surface (IImage)
	virtual video::ITexture* createDeviceDependentRTTexture(const core::dimension2d<s32>& size, 
															const char* name, 
															bool colorTexture, 
															bool depthTexture);

	CGcontext CgContext;

};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_PS3_
#endif


