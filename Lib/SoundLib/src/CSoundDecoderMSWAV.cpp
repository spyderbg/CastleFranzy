//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "CSoundDecoderMSWAV.h"
#include "CSoundDecoderPCM.h"
#include "CSoundDecoderMSADPCM.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundDecoderMSWAV::CSoundDecoderMSWAV(int soundID)
: m_file(0)
, m_rh()
, m_formh()
, decoder(0)
{
	m_file = CreateSoundFile(soundID);
	GLL_ASSERT(m_file);

	m_file->read(&m_rh, sizeof(RiffHeader));
	m_file->read(&m_formh, sizeof(FormatHeader));

	switch(m_formh.compressionCode)
	{
	case k_ccIMAADPCM:
		decoder = gll_new CSoundDecoderMSADPCM(m_file, m_rh, m_formh);
		break;
	case k_ccPCM:
		decoder = gll_new CSoundDecoderPCM(m_file, m_rh, m_formh);
		break;
	default:
		printf("CSoundDecoderMSWAV: SoundID: %d, asked to decompess unknown format: %i\n", soundID, m_formh.compressionCode);
		break;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundDecoderMSWAV::~CSoundDecoderMSWAV()
{
	if (decoder)
	{
		gll_delete decoder;
	}

	delete m_file;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSWAV::Decode(short* outbuf, int nbSample, bool loop)
{	
	return decoder ? decoder->Decode(outbuf, nbSample, loop) : 0;		
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundDecoderMSWAV::GoTo(int sample)
{
	if (decoder) 
		decoder->GoTo(sample);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMSWAV::GetNumSamplesPerDecode(int neededSamplesPerDecode)
{
	return decoder ? decoder->GetNumSamplesPerDecode(neededSamplesPerDecode) : 0;	
}
