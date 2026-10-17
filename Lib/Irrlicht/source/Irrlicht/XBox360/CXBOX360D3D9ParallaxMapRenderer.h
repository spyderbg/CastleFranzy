// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_XBOX360_D3D9_PARALLAX_MAPMATERIAL_RENDERER_H_INCLUDED__
#define __C_XBOX360_D3D9_PARALLAX_MAPMATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"


#ifdef _IRR_XBOX_PLATFORM_

#include <xtl.h>
#include <d3d9.h>

#include "CXBOX360D3D9ShaderMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"

namespace irr
{
namespace video
{
//TODO:XBOX 360 Material: priority:6
//with:EMT_NORMAL_MAP_SOLID, EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR, EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA
//! Renderer for normal maps using parallax mapping
class CXBOX360D3D9ParallaxMapRenderer :
	public CXBOX360D3D9ShaderMaterialRenderer, IShaderConstantSetCallBack
{
public:

	CXBOX360D3D9ParallaxMapRenderer(
		IDirect3DDevice9* d3ddev, video::IVideoDriver* driver,
		s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial);

	~CXBOX360D3D9ParallaxMapRenderer();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void onSetConstants(IMaterialRendererServices* services, s32 userData);

	virtual bool onRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

	//! Returns the render capability of the material.
	virtual s32 getRenderCapability() const;

	virtual void onSetMaterial(const video::SMaterial& material,
		const video::SMaterial& lastMaterial,
		bool resetAllRenderstates, video::IMaterialRendererServices* services);

private:

	f32 CurrentScale;

};

} // end namespace video
} // end namespace irr

#endif
#endif


