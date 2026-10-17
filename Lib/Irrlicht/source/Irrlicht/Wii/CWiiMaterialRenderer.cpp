#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_USE_WII_DEVICE_
#include "CWiiMaterialRenderer.h"

namespace irr
{
namespace video
{
namespace
{
static GXBlendFactor BlendFactors[] = 
{
	GX_BL_ZERO,			// EBF_ZERO
	GX_BL_ONE,			// EBF_ONE
	GX_BL_DSTCLR,		// EBF_DST_COLOR
	GX_BL_INVDSTCLR,	// EBF_ONE_MINUS_DST_COLOR,
	GX_BL_SRCCLR,		// EBF_SRC_COLOR
	GX_BL_INVSRCCLR,	// EBF_ONE_MINUS_SRC_COLOR
	GX_BL_SRCALPHA,		// EBF_SRC_ALPHA
	GX_BL_INVSRCALPHA,	// EBF_ONE_MINUS_SRC_ALPHA
	GX_BL_DSTALPHA,		// EBF_DST_ALPHA
	GX_BL_INVDSTALPHA,	// EBF_ONE_MINUS_DST_ALPHA
	GX_BL_SRCALPHA		//EBF_SRC_ALPHA_SATURATE
};
//u32 getTexelAlpha ( E_BLEND_FACTOR factor )
//{
//	u32 r;
//	switch ( factor )
//	{
//		case EBF_SRC_ALPHA:		r = 1; break;
//		case EBF_ONE_MINUS_SRC_ALPHA:	r = 1; break;
//		case EBF_DST_ALPHA:		r = 1; break;
//		case EBF_ONE_MINUS_DST_ALPHA:	r = 1; break;
//		case EBF_SRC_ALPHA_SATURATE:	r = 1; break;
//		default:			r = 0; break;
//	}
//	return r;
//}
}
//------------------------------------------------------------------------------

void CWiiMaterialRenderer::baseTevSetup(const SMaterial& material,
										const SMaterial& lastMaterial,
										bool resetAllRenderstates)
{
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	GXSetNumTevStages(1);
	GXSetNumChans(1);
	GXSetNumTexGens(1);

	GXSetTevSwapModeTable(GX_TEV_SWAP0,GX_CH_RED, GX_CH_GREEN,GX_CH_BLUE,GX_CH_ALPHA); // identity
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		
	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
	//GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		
	GXColor ambient = {0, 0, 0, 255};
		
	GXColor diffuse = {255, 255, 255, 255};
	material.getDiffuseColor().toOpenGLColor((u8*)&diffuse);

	const SColorf& ambiantLight = Driver->getAmbientLight();
	SColor ambientColor;
	ambientColor.setRed(ambiantLight.getRed() * 255); // * material.getAmbientColor().getRed());
	ambientColor.setGreen(ambiantLight.getGreen() * 255); // * material.getAmbientColor().getGreen());
	ambientColor.setBlue(ambiantLight.getBlue() * 255); // * material.getAmbientColor().getBlue());
	ambientColor.setAlpha(255); // * material.getAmbientColor().getAlpha());
	ambientColor.toOpenGLColor((u8*)&ambient);

	GXSetChanAmbColor(GX_COLOR0A0, ambient);
	GXSetChanMatColor(GX_COLOR0A0, diffuse);

	static int LightMasks[] =
	{
		GX_LIGHT_NULL,
		GX_LIGHT0,
		GX_LIGHT0 | GX_LIGHT1,
		GX_LIGHT0 | GX_LIGHT1 |GX_LIGHT2,
		GX_LIGHT0 | GX_LIGHT1 |GX_LIGHT2 | GX_LIGHT3
	};

	int lightMask = LightMasks[core::min_(Driver->getDynamicLightCount(), 4u)];

	GXSetChanCtrl(GX_COLOR0, 
				  material.getFlag(EMF_LIGHTING), 
				  GX_SRC_REG, 
				  GX_SRC_REG, 
				  lightMask, 
				  GX_DF_CLAMP, 
				  GX_AF_NONE);
	GXSetChanCtrl(GX_ALPHA0, 
				  GX_DISABLE, 
				  GX_SRC_REG, 
				  GX_SRC_REG, 
				  lightMask, 
				  GX_DF_NONE, 
				  GX_AF_NONE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );

	//GXSetTevOp( GX_TEVSTAGE0, GX_MODULATE );

	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);	
	
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_SOLID::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	Driver->setTexture(0, material.getTexture(0));
	//GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

u32 CWiiMaterialRenderer_SOLID::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}


//------------------------------------------------------------------------------

void CWiiMaterialRenderer_ONETEXTURE_BLEND::onSetMaterial(const SMaterial& material,
														  const SMaterial& lastMaterial,
														  bool resetAllRenderstates,
														  IMaterialRendererServices* services)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	Driver->setTexture(0, material.getTexture(0));
	
	E_BLEND_FACTOR srcFact,dstFact;
	E_MODULATE_FUNC modulate;
	unpack_texureBlendFunc ( srcFact, dstFact, modulate, material.getMaterialTypeParam(0) );
	GXSetBlendMode(GX_BM_BLEND, BlendFactors[srcFact], BlendFactors[dstFact], GX_LO_CLEAR);

//		Driver->disableTextures(1);
//		Driver->setTexture(0, material.getTexture(0));
//		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
//
////	if (material.MaterialType != lastMaterial.MaterialType ||
////		material.MaterialTypeParam != lastMaterial.MaterialTypeParam ||
////		resetAllRenderstates)
//	{
//		E_BLEND_FACTOR srcFact,dstFact;
//		E_MODULATE_FUNC modulate;
//		unpack_texureBlendFunc ( srcFact, dstFact, modulate, material.MaterialTypeParam );
//
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
//		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
//
//		glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, (f32) modulate );
//
//		glBlendFunc( getGLBlend(srcFact), getGLBlend(dstFact) );
//		glEnable(GL_ALPHA_TEST);
//		glEnable(GL_BLEND);
//
//		if ( getTexelAlpha ( srcFact ) + getTexelAlpha ( dstFact ) )
//		{
//			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
//			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
//
//			glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
//		}
//	}
}

//void CWiiMaterialRenderer_ONETEXTURE_BLEND::onUnsetMaterial()
//{
//	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.f );
//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);

//	glDisable(GL_BLEND);
//	glDisable(GL_ALPHA_TEST);
//}

u32 CWiiMaterialRenderer_ONETEXTURE_BLEND::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_SOLID_2_LAYER::onSetMaterial(const SMaterial& material,
													   const SMaterial& lastMaterial,	
													   bool resetAllRenderstates,
													   IMaterialRendererServices* services)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));
	Driver->setTexture(1, material.getTexture(1));
	//GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//		Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
	//	}

	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//}
}

u32 CWiiMaterialRenderer_SOLID_2_LAYER::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_SOLID_2_LAYER_DECAL::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));
	//GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//		Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
	//	}

	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//}
}

u32 CWiiMaterialRenderer_SOLID_2_LAYER_DECAL::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}


//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
	//GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	
	Driver->setTexture(0, material.getTexture(0));
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

// void CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR::onUnsetMaterial()
// {
// 	//glDisable(GL_BLEND);
// }

bool CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	
	GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	Driver->setTexture(0, material.getTexture(0));
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);

	//Driver->disableTextures(1);
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	//	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
	//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT );

	//	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PRIMARY_COLOR_EXT );
	//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);

	//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//	glEnable(GL_BLEND);
	//}
}

// void CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA::onUnsetMaterial()
// {
// 	//// default values
// 	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_MODULATE );
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE );
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT );
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE );
// 	//glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);

// 	//glDisable(GL_BLEND);
// }

bool CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	Driver->setTexture(0, material.getTexture(0));

	GXSetChanCtrl(GX_COLOR0A0,
				  material.getFlag(EMF_LIGHTING),
				  GX_SRC_REG,
				  GX_SRC_REG,
				  GX_LIGHT0 | GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT1,
				  GX_DF_CLAMP,
				  GX_AF_NONE);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

// void CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::onUnsetMaterial()
// {
// 	//glDisable(GL_ALPHA_TEST);
// 	//glDisable(GL_BLEND);
// }

bool CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}


//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));

	//Driver->disableTextures(1);
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	glEnable(GL_ALPHA_TEST);
	//	glAlphaFunc(GL_GREATER, 0.5);
	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//}
}

// void CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF::onUnsetMaterial()
// {
// 	//glDisable(GL_ALPHA_TEST);
// }

bool CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF::isTransparent() const
{
	return false;  // this material is not really transparent because it does no blending.
}

u32 CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_LIGHTMAP::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));

	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	// diffuse map

	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	//	switch (material.MaterialType)
	//	{
	//		case EMT_LIGHTMAP_LIGHTING:
	//		case EMT_LIGHTMAP_LIGHTING_M2:
	//		case EMT_LIGHTMAP_LIGHTING_M4:
	//			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//			break;
	//		case EMT_LIGHTMAP_ADD:
	//		case EMT_LIGHTMAP:
	//		case EMT_LIGHTMAP_M2:
	//		case EMT_LIGHTMAP_M4:
	//		default:
	//			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
	//			break;
	//	}
	//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
	//	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
	//	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
	//	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR );

	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		// lightmap

	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	//		if (material.MaterialType == EMT_LIGHTMAP_ADD)
	//			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_ARB);
	//		else
	//			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);

	//		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_MODULATE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PREVIOUS_ARB);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_ARB);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);

	//		switch (material.MaterialType)
	//		{
	//			case EMT_LIGHTMAP_M4:
	//			case EMT_LIGHTMAP_LIGHTING_M4:
	//				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 4.0f);
	//				break;
	//			case EMT_LIGHTMAP_M2:
	//			case EMT_LIGHTMAP_LIGHTING_M2:
	//				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2.0f);
	//				break;
	//			default:
	//				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f);
	//		}
	//	}
	//}
}

// void CWiiMaterialRenderer_LIGHTMAP::onUnsetMaterial()
// {
// 	//if (Driver->queryFeature(EVDF_MULTITEXTURE))
// 	//{
// 	//	Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
// 	//	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.f );
// 	//	Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
// 	//}
// }

u32 CWiiMaterialRenderer_LIGHTMAP::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}
//------------------------------------------------------------------------------

void CWiiMaterialRenderer_DETAIL_MAP::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));

	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	// diffuse map

	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	//	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);

	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		// detail map

	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT);

	//		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);

	//		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	//		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);
	//		Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
	//	}
	//}
}

u32 CWiiMaterialRenderer_DETAIL_MAP::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_SPHERE_MAP::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));

	//Driver->disableTextures(1);
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	//	glEnable(GL_TEXTURE_GEN_S);
	//	glEnable(GL_TEXTURE_GEN_T);
	//}
}

// void CWiiMaterialRenderer_SPHERE_MAP::onUnsetMaterial()
// {
// 	//glDisable(GL_TEXTURE_GEN_S);
// 	//glDisable(GL_TEXTURE_GEN_T);
// }

u32 CWiiMaterialRenderer_SPHERE_MAP::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_NORMAL | EVA_COLOR0 | EVA_TEXCOORD0;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_REFLECTION_2_LAYER::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
	Driver->setTexture(1, material.getTexture(1));
	Driver->setTexture(0, material.getTexture(0));

	GXSetNumTevStages(3);
	GXSetNumChans(1);
	GXSetNumTexGens(2);

	u8 k;
	if(material.getMaterialTypeParam(0) > 1.0)
	{
		k = 255;
	}
	else if(material.getMaterialTypeParam(0) < 0.0)
	{
		k = 0;
	}
	else
	{
		k = material.getMaterialTypeParam(0) * 255;
	}

	GXColor k3 = {0, 0, 0, k};

	GXSetTevKColor(GX_KCOLOR3, k3);

	// EnvTexMtx is fixed below
	f32 envMtx[][4] =
        {
            0.5f,  0.f,  0.f, 0.5f,
            0.f,  -0.5f, 0.f, 0.5f,
            0.f,   0.f,  0.f, 1.0f
        };
	GXLoadTexMtxImm(envMtx, GX_PTTEXMTX0, GX_MTX3x4);
		
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX2, GX_TRUE, GX_PTTEXMTX0);

	// Stage 0
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
	// Stage 0 - Color
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K3_A);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	// Stage 0 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K3_A);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// Stage 1
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL );
	// Stage 1 - Color
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
	GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K3_A);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
	// Stage 1 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K3_A);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// Stage 2
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
	// Stage 2 - Color
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0,  GX_CC_RASC, GX_CC_CPREV);
	GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	// Stage 2 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_K0_A);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	

	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE );
	//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT );

	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);

	//	}
	//	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//	glEnable(GL_TEXTURE_GEN_S);
	//	glEnable(GL_TEXTURE_GEN_T);
	//}
}

// void CWiiMaterialRenderer_REFLECTION_2_LAYER::onUnsetMaterial()
// {
// 	//if (Driver->queryFeature(EVDF_MULTITEXTURE))
// 	//{
// 	//	Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
// 	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	//}
// 	//glDisable(GL_TEXTURE_GEN_S);
// 	//glDisable(GL_TEXTURE_GEN_T);
// 	//if (Driver->queryFeature(EVDF_MULTITEXTURE))
// 	//{
// 	//	Driver->extGlActiveTexture(GL_TEXTURE0_ARB);

// 	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	//}
// }

u32 CWiiMaterialRenderer_REFLECTION_2_LAYER::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_NORMAL | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCCLR, GX_LO_CLEAR);

	Driver->setTexture(1, material.getTexture(1));
	Driver->setTexture(0, material.getTexture(0));

	GXSetNumTevStages(3);
	GXSetNumChans(1);
	GXSetNumTexGens(2);

	u8 k;
	if(material.getMaterialTypeParam(0) > 1.0)
	{
		k = 255;
	}
	else if(material.getMaterialTypeParam(0) < 0.0)
	{
		k = 0;
	}
	else
	{
		k = material.getMaterialTypeParam(0) * 255;
	}

	GXColor k3 = {0, 0, 0, k};

	GXSetTevKColor(GX_KCOLOR3, k3);

	// EnvTexMtx is fixed below
	f32 envMtx[][4] =
        {
            0.5f,  0.f,  0.f, 0.5f,
            0.f,  -0.5f, 0.f, 0.5f,
            0.f,   0.f,  0.f, 1.0f
        };
	GXLoadTexMtxImm(envMtx, GX_PTTEXMTX0, GX_MTX3x4);
		
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX2, GX_TRUE, GX_PTTEXMTX0);

	// Stage 0
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL );
	// Stage 0 - Color
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K3_A);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	// Stage 0 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K3_A);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// Stage 1
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
	// Stage 1 - Color
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV,  GX_CC_RASC, GX_CC_ZERO);
	GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	// Stage 1 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K0_A);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// Stage 2
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL );
	// Stage 2 - Color
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
	GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K3_A);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	// Stage 2 - Alpha
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_K3_A);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		
	//Driver->disableTextures(2);
	//Driver->setTexture(1, material.getTexture(1));
	//Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	//{
	//	if (Driver->queryFeature(EVDF_MULTITEXTURE))
	//	{
	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE );
	//		glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT );

	//		Driver->extGlActiveTexture(GL_TEXTURE1_ARB);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	//		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
	//		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
	//	}
	//	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//	glEnable(GL_TEXTURE_GEN_S);
	//	glEnable(GL_TEXTURE_GEN_T);

	//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	//	glEnable(GL_BLEND);
	//}
}

// void CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::onUnsetMaterial()
// {
// 	//if (Driver->queryFeature(EVDF_MULTITEXTURE))
// 	//{
// 	//	Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
// 	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	//}
// 	//glDisable(GL_TEXTURE_GEN_S);
// 	//glDisable(GL_TEXTURE_GEN_T);
// 	//if (Driver->queryFeature(EVDF_MULTITEXTURE))
// 	//{
// 	//	Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
// 	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// 	//}
// 	//glDisable(GL_BLEND);
// }

bool CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_NORMAL | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}

//------------------------------------------------------------------------------

void CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA::onSetMaterial(
	const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates,
	IMaterialRendererServices* services
)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);

	Driver->setTexture(0, material.getTexture(0));
	//Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	// culling
/*	if(material.BackfaceCulling && material.FrontfaceCulling)
		GXSetCullMode(GX_CULL_ALL);
	else if(material.BackfaceCulling)
		GXSetCullMode(GX_CULL_BACK);
	else if(material.FrontfaceCulling)
		GXSetCullMode(GX_CULL_FRONT);
	else
		GXSetCullMode(GX_CULL_NONE);
*/
		
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetChanCtrl(GX_COLOR0,
				  material.getFlag(EMF_LIGHTING),
				  GX_SRC_VTX,
				  GX_SRC_VTX,
				  GX_LIGHT0 | GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT1,
				  GX_DF_CLAMP,
				  GX_AF_NONE);
	GXSetChanCtrl(GX_ALPHA0,
				  GX_FALSE,
				  GX_SRC_REG,
				  GX_SRC_REG,
				  GX_LIGHT0 | GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT1,
				  GX_DF_NONE,
				  GX_AF_NONE);
		
	if(material.getTexture(0) != NULL)
	{
		//GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GXSetNumTevStages(1);
		GXSetNumTexGens(1);
		GXSetNumChans(0);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); 

		GXColor trans_fx = { 0xff, 0xff, 0xff, 0xff };
		//GXSetTevColor(GX_TEVREG0, trans_fx);

		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC);

		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetTevSwapModeTable(GX_TEV_SWAP0,GX_CH_RED,GX_CH_GREEN,GX_CH_BLUE,GX_CH_ALPHA);
	}

	GXInvalidateVtxCache();
}

// void CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA::onUnsetMaterial()
// {
// 	//glDisable(GL_ALPHA_TEST);
// 	//glDisable(GL_BLEND);
// }

bool CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_NORMAL | EVA_COLOR0 | EVA_TEXCOORD0 | EVA_TEXCOORD1;
}


//------------------------------------------------------------------------------

void CWiiMaterialRenderer_2D_ALPHA::onSetMaterial(const SMaterial& material,
						   const SMaterial& lastMaterial,
						   bool resetAllRenderstates,
						   IMaterialRendererServices* services)
{
	baseTevSetup(material, lastMaterial, resetAllRenderstates);
	
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
	
	if ( material.getTexture(0) )
	{
		Driver->setTexture(0, material.getTexture(0));
	}
}

bool CWiiMaterialRenderer_2D_ALPHA::isTransparent() const
{
	return true;
}

u32 CWiiMaterialRenderer_2D_ALPHA::getVertexAttributeMask() const
{
	return EVA_POSITION | EVA_COLOR0 | EVA_TEXCOORD0;
}

} // end namespace video
} // end namespace irr

#endif
