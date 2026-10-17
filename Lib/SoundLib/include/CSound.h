#ifndef __CSOUND_H__
#define __CSOUND_H__

#include "AudioDefs.h"

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#	include <OpenAL/al.h>
#else
#	include <al.h>
#endif

using namespace gameloft;
class CSoundData;

class CSound
{
public:
	enum
	{
		kPlaying = 0,
		kPaused,
		kStopping,
		kStopped,
	};
	
public:
	CSound(CSoundData* data);
	virtual ~CSound();

	int GetStatus() const;

	void Release();

	virtual void Play(bool loop, float volume, int fadeInTime = 0);
	virtual void Stop(int fadeOutTime = 0);
	virtual void Pause();
	virtual void Resume();
	virtual void SetPosition(const float* pos);
	virtual void Update(int dt);

	virtual void SetVolume(float volume);
	virtual void SetPitch(float pitch);
	inline void setSoundEndCallback(void (*callback)());
	inline void invokeCallback();

	float GetAbsoluteVolume() const;
	void QueueStreamBuffer(char* buffer, int bufferNo, int bufferSize);
	int GetProcessedBuffer(void);

	int	GetId() { return m_sourceId; };

protected:
	CSoundData* m_data;
	bool m_loop;
	float m_volume;
	int m_status;
	ALuint m_sourceId;
	ALint m_totalBuffersProcessed;

	// fade in/out
	int m_fading;
	int m_fadeTimeCurrent;
	int m_fadeTimeTotal;

	void (*m_soundEndCallback)();

	int	m_delayBeforeRemove;
};

void
CSound::setSoundEndCallback(void (*callback)())
{ 
	m_soundEndCallback = callback; 
}

void
CSound::invokeCallback()
{	
	if(m_soundEndCallback)	
	{ m_soundEndCallback(); }
}

#endif //__CSOUND_H__
