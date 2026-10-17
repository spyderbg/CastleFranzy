#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#include "CPS3ShaderMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "irros.h"
#include "CPS3Driver.h"

namespace irr
{
namespace video
{


//! Constructor
CPS3ShaderMaterialRenderer::CPS3ShaderMaterialRenderer(video::CPS3Driver* driver,
	s32& outMaterialTypeNr, const c8* vertexShaderProgram, const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback, IMaterialRenderer* baseMaterial, s32 userData)
	: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		VertexShader(0), PixelShader(0), UserData(userData)
{
	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram, EVT_STANDARD);
}


//! constructor only for use by derived classes who want to
//! create a fall back material for example.
CPS3ShaderMaterialRenderer::CPS3ShaderMaterialRenderer(CPS3Driver* driver,
							IShaderConstantSetCallBack* callback,
							IMaterialRenderer* baseMaterial, s32 userData)
: Driver(driver), CallBack(callback), BaseMaterial(baseMaterial),
		VertexShader(0), PixelShader(0), UserData(userData)
{
	if (BaseMaterial)
		BaseMaterial->grab();

	if (CallBack)
		CallBack->grab();
}


//! Destructor
CPS3ShaderMaterialRenderer::~CPS3ShaderMaterialRenderer()
{
	if (CallBack)
		CallBack->drop();
#if !defined(_IRR_USE_PS3_DEVICE_)
	if (VertexShader)
		Driver->extGlDeletePrograms(1, &VertexShader);

	if (PixelShader)
		Driver->extGlDeletePrograms(1, &PixelShader);
#endif
	if (BaseMaterial)
		BaseMaterial->drop ();
}

void CPS3ShaderMaterialRenderer::init(s32& outMaterialTypeNr, const c8* vertexShaderProgram,
	const c8* pixelShaderProgram, E_VERTEX_TYPE type)
{
	outMaterialTypeNr = -1;

	// create vertex shader
	if (!createVertexShader(vertexShaderProgram))
		return;

	// create pixel shader
	if (!createPixelShader(pixelShaderProgram))
		return;

	// register as a new material
	outMaterialTypeNr = Driver->addMaterialRenderer(this);
}

bool CPS3ShaderMaterialRenderer::onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	// call callback to set shader constants
	if (CallBack && (VertexShader || PixelShader))
		CallBack->onSetConstants(service, UserData);

	return true;
}


void CPS3ShaderMaterialRenderer::onSetMaterial(const video::SMaterial& material, const video::SMaterial& lastMaterial,
	bool resetAllRenderstates, video::IMaterialRendererServices* services)
{
	if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
	{
#ifdef GL_ARB_vertex_program
		if (VertexShader)
		{
			// set new vertex shader
			Driver->extGlBindProgram(GL_VERTEX_PROGRAM_ARB, VertexShader);
			glEnable(GL_VERTEX_PROGRAM_ARB);
		}
#endif

		// set new pixel shader
#ifdef GL_ARB_fragment_program
		if (PixelShader)
		{
			Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader);
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
		}
#endif

		if (BaseMaterial)
			BaseMaterial->onSetMaterial(material, material, true, services);
	}

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		Driver->setTexture(i, material.getTexture(i));
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}


void CPS3ShaderMaterialRenderer::onUnsetMaterial()
{
	// disable vertex shader
#ifdef GL_ARB_vertex_program
	if (VertexShader)
		glDisable(GL_VERTEX_PROGRAM_ARB);
#endif

#ifdef GL_ARB_fragment_program
	if (PixelShader)
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif

	if (BaseMaterial)
		BaseMaterial->onUnsetMaterial();
}

//! Returns if the material is transparent.
bool CPS3ShaderMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}

bool CPS3ShaderMaterialRenderer::createPixelShader(const c8* pxsh)
{
#if !defined(_IRR_USE_PS3_DEVICE_)
	if (!pxsh)
		return true;

	Driver->extGlGenPrograms(1, &PixelShader);
#ifdef GL_ARB_fragment_program
	Driver->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, PixelShader);

	// clear error buffer
	while(glGetError() != GL_NO_ERROR)
		{}

	// compile
	Driver->extGlProgramString(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen(pxsh), pxsh);
#endif

#ifdef GL_ARB_vertex_program
	GLenum g = glGetError();
	if (g != GL_NO_ERROR)
	{
		GLint errPos;
		glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );

		const char* errString = reinterpret_cast<const char*>(glGetString(GL_PROGRAM_ERROR_STRING_ARB));

		char tmp[2048];
		sprintf(tmp, "Pixel shader compilation failed at position %d:\n%s", errPos, errString);
		os::Printer::log(tmp);

		return false;
	}
#else
	return false;
#endif

	return true;
#else
	return false;
#endif
}

bool CPS3ShaderMaterialRenderer::createVertexShader(const char* vtxsh)
{
	if (!vtxsh)
		return true;

#ifdef GL_ARB_vertex_program
	Driver->extGlGenPrograms(1, &VertexShader);
	Driver->extGlBindProgram(GL_VERTEX_PROGRAM_ARB, VertexShader);

	// clear error buffer
	while(glGetError() != GL_NO_ERROR)
		{}

	// compile
	Driver->extGlProgramString(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
		strlen(vtxsh), vtxsh);

	GLenum g = glGetError();
	if (g != GL_NO_ERROR)
	{
		GLint errPos;
		glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );

		const char* errString = reinterpret_cast<const char*>(glGetString(GL_PROGRAM_ERROR_STRING_ARB));

		char tmp[2048];
		sprintf(tmp, "Vertex shader compilation failed at position %d:\n%s", errPos, errString);
		os::Printer::log(tmp);

		return false;
	}
#else
	return false;
#endif

	return true;
}

u32 CPS3ShaderMaterialRenderer::getVertexAttributeMask() const
{
	// requires analysis of shader varying vertex input
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	return 0;
}

} // end namespace video
} // end namespace irr


#endif