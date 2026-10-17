#pragma once

#include "Core/irrlicht.h"
#include "ParticleSystem/IFpsParticleSystemSceneNode.h"

class CParticlePreset
{
public:
	CParticlePreset(irr::core::stringw name);
	~CParticlePreset();
			
	irr::core::array<irr::IFpsParticleSystemSceneNode*> m_ParticleSystems;
	irr::core::stringw m_particleName;
};
