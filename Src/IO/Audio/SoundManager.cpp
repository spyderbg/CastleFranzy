//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "IO/Audio/SoundManager.h"
#include "IO/Audio/SoundFileManager.h"

#include "Game/Config/UserProfile.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundManager::CSoundManager() :
	BaseSoundManager()
{
	m_numSounds = GetSoundFileManager()->GetNumberOfRegisteredSounds();

	GLL_ASSERT(m_numSounds >= 0);
	GLL_ASSERT(m_numSounds <= 512);

	m_sounds			=	gll_new std::vector<CSound*>[m_numSounds];
	m_soundsData		=	gll_new CSoundData*[m_numSounds];
	memset(m_soundsData, 0, m_numSounds*sizeof(CSoundData*));
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundManager::init( const char* profile_name )
{
	BaseSoundManager::init();

	DBG_OUT("SOUND MEMORY BUFFERS : %d\n", m_sndBuffersSize);

	/// Set SFX and music volumes
	f32 vol_sfx;
	f32 vol_music;

	CUserProfile profile = __PROFILE_ID( profile_name );
	profile.Get( "sfx_volume", vol_sfx );
	profile.Get( "music_volume", vol_music );
	
	setSFxVolume( vol_sfx / 100.f );
	setMusicVolume( vol_music / 100.f );
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundManager::update(TimeStamp timestamp)
{
	BaseSoundManager::update(timestamp);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundManager::getSoundType(int soundId) const 
{
	GLL_ASSERT(0 <= soundId && soundId < m_numSounds);

	return GetSoundFileManager()->GetSoundType(soundId);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundManager::playInPosition(int soundId, const irr::core::vector3df& position, bool loop, float volume)
{
	float pos[3] = { position.X, position.Y, position.Z };
	return playEx(soundId, pos, loop, volume, 0);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundManager::updatePosition(int soundId, int soundIndex, const irr::core::vector3df& position)
{
	float pos[3] = { position.X, position.Y, position.Z };

	setSourcePos(soundId, soundIndex, pos);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundManager::GetSoundIndexFromName( const char* soundName )
{
	if( strlen( soundName ) < 1 )
		return -1;

	return GetSoundFileManager()->GetSoundIndexFromName( soundName );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
const char* CSoundManager::GetSoundNameFromIndex( int soundId )
{
	return GetSoundFileManager()->GetSoundNameFromIndex( soundId );
}