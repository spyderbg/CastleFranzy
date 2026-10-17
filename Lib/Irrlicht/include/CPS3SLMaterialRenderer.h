// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PS3_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__
#define __C_PS3_SHADER_LANGUAGE_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#if !defined (_IRR_USE_IPHONEOS_DEVICE_)

#ifdef _IRR_WINDOWS_API_
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
	#include "glext.h"
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
#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"

namespace irr
{
namespace video  
{

class CPS3Driver;
class IShaderConstantSetCallBack;

//! Class for using GLSL shaders with OpenGL
//! Please note: This renderer implements its own IMaterialRendererServices
class CPS3SLMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:

	//! Constructor
	CPS3SLMaterialRenderer(
		CPS3Driver* driver, 
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
		s32 userData);

	//! Destructor
	virtual ~CPS3SLMaterialRenderer();

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services);

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

	CGprogram getCgVertexProgram();
	CGprogram getCgFragmentProgram();

protected:

	virtual bool setShaderConstant(CGprogram program, const c8* name, const f32* floats, int count);

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	CPS3SLMaterialRenderer(CPS3Driver* driver,
					IShaderConstantSetCallBack* callback,
					IMaterialRenderer* baseMaterial,
					u32 vertexAttributeMask,
					s32 userData=0);

	void init(s32& outMaterialTypeNr, 
		const c8* vertexShaderProgram, 
		const c8* pixelShaderProgram);

	bool getCgContext();
	bool createShader(CGprofile shaderType, const char* shader);
	bool bindProgram();
	bool unbindProgram();
	bool linkProgram();
	bool linkProgram(CGprogram CgProgram, CGparameter CgParam);
	
	CPS3Driver* Driver;
	IShaderConstantSetCallBack* CallBack;
	IMaterialRenderer* BaseMaterial;

	struct SUniformInfo
	{
		core::stringc name;
		uint32_t type;
		CGparameter param;
		CGprogram program;
	};

	CGcontext CgContext;
	CGprogram VertexCgProgram;
	CGprogram FragmentCgProgram;
	const char* VertexSourceProgram;
	const char* FragmentSourceProgram;
	//CGparameter WorldViewProjectionMtxCgParam;

	core::array<SUniformInfo> UniformInfo;
	s32 UserData;
	u32 VertexAttributeMask;
};


} // end namespace video
} // end namespace irr

#endif // compile with OpenGL
#endif
#endif // if included

