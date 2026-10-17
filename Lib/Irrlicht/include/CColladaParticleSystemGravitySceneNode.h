#ifndef __CCOLLADAPARTICLESYSTEMGRAVITYFORCESCENENODE_H__
#define __CCOLLADAPARTICLESYSTEMGRAVITYFORCESCENENODE_H__

#include "CColladaParticleSystemForceSceneNode.h"
//#include "PForceImpl.h"

namespace irr
{
namespace collada
{
namespace particle_system
{
class CGravityForceSceneNode
	: public CForceSceneNode
{
public:
	CGravityForceSceneNode(const CColladaDatabase &database, const SForce &force)
		: CForceSceneNode(database, force)
	{
		#ifdef _DEBUG
		setDebugName("CGravityForceSceneNode");
		#endif
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		GravityParameters.transformation = &AbsoluteTransformation;
		GravityParameters.strength = Force.pGravity->strength;
		GravityParameters.decay = Force.pGravity->decay;
		GravityParameters.type = static_cast<ps::PGravity::PGravityType>(Force.pGravity->type);
#endif
	}

	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		CForceSceneNode::serializeAttributes(out,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		out->addFloat("decay", GravityParameters.decay);
		out->addFloat("strength", GravityParameters.strength);
		out->addInt("type", GravityParameters.type);
#endif
	}

	//! Reads attributes of the element
	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		CForceSceneNode::deserializeAttributes(in,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_		
		GravityParameters.decay = in->getAttributeAsFloat("decay");
		GravityParameters.strength = in->getAttributeAsFloat("strength");
		//GravityParameters.type = (PGravityType)in->getAttributeAsInt("type");
#endif
	}

	void bind(CParticleSystemSceneNode* node)
	{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		node->getParticleSystem()->bindForce<ps::PGravity>(GravityParameters);
#endif
	}
private:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	ps::PGravity::Parameters GravityParameters;
#endif
};

}; //end namespace particle_system
}; //end namespace collada
}; //end namespace irr

#endif