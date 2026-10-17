//include "stdinc.h"
#include "AudioDefs.h"
#include "ISoundFile.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#if USE_DECODER_MPC

#include "CSoundDecoderMPC.h"
#include "../../../lib/LibMPCDec/Sources/Stream/CMpcStream.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
/*CSoundDecoderMPC::CSoundDecoderMPC(const char* filename, int realSamplingRate)
{
	m_mpcStream = gll_new CMpcStream();
	m_mpcStream->Load(filename, realSamplingRate);
}*/


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundDecoderMPC::CSoundDecoderMPC( int soundID, int realSamplingRate )
{
	m_file = CreateSoundFile(soundID);
	GLL_ASSERT(m_file);

	m_mpcStream = gll_new CMpcStream();
	m_mpcStream->Load(, realSamplingRate);	
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundDecoderMPC::~CSoundDecoderMPC()
{
	gll_delete m_mpcStream;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMPC::Decode(short* outbuf, int nbSample, bool loop)
{
	return m_mpcStream->Decode(outbuf, nbSample, loop);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundDecoderMPC::GoTo(int sample)
{
	m_mpcStream->GoTo(sample);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundDecoderMPC::GetNumSamplesPerDecode(int neededSamplesPerDecode)
{
	// align m_numSamplesPerDecode on a multiple of MPC_FRAME_LENGTH
	int mod = neededSamplesPerDecode % MPC_FRAME_LENGTH;
	if (mod)
	{
		neededSamplesPerDecode += (MPC_FRAME_LENGTH-mod);
	}
	GLL_ASSERT(neededSamplesPerDecode == MPC_FRAME_LENGTH * (neededSamplesPerDecode / MPC_FRAME_LENGTH));
	GLL_ASSERT(neededSamplesPerDecode >= MPC_FRAME_LENGTH);
	return neededSamplesPerDecode;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#endif //USE_DECODER_ADPCM