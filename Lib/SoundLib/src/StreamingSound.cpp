#include "StreamingSound.h"
#include "CSoundData.h"

#if USE_DECODER_MPC
#include "../../../lib/LibMPCDec/Sources/Stream/CMpcStream.h"
#include "CSoundDecoderMPC.h"
#endif //USE_DECODER_MPC

#if USE_DECODER_ADPCM
#include "CSoundDecoderADPCM.h"
#endif //USE_DECODER_ADPCM

#if USE_DECODER_WAV
#include "CSoundDecoderMSWAV.h"
#endif //USE_DECODER_MSADPCM

////////////////////////////////////////////////////////////////////////

using namespace gameloft;

////////////////////////////////////////////////////////////////////////

StreamingSound::StreamingSound(CSoundData* data):
	CSound(data),
	m_soundDecoder(0),
	m_decodeBufferSize(0),
	m_currentDecodeBuffer(0)
{
	GLL_ASSERT(data->GetType() & audio::k_typeStreamedFile); // make sure no un used buffer was allocated

#if USE_DECODER_ADPCM
	if (data->GetType() & audio::k_typeADPCM)
	{
		m_soundDecoder = gll_new CSoundDecoderADPCM(filePath);
	}
	else
#endif 
#ifdef USE_DECODER_WAV
	if (data->GetType() & audio::k_typeWAV)
	{
		m_soundDecoder = gll_new CSoundDecoderMSWAV(data->GetSoundId());
	}
#endif
	else
	{
		GLL_ASSERT(false);
	}

	m_numSamplesPerDecode = m_data->GetRate() / 30; // to have 1 decode per frame
	m_numSamplesPerDecode = m_soundDecoder->GetNumSamplesPerDecode(m_numSamplesPerDecode);
	m_numDecodePerUpload = 4;

	m_decodeBufferSize = m_numDecodePerUpload*m_numSamplesPerDecode*m_data->GetChannel()*CSoundData::k_bytesPerChannel;
	for(int i = 0; i < NUM_DECODE_BUFFER; ++i)
	{
		m_decodeBuffer[i] = gll_new char[m_decodeBufferSize];
	}

	m_status = kStopped;

	m_delayBeforeRemove = 500; // 0,5 sec
}

StreamingSound::~StreamingSound()
{
	gll_delete m_soundDecoder;
	for(int i = 0; i < NUM_DECODE_BUFFER; ++i)
	{
		gll_delete[] m_decodeBuffer[i];
	}
}


void StreamingSound::Play(bool loop, float volume, int fadeInTime)
{
	m_soundDecoder->GoTo(0);
	SaturateBuffersAndFlush();

	// disable loop on source, but do it manually
	CSound::Play(false, volume, fadeInTime);
	m_loop = loop;
}

bool StreamingSound::UpdateDecoding()
{
	if (m_currentDecode < m_numDecodePerUpload)
	{
		char* decodePtr = m_decodeBuffer[m_currentDecodeBuffer] + m_numSamplesInBuffer/*m_currentDecode * m_numSamplesPerDecode*/* m_data->GetChannel()*CSoundData::k_bytesPerChannel;
		m_numSamplesInBuffer += m_soundDecoder->Decode((short*)decodePtr, m_numSamplesPerDecode, m_loop);
		++m_currentDecode;
	}
	// returns true when buffer is full
	return (m_currentDecode == m_numDecodePerUpload);
}

void StreamingSound::Flush(int bufferNo)
{
	QueueStreamBuffer(m_decodeBuffer[m_currentDecodeBuffer], bufferNo, m_numSamplesInBuffer*m_data->GetChannel()*CSoundData::k_bytesPerChannel);
	m_currentDecode = 0;
	m_numSamplesInBuffer = 0;
	m_currentDecodeBuffer = (m_currentDecodeBuffer + 1) % NUM_DECODE_BUFFER;
}

void StreamingSound::SaturateBuffersAndFlush()
{
	// fill all buffers and upload
	m_currentDecode = 0;
	m_numSamplesInBuffer = 0;
	for(int bufferNo = 0; bufferNo < CSoundData::NUM_STREAMING_BUFFER; bufferNo++)
	{
		bool readyToUpload = false;
		while(!readyToUpload)
		{
			readyToUpload = UpdateDecoding();
		}
		Flush(bufferNo);
	}

}

void StreamingSound::Update(int dt)
{
	CSound::Update(dt);

	if (m_status != kPlaying)
	{
		return;
	}

	bool readyToUpload = UpdateDecoding();

	if (GetProcessedBuffer() > 0)
	{
		// finish filling the decode buffer before uploading
		while (!readyToUpload)
		{
			readyToUpload = UpdateDecoding();
		}
		Flush();
	}
}
