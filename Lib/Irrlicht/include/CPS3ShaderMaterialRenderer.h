// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PS3_SHADER_MATERIAL_RENDERER_H_INCLUDED__
#define __C_PS3_SHADER_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#if !defined (_IRR_USE_IPHONEOS_DEVICE_)

#ifdef _IRR_WINDOWS_API_
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
#else
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_LEGACY 1
#endif
#if defined(_IRR_USE_IPHONEOS_DEVICE_)
	#include <OpenGL/gl.h>
#elif defined(_IRR_USE_PS3_DEVICE_)
	#include <PSGL/psgl.h>
	#include <PSGL/psglu.h>
	#include <Cg/cg.h>
#else
	#include <GL/gl.h>
#endif
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#include "glext.h"
#endif
#endif

#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

class CPS3Driver;
class IShaderConstantSetCallBack;
class IMaterialRenderer;

//! Class for using vertex and pixel shaders with OpenGL
class CPS3ShaderMaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	CPS3ShaderMaterialRenderer(CPS3Driver* driver,
		s32& outMaterialTypeNr, const c8* vertexShaderProgram, const c8* pixelShaderProgram,
		IShaderConstantSetCallBack* callback, IMaterialRenderer* baseMaterial, s32 userData);

	//! Destructor
	virtual ~CPS3ShaderMaterialRenderer();

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services);

	virtual bool onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;

protected:

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	CPS3ShaderMaterialRenderer(CPS3Driver* driver,
					IShaderConstantSetCallBack* callback,
					IMaterialRenderer* baseMaterial, s32 userData=0);

	void init(s32& outMaterialTypeNr, const c8* vertexShaderProgram,
		const c8* pixelShaderProgram, E_VERTEX_TYPE type);

	bool createPixelShader(const c8* pxsh);
	bool createVertexShader(const char* vtxsh);

	CPS3Driver* Driver;
	IShaderConstantSetCallBack* CallBack;
	IMaterialRenderer* BaseMaterial;

	GLuint VertexShader;
	GLuint PixelShader;
	s32 UserData;
};


} // end namespace video
} // end namespace irr

#endif
#endif
#endif