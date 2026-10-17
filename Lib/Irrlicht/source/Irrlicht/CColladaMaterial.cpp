#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaMaterial.h"

#include "IColladaRootSceneNode.h"

namespace irr
{
namespace collada
{

CMaterial::~CMaterial()
{
	if (Diffuse)
	{
		Diffuse->drop();
	}
}

void 
CMaterial::prepareMaterial(collada::IRootSceneNode* root)
{
	video::SMaterial& mat = DriverFormatMaterial;
	collada::SEffect* colladaFx = OriginalMaterial.instanceEffect.pEffect;

	if (colladaFx->diffuse.eType == 1)
	{
		res::vector<collada::STexture>* textures = colladaFx->diffuse.pTexture;
		
		for (int j = 0, sz = textures->size(); j < sz; ++j)
		{
			collada::SImage& image = *(*textures)[j].sampler->pSurface->pImage;
			
			if (image.type == SImage::CIT_IFL)
			{
				Diffuse = root->getLibraryImage(image.id);
				_IRR_DEBUG_BREAK_IF(Diffuse == 0);
				if(Diffuse)
				{
					_IRR_DEBUG_BREAK_IF(image.pTexture == 0);
					mat.setTexture(j, Diffuse->getTexture());
					Diffuse->grab();
				}
			}
			else
			{
				mat.setTexture(j, image.pTexture);
			}
			core::matrix4 transform;
			transform.buildTextureTransform((*textures)[j].rotateUV, core::vector2df(0.0, 0.0),
				 core::vector2df((*textures)[j].offsetU, (*textures)[j].offsetV), 
				 core::vector2df((*textures)[j].repeatU,(*textures)[j].repeatV));
			if(!transform.isIdentity())
			{
				mat.setTextureMatrix(j, transform);
			}
		}

		
	}
	else
	{
		if (Diffuse)
		{
			Diffuse->drop();
		}
		Diffuse = 0;
		collada::SColor* clr = colladaFx->diffuse.pColor;
		mat.setDiffuseColor(
			video::SColor(u32(clr->a * colladaFx->transparency),
						  clr->r,
						  clr->g,
						  clr->b)

		
		);
	}

	if (OriginalMaterial.pDiffuseSurface)
	{
		mat.setTexture(0, OriginalMaterial.pDiffuseSurface->pTexture);
		collada::SColor* clr = &OriginalMaterial.diffuseColor;
		mat.setDiffuseColor(video::SColor(clr->a, clr->r, clr->g, clr->b));
	} 
	
#if 1//defined(_WII)
	if (OriginalMaterial.pEnvmapSurface)
	{
		if (OriginalMaterial.renderSet == 1)
		{
			mat.setMaterialType(video::EMT_TRANSPARENT_REFLECTION_2_LAYER);
		}
		else
		{
			mat.setMaterialType(video::EMT_REFLECTION_2_LAYER);
		}
		mat.setTexture(1, OriginalMaterial.pEnvmapSurface->pTexture);
		mat.setTexture(0, OriginalMaterial.pDiffuseSurface->pTexture);
		mat.setMaterialTypeParam(0, OriginalMaterial.envmapIntensity);
	}
#endif

	mat.setFlag(video::EMF_BACK_FACE_CULLING, OriginalMaterial.backfaceCulling !=0);
	mat.setFlag(video::EMF_FRONT_FACE_CULLING, OriginalMaterial.frontfaceCulling != 0);

	//mat.DiffuseColor.setAlpha(u32(colladaFx->transparency));
#if 1//defined(_WII)
	if (!OriginalMaterial.pEnvmapSurface)
#endif
	{
		if (colladaFx->transparent.eType == 1 || OriginalMaterial.renderSet == 1)
		{
			mat.setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		}
		else if (colladaFx->transparency != 1.0)
		{
			mat.setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		}
	}

	if (OriginalMaterial.isAdditive)
	{
		mat.setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		collada::SColor *clr = &OriginalMaterial.diffuseColor;
		mat.setDiffuseColor(video::SColor(clr->a, clr->r, clr->g, clr->b));
	}

	mat.setShininess(colladaFx->shininess);

	if(colladaFx->ambient.eType == 1)
	{
		mat.setMaterialType(video::EMT_LIGHTMAP);
		mat.setTexture(1, colladaFx->ambient.pTexture[0]->sampler->pSurface->pImage->pTexture);
	}
	else
	{
		collada::SColor* ambient = colladaFx->ambient.pColor;
		mat.setAmbientColor(video::SColor(ambient->a, ambient->r, ambient->g, ambient->b));
	}

	if(colladaFx->emission.eType == 1)
	{
	}
	else
	{
		collada::SColor* emission = colladaFx->emission.pColor;
		mat.setEmissiveColor(video::SColor(emission->a, emission->r, emission->g, emission->b));
	}

	if(colladaFx->specular.eType == 1)
	{
	}
	else
	{
		collada::SColor* clr = colladaFx->specular.pColor;
		mat.setSpecularColor(video::SColor(clr->a, clr->r, clr->g, clr->b));
	}

	if(colladaFx->doubleSided)
	{
		mat.setFlag(video::EMF_BACK_FACE_CULLING, false);
		mat.setFlag(video::EMF_FRONT_FACE_CULLING, false);
	} 
	else 
	{
		mat.setFlag(video::EMF_BACK_FACE_CULLING , OriginalMaterial.backfaceCulling !=0);
		mat.setFlag(video::EMF_FRONT_FACE_CULLING, OriginalMaterial.frontfaceCulling !=0);
	}

	
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
