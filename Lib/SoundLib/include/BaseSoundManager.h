//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_SOUNDMANAGER_H__
#define __BASE_SOUNDMANAGER_H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AudioDefs.h"
#include "irrArray.h"

#include <vector>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#	include <AudioToolbox/AudioServices.h>
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <al.h>
#	include <alc.h>
#	define kAudioSessionBeginInterruption	0
#	define kAudioSessionEndInterruption		1
	typedef unsigned int UInt32;
	typedef unsigned int AudioSessionPropertyID;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace gameloft;

class CSoundData;
class CSound;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BaseSoundManager
{
public:
	enum
	{
		k_maxInstancesOfSameSound = 5,
	};

public:
	BaseSoundManager();
	~BaseSoundManager();

	void	init();
	void	initDevice();
	void	allocBuffers();
	void	unInit();

	void	toggleSound();

	void	OpenAL_Initialize();
	void	OpenAL_Destroy();

public:

	int		playEx(int soundId, const float* pos, 
						bool loop, float volume, int fadeInTime, 
						float pitch = 1.0f, void (*callback)() = NULL);

	int		play(int soundId, bool loop = false, int fadeInTime = 0, float volume=1.0f )    { return playEx(soundId, 0, loop, volume, fadeInTime); }
	int		play3D(int resId, const float* pos, bool loop = false)      { return playEx(resId, pos, loop, 1.0f, 0);	}

	void	stop(int soundId, int soundIndex,  int fadeOutTime = 0);
	void	stop(int soundId, int fadeOutTime = 0);
	void	stopAllSounds();
	void	stopAllMusics();
	void	stopAllSecondMusics();
	void	stopAllSfx();

	// helpers
	bool	isSoundPlaying(int soundId, int soundIndex = 0);
	int     playIfNotPlaying( int soundId, bool loop = false, int fadeInTime = 0 );

	void	update(int dt);

	void	releaseAllSounds_UponInterrupt();

public:
	static float getGlobalVolume() { return s_globalVolumeGain; }

	void	setListenerPos(const float* pos, const float* orientation);
	void	setSourcePos(const int soundId, const int soundIndex, const float* pos);

public:
	// debug info
	int		getNumberOfAllocatedSounds();
	int		getNumberOfAllocatedSoundsData();
	int		getNumberOfPlayingSounds();

public:

	void	setVolume(int soundId, int soundIndex, float volume);
	void	setPitch(int soundId, int soundIndex, float pitch);

	float	getSFxVolume()	const { return m_sfxVolume; };
	void	setSFxVolume(float volume);
	float	getMusicVolume() const {return m_musicVolume; };
	void	setMusicVolume(float volume);


public:
	CSoundData*		getSoundData(int soundId);
	int				getReadySound(int soundId);
	void			releaseSoundData(int soundId);

public:

	virtual int				getSoundType(int soundId) const = 0;

protected:
	inline	bool			IsSoundIdCorrect( int soundId ) { return ( soundId >= 0 ); }

protected:
	std::vector<CSound*>*	m_sounds;

	CSoundData**			m_soundsData;

	static float			s_globalVolumeGain;
	float					m_musicVolume;
	float					m_sfxVolume;

	int						m_numSounds;
	bool					m_isSoundInitialized;

	int						m_sndBuffersSize;

public:

	static	bool	s_bAudioSessionInitialized;

	static	void	interruptionListenerCallback(void *inUserData, UInt32 interruptionState);
	static	void	volumeChangeCallback (void * inClientData, AudioSessionPropertyID inID, UInt32 inDataSize, const void * inData);

private:	
			void	beginInterrupt();
			void	endInterrupt();

private:
	ALCdevice*      m_alDevice;
	ALCcontext*     m_alContext;

	bool			IsOtherAudioPlaying (bool interrupt);
	int				m_checkOtherAudioPlayingTimer;
	bool			m_isOtherAudioPlaying;
public:

	static float	k_AL_REFERENCE_DISTANCE;
	static float	k_AL_MAX_DISTANCE;
	static float	k_AL_ROLLOFF_FACTOR;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //__BASE_SOUNDMANAGER_H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
