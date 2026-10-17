#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// This file was originally written by William Finlayson.  I (Nikolaus
// Gebhardt) did some minor modifications and changes to it and integrated it
// into Irrlicht. Thanks a lot to William for his work on this and that he gave
// me his permission to add it into Irrlicht using the zlib license.

// After Irrlicht 0.12, Michael Zoech did some improvements to this renderer, I
// merged this into Irrlicht 0.14, thanks to him for his work.

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#if !defined (_IRR_USE_IPHONEOS_DEVICE_) && defined(_IRR_COMPILE_WITH_PS3_)

#include "CPS3SLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "irros.h"
#include "CPS3Driver.h"

namespace irr
{
namespace video
{


//! Constructor
CPS3SLMaterialRenderer::CPS3SLMaterialRenderer(video::CPS3Driver* driver,
		s32& outMaterialTypeNr, const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderProgram,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		IShaderConstantSetCallBack* callback,
		video::IMaterialRenderer* baseMaterial,
		u32 vertexAttributeMask,
		s32 userData)
	: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		CgContext(0),
		VertexCgProgram(0),
		FragmentCgProgram(0),
		VertexSourceProgram(0),
		FragmentSourceProgram(0),
		UserData(userData),
		VertexAttributeMask(vertexAttributeMask)
{
	//entry points must always be main, and the compile target isn't selectable
	//it is fine to ignore what has been asked for, as the compiler should spot anything wrong
	//just check that GLSL is available

	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram);
}


//! constructor only for use by derived classes who want to
//! create a fall back material for example.
CPS3SLMaterialRenderer::CPS3SLMaterialRenderer(CPS3Driver* driver,
					IShaderConstantSetCallBack* callback,
					IMaterialRenderer* baseMaterial, u32 vertexAttributeMask, s32 userData)
: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		CgContext(0),
		VertexCgProgram(0),
		FragmentCgProgram(0),
		VertexSourceProgram(0),
		FragmentSourceProgram(0),
		UserData(userData),
		VertexAttributeMask(vertexAttributeMask)
{
	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();
}


//! Destructor
CPS3SLMaterialRenderer::~CPS3SLMaterialRenderer()
{
	if (CallBack)
		CallBack->drop();

	if(VertexCgProgram)
		cgDestroyProgram(VertexCgProgram);
	
	if (FragmentCgProgram)
		cgDestroyProgram(FragmentCgProgram);

	if(VertexSourceProgram)
		delete VertexSourceProgram;

	if (FragmentSourceProgram)
		delete FragmentSourceProgram;

	UniformInfo.clear();

	if (BaseMaterial)
		BaseMaterial->drop();
}

void CPS3SLMaterialRenderer::init(s32& outMaterialTypeNr,
	const c8* vertexShaderProgram,
	const c8* pixelShaderProgram)
{
	outMaterialTypeNr = -1;

	if (!getCgContext())
		return;

	if (!createShader(CG_PROFILE_SCE_VP_RSX, vertexShaderProgram))
		return;

	if (!createShader(CG_PROFILE_SCE_FP_RSX, pixelShaderProgram))
		return;

	if (!linkProgram())
	return;

	// register myself as new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}

bool CPS3SLMaterialRenderer::onRender(IMaterialRendererServices* service,
					E_VERTEX_TYPE vtxtype)
{

	// call callback to set shader constants
	if (CallBack && (VertexCgProgram || FragmentCgProgram))
		CallBack->onSetConstants(this, UserData);

	//cgGLSetStateMatrixParameter (WorldViewProjectionMtxCgParam, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);	

	return true;
}


void CPS3SLMaterialRenderer::onSetMaterial(const video::SMaterial& material,
				const video::SMaterial& lastMaterial,
				bool resetAllRenderstates,
				video::IMaterialRendererServices* services)
{
	if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
	{

		bindProgram();

		if (BaseMaterial)
			BaseMaterial->onSetMaterial(material, material, true, this);
	}

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Driver->setTexture(i, material.getTexture(i));
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//let callback know used material
	if (CallBack)
		CallBack->onSetMaterial(material);

}


void CPS3SLMaterialRenderer::onUnsetMaterial()
{
	unbindProgram();

	//let callback know used material
	if (CallBack)
		CallBack->onUnsetMaterial();

	if (BaseMaterial)
		BaseMaterial->onUnsetMaterial();

}

//! Returns if the material is transparent.
bool CPS3SLMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}

bool CPS3SLMaterialRenderer::getCgContext()
{
	CgContext = Driver->getCgContext();
	return true;
}

bool CPS3SLMaterialRenderer::bindProgram()
{
	if (VertexCgProgram)
	{
		cgGLBindProgram(VertexCgProgram);
		cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
	}
	if (FragmentCgProgram)
	{
		cgGLBindProgram(FragmentCgProgram);
		cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));
	}

	return true;
}

bool CPS3SLMaterialRenderer::unbindProgram()
{
	if (VertexCgProgram)
	{
		cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
		cgGLUnbindProgram(CG_PROFILE_SCE_VP_RSX);
	}
	if (FragmentCgProgram)
	{
		cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));
		cgGLUnbindProgram(CG_PROFILE_SCE_FP_RSX);
	}	
	return true;
}


bool CPS3SLMaterialRenderer::createShader(CGprofile shaderType, const char* shader)
{
	if(shaderType == CG_PROFILE_SCE_VP_RSX)
	{
		VertexSourceProgram = shader;
		VertexCgProgram = cgCreateProgram(Driver->getCgContext(),
										  CG_BINARY,
										  shader,
										  cgGLGetLatestProfile(CG_GL_VERTEX),
										  NULL,
										  NULL);
		if (!VertexCgProgram)
		{
			os::Printer::log("Cg vertex shader failed to load");
			return false;
		}
		cgGLLoadProgram(VertexCgProgram);
	}
	else
	{
		FragmentSourceProgram = shader;
		FragmentCgProgram = cgCreateProgram(Driver->getCgContext(),
										  CG_BINARY,
										  shader,
										  cgGLGetLatestProfile(CG_GL_FRAGMENT),
										  NULL,
										  NULL);
		if (!FragmentCgProgram)
		{
			os::Printer::log("Cg fragment shader failed to load");
			return false;
		}
		cgGLLoadProgram(FragmentCgProgram);
	}

	return true;

}


/* The function "GetTypeName" converts type_id to type_name. */
const char* GetTypeName(uint32_t i){
    switch (i) {
    case CG_HALF: return "half";
    case CG_HALF1: return "half1";
    case CG_HALF2: return "half2";
    case CG_HALF3: return "half3";
    case CG_HALF4: return "half4";
    case CG_HALF1x1: return "half1x1";
    case CG_HALF1x2: return "half1x2";
    case CG_HALF1x3: return "half1x3";
    case CG_HALF1x4: return "half1x4";
    case CG_HALF2x1: return "half2x1";
    case CG_HALF2x2: return "half2x2";
    case CG_HALF2x3: return "half2x3";
    case CG_HALF2x4: return "half2x4";
    case CG_HALF3x1: return "half3x1";
    case CG_HALF3x2: return "half3x2";
    case CG_HALF3x3: return "half3x3";
    case CG_HALF3x4: return "half3x4";
    case CG_HALF4x1: return "half4x1";
    case CG_HALF4x2: return "half4x2";
    case CG_HALF4x3: return "half4x3";
    case CG_HALF4x4: return "half4x4";
    case CG_FLOAT: return "float";
    case CG_FLOAT1: return "float1";
    case CG_FLOAT2: return "float2";
    case CG_FLOAT3: return "float3";
    case CG_FLOAT4: return "float4";
    case CG_FLOAT1x1: return "float1x1";
    case CG_FLOAT2x1: return "float2x1";
    case CG_FLOAT3x1: return "float3x1";
    case CG_FLOAT4x1: return "float4x1";
    case CG_FLOAT1x2: return "float1x2";
    case CG_FLOAT2x2: return "float2x2";
    case CG_FLOAT3x2: return "float3x2";
    case CG_FLOAT4x2: return "float4x2";
    case CG_FLOAT1x3: return "float1x3";
    case CG_FLOAT2x3: return "float2x3";
    case CG_FLOAT3x3: return "float3x3";
    case CG_FLOAT4x3: return "float4x3";
    case CG_FLOAT1x4: return "float1x4";
    case CG_FLOAT2x4: return "float2x4";
    case CG_FLOAT3x4: return "float3x4";
    case CG_FLOAT4x4: return "float4x4";
    case CG_SAMPLER1D: return "sampler1D";
    case CG_SAMPLER2D: return "sampler2D";
    case CG_SAMPLER3D: return "sampler3D";
    case CG_SAMPLERCUBE: return "samplerCUBE";
    case CG_SAMPLERRECT: return "samplerRECT";
    case CG_BOOL: return "bool";
    case CG_BOOL1: return "bool";
    case CG_BOOL2: return "bool";
    case CG_BOOL3: return "bool";
    case CG_BOOL4: return "bool";
    case CG_BOOL1x1: return "bool1x1";
    case CG_BOOL1x2: return "bool1x2";
    case CG_BOOL1x3: return "bool1x3";
    case CG_BOOL1x4: return "bool1x4";
    case CG_BOOL2x1: return "bool2x1";
    case CG_BOOL2x2: return "bool2x2";
    case CG_BOOL2x3: return "bool2x3";
    case CG_BOOL2x4: return "bool2x4";
    case CG_BOOL3x1: return "bool3x1";
    case CG_BOOL3x2: return "bool3x2";
    case CG_BOOL3x3: return "bool3x3";
    case CG_BOOL3x4: return "bool3x4";
    case CG_BOOL4x1: return "bool4x1";
    case CG_BOOL4x2: return "bool4x2";
    case CG_BOOL4x3: return "bool4x3";
    case CG_BOOL4x4: return "bool4x4";
    case CG_INT: return "bool";
    case CG_INT1: return "bool";
    case CG_INT2: return "bool";
    case CG_INT3: return "bool";
    case CG_INT4: return "bool";
    case CG_INT1x1: return "bool1x1";
    case CG_INT1x2: return "bool1x2";
    case CG_INT1x3: return "bool1x3";
    case CG_INT1x4: return "bool1x4";
    case CG_INT2x1: return "bool2x1";
    case CG_INT2x2: return "bool2x2";
    case CG_INT2x3: return "bool2x3";
    case CG_INT2x4: return "bool2x4";
    case CG_INT3x1: return "bool3x1";
    case CG_INT3x2: return "bool3x2";
    case CG_INT3x3: return "bool3x3";
    case CG_INT3x4: return "bool3x4";
    case CG_INT4x1: return "bool4x1";
    case CG_INT4x2: return "bool4x2";
    case CG_INT4x3: return "bool4x3";
    case CG_INT4x4: return "bool4x4";
    default: return "UNDEFINED";
    }
}

bool CPS3SLMaterialRenderer::linkProgram(CGprogram CgProgram, CGparameter CgParam)
{
	while(CgParam != NULL)
	{
			switch(cgGetParameterType(CgParam))
			{
			case CG_STRUCT:
				linkProgram(CgProgram, cgGetFirstStructParameter(CgParam));
				break;
			case CG_ARRAY:
				{
					int ArraySize = cgGetArraySize(CgParam, 0);
					int i;
					for (i=0; i < ArraySize; ++i)
					{
						linkProgram(CgProgram, cgGetArrayParameter(CgParam, i));
					}
				}
				break;
			default:
				{
					//printf("Parameter: %#x\n", CgParam);
					uint32_t type = cgGetParameterType(CgParam);
					//printf("\ttype: %d, %s\n", type, GetTypeName(type));
					const char* name = cgGetParameterName(CgParam);
					//if(name != NULL)
					//	printf("\tname: %s\n",name);
					//else
					//	printf("\tname: UNDEFINED\n");
					SUniformInfo ui;
					ui.type = type;
					ui.name = name;
					ui.param = CgParam;
					ui.program = CgProgram;
					UniformInfo.push_back(ui);
				}
				break;
			}
		CgParam = cgGetNextParameter(CgParam);
	}
}

bool CPS3SLMaterialRenderer::linkProgram()
{
	bindProgram();

	UniformInfo.clear();

	CGparameter param = cgGetFirstParameter( VertexCgProgram, CG_PROGRAM );
	linkProgram(VertexCgProgram, param);
	param = cgGetFirstParameter( FragmentCgProgram, CG_PROGRAM );
	linkProgram(FragmentCgProgram, param);

	//WorldViewProjectionMtxCgParam = cgGetNamedParameter(VertexCgProgram, "worldViewProjectionMtx");

	unbindProgram();

	return true;
}


void CPS3SLMaterialRenderer::setBasicRenderStates(const SMaterial& material,
						const SMaterial& lastMaterial,
						bool resetAllRenderstates)
{
	// forward
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}


bool CPS3SLMaterialRenderer::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	return setShaderConstant(VertexCgProgram, name, floats, count);
}

bool CPS3SLMaterialRenderer::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	return setShaderConstant(FragmentCgProgram, name, floats, count);
}


void CPS3SLMaterialRenderer::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.");
}

bool CPS3SLMaterialRenderer::setShaderConstant(CGprogram program, const c8* name, const f32* floats, int count)
{
	int i, num = static_cast<int>(UniformInfo.size());

	for (i = 0; i < num; ++i)
	{
		if (UniformInfo[i].name == name && UniformInfo[i].program == program)
			break;
	}

	if (i == num)
	{		
		CGparameter CgParam = cgGetNamedParameter(program, name);
		if (CgParam)
		{
			uint32_t type = cgGetParameterType(CgParam);
			SUniformInfo ui;
			ui.type = type;
			ui.name = name;
			ui.param = CgParam;
			ui.program = program;
			UniformInfo.push_back(ui);
		} 
		else
		{
			return false;
		}
	}

	CGparameter parameter;

	parameter = UniformInfo[i].param;

	switch (UniformInfo[i].type)
	{
		case CG_FLOAT:
		case CG_FLOAT1:
		case CG_HALF:
		case CG_HALF1:
			cgGLSetParameter1fv(parameter, floats);
			break;
		case CG_FLOAT2:
		case CG_HALF2:
			cgGLSetParameter2fv(parameter, floats);
			break;
		case CG_FLOAT3:
		case CG_HALF3:
			cgGLSetParameter3fv(parameter, floats);
			break;
		case CG_FLOAT4:
		case CG_HALF4:
			cgGLSetParameter4fv(parameter, floats);
			break;
		case CG_FLOAT4x4:
		case CG_HALF4x4:
			cgSetMatrixParameterfc(parameter, floats);
			break;
		case CG_SAMPLER2D:
			{
				if (floats)
				{
					GLint texture = *(reinterpret_cast<const GLint*>(floats));
					cgGLSetTextureParameter(parameter, texture);
					cgGLEnableTextureParameter(parameter) ;
				} else
				{
					cgGLDisableTextureParameter(parameter) ;
				}
			}
			break;
		default:
			cgGLSetParameter1fv(parameter, floats);
			break;
	}

	return true;
}

void CPS3SLMaterialRenderer::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, use high level shader call.");
}

IVideoDriver* CPS3SLMaterialRenderer::getVideoDriver()
{
	return Driver;
}

CGprogram CPS3SLMaterialRenderer::getCgVertexProgram()
{
	return VertexCgProgram;
}

CGprogram CPS3SLMaterialRenderer::getCgFragmentProgram()
{
	return FragmentCgProgram;
}

u32 CPS3SLMaterialRenderer::getVertexAttributeMask() const
{
	return VertexAttributeMask;
}


} // end namespace video
} // end namespace irr


#endif
#endif