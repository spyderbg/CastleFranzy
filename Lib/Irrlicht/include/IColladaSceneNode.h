#ifndef __ICOLLADASCENENODE_H__
#define __ICOLLADASCENENODE_H__

#include "irrList.h"

namespace irr
{
namespace collada
{

class IColladaSceneNode
	: public scene::ISceneNode
{
public:
	void addMaterial(CMaterial *newMat) 
	{
		m_pMaterials->push_back(newMat);
	}

	void removeMaterial(CMaterial *oldMat)
	{
		std::list<CMaterial*>::iterator matIt = m_pMaterials.begin();
		for(; matIt != m_pMaterials.end(); matIt++)
		{
			if(*matIt == oldMat)
			{
				m_pMaterials.
			}
		}
	}

protected:
	core::list<CMaterial*> m_pMaterials;
};

}; // namespace collada
}; // namespace irr


#endif