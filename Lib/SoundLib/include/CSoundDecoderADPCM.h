#ifndef __CSOUNDDECODERADPCM_H__
#define __CSOUNDDECODERADPCM_H__

#include "ISoundDecoder.h"
#include "stdio.h"

class AdpcmState;

class CSoundDecoderADPCM : public ISoundDecoder
{
public:
	CSoundDecoderADPCM(const char* filename);
	virtual ~CSoundDecoderADPCM();

	virtual int Decode(short* outbuf, int nbSample, bool loop);
	virtual void GoTo(int sample);

protected:
	AdpcmState*		m_state;
	FILE*			m_file;
	char*			m_readBuffer;
	int				m_readBufferSize;
};

#endif //__CSOUNDDECODERMPC_H__
