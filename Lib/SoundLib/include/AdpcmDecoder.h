#ifndef __ADPCMDECODER_H__
#define __ADPCMDECODER_H__

#include "AdpcmState.h"

////////////////////////////////////////////////////////////////////////
//*******************************************************************
//	static class ADPCM DECODER
//*******************************************************************
////////////////////////////////////////////////////////////////////////
class AdpcmDecoder
{
public:
	enum
	{
		MTAG_MCAS			= (0x5341434D),
		MTAG_MCAW			= (0x5741434D),
		ADPCM_INDEX_NUM		= 89,
		ADPCM_NUM_INDEX_IN_TABLE = 16,
		STATIC_BUFFER_SIZE	= 1024,
	};

	static const char cAdpcmIndexTable[ ADPCM_NUM_INDEX_IN_TABLE ];
	static const short cAdpcmStepSizeTable[ ADPCM_INDEX_NUM ];

public:
	static inline short DecodeAdpcm( int code, AdpcmState* state );

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------*
Name:         DecodeAdpcm

Description:  Decode ADPCM

Arguments:    code - Value to be decoded
state - ADPCM state

Returns:      Decoded value
*---------------------------------------------------------------------------*/
short AdpcmDecoder::DecodeAdpcm( int code, AdpcmState* state )
{
	int step;
	int sample;
	int index;
	int d;

	sample = state->GetPrevSample();
	index  = state->GetPrevIndex();

	step = cAdpcmStepSizeTable[ index ];

	d = step >> 3;
	if ( code & 4 ) d += step;
	if ( code & 2 ) d += step >> 1;
	if ( code & 1 ) d += step >> 2;

	if ( code & 8 ) 
	{
		sample -= d;
		if ( sample < -32768 ) 
		{
			sample = -32768;
		}
	}
	else 
	{
		sample += d;
		
		if ( sample > 32767 ) 
		{
			sample = 32767;
		}
	}

	index += cAdpcmIndexTable[ code ];

	if ( index < 0 ) 
	{
		index = 0;
	}
	else if ( index > ADPCM_INDEX_NUM-1 ) 
	{
		index = ADPCM_INDEX_NUM-1;
	}

	state->SetPrevSample((short)sample);
	state->SetPrevIndex((unsigned char)index);

	return (short)sample;
}

#endif //__ADPCMDECODER_H__