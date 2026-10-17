#ifndef __ADPCMSTATE_H__
#define __ADPCMSTATE_H__

#pragma once
////////////////////////////////////////////////////////////////////////
//*******************************************************************
//	ADPCM STATE
//*******************************************************************
////////////////////////////////////////////////////////////////////////
class AdpcmState
{
private:
	short m_prevSample;
	unsigned char  m_prevIndex;
	unsigned char  _padding;

	AdpcmState(AdpcmState& state) {};

public:
	AdpcmState();
	~AdpcmState();

	inline void SetPrevSample(short sample);
	inline short GetPrevSample() const;

	inline void SetPrevIndex(unsigned char index);
	inline unsigned char GetPrevIndex() const;
};

////////////////////////////////////////////////////////////////////////
//*******************************************************************
//INLINE FUNCTIONS
//*******************************************************************
////////////////////////////////////////////////////////////////////////
void AdpcmState::SetPrevSample(short sample)
{
	m_prevSample = sample;
}

////////////////////////////////////////////////////////////////////////
short AdpcmState::GetPrevSample() const
{
	return m_prevSample;
}

////////////////////////////////////////////////////////////////////////
void AdpcmState::SetPrevIndex(unsigned char index)
{
	m_prevIndex = index;
}

////////////////////////////////////////////////////////////////////////
unsigned char AdpcmState::GetPrevIndex() const
{
	return m_prevIndex;
}

#endif //__ADPCMSTATE_H__