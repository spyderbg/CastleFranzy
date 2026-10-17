
#include "stdafx.h"
#include "ParticleSystem.h"
#include "ParticleSystemManager.h"
#include <CColladaParticleSystemSceneNode.h>

ParticleSystem::ParticleSystem() : 
	m_node(0),
	m_timeToLive(-1),
	m_lifeTime(0)
{ 
	m_node = Application::GetInstance()->GetDevice()->getSceneManager()->addEmptySceneNode();
	m_node->grab();
}

ParticleSystem::ParticleSystem(const char *typeName) :
	m_node(0),
	m_timeToLive(-1),
	m_lifeTime(0)
{
	m_node = Application::GetInstance()->GetDevice()->getSceneManager()->addEmptySceneNode();
	m_node->grab();

	LoadParticleSystem(typeName);
}

void ParticleSystem::Update(f32 timestep)
{
	m_lifeTime += timestep;
	if (m_timeToLive > 0 && m_lifeTime > m_timeToLive)
	{
		//Stop();
	}
}

void ParticleSystem::SetTimeToLive(f32 timeToLive)
{
	m_timeToLive = timeToLive;
}

ParticleSystem::~ParticleSystem() 
{
	UnloadParticleSystem();
}

void ParticleSystem::UnloadParticleSystem()
{
	if (m_node) 
	{
		m_node->removeAll();
		m_node->drop();
		m_node = 0;
	}
}

void ParticleSystem::LoadParticleSystem(const char *typeName)
{
	m_node->removeAll();

	SetObjectParticleType(m_node, typeName);
}

bool ParticleSystem::IsAlive()
{
	irr::core::list<irr::scene::ISceneNode*> children = m_node->getChildren();
	for(irr::core::list<irr::scene::ISceneNode*>::Iterator it = children.begin(); it != children.end(); it++)
	{
		if((*it)->getType() == irr::scene::ESNT_PARTICLE_SYSTEM)
		{
			irr::IFpsParticleSystemSceneNode* ps	=	(irr::IFpsParticleSystemSceneNode*)(*it);
			if(!ps->IsEnded())
			{
				return true;
			}
		}
	}

	return false;
}

void ParticleSystem::SetPosition(const irr::core::vector3df& v)
{
	if (m_node)
	{
		m_node->setPosition(v);
	}
}

void ParticleSystem::SetScale (float scale)
{
	if (m_node)
	{
		irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
		for (; it != m_node->getChildren().end();it++)
		{
			((irr::IFpsParticleSystemSceneNode*)(*it))->SetSystemScale(scale);
		}
	}
}

void ParticleSystem::SetRotation (const quaternion& rotation)
{
	if (m_node)
	{
		irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
		for (; it != m_node->getChildren().end();it++)
		{
			((irr::IFpsParticleSystemSceneNode*)(*it))->SetSystemRotation(rotation);
		}
	}
}

void ParticleSystem::Restart()
{
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
	for (; it != m_node->getChildren().end();it++)
	{
		irr::IFpsParticleSystemSceneNode* pParticleSys = (irr::IFpsParticleSystemSceneNode*)(*it);
		pParticleSys->Restart();
	}
}

void ParticleSystem::Pause()
{
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
	for (; it != m_node->getChildren().end();it++)
	{
		irr::IFpsParticleSystemSceneNode* pParticleSys = (irr::IFpsParticleSystemSceneNode*)(*it);
		pParticleSys->Pause();
	}
}

void ParticleSystem::UnPause()
{
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
	for (; it != m_node->getChildren().end();it++)
	{
		irr::IFpsParticleSystemSceneNode* pParticleSys = (irr::IFpsParticleSystemSceneNode*)(*it);
		pParticleSys->UnPause();
	}
}

void ParticleSystem::SetZBuffer (bool enabled)
{
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
	for (; it != m_node->getChildren().end();it++)
	{
		irr::IFpsParticleSystemSceneNode* pParticleSys = (irr::IFpsParticleSystemSceneNode*)(*it);
		pParticleSys->getMaterial(0).setFlag(EMF_ZBUFFER, enabled);
	}
}

void ParticleSystem::SetFogEnabled (bool enabled)
{
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_node->getChildren().begin();
	for (; it != m_node->getChildren().end();it++)
	{
		irr::IFpsParticleSystemSceneNode* pParticleSys = (irr::IFpsParticleSystemSceneNode*)(*it);
		pParticleSys->getMaterial(0).setFlag(EMF_FOG_ENABLE, enabled);
	}
}