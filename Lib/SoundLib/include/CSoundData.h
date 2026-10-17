//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CSOUNDDATA_H__
#define __CSOUNDDATA_H__

#include "AudioDefs.h"

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#	include <OpenAL/al.h>
#else
#	include <al.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSoundData
{
public:
	enum
	{
		NUM_STREAMING_BUFFER	=	4,
		k_bytesPerChannel		=	2,
	};

public:
	CSoundData(int type, int soundId, int rate, int channel, char* buffer = 0, int bufferSize = 0);
	virtual ~CSoundData();

	int GetType()			const {return m_type;}
	int GetRate()			const {return m_rate;}
	int GetChannel()		const {return m_channel;}
	int GetSoundId()		const {return m_soundId;}
	int GetBufferCount()	const {return m_bufferCount;}

	ALuint GetBufferId(int index) const
	{
		GLL_ASSERT(0 <= index && index < m_bufferCount);
		return m_bufferIds[index];
	}

	static ALvoid UploadBufferData(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq, bool bStreaming);
	
protected:
	int m_type;
	int m_soundId;
	int m_rate;
	int m_channel;
	char* m_buffer;

	ALuint m_bufferIds[NUM_STREAMING_BUFFER];
	int m_bufferCount;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //__CSOUNDDATA_H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
