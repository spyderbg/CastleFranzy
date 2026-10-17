#ifndef __CSOUNDDECODERMPC_H__
#define __CSOUNDDECODERMPC_H__
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "ISoundDecoder.h"

class CMpcStream;
class ISoundFile;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class CSoundDecoderMPC : public ISoundDecoder
{
public:
	             //CSoundDecoderMPC(const char* filename, int realSamplingRate);
				 CSoundDecoderMPC(int soundID, int realSamplingRate);
	virtual      ~CSoundDecoderMPC();

	virtual int  Decode(short* outbuf, int nbSample, bool loop);
	virtual void GoTo(int sample);
	virtual int  GetNumSamplesPerDecode(int neededSamplesPerDecode);

protected:
	CMpcStream*  m_mpcStream;
	ISoundFile*  m_file;
};
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif //__CSOUNDDECODERMPC_H__
