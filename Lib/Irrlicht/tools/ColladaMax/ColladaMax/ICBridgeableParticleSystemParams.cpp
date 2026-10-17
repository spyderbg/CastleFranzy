#include "stdafx.h"
#include "ICBridgeableParticleSystemParams.h"

IICBridgeableParticleSystemUIParam *ICBridgeableParticleSystemInParams::GetParamByParamBlockId( int paramBlockId )
{
	IICBridgeableParticleSystemUIParam *pParam = NULL;

	for ( InParamList::iterator paramIter = m_uiParams.begin(); paramIter != m_uiParams.end(); ++paramIter )
	{
		if ( (*paramIter)->GetParamBlockId() == paramBlockId )
		{
			pParam = *paramIter;
			break;
		}
	}

	return pParam;
}