//#include "stdinc.h"
#include "AudioDefs.h"

#if USE_DECODER_ADPCM

#include "CSoundDecoderADPCM.h"
#include "AdpcmState.h"
#include "AdpcmDecoder.h"


struct SNDBinaryFileHeader
{
    char    signature[4];
    unsigned short     byteOrder;
    unsigned short     version;
    unsigned int     fileSize;
    unsigned short     headerSize;
    unsigned short     dataBlocks;
};

struct SNDBinaryBlockHeader
{
    unsigned int     kind;
    unsigned int     size;
};

struct SNDWaveParam
{
    unsigned char      format;                    // enum SNDWaveFormat
    unsigned char      loopflag;                  // boolean
    unsigned short     rate;                      // sampling rate
    unsigned short     timer;                     // = SND_TIMER_CLOCK / rate
    unsigned int     loopstart;                 // Loop-Start-Point [sample length]
    unsigned int     looplen;                   // Loop-Length [sample length]
};


CSoundDecoderADPCM::CSoundDecoderADPCM(const char* filename) :
	m_state(0),
	m_file(0),
	m_readBuffer(0),
	m_readBufferSize(4096)
{
	m_file = fopen(filename, "rb");
	GLL_ASSERT(m_file);
	m_readBuffer = gll_new char[m_readBufferSize];
	
	SNDBinaryFileHeader		fh;
	SNDBinaryBlockHeader	bh;
	SNDWaveParam			wp;
	
	fread(&fh, sizeof(SNDBinaryFileHeader), 1, m_file);
	fread(&bh, sizeof(SNDBinaryBlockHeader), 1, m_file);
	fread(&wp, sizeof(SNDWaveParam), 1, m_file);
}

CSoundDecoderADPCM::~CSoundDecoderADPCM()
{
	fclose(m_file);
	gll_delete[] m_readBuffer;
	gll_delete m_state;
}

int CSoundDecoderADPCM::Decode(short* outbuf, int nbSample, bool loop)
{
	if ( !m_state )
	{		
		m_state = gll_new AdpcmState();
		fread(m_state, sizeof(AdpcmState), 1, m_file);
	}

	int targetReadLen = nbSample/2; //  MONO TO STEREO: should be nbSample

	GLL_ASSERT(targetReadLen < m_readBufferSize);
	int readLen = fread(m_readBuffer, 1, targetReadLen, m_file);

	int nbSampleDecoded = readLen*2; //  MONO TO STEREO: should be readLen

	char * c = m_readBuffer;
	short * dst = outbuf;
	int cnt = readLen;

	while ( cnt -- )
	{
		*dst ++ = AdpcmDecoder::DecodeAdpcm(*c & 0x0f, m_state);
		*dst = *(dst-1); ++dst; // MONO TO STEREO

		*dst ++ = AdpcmDecoder::DecodeAdpcm((*c >> 4 ) & 0x0f, m_state);
		*dst = *(dst-1); ++dst; // MONO TO STEREO
		c ++;
	}

	if (loop && nbSampleDecoded < nbSample)
	{
		GoTo(0);
	}

	return nbSampleDecoded;
}

void CSoundDecoderADPCM::GoTo(int sample)
{
	GLL_ASSERT(sample == 0);
	fseek(m_file, sizeof(SNDBinaryFileHeader) + sizeof(SNDBinaryBlockHeader) + sizeof(SNDWaveParam), SEEK_SET);
	gll_delete m_state;
	m_state = 0;
}

#endif //USE_DECODER_ADPCM