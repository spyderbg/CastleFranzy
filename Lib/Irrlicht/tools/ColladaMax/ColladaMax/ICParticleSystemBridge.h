/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PARTICLE_SYSTEM_BRIDGE_H
#define _IC_PARTICLE_SYSTEM_BRIDGE_H

#include <vector>

#undef snprintf
#include "PSManager.h"

class ICBridgeableParticleSystemInParams;
class ICBridgeableParticleSystemOutParams;

class ICParticleSystemBridge
{
private:
	static ICParticleSystemBridge *s_pParticleSystemBridge;

	ICParticleSystemBridge(){}
	~ICParticleSystemBridge(){}

	void PreUpdate( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemInParams &inParams );
	void PostUpdate( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemOutParams &outParams );

public:
	static ICParticleSystemBridge *GetInst();
	static void Release();

	static irr::core::matrix4 MaxMatrix3ToIrrCoreMatrix4( const Matrix3 &m );

	void Update( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemInParams &inParams, ICBridgeableParticleSystemOutParams &outParams );
};

inline
ICParticleSystemBridge *ICParticleSystemBridge::GetInst()
{
	return s_pParticleSystemBridge ? s_pParticleSystemBridge : ( s_pParticleSystemBridge = new ICParticleSystemBridge() );
}

inline
void ICParticleSystemBridge::Release()
{
	SAFE_DELETE( s_pParticleSystemBridge );
}

#endif // _IC_PARTICLE_SYSTEM_BRIDGE_H

