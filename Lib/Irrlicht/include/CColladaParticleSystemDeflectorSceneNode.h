#ifndef __CCOLLADAPARTICULESYSTEMDEFLECTORFORCESCENENODE_H__
#define __CCOLLADAPARTICULESYSTEMDEFLECTORFORCESCENENODE_H__


#include "CColladaParticleSystemForceSceneNode.h"
//#include "PForceImpl.h"

namespace irr
{
namespace collada
{
namespace particle_system
{
class CDeflectorForceSceneNode
	: public CForceSceneNode
{
public:
	CDeflectorForceSceneNode(const CColladaDatabase &database, const SForce &force)
		: CForceSceneNode(database, force)
	{
		#ifdef _DEBUG
		setDebugName("CDeflectorForceSceneNode");
		#endif
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		DeflectorParameters.transformation = &AbsoluteTransformation;
		DeflectorParameters.bounce = Force.pDeflector->bounce;
		DeflectorParameters.variation = Force.pDeflector->variation;
		DeflectorParameters.chaos = Force.pDeflector->chaos;
		DeflectorParameters.friction = Force.pDeflector->friction;
		DeflectorParameters.inheritVelocity = Force.pDeflector->inheritVelocity;
		DeflectorParameters.width = Force.pDeflector->width;
		DeflectorParameters.length = Force.pDeflector->height; //length is exported as height in the dae
#endif
	}

	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		CForceSceneNode::serializeAttributes(out,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		out->addFloat("bounce", DeflectorParameters.bounce);
		out->addFloat("variation", DeflectorParameters.variation);
		out->addFloat("chaos", DeflectorParameters.chaos);
		out->addFloat("friction", DeflectorParameters.friction);
		out->addFloat("inheritVelocity", DeflectorParameters.inheritVelocity);
		out->addFloat("width", DeflectorParameters.width);
		out->addFloat("length", DeflectorParameters.length);
#endif
	}

	//! Reads attributes of the element
	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		CForceSceneNode::deserializeAttributes(in,options);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_		
		DeflectorParameters.bounce = in->getAttributeAsFloat("bounce");
		DeflectorParameters.variation = in->getAttributeAsFloat("variation");
		DeflectorParameters.chaos = in->getAttributeAsFloat("chaos");
		DeflectorParameters.friction = in->getAttributeAsFloat("friction");
		DeflectorParameters.inheritVelocity = in->getAttributeAsFloat("inheritVelocity");
		DeflectorParameters.width = in->getAttributeAsFloat("width");
		DeflectorParameters.length = in->getAttributeAsFloat("length");
#endif
	}

	void bind(CParticleSystemSceneNode* node)
	{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		node->getParticleSystem()->bindForce<ps::PDeflector>(DeflectorParameters);
#endif
	}

private:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	ps::PDeflector::Parameters DeflectorParameters;
#endif
};

}; //end namespace particle_system
}; //end namespace collada
}; //end namespace irr

#endif //__CCOLLADAPARTICULESYSTEMDEFLECTORFORCESCENENODE_H__