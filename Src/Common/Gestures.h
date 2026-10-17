///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Common/Singleton.h"

#include "Game/Config/Upgrades.h"

///////////////////////////////////////////////////////////////////////

class CGesture : public CSingleton<CGesture>
{
private:
	friend class CSingleton<CGesture>;

	explicit CGesture();
	virtual ~CGesture();

public:
	void			Reset();
	void			Update(p2i point, f32 time);

	void			Draw(int x, int y);
	void			DebugDraw();

	struct SKey
	{
		p2i Point;
		f32 Time;
	};

	typedef array<SKey>	TKeys;

	const TKeys&	GetInput () const { return m_input; }

public:
	ESpellType		MatchGesture();

private:
	void			smoothOutput(int n);
	void			calculateOutput();
	void			calculateOutput2();

	bool			MatchCircle();
	bool			MatchBoulder();
	bool			MatchTornado();
	bool			MatchLightning();
	bool			MatchGhostArmy();

	typedef array<v2f>	TPoints;
	typedef array<int>	TInts;

	f32			m_time;
	r2i			m_bbox;
	TKeys		m_input;
	TKeys		m_actualInput;

	TPoints		m_output;
	TInts		m_dir;
	TInts		m_angle;

	TPoints		m_output2;
	TInts		m_len2;
	TInts		m_dir2;
	TInts		m_angle2;

	ESpellType	m_lastGesture;
};
