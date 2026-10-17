
#include "stdafx.h"
#include "ParticlePreset.h"

using namespace irr;
using namespace core;
using namespace scene;

CParticlePreset::CParticlePreset(stringw name)
{
	m_particleName = name;
}

CParticlePreset::~CParticlePreset()
{
	for (int i=0; i < m_ParticleSystems.size(); i++)
	{
		m_ParticleSystems[i]->drop();
	}

	m_ParticleSystems.clear();
}