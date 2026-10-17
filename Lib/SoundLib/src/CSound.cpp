//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CSound.h"
#include "CSoundData.h"
#include "BaseSoundManager.h"
#include "SourcesPool.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSound::CSound(CSoundData* data):
	m_data(data),
	m_status(kStopped),
	m_loop(false),
	m_sourceId(0),
	m_totalBuffersProcessed(0),
	m_fading(0),
	m_fadeTimeCurrent(0),
	m_fadeTimeTotal(0),
	m_soundEndCallback(0),
	m_delayBeforeRemove(0)
{
	//alGenSources(1,&m_sourceId);
	//LOG_OPENAL_ERROR;

	m_sourceId	=	g_OpenAlSourcesPool.GetNextAvailableSource();
	
	alSourceStop(m_sourceId);
	LOG_OPENAL_ERROR;
	alSourcei(m_sourceId,AL_BUFFER,0);
	LOG_OPENAL_ERROR;

#ifdef USE_SOUND_LOCALIZATION
	// setup 3d params
	// the distance at which the gain clamps to full volume
    alSourcef(m_sourceId, AL_REFERENCE_DISTANCE,	BaseSoundManager::k_AL_REFERENCE_DISTANCE);
	LOG_OPENAL_ERROR;
	// the distance at which rolloff begins to occur
    alSourcef(m_sourceId, AL_MAX_DISTANCE,			BaseSoundManager::k_AL_MAX_DISTANCE);
	LOG_OPENAL_ERROR;
	// the rolloff factor...bigger == quicker volume decay
    alSourcef(m_sourceId, AL_ROLLOFF_FACTOR,		BaseSoundManager::k_AL_ROLLOFF_FACTOR);
	LOG_OPENAL_ERROR;

	// by default, make all sounds appear as if they are coming from the listener
	// this will make them full volume and ignore panning
	alSourcei(m_sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
	LOG_OPENAL_ERROR;
	alSource3f(m_sourceId, AL_POSITION, 0.f, 0.f, 0.f);
	LOG_OPENAL_ERROR;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSound::~CSound()
{
	Release();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Release()
{
	if (!m_sourceId)
		return;

	alSourceStop(m_sourceId);
	LOG_OPENAL_ERROR;

	alSourcei(m_sourceId, AL_BUFFER, 0);
	LOG_OPENAL_ERROR;
	
	//alDeleteSources(1, &m_sourceId);
	//LOG_OPENAL_ERROR;

	g_OpenAlSourcesPool.ReleaseSource(m_sourceId);

	m_sourceId = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CSound::GetStatus() const
{
	return m_status;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Play(bool loop, float volume, int fadeInTime)
{
	//printf("%p->Play(%d, %f, %d)\n", this, (int)loop, volume, fadeInTime);

	m_loop = loop;
	alSourcei(m_sourceId, AL_LOOPING, m_loop ? AL_TRUE : AL_FALSE);
	LOG_OPENAL_ERROR;

	if (fadeInTime > 0)
	{
		m_fading          = 1;
		m_fadeTimeCurrent = 1;
		m_fadeTimeTotal   = fadeInTime;
	}

	m_volume = volume;
	alSourcef(m_sourceId, AL_GAIN, GetAbsoluteVolume());
	LOG_OPENAL_ERROR;

	alSourcePlay(m_sourceId);
	LOG_OPENAL_ERROR;

	m_status = kPlaying;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Stop(int fadeOutTime)
{
	//printf("%p->Stop(%d)\n", this, fadeOutTime);

	if (fadeOutTime > 0)
	{
		m_fading          = -1;
		m_fadeTimeCurrent = fadeOutTime;
		m_fadeTimeTotal   = fadeOutTime;
		return;
	}
	
	// m_sourceId should not be zero here
	if ( m_sourceId != 0 )
	{
		alSourceStop(m_sourceId);	
		LOG_OPENAL_ERROR;
	}
	
	if ( m_status != kStopped )
	{
		m_status = kStopping;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Pause()
{
	alSourcePause(m_sourceId);
	LOG_OPENAL_ERROR;
	m_status = kPaused;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Resume()
{
	alSourcePlay(m_sourceId);
	LOG_OPENAL_ERROR;
	m_status = kPlaying;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::SetPosition(const float* pos)
{
#ifdef USE_SOUND_LOCALIZATION
	alSourcei(m_sourceId, AL_SOURCE_RELATIVE, AL_FALSE);
	LOG_OPENAL_ERROR;
	alSourcefv(m_sourceId, AL_POSITION, pos);
	LOG_OPENAL_ERROR;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::Update(int dt)
{
	ALint sourceState;
	alGetSourcei(m_sourceId, AL_SOURCE_STATE, &sourceState);
	LOG_OPENAL_ERROR;

	if (m_status == kPlaying)
	{
		if(sourceState != AL_PLAYING)
		{
			Stop();
			return;
		}

		// fading
		if (m_fading)
		{
			m_fadeTimeCurrent += m_fading * dt;
			if (m_fading > 0)
			{
				// fading in
				if (m_fadeTimeCurrent >= m_fadeTimeTotal)
				{
					// finished fading in
					m_fading = 0;
				}
			}
			else
			{
				GLL_ASSERT(m_fading < 0);
				if (m_fadeTimeCurrent <= 0)
				{
					m_fading = 0;
					Stop();
				}
			}

			//printf("Fading: %d, fade time: %d, status: %d\n", m_fading, m_fadeTimeCurrent, m_status);

			if (m_status == kPlaying)
			{
				// update volume
				alSourcef(m_sourceId, AL_GAIN, GetAbsoluteVolume());
				LOG_OPENAL_ERROR;
			}
		}
	}
	else if (m_status == kStopping)
	{
		m_delayBeforeRemove -= dt;
		if(sourceState == AL_STOPPED && m_delayBeforeRemove < 0)
		{
			m_status = kStopped;
			invokeCallback();
		}
	}
	else if (m_status == kStopped)
	{
		GLL_ASSERT(sourceState == AL_STOPPED);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float CSound::GetAbsoluteVolume() const
{
	float absoluteVolume = m_volume * BaseSoundManager::getGlobalVolume();
	if (m_fading)
	{
		absoluteVolume = absoluteVolume * float(m_fadeTimeCurrent) / m_fadeTimeTotal;
	}
	//GLL_ASSERT(absoluteVolume != 0.0f); // should not be supported
	return absoluteVolume;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::QueueStreamBuffer(char* buffer, int bufferNo, int bufferSize)
{	
	ALint iState;
	
	ALuint uiBuffer = 0;
	
	if(bufferNo == -1)
	{
		alSourceUnqueueBuffers(m_sourceId, 1, &uiBuffer);
		LOG_OPENAL_ERROR;
		++m_totalBuffersProcessed;
	}
	else
	{
		uiBuffer = m_data->GetBufferId(bufferNo);
	}
	
	unsigned long ulFormat = (m_data->GetChannel() == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	CSoundData::UploadBufferData(uiBuffer, ulFormat, buffer, bufferSize, m_data->GetRate(), true);

	alSourceQueueBuffers(m_sourceId, 1, &uiBuffer);
	LOG_OPENAL_ERROR;
	
	alGetSourcei(m_sourceId, AL_SOURCE_STATE, &iState);
	LOG_OPENAL_ERROR;
	
	if(iState != AL_PLAYING && bufferNo == -1)
	{
		ALint iQueuedBuffers;
		alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		LOG_OPENAL_ERROR;
		if(iQueuedBuffers)
		{	
			alSourcePlay(m_sourceId);
			LOG_OPENAL_ERROR;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CSound::GetProcessedBuffer()
{
	ALint iBuffersProcessed;
	alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
	LOG_OPENAL_ERROR;
	return iBuffersProcessed;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::SetVolume(float volume)
{
	volume = volume * BaseSoundManager::getGlobalVolume();
	alSourcef(m_sourceId,AL_GAIN,volume);
	LOG_OPENAL_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSound::SetPitch(float pitch)
{
	alSourcef(m_sourceId,AL_PITCH, pitch);
	LOG_OPENAL_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
