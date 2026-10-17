#ifndef __CCOLLADAPARTICULESYSTEMFORCESCENENODE_H__
#define __CCOLLADAPARTICULESYSTEMFORCESCENENODE_H__

#include "ISceneNode.h"
#include "IColladaObject.h"
#include "CColladaParticleSystemSceneNode.h"

namespace irr
{
namespace collada
{
namespace particle_system
{
class CForceSceneNode
	: public scene::ISceneNode
	, public IObject
{
public:
	CForceSceneNode(const CColladaDatabase &database, const SForce &force)
		: IObject(database)
		, Force(force)
	{	
	}
	
	const SForce &getForce() { return Force; }

	virtual void bind(CParticleSystemSceneNode* node)
	{
		//TODO-FH: Find a way to eliminate getParticleSystem from derived implementation...
	}

	const irr::core::aabbox3d<f32> &getBoundingBox(void) const
	{
		static irr::core::aabbox3d<f32> empty;
		return empty;
	}

	//! Returns type of the scene node
	/** \return The type of this node. */
	virtual scene::ESCENE_NODE_TYPE getType() const { return scene::ESNT_COLLADA_FORCE; }

	virtual void render(void* renderData = 0)
	{
	}

protected:
	const SForce &Force;
};

}; //end namespace particle_system
}; //end namespace collada
}; //end namespace irr

#endif
