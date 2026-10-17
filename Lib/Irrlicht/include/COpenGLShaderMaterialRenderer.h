//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_SHADER_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_SHADER_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "CCommonGLHeaders.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

class COpenGLDriver;
class IShaderConstantSetCallBack;
class IMaterialRenderer;

//! Class for using vertex and pixel shaders with OpenGL
class COpenGLShaderMaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	COpenGLShaderMaterialRenderer(COpenGLDriver* driver,
								  s32& outMaterialTypeNr,
								  const c8* vertexShaderProgram,
								  const c8* pixelShaderProgram,
								  IShaderConstantSetCallBack* callback,
								  IMaterialRenderer* baseMaterial,
								  s32 userData);

	//! Destructor
	virtual ~COpenGLShaderMaterialRenderer();

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	virtual bool onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;

protected:

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	COpenGLShaderMaterialRenderer(COpenGLDriver* driver,
								  IShaderConstantSetCallBack* callback,
								  IMaterialRenderer* baseMaterial,
								  s32 userData=0);

	void init(s32& outMaterialTypeNr,
			  const c8* vertexShaderProgram,
			  const c8* pixelShaderProgram,
			  E_VERTEX_TYPE type);

	bool createPixelShader(const c8* pxsh);
	bool createVertexShader(const char* vtxsh);

	COpenGLDriver* Driver;
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

