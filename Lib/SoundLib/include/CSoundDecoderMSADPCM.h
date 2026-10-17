#ifndef __CSOUNDDECODERMSADPCM_H__
#define __CSOUNDDECODERMSADPCM_H__
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "ISoundDecoder.h"
#include "stdio.h"
#include "AdpcmState.h"
#include "MSHeaders.h"
#include "AudioDefs.h"
#include "ISoundFile.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#define SAMPLES_PER_CHUNK 8

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class CSoundDecoderMSADPCM : public ISoundDecoder
{
private:
	enum
	{
		k_needToProcessPreamble = 0x00000001,
	};

public:
	              CSoundDecoderMSADPCM(ISoundFile *file, const RiffHeader& rh, const FormatHeader& formh);
	virtual       ~CSoundDecoderMSADPCM();

	virtual int   Decode(short* outbuf, int nbSample, bool loop);
	virtual void  GoTo(int sample);
	int           GetNumSamplesPerDecode(int neededSamplesPerDecode);
	inline int    GetSamplingRate() const;
	inline int    GetChannel() const;
	inline int    GetPCMSize() const;
	inline int    GetNbEffectiveSamples() const;
	inline int    GetTotalSamples() const;


protected:
	AdpcmState          m_stateLeft;
	AdpcmState          m_stateRight;
	ISoundFile*         m_file;
	char*               m_readBuffer;
	int                 m_readBufferSize;
	unsigned int        m_curBlockRemaining;
	unsigned int        m_curSampleInChunk;
	unsigned int        m_curByteInBlock;
	const RiffHeader&   m_rh;
	const FormatHeader& m_formh;
	FactHeader          m_facth;
	DataHeader          m_dh;
	unsigned int        m_pcmSize;
	unsigned int        m_effectiveNbSamples;	// Exact nb of samples in file (without zero padding).
	unsigned int        m_samplesPerBlock;
	unsigned char       m_preambleSize;
	unsigned int        m_totalDataBytesRead;

	unsigned int        m_flags;
};
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSADPCM::GetSamplingRate() const
{
	return m_formh.sampleRate;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSADPCM::GetChannel() const
{
	#ifdef FORCE_STEREO_AUDIO
		return 2;
	#else
		return m_formh.numChannels;
	#endif
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSADPCM::GetPCMSize() const
{
	#ifdef FORCE_STEREO_AUDIO
		if(m_formh.numChannels == 1)
		{ 
			return (m_pcmSize << 1); 
		}
		else
		{
	#endif
			return m_pcmSize;
	#ifdef FORCE_STEREO_AUDIO
		}
	#endif
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSADPCM::GetNbEffectiveSamples() const
{
	return m_effectiveNbSamples;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSADPCM::GetTotalSamples() const
{
	return (m_dh.chunkSize/m_formh.blockAlign)*m_samplesPerBlock;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#endif //__CSOUNDDECODERMSADPCM_H__
