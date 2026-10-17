// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_PS3_NORMAL_MAP_RENDERER_H_INCLUDED__
#define __C_PS3_NORMAL_MAP_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_PS3_

#if !defined (_IRR_USE_IPHONEOS_DEVICE_)

#include "CPS3ShaderMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"

namespace irr
{
namespace video
{

//! Class for rendering normal maps with OpenGL
class CPS3NormalMapRenderer : public CPS3ShaderMaterialRenderer, public IShaderConstantSetCallBack
{
public:

	//! Constructor
	CPS3NormalMapRenderer(video::CPS3Driver* driver,
		s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial);

	//! Destructor
	~CPS3NormalMapRenderer();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

	//! Returns the render capability of the material.
	virtual s32 getRenderCapability() const;

protected:

	bool CompiledShaders;
};


} // end namespace video
} // end namespace irr

#endif
#endif
#endif
