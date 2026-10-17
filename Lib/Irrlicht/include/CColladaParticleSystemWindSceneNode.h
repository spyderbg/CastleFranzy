#ifndef __CCOLLADAPARTICULESYSTEMWINDFORCESCENENODE_H__
#define __CCOLLADAPARTICULESYSTEMWINDFORCESCENENODE_H__


#include "CColladaParticleSystemForceSceneNode.h"
//#include "PForceImpl.h"

namespace irr
{
namespace collada
{
namespace particle_system
{
class CWindForceSceneNode
	: public CForceSceneNode
{
public:
	CWindForceSceneNode(const CColladaDatabase &database, const SForce &force)
		: CForceSceneNode(database, force)
	{
		#ifdef _DEBUG
		setDebugName("CWindForceSceneNode");
		#endif
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		WindParameters.transformation = &AbsoluteTransformation;
		WindParameters.strength = Force.pWind->strength;
		WindParameters.decay = Force.pWind->decay;
		WindParameters.turbulence = Force.pWind->turbulence;
		WindParameters.frequency = Force.pWind->frequency;
		WindParameters.scale = Force.pWind->scale;
		WindParameters.type = static_cast<ps::PWind::PWindType>(Force.pWind->type);
#endif
	}

	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		CForceSceneNode::serializeAttributes(out,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		out->addFloat("strength", WindParameters.strength);
		out->addFloat("decay", WindParameters.decay);
		out->addFloat("turbulence", WindParameters.turbulence);
		out->addFloat("frequency", WindParameters.frequency);
		out->addFloat("scale", WindParameters.scale);
#endif
	}

	//! Reads attributes of the element
	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		CForceSceneNode::deserializeAttributes(in,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_		
		WindParameters.strength = in->getAttributeAsFloat("strength");
		WindParameters.decay = in->getAttributeAsFloat("decay");
		WindParameters.turbulence = in->getAttributeAsFloat("turbulence");
		WindParameters.frequency = in->getAttributeAsFloat("frequency");
		WindParameters.scale = in->getAttributeAsFloat("scale");
#endif
	}

	void bind(CParticleSystemSceneNode* node)
	{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		node->getParticleSystem()->bindForce<ps::PWind>(WindParameters);
#endif
	}

private:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	ps::PWind::Parameters WindParameters;
#endif
};

}; //end namespace particle_system
}; //end namespace collada
}; //end namespace irr

#endif