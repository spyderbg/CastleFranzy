#ifndef __STREAMINGSOUND_H__
#define __STREAMINGSOUND_H__

#include "AudioDefs.h"
#include "CSound.h"
#include "CSoundData.h"

class CMpcStream;
class ISoundDecoder;

////////////////////////////////////////////////////////////////////////
//*******************************************************************
//	class Sound:
//*******************************************************************
////////////////////////////////////////////////////////////////////////
class StreamingSound : public CSound
{
public:
	enum
	{
		NUM_DECODE_BUFFER = CSoundData::NUM_STREAMING_BUFFER + 1,
	};

public:
	StreamingSound(CSoundData* data);
	virtual ~StreamingSound();

	virtual void Play(bool loop, float volume, int fadeInTime = 0);
	virtual void Update(int dt);

	bool UpdateDecoding();
	void Flush(int bufferNo = -1);

	void SaturateBuffersAndFlush();

private:
	ISoundDecoder* m_soundDecoder;
	int m_decodeBufferSize;
	char* m_decodeBuffer[NUM_DECODE_BUFFER];
	int m_currentDecodeBuffer;
	int m_numSamplesPerDecode;
	int m_numDecodePerUpload;
	int m_currentDecode;
	int m_numSamplesInBuffer;
};

#endif //__STREAMINGSOUND_H__
