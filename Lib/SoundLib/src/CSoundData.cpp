//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CSoundData.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace gameloft;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)

#include <OpenAL/alc.h>
#include <OpenAL/oalStaticBufferExtension.h>

ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	static	alBufferDataStaticProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    }
    
    if (proc)
        proc(bid, format, data, size, freq);
	
    return;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundData::CSoundData(int type, int soundId, int rate, int channel, char* buffer, int bufferSize) :
	m_type(type),
	m_soundId(soundId),
	m_rate(rate),
	m_channel(channel),
	m_buffer(buffer),
	m_bufferCount(0)
{
	if (m_type & audio::k_typeMemory)
	{
		GLL_ASSERT(m_buffer);
		m_bufferCount = 1;
		alGenBuffers(m_bufferCount, m_bufferIds);
		LOG_OPENAL_ERROR;

		// fill the OpenAL buffer immediatly
		unsigned long ulFormat = (m_channel == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		CSoundData::UploadBufferData(m_bufferIds[0], ulFormat, m_buffer, bufferSize, m_rate, false);
	}
	else if (m_type & audio::k_typeStreamedFile)
	{
		GLL_ASSERT(!m_buffer);
		m_bufferCount = NUM_STREAMING_BUFFER;
		alGenBuffers(m_bufferCount, m_bufferIds);
		LOG_OPENAL_ERROR;
	}
	else
	{
		GLL_ASSERT(!"Format not supported");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundData::~CSoundData()
{
	alDeleteBuffers(m_bufferCount, m_bufferIds);
	LOG_OPENAL_ERROR;

	if(m_buffer)
	{
		gll_delete [] m_buffer;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ALvoid CSoundData::UploadBufferData(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq, bool bStreaming)
{
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
	//if(bStreaming)
	//{
	//	alBufferData(bid, format, data, size, freq);
	//}
	//else
	{
		alBufferDataStaticProc(bid, format, data, size, freq);
	}
#else
	alBufferData(bid, format, data, size, freq);
#endif
	LOG_OPENAL_ERROR_STATIC;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////