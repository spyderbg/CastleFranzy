#ifndef __STATICSOUND_H__
#define __STATICSOUND_H__

#include "CSound.h"

////////////////////////////////////////////////////////////////////////
//*******************************************************************
//	class Sound:
//*******************************************************************
////////////////////////////////////////////////////////////////////////
class StaticSound : public CSound
{
public:
	StaticSound(CSoundData* data);
	virtual ~StaticSound();

};

#endif //__STATICSOUND_H__
