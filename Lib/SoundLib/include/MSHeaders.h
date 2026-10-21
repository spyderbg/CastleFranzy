#ifndef IO_AUDIO_MSHEADERS
#define IO_AUDIO_MSHEADERS

enum CompressionCode
{
	k_ccUnknown          = 0x0000,
	k_ccPCM              = 0x0001,
	k_ccMSADPCM          = 0x0002,
	k_ccITUG_711_A_LAW   = 0x0006,
	k_ccITUG_711_AMU_LAW = 0x0007,
	k_ccIMAADPCM         = 0x0011,
	k_ccITUG_723_ADPCM   = 0x0016,
	k_ccGSM_6_10         = 0x0031,
	k_ccITUG_721_ADPCM   = 0x0040,
	k_ccMPEG             = 0x0050,
	k_ccExperimental     = 0xFFFF,
};

struct RiffHeader
{
	char               chunkId[4];  // "RIFF"
	unsigned int	   filesize;    // total file size -8 bytes for this header
	char               riffType[4]; // usually "WAVE"
};

struct FormatHeader
{
	char               chunkId[4];          // "fmt "
	unsigned int       chunkDataSize;       // size of this header: 16 + extra format bytes
	unsigned short     compressionCode;     // 0x0001 = PCM, 0x0011 = IMA ADPCM 
	unsigned short     numChannels;
	unsigned int       sampleRate;
	unsigned int       avgBytesPerSample;
	unsigned short     blockAlign;
	unsigned short     significantBitsPerSample;
};

struct FactHeader
{
	char               chunkId[4];     // "fact"
	unsigned int       chunkDataSize;  // for IMA ADPCM this is the # of samples
	unsigned int       factData;
};

struct DataHeader
{
	char               chunkId[4]; // "data"
	unsigned int       chunkSize;  // data size including any padding
};

#endif /* IO_AUDIO_MSHEADERS */
