
#include "stdafx.h"
#include "Particle/ParticleSystemManager.h"
#include "Particle/ParticleSystem/CFpsParticleSystemSceneNode.h"

typedef std::vector<ParticleSystem*> ParticleSystemList;
ParticleSystemList m_particleSystemList;

irr::core::array<CParticlePreset*> m_ParticlePresets;

void ClearParticleSystem() 
{
	ParticleSystemList::iterator it = m_particleSystemList.begin();
	for(; it != m_particleSystemList.end();) 
	{
		ParticleSystem* p = *it;
		it = m_particleSystemList.erase(it);
		p->drop();
	}
}


ParticleSystem* AddParticleSystem(const char *typeName)
{
	ParticleSystem* particleSystem = NEW ParticleSystem(typeName);

	if(particleSystem)
		m_particleSystemList.push_back(particleSystem);

	return particleSystem;
}

void UpdateParticleSystem(f32 timestep)
{
	ParticleSystemList::iterator it = m_particleSystemList.begin();
	for(; it != m_particleSystemList.end();) 
	{
		(*it)->Update(timestep);
		if (!(*it)->IsAlive()) 
		{
			ParticleSystem* p = *it;
			it = m_particleSystemList.erase(it);
			p->drop();
		}
		else
		{
			it++;
		}
	}
}

bool IsParticleSystemAlive(ParticleSystem* particle)
{
	ParticleSystemList::iterator it = m_particleSystemList.begin();
	for(; it != m_particleSystemList.end(); it++) 
	{
		if((*it) == particle)
		{
			return particle->IsAlive();
		}
	}

	return false;
}

void RemoveParticleSystem(ParticleSystem* particle)
{
	ParticleSystemList::iterator it = m_particleSystemList.begin();
	for(; it != m_particleSystemList.end();it++) 
	{
		if((*it) == particle)
		{
			ParticleSystem* p = *it;
			m_particleSystemList.erase(it);
			p->drop();
			break;
		}
	}
}

int LoadParticlePresets()
{
	RemoveParticlePresets();

	irr::IrrlichtDevice* pDevice = Application::GetInstance()->GetDevice();
	irr::scene::ISceneManager* smgr = pDevice->getSceneManager();

	irr::io::IXMLReader* xml = pDevice->getFileSystem()->createXMLReader("Effects/effects.xml");

	if (!xml)
		return 1;

	while(xml->read())
	{
		if (irr::core::stringw(L"effect") != xml->getNodeName())
		{
			continue;
		}

		irr::io::IAttributes* attributes = NULL;

		CParticlePreset* pParticle = new CParticlePreset(irr::core::stringw(xml->getAttributeValue(L"name")));
		m_ParticlePresets.push_back(pParticle);

		while(xml->read())
		{
			bool endreached = false;

			switch (xml->getNodeType())
			{
			case irr::io::EXN_ELEMENT_END:
				if (irr::core::stringw(L"effect") == xml->getNodeName())
				{
					endreached = true;
				}
				break;

			case irr::io::EXN_ELEMENT:
				if (irr::core::stringw(L"ps")==xml->getNodeName())
				{
					while(xml->read())
					{
						bool ps_endreached = false;
						switch (xml->getNodeType())
						{
						case irr::io::EXN_ELEMENT_END:
							if (irr::core::stringw(L"ps") == xml->getNodeName())
								{
									ps_endreached = true;
								}
								break;

						case irr::io::EXN_ELEMENT:
							if (irr::core::stringw(L"attributes")==xml->getNodeName())
								{
									// read attributes
									attributes = pDevice->getFileSystem()->createEmptyAttributes();
									attributes->read(xml, true);

									irr::IFpsParticleSystemSceneNode* pNode = new irr::CFpsParticleSystemSceneNode(
										true, -1, v3f(0, 0, 0), v3f(0, 0, 0), v3f(1, 1, 1));
									__IRR_SCENE()->getRootSceneNode()->addChild(pNode);
									pNode->drop();
									const irr::core::stringc& texName = attributes->getAttributeAsString("Texture");
									irr::video::ITexture* pTexture = pDevice->getVideoDriver()->getTexture((stringc("Effects/") + texName).c_str());
									pNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
									pNode->setMaterialTexture(0, pTexture);

									const irr::core::stringc& strMatType = attributes->getAttributeAsString("MaterialType");
									for (int z = 0; z < pDevice->getVideoDriver()->getMaterialRendererCount(); z ++)
									{
										const char* pMaterialRendererName = pDevice->getVideoDriver()->getMaterialRendererName(z);

										if(pMaterialRendererName && (strcmp(strMatType.c_str(), pMaterialRendererName) == 0))
										{
											pNode->setMaterialType((irr::video::E_MATERIAL_TYPE)z);
											break;
										}
									}

									pNode->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
									pNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);

									pNode->grab();
									pNode->deserializeAttributes(attributes);

									pNode->setVisible(false);

									m_ParticlePresets.getLast()->m_ParticleSystems.push_back(pNode);

									attributes->drop();

								}
								break;
						}

						if (ps_endreached)
							break;
					}
				}
				break;

			default:
				break;
			}

			if (endreached)
				break;
		}
	}

	xml->drop();

	return 0;
}

void RemoveParticlePresets()
{
	for (u32 i = 0 ; i < m_ParticlePresets.size() ; i++)
	{
		SAFE_DEL(m_ParticlePresets[i]);
	}

	m_ParticlePresets.clear();
}

void SetObjectParticleType(irr::scene::ISceneNode* node , irr::core::stringw particleType)
{		
	int type = 0;
	bool bFound = false;

	for (int i=0; i < m_ParticlePresets.size(); i++)
	{
		if (m_ParticlePresets[i]->m_particleName == particleType)
		{
			type = i;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		irr::core::stringw strError = L"Object ID ";
		strError += node->getID();
		strError += L" cannot load particle '";
		strError += particleType;
		strError += L"'!!!\n";

		return;
	}

	for (u32 i = 0; i < m_ParticlePresets[type]->m_ParticleSystems.size(); i++)
	{
		irr::IFpsParticleSystemSceneNode* pNode = (irr::IFpsParticleSystemSceneNode*)m_ParticlePresets[type]->m_ParticleSystems[i]->clone();
		pNode->setVisible(true);
		pNode->Restart();
		node->addChild(pNode);
		pNode->drop();

		irr::core::list<irr::IFpsParticleAffector*> _affectors = ((irr::IFpsParticleSystemSceneNode*)pNode)->getAffectors();
		irr::core::list<irr::IFpsParticleAffector*>::Iterator it = _affectors.begin();
		for(; it != _affectors.end(); it++)
		{
			if ((*it)->getType() == irr::scene::EPAT_ATTRACT)
			{
				irr::core::vector3df newPoint = node->getAbsolutePosition() + ((irr::IFpsParticleAttractionAffector*)(*it))->getPoint();
				((irr::IFpsParticleAttractionAffector*)(*it))->setPoint(newPoint);
			}

			if ((*it)->getType() == irr::scene::EPAT_ROTATE)
			{
				irr::core::vector3df newPoint = node->getAbsolutePosition() + ((irr::IFpsParticleRotationAffector*)(*it))->getPivotPoint();
				((irr::IFpsParticleRotationAffector*)(*it))->setPivotPoint(newPoint);
			}
		}
	}
}