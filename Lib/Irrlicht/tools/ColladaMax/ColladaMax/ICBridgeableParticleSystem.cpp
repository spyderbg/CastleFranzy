#include "stdafx.h"
#include "ICBridgeableParticleSystem.h"

void ICBridgeableParticleSystem::InitParams()
{
	m_pParticleSystem = irr::ps::PSManager::getInstance().createPCloudSystem();
}


void ICBridgeableParticleSystem::ReleaseParams()
{
	SAFE_DELETE( m_pParticleSystem );
	CLEAR_POINTER_VECTOR( m_inParams.m_uiParams );
}