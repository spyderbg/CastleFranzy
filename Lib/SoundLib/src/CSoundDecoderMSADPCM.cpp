//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AudioDefs.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_DECODER_MSADPCM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CSoundDecoderMSADPCM.h"
#include "AdpcmDecoder.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundDecoderMSADPCM::CSoundDecoderMSADPCM(ISoundFile *file, const RiffHeader& rh, const FormatHeader& formh)
: m_stateLeft()
, m_stateRight()
, m_file(file)
, m_readBuffer(0)
, m_readBufferSize(4096)
, m_curSampleInChunk(0)
, m_curByteInBlock(0)
, m_rh(rh)
, m_formh(formh)
, m_facth()
, m_dh()
, m_flags(k_needToProcessPreamble)
, m_totalDataBytesRead(0)
{
	GLL_ASSERT(m_file);
	m_readBuffer = gll_new char[m_readBufferSize];
	
	//fread(&m_rh, sizeof(RiffHeader), 1, m_file);
	//fread(&m_formh, sizeof(FormatHeader), 1, m_file);
	// skip over the extra formatting crap
	if(m_formh.chunkDataSize > 16)
	{
		m_file->seek(m_formh.chunkDataSize - 16, SEEK_CUR);
		//fseek(m_file, m_formh.chunkDataSize - 16, SEEK_CUR);
	}
	m_file->read(&m_facth, sizeof(FactHeader));
	m_file->read(&m_dh, sizeof(DataHeader));
	//fread(&m_facth, sizeof(FactHeader), 1, m_file);
	//fread(&m_dh, sizeof(DataHeader), 1, m_file);

	m_preambleSize = sizeof(AdpcmState)*m_formh.numChannels;

	// + 1 because the preamble counts as a sample
	// in stereo 1 byte == 1 sample...4 bits/sample in mono
	if(m_formh.numChannels == 1)
	{
		m_samplesPerBlock = ((m_formh.blockAlign - m_preambleSize)<<1) + 1;
	}
	else
	{
		m_samplesPerBlock = m_formh.blockAlign - m_preambleSize + 1;
	}

	int num_blocks = m_dh.chunkSize/m_formh.blockAlign;

	// compression ratio is 4:1
	m_pcmSize = (num_blocks * m_samplesPerBlock)<<2;
	// CODE : Exact number of samples in file, without counting zero padding (Robert Houde)
	m_effectiveNbSamples = m_facth.factData;
	// End CODE (Robert Houde)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSoundDecoderMSADPCM::~CSoundDecoderMSADPCM()
{
	gll_delete[] m_readBuffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*int CSoundDecoderMSADPCM::Decode(short* outbuf, int nbSample, bool loop)
{
	int targetReadLen = (nbSample/m_samplesPerBlock)*m_formh.blockAlign;

	// if we're asking for more bytes than our data chunk will allow...	
	if(targetReadLen + m_totalDataBytesRead >= m_dh.chunkSize)
	{
		targetReadLen = m_dh.chunkSize - m_totalDataBytesRead;
	}

	// figure out how many reads we need to complete the request
	int numReads = targetReadLen / m_readBufferSize;
	if(numReads * m_readBufferSize < targetReadLen)
	{ ++numReads; }

	//GLL_ASSERT(targetReadLen < m_readBufferSize);
	//int readLen = fread(m_readBuffer, 1, targetReadLen, m_file);
	//m_totalDataBytesRead += readLen;

	//int numBlocks = readLen/m_formh.blockAlign;
	int nbSampleDecoded = 0;//m_samplesPerBlock*numBlocks;

	
	short * dst = outbuf; // dst will be for left channel (or mono)
	short * dst2 = dst + 1; // dst2 is right channel

	short * begin = dst;
	// -----------------------------------------------------------------------
	// mono
	// -----------------------------------------------------------------------
	while( numReads -- )
	{
		// grab buffer sized chunks
		int curTargetReadLen = m_readBufferSize;
		if(m_readBufferSize > targetReadLen)
		{ curTargetReadLen = targetReadLen; }

		targetReadLen -= m_readBufferSize;
		int readLen = fread(m_readBuffer, 1, curTargetReadLen, m_file);
		m_totalDataBytesRead += readLen;

		char * c = m_readBuffer;
		
		int numBlocks = readLen/m_formh.blockAlign;
		nbSampleDecoded += m_samplesPerBlock*numBlocks;

		if(m_formh.numChannels == 1)
		{
			int cnt = numBlocks*(m_formh.blockAlign - m_preambleSize + 1);
			// we want to loop once/2 encoded samples and 1 for each preamble
			while ( cnt -- )
			{
				if(m_flags & k_needToProcessPreamble)
				{
					// reinit our state
					AdpcmState * curState = (AdpcmState*)c;
					m_stateLeft = *curState;

					*dst ++ = m_stateLeft.GetPrevSample();
					*dst = *(dst-1); ++dst; // MONO to STEREO

					// skip our read pointer over the block preamble
					c += m_preambleSize;

					m_curByteInBlock = m_preambleSize;

					m_flags &= ~k_needToProcessPreamble;
				}
				else
				{
					*dst ++ = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateLeft);
					*dst = *(dst-1); ++dst; // MONO TO STEREO

					*dst ++ = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateLeft);
					*dst = *(dst-1); ++dst; // MONO TO STEREO

					++c;

					// if we're done the current block
					if(++m_curByteInBlock >= m_formh.blockAlign)
					{
						m_flags |= k_needToProcessPreamble;
					}
				}
			}
		}
		// -----------------------------------------------------------------------
		// stereo
		// -----------------------------------------------------------------------
		else
		{
			int half_chunk = SAMPLES_PER_CHUNK>>1;
			int cnt = m_samplesPerBlock*numBlocks;
			while ( cnt -- )
			{
				if(m_flags & k_needToProcessPreamble)
				{
					// reinit our state
					AdpcmState * curState = (AdpcmState*)c;
					m_stateLeft = *curState;
					++curState;
					m_stateRight = *curState;

					*dst  = m_stateLeft.GetPrevSample();
					dst += 2;
					*dst2 = m_stateRight.GetPrevSample();
					dst2 += 2;

					// skip our read pointer over the block preambles
					c += m_preambleSize;

					m_curByteInBlock = m_preambleSize;

					m_flags &= ~k_needToProcessPreamble;
				}
				else
				{
					// left channel
					if(m_curSampleInChunk < half_chunk)
					{
						*dst = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateLeft);
						dst += 2;

						*dst = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateLeft);
						dst += 2;
					}
					// right channel
					else
					{
						*dst2 = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateRight);
						dst2 += 2;

						*dst2 = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateRight);
						dst2 += 2;
					}

					++c;

					++m_curSampleInChunk;
					++m_curByteInBlock;

					// if we're done the current chunk
					if(m_curSampleInChunk >= SAMPLES_PER_CHUNK)
					{ 
						m_curSampleInChunk = 0; 
					
						// if we're also done the current block
						if(m_curByteInBlock >= m_formh.blockAlign)
						{
							m_flags |= k_needToProcessPreamble;
						}
					}
				}
			}
		}
	}

	// if we've reached the end of our data go back to the beginning
	if (loop && nbSampleDecoded < nbSample)
	{
		GoTo(0);
	}

	return nbSampleDecoded;
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CODE : Version with looping bug fixed, streaming case (Robert Houde)
int CSoundDecoderMSADPCM::Decode(short* outbuf, int nbSample, bool loop)
{
	int targetReadLen = (nbSample/m_samplesPerBlock)*m_formh.blockAlign;

	// if we're asking for more bytes than our data chunk will allow...	
	if(targetReadLen + m_totalDataBytesRead >= m_dh.chunkSize)
	{
		targetReadLen = m_dh.chunkSize - m_totalDataBytesRead;
	}

	// figure out how many reads we need to complete the request
	int numReads = targetReadLen / m_readBufferSize;
	if(numReads * m_readBufferSize < targetReadLen)
	{ ++numReads; }

	//GLL_ASSERT(targetReadLen < m_readBufferSize);
	//int readLen = fread(m_readBuffer, 1, targetReadLen, m_file);
	//m_totalDataBytesRead += readLen;

	//int numBlocks = readLen/m_formh.blockAlign;
	int nbSampleDecoded = 0;//m_samplesPerBlock*numBlocks;
	
	short * dst = outbuf; // dst will be for left channel (or mono)
	short * dst2 = dst + 1; // dst2 is right channel

	short * begin = dst;
	// -----------------------------------------------------------------------
	// mono
	// -----------------------------------------------------------------------
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

		char * c = m_readBuffer;
		
		int numBlocks = readLen/m_formh.blockAlign;
		nbSampleDecoded += m_samplesPerBlock*numBlocks;

		if(m_formh.numChannels == 1)
		{
			int cnt = numBlocks*(m_formh.blockAlign - m_preambleSize + 1);
			// we want to loop once/2 encoded samples and 1 for each preamble
			while ( cnt -- )
			{
				if(m_flags & k_needToProcessPreamble)
				{
					// reinit our state
					AdpcmState * curState = (AdpcmState*)c;
					m_stateLeft = *curState;

					*dst ++ = m_stateLeft.GetPrevSample();
#ifdef FORCE_STEREO_AUDIO
					*dst = *(dst-1); ++dst; // MONO to STEREO
#endif

					// skip our read pointer over the block preamble
					c += m_preambleSize;

					m_curByteInBlock = m_preambleSize;

					m_flags &= ~k_needToProcessPreamble;
				}
				else
				{
					*dst ++ = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateLeft);
#ifdef FORCE_STEREO_AUDIO
					*dst = *(dst-1); ++dst; // MONO TO STEREO
#endif
					*dst ++ = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateLeft);
#ifdef FORCE_STEREO_AUDIO
					*dst = *(dst-1); ++dst; // MONO TO STEREO
#endif

					++c;

					// if we're done the current block
					if(++m_curByteInBlock >= m_formh.blockAlign)
					{
						m_flags |= k_needToProcessPreamble;
					}
				}
			}
		}
		// -----------------------------------------------------------------------
		// stereo
		// -----------------------------------------------------------------------
		else
		{
			int half_chunk = SAMPLES_PER_CHUNK>>1;
			int cnt = m_samplesPerBlock*numBlocks;
			while ( cnt -- )
			{
				if(m_flags & k_needToProcessPreamble)
				{
					// reinit our state
					AdpcmState * curState = (AdpcmState*)c;
					m_stateLeft = *curState;
					++curState;
					m_stateRight = *curState;

					*dst  = m_stateLeft.GetPrevSample();
					dst += 2;
					*dst2 = m_stateRight.GetPrevSample();
					dst2 += 2;

					// skip our read pointer over the block preambles
					c += m_preambleSize;

					m_curByteInBlock = m_preambleSize;

					m_flags &= ~k_needToProcessPreamble;
				}
				else
				{
					// left channel
					if(m_curSampleInChunk < half_chunk)
					{
						*dst = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateLeft);
						dst += 2;

						*dst = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateLeft);
						dst += 2;
					}
					// right channel
					else
					{
						*dst2 = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, &m_stateRight);
						dst2 += 2;

						*dst2 = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, &m_stateRight);
						dst2 += 2;
					}

					++c;

					++m_curSampleInChunk;
					++m_curByteInBlock;

					// if we're done the current chunk
					if(m_curSampleInChunk >= SAMPLES_PER_CHUNK)
					{ 
						m_curSampleInChunk = 0; 
					
						// if we're also done the current block
						if(m_curByteInBlock >= m_formh.blockAlign)
						{
							m_flags |= k_needToProcessPreamble;
						}
					}
				}
			}
		}
	}

	// CODE (Robert Houde)
	int nbEffectiveSamplesDecoded;

	// if we've reached the end of our data go back to the beginning
	if(loop && m_totalDataBytesRead == m_dh.chunkSize)
	{
		GoTo(0);
		// Calculate the number of effective samples in the last decoded block.
		// NOTE : This calculation is valid as long as nbSample is constant from call to call.
		nbEffectiveSamplesDecoded = m_facth.factData % nbSample;
		if(nbEffectiveSamplesDecoded == 0)
		{
			nbEffectiveSamplesDecoded = nbSampleDecoded;
		}
	}
	else
	{
		nbEffectiveSamplesDecoded = nbSampleDecoded;
	}
	return(nbEffectiveSamplesDecoded);
	// End CODE (Robert Houde)
	
}
// End CODE (Robert Houde)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSoundDecoderMSADPCM::GoTo(int sample)
{
	GLL_ASSERT(sample == 0);
	//fseek(m_file, sizeof(RiffHeader) + sizeof(FormatHeader) + (m_formh.chunkDataSize - 16) + sizeof(FactHeader) + sizeof(DataHeader), SEEK_SET);
	m_file->seek(sizeof(RiffHeader) + sizeof(FormatHeader) + (m_formh.chunkDataSize - 16) + sizeof(FactHeader) + sizeof(DataHeader), SEEK_SET);
	m_flags |= k_needToProcessPreamble;
	m_totalDataBytesRead = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CSoundDecoderMSADPCM::GetNumSamplesPerDecode(int neededSamplesPerDecode)
{
	//align the samples with a full block
	int mod = neededSamplesPerDecode % m_samplesPerBlock;
	if(mod)
	{
		neededSamplesPerDecode += (m_samplesPerBlock-mod);
	}

	GLL_ASSERT(neededSamplesPerDecode == m_samplesPerBlock * (neededSamplesPerDecode / m_samplesPerBlock));
	GLL_ASSERT(neededSamplesPerDecode >= m_samplesPerBlock);
	return neededSamplesPerDecode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //USE_DECODER_MSADPCM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
