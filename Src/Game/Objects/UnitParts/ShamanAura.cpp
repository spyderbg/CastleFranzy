#include "stdafx.h"
#include "ShamanAura.h"
#include "Particle/ParticleSystemManager.h"

ShamanAura::ShamanAura (const stringc& name, float scale, ISceneNode* dummy)
{
	m_effectName = name;
	m_effectScale = scale;
	m_dummy = dummy;

	m_hiding = false;
	m_particleSystem = 0;
	m_currentScale = 1.0f;
}

ShamanAura::~ShamanAura ()
{
	if (m_particleSystem)
	{
		RemoveParticleSystem(m_particleSystem);
		m_particleSystem = 0;
	}
}

void ShamanAura::update (float secs)
{
	if (m_hiding)
	{
		float newScale = m_currentScale - 1.5f * secs;
		if (newScale > 0.0f)
		{
			m_particleSystem->SetScale(newScale * m_effectScale);
			m_currentScale = newScale;
		}
		else
		{
			m_hiding = false;
			RemoveParticleSystem(m_particleSystem);
			m_particleSystem = 0;
		}
	}
}

void ShamanAura::draw ()
{
}

void ShamanAura::hide ()
{
	if (!m_hiding && m_particleSystem)
	{
		m_hiding = true;
		m_currentScale = 1.0f;
	}
}

void ShamanAura::hideImmediately ()
{
	if (m_particleSystem)
	{
		RemoveParticleSystem(m_particleSystem);
		m_particleSystem = 0;
		m_hiding = false;
	}
}

void ShamanAura::show ()
{
	if (!m_particleSystem || m_currentScale != 1.0f)
	{
		if (m_particleSystem)
		{
			RemoveParticleSystem(m_particleSystem);
		}

		m_particleSystem = AddParticleSystem(m_effectName.c_str());
		m_particleSystem->SetScale(m_effectScale);
		m_particleSystem->SetRotation(quaternion(0.5f * PI, 0.0f, 0.0f));
		m_currentScale = 1.0f;
		m_dummy->addChild(m_particleSystem->getNode());
		m_hiding = false;
	}
}
