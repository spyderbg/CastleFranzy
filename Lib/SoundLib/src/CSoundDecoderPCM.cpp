//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AudioDefs.h"

#if USE_DECODER_PCM

#include "CSoundDecoderPCM.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundDecoderPCM::CSoundDecoderPCM(ISoundFile *file, const RiffHeader& rh, const FormatHeader& formh)
: m_file(file)
, m_readBufferSize(4096)
, m_readBuffer(0)
, m_rh(rh)
, m_formh(formh)
, m_dh()
, m_totalDataBytesRead(0)
{
	GLL_ASSERT(m_file);
	m_readBuffer = gll_new char[m_readBufferSize];
	
	m_file->read(&m_dh, sizeof(DataHeader));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundDecoderPCM::~CSoundDecoderPCM()
{
	gll_delete[] m_readBuffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CSoundDecoderPCM::Decode(short* outbuf, int nbSample, bool loop)
{
	int targetReadLen = nbSample*m_formh.blockAlign;

	// if we're asking for more bytes than our data chunk will allow...	
	if(targetReadLen + m_totalDataBytesRead >= m_dh.chunkSize)
	{
		targetReadLen = m_dh.chunkSize - m_totalDataBytesRead;
	}

	// figure out how many reads we need to complete the request
	int numReads = targetReadLen / m_readBufferSize;
	if(numReads * m_readBufferSize < targetReadLen)
	{ ++numReads; }

	int nbSampleDecoded = 0;
	
	// -----------------------------------------------------------------------
	// stereo
	// -----------------------------------------------------------------------
#ifdef FORCE_STEREO_AUDIO
	if(m_formh.numChannels == 2)
#endif
	{
		// if this is stereo then just read directly into the output buffer
		//m_totalDataBytesRead = fread(outbuf, 1, targetReadLen, m_file);
		m_totalDataBytesRead = m_file->read(outbuf, targetReadLen);

		nbSampleDecoded = m_totalDataBytesRead/m_formh.blockAlign;
	}
	// -----------------------------------------------------------------------
	// mono
	// -----------------------------------------------------------------------
#ifdef FORCE_STEREO_AUDIO
	else
	{
		short * dst = outbuf; // dst will be for left channel
		short * dst2 = dst + 1; // dst2 is right channel
		while( numReads -- )
		{
			// grab buffer sized chunks
			int curTargetReadLen = m_readBufferSize;
			if(m_readBufferSize > targetReadLen)
			{ curTargetReadLen = targetReadLen; }

			targetReadLen -= m_readBufferSize;
			//int readLen = fread(m_readBuffer, 1, curTargetReadLen, m_file);
			int readLen = m_file->read(m_readBuffer, curTargetReadLen);
			m_totalDataBytesRead += readLen;

			short *sample = (short*)m_readBuffer;
			
			int numBlocks = readLen/m_formh.blockAlign;
			nbSampleDecoded += numBlocks;

			while( numBlocks -- )
			{
				*dst ++ = *sample ++;
				*dst = *(dst-1); ++dst; // MONO TO STEREO
			}
		}
	}
#endif
	
	// if we've reached the end of our data go back to the beginning
	if (loop && nbSampleDecoded < nbSample)
	{
		GoTo(0);
	}

	return nbSampleDecoded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSoundDecoderPCM::GoTo(int sample)
{
	GLL_ASSERT(sample == 0);

	m_file->seek(sizeof(RiffHeader) + sizeof(FormatHeader) + sizeof(DataHeader), SEEK_SET);
	m_totalDataBytesRead = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //USE_DECODER_PCM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
