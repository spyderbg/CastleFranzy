//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "CCommonGLHeaders.h"

#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"

namespace irr
{
namespace video  
{

class COpenGLDriver;
class IShaderConstantSetCallBack;

//! Class for using GLSL shaders with OpenGL
//! Please note: This renderer implements its own IMaterialRendererServices
class COpenGLSLMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:

	//! Constructor
	COpenGLSLMaterialRenderer(
		COpenGLDriver* driver, 
		s32& outMaterialTypeNr, 
		const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderProgram, 
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		IShaderConstantSetCallBack* callback,
		IMaterialRenderer* baseMaterial,
		u32 vertexAttributeMask,
		s32 userData
	);

	//! Destructor
	virtual ~COpenGLSLMaterialRenderer();

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	virtual bool onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;

	// implementations for the render services
	virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates);
	virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);
	virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);
	virtual IVideoDriver* getVideoDriver();

	GLhandleARB getProgram() const;

protected:

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	COpenGLSLMaterialRenderer(COpenGLDriver* driver,
							  IShaderConstantSetCallBack* callback,
							  IMaterialRenderer* baseMaterial,
							  s32 userData=0);

	void init(s32& outMaterialTypeNr, 
			  const c8* vertexShaderProgram, 
			  const c8* pixelShaderProgram);

	bool createProgram();
	bool createShader(GLenum shaderType, const char* shader);
	bool linkProgram();
	
	COpenGLDriver* Driver;
	IShaderConstantSetCallBack* CallBack;
	IMaterialRenderer* BaseMaterial;

	struct SUniformInfo
	{
		core::stringc name;
		GLenum type;
	};

	GLhandleARB Program;
	core::array<SUniformInfo> UniformInfo;
	s32 UserData;
	u32 VertexAttributeMask;
};


} // end namespace video
} // end namespace irr

#endif // compile with OpenGL
#endif // if included

