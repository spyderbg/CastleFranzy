// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_SHADER_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_SHADER_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#if !defined (_IRR_USE_IPHONEOS_DEVICE_)

#include "CCommonGLHeaders.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

class COpenGLESDriver;
class IShaderConstantSetCallBack;
class IMaterialRenderer;

//! Class for using vertex and pixel shaders with OpenGL
class COpenGLESShaderMaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	COpenGLESShaderMaterialRenderer(COpenGLESDriver* driver,
		s32& outMaterialTypeNr, const c8* vertexShaderProgram, const c8* pixelShaderProgram,
		IShaderConstantSetCallBack* callback, IMaterialRenderer* baseMaterial, s32 userData);

	//! Destructor
	virtual ~COpenGLESShaderMaterialRenderer();

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services);

	virtual bool onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

protected:

	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	COpenGLESShaderMaterialRenderer(COpenGLESDriver* driver,
					IShaderConstantSetCallBack* callback,
					IMaterialRenderer* baseMaterial, s32 userData=0);

	void init(s32& outMaterialTypeNr, const c8* vertexShaderProgram,
		const c8* pixelShaderProgram, E_VERTEX_TYPE type);

	bool createPixelShader(const c8* pxsh);
	bool createVertexShader(const char* vtxsh);

	COpenGLESDriver* Driver;
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

