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
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLSLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "irros.h"
#include "COpenGLDriver.h"

namespace irr
{
namespace video
{

//! Constructor
COpenGLSLMaterialRenderer::COpenGLSLMaterialRenderer(
	video::COpenGLDriver* driver,
	s32& outMaterialTypeNr, const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	IShaderConstantSetCallBack* callback,
	video::IMaterialRenderer* baseMaterial,
	u32 vertexAttributeMask,
	s32 userData
)
	: Driver(driver)
	, CallBack(callback)
	, BaseMaterial(baseMaterial)
	, Program(0)
	, UserData(userData)
	, VertexAttributeMask(vertexAttributeMask)
{
	#ifdef _DEBUG
	setDebugName("COpenGLSLMaterialRenderer");
	#endif

	//entry points must always be main, and the compile target isn't selectable
	//it is fine to ignore what has been asked for, as the compiler should spot anything wrong
	//just check that GLSL is available

	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();

	if (!Driver->queryFeature(EVDF_ARB_GLSL))
		return;

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram);
}

//! constructor only for use by derived classes who want to
//! create a fall back material for example.
COpenGLSLMaterialRenderer::COpenGLSLMaterialRenderer(
	COpenGLDriver* driver,
	IShaderConstantSetCallBack* callback,
	IMaterialRenderer* baseMaterial,
	s32 userData
)
	: Driver(driver)
	, CallBack(callback)
	, BaseMaterial(baseMaterial)
	, Program(0)
	, UserData(userData)
{
	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();
}

//! Destructor
COpenGLSLMaterialRenderer::~COpenGLSLMaterialRenderer()
{
	if (CallBack)
		CallBack->drop();

	if(Program)
	{
		Driver->deleteObject(Program);
		Program = 0;
	}

	UniformInfo.clear();

	if (BaseMaterial)
		BaseMaterial->drop();
}

void COpenGLSLMaterialRenderer::init(s32& outMaterialTypeNr,
									 const c8* vertexShaderProgram,
									 const c8* pixelShaderProgram)
{
	outMaterialTypeNr = -1;

	if (!createProgram())
		return;

#if defined(GL_ARB_vertex_shader) && defined (GL_ARB_fragment_shader)
	if (vertexShaderProgram)
		if (!createShader(GL_VERTEX_SHADER_ARB, vertexShaderProgram))
			return;


	if (pixelShaderProgram)
		if (!createShader(GL_FRAGMENT_SHADER_ARB, pixelShaderProgram))
			return;
#endif

	if (!linkProgram())
		return;

	// register myself as new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}

bool COpenGLSLMaterialRenderer::onRender(IMaterialRendererServices* service,
										 E_VERTEX_TYPE vtxtype)
{
	// call callback to set shader constants
	if (CallBack && (Program))
		CallBack->onSetConstants(this, UserData);

	return true;
}

void COpenGLSLMaterialRenderer::onSetMaterial(
	const video::SMaterial& material,
	const video::SMaterial& lastMaterial,
	bool resetAllRenderstates,
	video::IMaterialRendererServices* services
)
{
	if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
	{
		if(Program)
			Driver->useProgramObject(Program);

		if (BaseMaterial)
			BaseMaterial->onSetMaterial(material, material, true, this);
	}

	//let callback know used material
	if (CallBack)
		CallBack->onSetMaterial(material);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Driver->setTexture(i, material.getTexture(i));
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

void COpenGLSLMaterialRenderer::onUnsetMaterial()
{
	Driver->useProgramObject(0);

	if (BaseMaterial)
		BaseMaterial->onUnsetMaterial();
}

//! Returns if the material is transparent.
bool COpenGLSLMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}

bool COpenGLSLMaterialRenderer::createProgram()
{
	Program = Driver->createProgramObject();
	return true;
}

bool COpenGLSLMaterialRenderer::createShader(GLenum shaderType, const char* shader)
{
	GLhandleARB shaderHandle = Driver->createShaderObject(shaderType);

	Driver->shaderSource(shaderHandle, 1, &shader, NULL);
	Driver->compileShader(shaderHandle);

	int status = 0;

#ifdef GL_ARB_shader_objects
	Driver->getObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);
#endif

	if (!status)
	{
		os::Printer::log("GLSL shader failed to compile");
		// check error message and log it
		int maxLength=0;
		GLsizei length;
#ifdef GL_ARB_shader_objects
		Driver->getObjectParameteriv(shaderHandle,
				GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
		GLcharARB *pInfoLog = irrnew GLcharARB[maxLength];
		Driver->getInfoLog(shaderHandle, maxLength, &length, pInfoLog);
		os::Printer::log(reinterpret_cast<const c8*>(pInfoLog));
		delete [] pInfoLog;

		return false;
	}

	Driver->attachObject(Program, shaderHandle);

	return true;
}

bool COpenGLSLMaterialRenderer::linkProgram()
{
	Driver->linkProgram(Program);

	int status = 0;

#ifdef GL_ARB_shader_objects
	Driver->getObjectParameteriv(Program, GL_OBJECT_LINK_STATUS_ARB, &status);
#endif

	if (!status)
	{
		os::Printer::log("GLSL shader program failed to link");
		// check error message and log it
		int maxLength=0;
		GLsizei length;
#ifdef GL_ARB_shader_objects
		Driver->getObjectParameteriv(Program,
				GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
		GLcharARB *pInfoLog = irrnew GLcharARB[maxLength];
		Driver->getInfoLog(Program, maxLength, &length, pInfoLog);
		os::Printer::log(reinterpret_cast<const c8*>(pInfoLog));
		delete [] pInfoLog;

		return false;
	}

	// get uniforms information

	int num = 0;
#ifdef GL_ARB_shader_objects
	Driver->getObjectParameteriv(Program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &num);
#endif

	if (num == 0)
	{
		// no uniforms
		return true;
	}

	int maxlen = 0;
#ifdef GL_ARB_shader_objects
	Driver->getObjectParameteriv(Program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxlen);
#endif

	if (maxlen == 0)
	{
		os::Printer::log("GLSL: failed to retrieve uniform information");
		return false;
	}

	c8 *buf = irrnew c8[maxlen];

	UniformInfo.clear();
	UniformInfo.reallocate(num);

	for (int i=0; i < num; ++i)
	{
		SUniformInfo ui;
		memset(buf, 0, maxlen);

		GLint size;
		Driver->getActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLcharARB*>(buf));
		ui.name = buf;

		UniformInfo.push_back(ui);
	}

	delete [] buf;

	return true;
}

void COpenGLSLMaterialRenderer::setBasicRenderStates(const SMaterial& material,
						const SMaterial& lastMaterial,
						bool resetAllRenderstates)
{
	// forward
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

bool COpenGLSLMaterialRenderer::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	return setPixelShaderConstant(name, floats, count);
}

void COpenGLSLMaterialRenderer::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.");
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	int i, num = static_cast<int>(UniformInfo.size());

	for (i=0; i < num; ++i)
	{
		if (UniformInfo[i].name == name)
			break;
	}

	if (i == num)
		return false;

#ifdef GL_ARB_shader_objects
	GLint Location=Driver->getUniformLocation(Program,name);

	switch (UniformInfo[i].type)
	{
		case GL_FLOAT:
			Driver->uniform1fv(Location, count, floats);
			break;
		case GL_FLOAT_VEC2_ARB:
			Driver->uniform2fv(Location, count/2, floats);
			break;
		case GL_FLOAT_VEC3_ARB:
			Driver->uniform3fv(Location, count/3, floats);
			break;
		case GL_FLOAT_VEC4_ARB:
			Driver->uniform4fv(Location, count/4, floats);
			break;
		case GL_FLOAT_MAT2_ARB:
			Driver->uniformMatrix2fv(Location, count/4, false, floats);
			break;
		case GL_FLOAT_MAT3_ARB:
			Driver->uniformMatrix3fv(Location, count/9, false, floats);
			break;
		case GL_FLOAT_MAT4_ARB:
			Driver->uniformMatrix4fv(Location, count/16, false, floats);
			break;
		default:
			Driver->uniform1iv(Location, count, reinterpret_cast<const GLint*>(floats));
			break;
	}

	_IRR_DEBUG_BREAK_IF(glGetError() == GL_INVALID_VALUE);

#endif

	return true;
}

void COpenGLSLMaterialRenderer::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, use high level shader call.");
}

IVideoDriver* COpenGLSLMaterialRenderer::getVideoDriver()
{
	return Driver;
}

GLhandleARB  COpenGLSLMaterialRenderer::getProgram() const
{
	return Program;
}

u32 COpenGLSLMaterialRenderer::getVertexAttributeMask() const
{
	// requires analysis of shader varying vertex input
//	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	//return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
	return VertexAttributeMask;
}

} // end namespace video
} // end namespace irr

#endif
