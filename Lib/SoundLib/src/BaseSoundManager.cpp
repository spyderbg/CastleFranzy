//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BaseSoundManager.h"
#include "CSoundData.h"
#include "CSound.h"
#include "StaticSound.h"
#include "StreamingSound.h"
#include "SourcesPool.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_DECODER_MPC
#	include "../../../lib/LibMPCDec/Sources/Stream/CMpcStream.h"
#endif //USE_DECODER_MPC

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_DECODER_WAV
#	include "CSoundDecoderMSWAV.h"
#endif //USE_DECODER_MSADPCM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#	include <AudioToolbox/AudioToolbox.h>
#	include "iPhoneMusicPlayer.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float BaseSoundManager::s_globalVolumeGain			=	1.f;

float BaseSoundManager::k_AL_REFERENCE_DISTANCE		=	1000.f;
float BaseSoundManager::k_AL_MAX_DISTANCE			=	10000.f;
float BaseSoundManager::k_AL_ROLLOFF_FACTOR			=	1.f;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSoundManager::BaseSoundManager() :
	m_musicVolume(0.75f),
	m_sfxVolume(0.85f),
	m_numSounds(0),
	m_isSoundInitialized(false),
	m_sndBuffersSize(0)
{
	m_checkOtherAudioPlayingTimer = 0;
	m_isOtherAudioPlaying = false;

#ifdef TARGET_OS_IPHONE
	Init_iPodPlayer();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSoundManager::~BaseSoundManager()
{
	unInit();

	gll_delete[] m_soundsData;
	gll_delete[] m_sounds;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::OpenAL_Initialize()
{
	#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
		m_alDevice = alcOpenDevice( NULL );
	#else
		m_alDevice = alcOpenDevice( NULL );
	#endif
	
	LOG_OPENAL_ERROR;
	
	m_alContext = alcCreateContext( m_alDevice, NULL );
	LOG_OPENAL_ERROR;

#ifdef OS_IPHONE 

	/// Check user music
	if( !g_canUsePlayer )
	{
		//printf("  using audio session property\n");

		UInt32 propertySize = sizeof(UInt32);
		UInt32 audioIsAlreadyPlaying;		
		
		AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &audioIsAlreadyPlaying);
		LOG_OPENAL_ERROR;

		m_isOtherAudioPlaying = (audioIsAlreadyPlaying == 1);
	}

#endif

	alcMakeContextCurrent( m_alContext );
	LOG_OPENAL_ERROR;
	
	#ifdef USE_SOUND_LOCALIZATION
		alDistanceModel( AL_LINEAR_DISTANCE_CLAMPED );
		LOG_OPENAL_ERROR;
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::OpenAL_Destroy()
{
	alcMakeContextCurrent( NULL ); 
	LOG_OPENAL_ERROR;
	alcDestroyContext( m_alContext ); 
	LOG_OPENAL_ERROR;
	alcCloseDevice( m_alDevice );
	LOG_OPENAL_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::init()
{
	if(!m_isSoundInitialized)
	{
		initDevice();

		OpenAL_Initialize();

		g_OpenAlSourcesPool.GenerateSources();

		allocBuffers();

		m_isSoundInitialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::allocBuffers()
{
	for(int i = 0; i < m_numSounds; i++)
	{
		int type = getSoundType(i);
		if (type & audio::k_typePreloadData)
		{
			// force allocation
			getSoundData(i);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::unInit()
{
	if(m_isSoundInitialized)
	{
		stopAllSounds();

		for(int i = 0; i < m_numSounds; i++)
		{
			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				gll_delete m_sounds[i][j];
				m_sounds[i][j] = 0;
			}
			m_sounds[i].resize(0);
		}

		g_OpenAlSourcesPool.DestroySources();

		OpenAL_Destroy();

		for(int i = 0; i < m_numSounds; i++)
		{
			gll_delete m_soundsData[i];
			m_soundsData[i] = 0;
		}

		m_isSoundInitialized = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::toggleSound()
{
	if(m_isSoundInitialized)
	{
		unInit();
		GLL_ASSERT(!m_isSoundInitialized);
	}
	else
	{
		init();
		GLL_ASSERT(m_isSoundInitialized);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::playEx(int soundId, const float* pos, bool loop, float volume, int fadeInTime, float pitch, void (*callback)())
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		//if this is a streaming sound and there is already one playing just return
		if((getSoundType(soundId) & audio::k_typeStreamedFile) && m_sounds[soundId].size() != 0)
		{
			return -1;
		}

		if (m_isOtherAudioPlaying && (getSoundType(soundId) & audio::k_typeMusic))
		{
			//printf( "BaseSoundManager::playEx soundId=%d\n", soundId );
			return -1;
		}

		int soundIndex = getReadySound(soundId);

		if(soundIndex != -1)
		{
			if (pos)
			{
				m_sounds[soundId][soundIndex]->SetPosition(pos);
			}
			m_sounds[soundId][soundIndex]->SetPitch(pitch);
			m_sounds[soundId][soundIndex]->setSoundEndCallback(callback);

			// scale the volume based on the type of sound
			if(getSoundType(soundId) & audio::k_typeMusic)
			{
				volume *= m_musicVolume;
			}
			else
			{
				volume *= m_sfxVolume; 
			}
			m_sounds[soundId][soundIndex]->Play(loop, volume, fadeInTime);
		}
		
		return soundIndex;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stop(int soundId, int soundIndex, int fadeOutTime)
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		if(soundIndex < m_sounds[soundId].size())
		{
			m_sounds[soundId][soundIndex]->Stop(fadeOutTime);
		}
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stop(int soundId, int fadeOutTime)
{	
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		for(int i = 0; i < m_sounds[soundId].size() ; i++)
		{
			m_sounds[soundId][i]->Stop(fadeOutTime);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stopAllSounds()
{
	if(m_isSoundInitialized)
	{
		for(int i = 0; i < m_numSounds; i++)
		{
			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				m_sounds[i][j]->Stop();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stopAllMusics()
{
	if(m_isSoundInitialized)
	{
		for(int i = 0; i < m_numSounds; i++)
		{
			if(getSoundType(i) & audio::k_typeMusic)
			{
				for(int j = 0; j < m_sounds[i].size(); j++)
				{
					m_sounds[i][j]->Stop();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stopAllSfx()
{
	if(m_isSoundInitialized)
	{
		for(int i = 0; i < m_numSounds; i++)
		{
			if((getSoundType(i) & audio::k_typeMusic) == 0 && (getSoundType(i) & audio::k_typeSecondMusic) == 0)
			{
				for(int j = 0; j < m_sounds[i].size(); j++)
				{
					m_sounds[i][j]->Stop();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::stopAllSecondMusics()
{
	if(m_isSoundInitialized)
	{
		for(int i = 0; i < m_numSounds; i++)
		{
			if(getSoundType(i) & audio::k_typeSecondMusic)
			{
				for(int j = 0; j < m_sounds[i].size(); j++)
				{
					m_sounds[i][j]->Stop();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setListenerPos(const float* pos, const float* orientation)
{
#ifdef USE_SOUND_LOCALIZATION
	// position
	alListenerfv(AL_POSITION, pos);
	LOG_OPENAL_ERROR;

	// orientation
	alListenerfv(AL_ORIENTATION, orientation);
	LOG_OPENAL_ERROR;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::update(int dt)
{
	if(m_isSoundInitialized)
	{
		// update any streaming audio
		for(int i = 0; i < m_numSounds; i++)
		{
			for(int j = 0; j < m_sounds[i].size() ; j++)
			{
				m_sounds[i][j]->Update(dt);
				if (m_sounds[i][j]->GetStatus() == CSound::kStopped)
				{
					int type = getSoundType(i);

					// delete sound instances
					if (j == m_sounds[i].size() - 1)
					{
						gll_delete m_sounds[i].back();
						m_sounds[i].pop_back();
					}

					if (!(type & audio::k_typeKeepDataLoaded))
					{
						if (m_sounds[i].size() == 0)
						{
							releaseSoundData(i);
						}
					}
				}
			}
		}
	}
	
	//m_checkOtherAudioPlayingTimer -= dt;
	//if (m_checkOtherAudioPlayingTimer < 0)
	//{
	//	m_checkOtherAudioPlayingTimer = 2000;
	//	bool wasPlaying = m_isOtherAudioPlaying;
	//	m_isOtherAudioPlaying = IsOtherAudioPlaying(false);
	//	if (!wasPlaying && m_isOtherAudioPlaying)
	//	{
	//		stopAllMusics();
	//	}
	//	//printf("Other audio playing = %s\n", m_isOtherAudioPlaying ? "true" : "false");
	//}
}


bool BaseSoundManager::IsOtherAudioPlaying (bool interrupt)
{
	#ifdef TARGET_OS_IPHONE
	{
		//printf("IsOtherAudioPlaying()\n");
		if( g_canUsePlayer )
		{
			m_isOtherAudioPlaying = iPodPlayerState();
		}
		else
		{
			return m_isOtherAudioPlaying;
		}
		return false;
	}
	#else
	{
		// On windows no other audio is playing :)
		return false;
	}	
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::getNumberOfAllocatedSounds()
{
	int count = 0;
	for(int i=0; i < m_numSounds; i++)
	{
		count += m_sounds[i].size();
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::getNumberOfAllocatedSoundsData()
{
	int count = 0;
	for(int i = 0; i < m_numSounds; i++)
	{
		if (m_soundsData[i])
		{
			++count;
		}
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::getNumberOfPlayingSounds()
{
	int count = 0;

	if(m_isSoundInitialized)
	{
		for(int i = 0; i < m_numSounds; i++)
		{
			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				if (m_sounds[i][j]->GetStatus() != CSound::kStopped)
				{
					++count;
				}
			}
		}
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::getReadySound(int soundId)
{
	int numSoundOfSameType = m_sounds[soundId].size();

	int soundIndex = -1;

	for(int i = 0; i < numSoundOfSameType; i++)
	{
		if(m_sounds[soundId][i]->GetStatus() == CSound::kStopped)
		{
			soundIndex = i;
			break;
		}
	}

	//If no same sound already loaded and not playing
	if(soundIndex == -1)
	{
		if(numSoundOfSameType < k_maxInstancesOfSameSound)
		{
			CSound* sound = 0;
			CSoundData* soundData = getSoundData(soundId);

			int type = getSoundType(soundId);

			if (type & audio::k_typeMemory)
			{
				sound = gll_new StaticSound(soundData);
			}
			else if (type & audio::k_typeStreamedFile)
			{
				sound = gll_new StreamingSound(soundData);
			}
			GLL_ASSERT(sound);

			m_sounds[soundId].push_back(sound);
			soundIndex = numSoundOfSameType;
		}
		//max reached, so stop and start first sound of same type 
		else
		{
			m_sounds[soundId][0]->Stop();
			soundIndex = 0;
		}
	}
	return soundIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundData* BaseSoundManager::getSoundData(int soundId)
{
	GLL_ASSERT(0 <= soundId && soundId < m_numSounds);

	// create if needed
	if (!m_soundsData[soundId])
	{
		int type	=	getSoundType(soundId);

		//---------------------------------------------------------------------------------
		#if USE_DECODER_MPC
		//---------------------------------------------------------------------------------
		if (type & audio::k_typeMPC)
		{
			CMpcStream stream;
			/*
			FILE* f = 0;

			if(!fileAbsPath || !fileAbsPath[0])
			{
				f = GetSoundFile(soundId);
				stream.Load(f, GetSoundFileSize(soundId));
			}
			else
			{
				stream.Load(fileAbsPath);
			}

			int rate = stream.GetSamplingRate();
			rate = 22050;

			#define EXPORTED_32KHz_ARE_22KHz 0
			#if EXPORTED_32KHz_ARE_22KHz
				if (rate == 32000)
				{
					rate = 22050;
				}
			#endif

			int channel = stream.GetChannel();
			int bufferSize = 0;
			if (type & k_typeMemory)
			{
				//MEM_COUNTER_NAMED(FileManager::GetName(resId));
				bufferSize = stream.GetPCMSize() * channel * 2;
				GLL_ASSERT(!m_soundsBuffers[soundId]);
				m_soundsBuffers[soundId] = gll_new audio::s8[bufferSize];
				int nSample = stream.Decode((short*)m_soundsBuffers[soundId], bufferSize/channel/2);
			}
			m_soundsData[soundId] = gll_new CSoundData(type, bufferSize, m_soundsBuffers[soundId], rate, channel, 16, soundId);

			if(f)
			{
				fclose(f);
			}*/
		}
		else
		#endif // USE_DECODER_MPC
		//---------------------------------------------------------------------------------


		//---------------------------------------------------------------------------------
		#if USE_DECODER_ADPCM
		//---------------------------------------------------------------------------------
		if (type & k_typeADPCM)
		{
			m_soundsData[soundId] = gll_new CSoundData(type, 0, 0, 22050, 2, 16, soundId);
		}
		else
		#endif //USE_DECODER_ADPCM
		//---------------------------------------------------------------------------------


		//---------------------------------------------------------------------------------
		#if USE_DECODER_MSADPCM
		//---------------------------------------------------------------------------------
		if (type & audio::k_typeWAV)
		{
			CSoundDecoderMSWAV stream(soundId);

			int  rate                = stream.GetSamplingRate();
			int  numChannel          = stream.GetChannel();
			int  bufferSize          = 0;
			char *buffer             = 0;
			int  nbEffectiveSamples  = 0;
			int  effectiveBufferSize = 0;

			if (type & audio::k_typeMemory)
			{				
				bufferSize	=	stream.GetPCMSize();
				buffer		=	gll_new char[bufferSize];

				m_sndBuffersSize += bufferSize;

				int numSamples = stream.Decode((short*)buffer, stream.GetTotalSamples(), false);

				// CODE : Get exact nb of samples and bytes in file, without zero padding (Robert Houde)				
				nbEffectiveSamples	=	stream.GetNbEffectiveSamples();
				effectiveBufferSize = (nbEffectiveSamples * numChannel) << 1; // <- ????

				// Interpolate between last file sample and a sample where decoded signal
			#ifdef ADPCM_START_INTERPOLATED
				int lenght = 20.0f * stream.GetSamplingRate() / 44100.0f;
				short* sbuffer = buffer;

				if(channel == 2)	// ALWAYS true right now
				{
					// Get the index of the samples taken as end points for interpolation.
					int leftIndex = (lenght << 1) - 2;
					int rightIndex = (lenght << 1) - 1;

					int totalEffectiveSamples = nbEffectiveSamples << 1;
					float stepLeft = (sbuffer[leftIndex] - sbuffer[totalEffectiveSamples - 2]) / lenght;
					float stepRight = (sbuffer[rightIndex] - sbuffer[totalEffectiveSamples - 1]) / lenght;

					float leftValue = sbuffer[totalEffectiveSamples - 2];
					float rightValue = sbuffer[totalEffectiveSamples - 1];

					for(int i=0; i<lenght-1; ++i)
					{
						sbuffer[2*i] = leftValue + (i+1) * stepLeft;
						sbuffer[2*i+1] = rightValue + (i+1) * stepRight;
					}
				}
			#endif //ADPCM_START_INTERPOLATED
				// End CODE (Robert Houde)
			}


			// CODE : If static playback, provide buffer size with exact number of samples in file,
			// without counting the zero padding samples (Robert Houde).		
			if(effectiveBufferSize > 0)
			{
				m_soundsData[soundId] = gll_new CSoundData(type, soundId, rate, numChannel, buffer, effectiveBufferSize);
			}
			else
			{
				m_soundsData[soundId] = gll_new CSoundData(type, soundId, rate, numChannel, buffer, bufferSize);
			}
			// End CODE (Robert Houde)
		}
		else
		#endif //USE_DECODER_MSADPCM
		//---------------------------------------------------------------------------------

		// end else
		{
		}
	}

	GLL_ASSERT(m_soundsData[soundId]);
	return m_soundsData[soundId];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::releaseSoundData(int soundId)
{
	GLL_ASSERT(0 <= soundId && soundId < m_numSounds);
	gll_delete m_soundsData[soundId];
	m_soundsData[soundId] = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BaseSoundManager::isSoundPlaying(int soundId, int soundIndex)
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		int numSoundOfSameType = m_sounds[soundId].size();

		if(soundIndex >= numSoundOfSameType)
		{
			return false;
		}

		return (m_sounds[soundId][soundIndex]->GetStatus() == CSound::kPlaying);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BaseSoundManager::playIfNotPlaying( int soundId, bool loop, int fadeInTime )
{
	if ( !isSoundPlaying(soundId) )
		return play(soundId, loop, fadeInTime);

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setSFxVolume(float volume)
{
	if(volume < 0.f)
	{
		m_sfxVolume = 0.f; 
	}
	else if(volume > 1.f)
	{
		m_sfxVolume = 1.f; 
	}
	else
	{
		m_sfxVolume = volume; 
	}

	if(!m_isSoundInitialized)
	{
		return;
	}
	
	for(int i = 0; i < m_numSounds; i++)
	{
		// if this is not a music track...
		if(0 == (getSoundType(i) & audio::k_typeMusic))
		{
			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				m_sounds[i][j]->SetVolume(m_sfxVolume);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setMusicVolume(float volume)
{
	if(volume < 0.f)
	{
		m_musicVolume = 0.f; 
	}
	else if(volume > 1.f)
	{
		m_musicVolume = 1.f; 
	}
	else
	{
		m_musicVolume = volume; 
	}

	if(!m_isSoundInitialized)
	{
		return;
	}

	for(int i = 0; i < m_numSounds; i++)
	{
		// if this is a music track...
		if(0 != (getSoundType(i) & audio::k_typeMusic))
		{
			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				m_sounds[i][j]->SetVolume(m_musicVolume);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setVolume(int soundId, int soundIndex, float volume)
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		if(m_sounds[soundId].size() > 0)
		{
			m_sounds[soundId][soundIndex]->SetVolume(volume);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setPitch(int soundId, int soundIndex, float pitch)
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		if(m_sounds[soundId].size() > 0)
		{
			m_sounds[soundId][soundIndex]->SetPitch(pitch);
		}		
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::setSourcePos(int soundId, int soundIndex, const float* pos)
{
	if( m_isSoundInitialized && IsSoundIdCorrect( soundId ) )
	{
		if(m_sounds[soundId].size() > soundIndex)
		{
			m_sounds[soundId][soundIndex]->SetPosition(pos);
		}		
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BaseSoundManager::s_bAudioSessionInitialized	=	false;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::initDevice()
{
	if(!s_bAudioSessionInitialized)
	{
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)

		//intialize a sesion for this queue
		AudioSessionInitialize(NULL, NULL, BaseSoundManager::interruptionListenerCallback, this);

		// before instantiating the playback audio queue object, 
		//	set the audio session category
		UInt32 sessionCategory = kAudioSessionCategory_AmbientSound;
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof (sessionCategory), &sessionCategory);
							 
		AudioSessionAddPropertyListener(kAudioSessionProperty_CurrentHardwareOutputVolume,
                            BaseSoundManager::volumeChangeCallback,
                            NULL);
		
		//int result = AudioSessionSetActive(true);
		//if (result)
		//{
		//	printf("Error setting audio session active! %d\n", result);
		//}
		
		/*AudioSessionAddPropertyListener(kAudioSessionProperty_CurrentHardwareSampleRate,
										sampleRateChangeCallback,
										NULL);
		
		AudioSessionAddPropertyListener(kAudioSessionProperty_CurrentHardwareInputVolume,
										inputVolumeChangeCallback,
										NULL);
		*/

#endif		
		s_bAudioSessionInitialized = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::interruptionListenerCallback(void *inUserData, UInt32 interruptionState)
{
//	printf("BaseSoundManager::interruptionListenerCallback(%p, %d)\n", inUserData, (int)interruptionState);

	BaseSoundManager* audioManager = (BaseSoundManager*)(inUserData);
	
	if( interruptionState == kAudioSessionBeginInterruption )
	{
		audioManager->beginInterrupt();
	}
	else if( interruptionState == kAudioSessionEndInterruption )
	{
		audioManager->endInterrupt();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::volumeChangeCallback (void * inClientData, AudioSessionPropertyID inID, UInt32 inDataSize, const void * inData)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::beginInterrupt()
{
	if( !m_isSoundInitialized )
		return;
	
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
	int result = AudioSessionSetActive( false );
	if( result )
	{
		//printf("Setting audio session active! %d\n", result);
	}
#endif		
	
	stopAllSounds();
	releaseAllSounds_UponInterrupt();
	
	g_OpenAlSourcesPool.DestroySources();
	OpenAL_Destroy();
	
	m_isSoundInitialized = false;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::endInterrupt()
{
	if( m_isSoundInitialized )
		return;

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
	int result = AudioSessionSetActive( true );
	if( result )
	{
		//printf("Setting audio session active! %d\n", result);
	}
#endif
	
	OpenAL_Initialize();
	
	g_OpenAlSourcesPool.GenerateSources();
	
	// check for other sound playing
	
	bool wasPlaying = m_isOtherAudioPlaying;
	m_isOtherAudioPlaying = IsOtherAudioPlaying(true);
	if( !wasPlaying && m_isOtherAudioPlaying )
	{
		stopAllMusics();
	}

	m_isSoundInitialized = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BaseSoundManager::releaseAllSounds_UponInterrupt()
{
	if(m_isSoundInitialized)
	{
		// update any streaming audio
		for(int i = 0; i < m_numSounds; i++)
		{
			int type = getSoundType(i);

			for(int j = 0; j < m_sounds[i].size(); j++)
			{
				if ((type & audio::k_typePreloadData) != 0)
				{
					if(m_sounds[i][j]->GetId())
						m_sounds[i][j]->Release();
				}

				gll_delete m_sounds[i][j];
			}

			if (!(type & audio::k_typeKeepDataLoaded))
			{
				if (m_sounds[i].size() != 0)
				{
					releaseSoundData(i);
				}
			}

			m_sounds[i].resize(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
