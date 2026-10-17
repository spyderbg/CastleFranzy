// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_XBOX360_D3D9_MATERIAL_RENDERER_H_INCLUDED__
#define __C_XBOX360_D3D9_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"


#ifdef _IRR_XBOX_PLATFORM_
#include <d3d9.h>

#include "IMaterialRenderer.h"
#include "CXBOX360D3D9HLSLMaterialRenderer.h"
#include "CXBOX360D3D9Driver.h"
#include "CXBOX360D3D9BasicShaders.h"
namespace irr
{
namespace video
{

extern D3DMATRIX UnitMatrixD3D9;
extern D3DMATRIX SphereMapMatrixD3D9;
extern CXBOX360D3D9ShaderBasicConstantSetCallBack *g_pBasicShaderConstantCallBack;
//! Base class for all internal D3D9 material renderers
class CXBOX360D3D9MaterialRenderer : public CXBOX360D3D9HLSLMaterialRenderer
{
public:
 
	//! Constructor
	CXBOX360D3D9MaterialRenderer(IDirect3DDevice9* d3ddev, video::IVideoDriver* driver, irr::video::E_MATERIAL_TYPE mat_type)
		: CXBOX360D3D9HLSLMaterialRenderer(d3ddev, driver, nr, 
		g_basicshader, "vs_main", EVST_VS_3_0,
		g_basicshader, "ps_main", EPST_PS_3_0,
		g_pBasicShaderConstantCallBack, NULL,
		mat_type
		), 
		pID3DDevice(d3ddev), Driver(driver)
	{
		XBOXDirver = (CXBOX360D3D9Driver*)driver;
	}

	~CXBOX360D3D9MaterialRenderer()
	{
	}
	irr::u32 getVertexAttributeMask(void) const
	{
		return irr::video::EVA_POSITION | irr::video::EVA_TEXCOORD0 | irr::video::EVA_COLOR0;
	}

protected:
	irr::s32 nr;
	IDirect3DDevice9* pID3DDevice;
	video::IVideoDriver* Driver;
	CXBOX360D3D9Driver *XBOXDirver;
};

//! Solid material renderer
class CXBOX360D3D9MaterialRenderer_SOLID : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:1
	CXBOX360D3D9MaterialRenderer_SOLID(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_SOLID) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
//#ifndef _IRR_XBOX_PLATFORM_
//		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
//		{
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//		}
//
//		pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//#endif

	}


};

//! Generic Texture Blend
class CXBOX360D3D9MaterialRenderer_ONETEXTURE_BLEND : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:3
	CXBOX360D3D9MaterialRenderer_ONETEXTURE_BLEND(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_ONETEXTURE_BLEND) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() ||
			material.getMaterialTypeParam(0) != lastMaterial.getMaterialTypeParam(0) ||
			resetAllRenderstates)
		{

			E_BLEND_FACTOR srcFact,dstFact;
			E_MODULATE_FUNC modulate;
			unpack_texureBlendFunc ( srcFact, dstFact, modulate, material.getMaterialTypeParam(0) );

			if (srcFact == EBF_SRC_COLOR && dstFact == EBF_ZERO)
			{
				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
			else
			{

				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_SRCBLEND, getD3DBlend ( srcFact ) );
				pID3DDevice->SetRenderState(D3DRS_DESTBLEND, getD3DBlend ( dstFact ) );

			}
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, getD3DModulate ( modulate ) );
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//
//			if ( getTexelAlpha ( srcFact ) + getTexelAlpha ( dstFact ) )
//			{
//				pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//				pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//			}
//			else
//			{
//				pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//			}
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//#endif
		}
	}

	private:

		u32 getD3DBlend ( E_BLEND_FACTOR factor ) const
		{
			u32 r = 0;
			switch ( factor )
			{
				case EBF_ZERO:					r = D3DBLEND_ZERO; break;
				case EBF_ONE:					r = D3DBLEND_ONE; break;
				case EBF_DST_COLOR:				r = D3DBLEND_DESTCOLOR; break;
				case EBF_ONE_MINUS_DST_COLOR:	r = D3DBLEND_INVDESTCOLOR; break;
				case EBF_SRC_COLOR:				r = D3DBLEND_SRCCOLOR; break;
				case EBF_ONE_MINUS_SRC_COLOR:	r = D3DBLEND_INVSRCCOLOR; break;
				case EBF_SRC_ALPHA:				r = D3DBLEND_SRCALPHA; break;
				case EBF_ONE_MINUS_SRC_ALPHA:	r = D3DBLEND_INVSRCALPHA; break;
				case EBF_DST_ALPHA:				r = D3DBLEND_DESTALPHA; break;
				case EBF_ONE_MINUS_DST_ALPHA:	r = D3DBLEND_INVDESTALPHA; break;
				case EBF_SRC_ALPHA_SATURATE:	r = D3DBLEND_SRCALPHASAT; break;
			}
			return r;
		}

		u32 getTexelAlpha ( E_BLEND_FACTOR factor ) const
		{
			u32 r = 0;
			switch ( factor )
			{
				case EBF_SRC_ALPHA:				r = 1; break;
				case EBF_ONE_MINUS_SRC_ALPHA:	r = 1; break;
				case EBF_DST_ALPHA:				r = 1; break;
				case EBF_ONE_MINUS_DST_ALPHA:	r = 1; break;
				case EBF_SRC_ALPHA_SATURATE:	r = 1; break;
			}
			return r;
		}

		u32 getD3DModulate ( E_MODULATE_FUNC func ) const
		{
			u32 r = D3DTOP_MODULATE;
			switch ( func )
			{
				case EMFN_MODULATE_1X: r = D3DTOP_MODULATE; break;
				case EMFN_MODULATE_2X: r = D3DTOP_MODULATE2X; break;
				case EMFN_MODULATE_4X: r = D3DTOP_MODULATE4X; break;
			}
			return r;
		}

		bool transparent;

};



//! Solid 2 layer material renderer
class CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_SOLID_2_LAYER) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}
};


//! Solid 2 layer material (with decal) renderer
class CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER_DECAL : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER_DECAL(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_SOLID_2_LAYER_DECAL) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}
};


//! Transparent add color material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_ADD_COLOR : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:2
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_ADD_COLOR(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_ADD_COLOR) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		}
	}

	//! Returns if the material is transparent. The scene management needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};
//! Transparent vertex alpha x texture alpha material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:3
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1,  D3DTA_DIFFUSE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//			pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, core::floor32(material.getMaterialTypeParam(0) * 255.f));
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
	}
	virtual void onUnsetMaterial()
	{
		pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		CXBOX360D3D9MaterialRenderer::onUnsetMaterial();
	}

	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};

//! Transparent vertex alpha material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:2
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_VERTEX_ALPHA) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//pID3DDevice->SetRenderState(D3DRS_ALPHAREF, core::floor32(material.getMaterialTypeParam(0) * 255.f));
			//pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			//pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
	}
	virtual void onUnsetMaterial()
	{
		pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		CXBOX360D3D9MaterialRenderer::onUnsetMaterial();
	}
	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};


//! Transparent alpha channel material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:1
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_ALPHA_CHANNEL) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates
			|| material.getMaterialTypeParam(0) != lastMaterial.getMaterialTypeParam(0) )
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, core::floor32(material.getMaterialTypeParam(0) * 255.f));
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
	}

	virtual void onUnsetMaterial()
	{
		pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		CXBOX360D3D9MaterialRenderer::onUnsetMaterial();
	}

	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};



//! Transparent alpha channel material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:3
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_ALPHA_CHANNEL_REF) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, core::floor32(material.getMaterialTypeParam(0) * 255));
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		
	}

	virtual void onUnsetMaterial()
	{
		CXBOX360D3D9MaterialRenderer::onUnsetMaterial();
		pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return false; // this material is not really transparent because it does no blending.
	}
};


//! material renderer for all kinds of linghtmaps
class CXBOX360D3D9MaterialRenderer_LIGHTMAP : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:6
	CXBOX360D3D9MaterialRenderer_LIGHTMAP(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_LIGHTMAP) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			if (material.getMaterialType() >= EMT_LIGHTMAP_LIGHTING)
//			{
//				// with lighting
//				pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//			}
//			else
//			{
//				pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			}
//
//			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//			pID3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
//
//			if (material.getMaterialType() == EMT_LIGHTMAP_ADD)
//				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
//			else
//			if (material.getMaterialType() == EMT_LIGHTMAP_M4)
//				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
//			else
//			if (material.getMaterialType() == EMT_LIGHTMAP_M2)
//				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
//			else
//				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
//			pID3DDevice->SetTextureStageState (1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
	}
};



//! material renderer for detail maps
class CXBOX360D3D9MaterialRenderer_DETAIL_MAP : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_DETAIL_MAP(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_DETAIL_MAP) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//
//			pID3DDevice->SetTextureStageState (1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
//			pID3DDevice->SetTextureStageState (1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//			pID3DDevice->SetTextureStageState (1, D3DTSS_COLORARG2, D3DTA_CURRENT);
//			pID3DDevice->SetTextureStageState (1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
//
//			pID3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
	}
};


//! sphere map material renderer
class CXBOX360D3D9MaterialRenderer_SPHERE_MAP : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_SPHERE_MAP(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_SPHERE_MAP) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &SphereMapMatrixD3D9 );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
#endif
		}

		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
	}

//	virtual void onUnsetMaterial()
//	{
//#ifndef _IRR_XBOX_PLATFORM_
//		pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
//		pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrixD3D9 );
//#endif
//
//	}
};


//! reflection 2 layer material renderer
class CXBOX360D3D9MaterialRenderer_REFLECTION_2_LAYER : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_REFLECTION_2_LAYER(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_REFLECTION_2_LAYER) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

			pID3DDevice->SetTransform( D3DTS_TEXTURE1, &SphereMapMatrixD3D9 );
			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
	}
//
//	virtual void onUnsetMaterial()
//	{
//#ifndef _IRR_XBOX_PLATFORM_
//		pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
//		pID3DDevice->SetTransform( D3DTS_TEXTURE1, &UnitMatrixD3D9 );
//#endif
//	}
};


//! reflection 2 layer material renderer
class CXBOX360D3D9MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material: priority:5
	CXBOX360D3D9MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_REFLECTION_2_LAYER) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

			pID3DDevice->SetTransform( D3DTS_TEXTURE1, &SphereMapMatrixD3D9 );
			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}

		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
	}
//
//	virtual void onUnsetMaterial()
//	{
//#ifndef _IRR_XBOX_PLATFORM_
//		pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//		pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
//		pID3DDevice->SetTransform( D3DTS_TEXTURE1, &UnitMatrixD3D9 );
//#endif
//
//	}

	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};

//! Transparent alpha channel material renderer
class CXBOX360D3D9MaterialRenderer_2D_ALPHA : public CXBOX360D3D9MaterialRenderer
{
public:
	//TODO:XBOX 360 Material : CXBOX360D3D9MaterialRenderer_2D_ALPHA priority:1
	CXBOX360D3D9MaterialRenderer_2D_ALPHA(IDirect3DDevice9* p, video::IVideoDriver* d)
		: CXBOX360D3D9MaterialRenderer(p, d, EMT_TRANSPARENT_ALPHA_CHANNEL) {}

	virtual void onSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		CXBOX360D3D9MaterialRenderer::onSetMaterial(material, lastMaterial, resetAllRenderstates, services);
		if (material.getMaterialType() != lastMaterial.getMaterialType() || resetAllRenderstates
			|| material.getMaterialTypeParam(0) != lastMaterial.getMaterialTypeParam(0) )
		{
//#ifndef _IRR_XBOX_PLATFORM_
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//			pID3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, core::floor32(material.getMaterialTypeParam(0) * 255.f));
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
	}

	virtual void onUnsetMaterial()
	{
		pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		CXBOX360D3D9MaterialRenderer::onUnsetMaterial();
	}

	//! Returns if the material is transparent. The scene managment needs to know this
	//! for being able to sort the materials by opaque and transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};

} // end namespace video
} // end namespace irr

#endif
#endif


