/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_BRIDGEABLE_PARTICLE_SYSTEM_H
#define _IC_BRIDGEABLE_PARTICLE_SYSTEM_H

#include "ICBridgeableParticleSystemParams.h"

#undef snprintf
#include "PSManager.h"

class INode;

class ICBridgeableParticleSystem
{
protected:
	ICBridgeableParticleSystemInParams m_inParams;
	ICBridgeableParticleSystemOutParams m_outParams;
	irr::ps::IParticleSystem<irr::ps::SParticle> *m_pParticleSystem;

	virtual void ApplyOutParams( const ICBridgeableParticleSystemOutParams &outParams ){}
	virtual void FetchInParams( ICBridgeableParticleSystemInParams &inParams, TimeValue t, INode *pNode ){}

	virtual void InitParams();
	virtual void ReleaseParams();

public:
	inline irr::ps::IParticleSystem<irr::ps::SParticle> *GetParticleSystemPtr()
	{ 
		return m_pParticleSystem; 
	}
};

#endif // _IC_BRIDGEABLE_PARTICLE_SYSTEM_H

