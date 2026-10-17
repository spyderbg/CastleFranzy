//#include "stdinc.h"
#include "AudioDefs.h"
#include "StaticSound.h"
#include "CSoundData.h"

////////////////////////////////////////////////////////////////////////
StaticSound::StaticSound(CSoundData* data):
	CSound(data)
{
	// link the source to the static buffer
	alSourcei(m_sourceId,AL_BUFFER,data->GetBufferId(0));
	LOG_OPENAL_ERROR;
}

StaticSound::~StaticSound()
{

}
