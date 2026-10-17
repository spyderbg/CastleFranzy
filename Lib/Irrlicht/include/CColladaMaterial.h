//-*-c++-*-
#ifndef __CCOLLADAMATERIAL_H__
#define __CCOLLADAMATERIAL_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "SMaterial.h"
#include "IColladaObject.h"
#include "SDatabaseCollada.h"
#include "CColladaImage.h"
#include "IReferenceCounted.h"

namespace irr
{
namespace collada
{

class IRootSceneNode;
//class CImage;

class CMaterial
	: public IObject
	, public IReferenceCounted
{
public:

	CMaterial(const collada::CColladaDatabase& database,
			  collada::SMaterial& material,
			  collada::IRootSceneNode* root)
		: IObject(database)
		, Diffuse(0)	
		, OriginalMaterial(material)
	{
		setUID(material.id);
		prepareMaterial(root);
	}

	~CMaterial();

	const core::matrix4& getTextureMatrix() const
	{
		return DriverFormatMaterial.getTextureMatrix(0);
	}

	void setTextureMatrix(const core::matrix4& mat) 
	{
		DriverFormatMaterial.setTextureMatrix(0,mat);
	}

	video::SMaterial &get()
	{
		if(Diffuse)
		{
			DriverFormatMaterial.setTexture(0, Diffuse->getTexture());
		}
		return DriverFormatMaterial;
	}

	const res::String &getEffectUID() 
	{
		return OriginalMaterial.instanceEffect.pEffect->id;
	}

	const SMaterial& getOriginal() const 
	{
		return OriginalMaterial;
	}

protected:

	void prepareMaterial(collada::IRootSceneNode* root);

	CImage* Diffuse; // Used with IFL
	SMaterial& OriginalMaterial;
	video::SMaterial DriverFormatMaterial;

}; // class CMaterial

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif //__CCOLLADAMATERIAL_H__
