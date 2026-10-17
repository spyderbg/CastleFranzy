#ifndef __CCOLLADAIMAGE_H__
#define __CCOLLADAIMAGE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaObject.h"
//#include "SMaterial.h"
//#include "SDatabaseCollada.h"
#include "IReferenceCounted.h"

namespace irr
{
namespace collada
{
class CImage
	: public IObject
	, public IReferenceCounted
{
public:
	CImage(const collada::CColladaDatabase &database, collada::SImage &image)
		: m_Image(image)
		, m_pTexture(0)
		, IObject(database)
	{
		setUID(image.id);
		prepareImage();
	}

	void prepareImage()
	{
		if(m_Image.type == collada::SImage::CIT_SINGLE)
		{
			_IRR_DEBUG_BREAK_IF(m_Image.pTexture == 0);
			m_pTexture = m_Image.pTexture;
		}
		else
		{
			_IRR_DEBUG_BREAK_IF(m_Image.pIFL->getTexture(0) == 0);
			m_pTexture = m_Image.pIFL->getTexture(0);
		}
	}

	video::ITexture *& getTexture() { return m_pTexture; }
protected:
	video::ITexture *m_pTexture;
	SImage &m_Image;
};
}; // namespace collada
}; // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADAIMAGE_H__
