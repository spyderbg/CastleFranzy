////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"

#include "Common/Gestures.h"
#include "Game/Misc.h"

#include "Fonts/FontManager.h"

////////////////////////////////////////////////////////////////////////////////

CGesture::CGesture()
{
}

CGesture::~CGesture()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CGesture::Reset()
{
	m_time = 0.0f;
	m_input.set_used(0);
	m_actualInput.set_used(0);
}

void CGesture::Update(p2i point, f32 time)
{
	if (m_input.size() == 0)
	{
		m_time = 0.0f;

		SKey key = {point, m_time};
		m_input.push_back( key );
		m_actualInput.push_back( key );
		m_bbox.UpperLeftCorner = key.Point;
		m_bbox.LowerRightCorner = key.Point;
	}
	else
	{
		const v2f v1( m_input.getLast().Point.X, m_input.getLast().Point.Y );
		const v2f v2( point.X, point.Y );

		const int spacing = 20;
		const int len = (v2 - v1).getLength();
		//const int count = (len > 0) ? (len + (spacing - 1)) / spacing : 0;
		const int count = (len > 0) ? len / spacing : 0;

		for (int i = 0; i < count; i++)
		{
			const f32 k = (1.0f + i) / count;
			const v2f v = v1 + (i + 1) * spacing * (v2 - v1).normalize();//v2.getInterpolated(v1, k);
			const f32 t = m_time + time * k;

			SKey key = { p2i((int)v.X, (int)v.Y), t };
			m_input.push_back( key );
			m_bbox.addInternalPoint( key.Point );
		}

		if (count > 0)
		{
			SKey key = { point, m_time + time };
			m_actualInput.push_back( key );
		}
	}

	m_time += time;
}

void CGesture::Draw(int x, int y)
{
	IVideoDriver * driver = __IRR_DRIVER();

	int w = 40;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			SColor color = ((i + j) % 2) ? 0xFF404080 : 0xFF408040;
			driver->draw2DRectangle(color, r2i(p2i(x + i * w, y + j * w), d2i(w, w)));
		}
	}

	for (int i = 0; i < m_input.size(); i++)
	{
		for (int i = 0; i < m_input.size(); i++)
		{
			p2i p = m_input[i].Point;
			int dx = IntRemap3(p.X, m_bbox.UpperLeftCorner.X, m_bbox.LowerRightCorner.X, 0, 3 * w - 1);
			int dy = IntRemap3(p.Y, m_bbox.UpperLeftCorner.Y, m_bbox.LowerRightCorner.Y, 0, 3 * w - 1);

			SColor color = 0xFFFFFFFF;
			driver->draw2DRectangle(color, r2i(p2i(x + dx, y + dy), d2i(1, 1)));
		}
	}
}

void CGesture::DebugDraw()
{
	if (m_input.size() < 3)
		return;

	IVideoDriver * driver = __IRR_DRIVER();
	driver->draw2DRectangle(SColor(0x80000000), r2i(0,0,480,320));

	d2i d1(1, 1);
	d2i d2(2, 2);
	d2i d3(3, 3);
	d2i d5(5, 5);

	for (int i = 0; i < m_output2.size(); i++)
	{
		p2i p(m_output2[i].X, m_output2[i].Y);
		driver->draw2DRectangle(SColor(0xFF00FF00), r2i(p-d2, d5));
	}

	for (int i = 0; i < m_output.size(); i++)
	{
		p2i p(m_output[i].X, m_output[i].Y);
		driver->draw2DRectangle(SColor(0xFFFF0000), r2i(p-d1, d3));
	}

	for (int i = 0; i < m_input.size(); i++)
	{
		p2i p = m_input[i].Point;
		driver->draw2DRectangle(SColor(0xFFFFFFFF), r2i(p, d1));
	}

	const wchar_t * str;
	switch (m_lastGesture)
	{
	case ST_None:		str = L"None";		break;
	case ST_Boulder:	str = L"Boulder";	break;
	case ST_Lightning:	str = L"Lightning";	break;
	case ST_Tornado:	str = L"Tornado";	break;
	case ST_GhostArmy:	str = L"GhostArmy";	break;
	}

	__FONT("Arial")->DrawString(str, 480 - 10, 10, CFont::TOP | CFont::RIGHT);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool CGesture::MatchCircle()
{
	const int MIN_POINTS = 5;
	if (m_input.size() < MIN_POINTS)
		return false;

	const f32 MAX_DURATION = 3.0f;
	f32 duration = m_input.getLast().Time;
	if (duration > MAX_DURATION)
		return false;

	const int MIN_SIZE = 40;
	if (m_bbox.getHeight() < MIN_SIZE || m_bbox.getWidth() < MIN_SIZE)
		return false;

	const f32 MIN_ASPECT = 0.333f;
	f32 aspect = (f32)m_bbox.getWidth() / m_bbox.getHeight();
	if (aspect > 1.0f) aspect = 1.0f / aspect;
	if (aspect < MIN_ASPECT)
		return false;

	int grid[3][3] = { 0 };
	for (int i = 0; i < m_input.size(); i++)
	{
		p2i p = m_input[i].Point;
		int x = IntRemap3(p.X, m_bbox.UpperLeftCorner.X, m_bbox.LowerRightCorner.X, 0, 2);
		int y = IntRemap3(p.Y, m_bbox.UpperLeftCorner.Y, m_bbox.LowerRightCorner.Y, 0, 2);
		grid[x][y] = 1;
	}

	int center = grid[1][1];
	int edges = grid[1][0] + grid[0][1] + grid[2][1] + grid[1][2];
	int corners = grid[0][0] + grid[0][2] + grid[2][2] + grid[2][0];
	
	bool match = (center == 0 && edges == 4 && corners >= 3);

	if (match)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CGesture::smoothOutput(int n)
{
	while (m_output.size() > n)
	{
		float d_min = 1000000.0f;
		float p_min = 0;

		int p = 1;
		int i = 2;
		int last = m_output.size() - 1;

		for (; i != last; i++)
		{
			const float d = (m_output[i] - m_output[p]).getLengthSQ();
			
			if (d < d_min)
			{
				d_min = d;
				p_min = p;
			}
			p = i;
		}

		p = p_min;
		i = p_min + 1;

		v2f mid = (m_output[i] + m_output[p]) / 2;
		m_output[i] = mid;
		m_output.erase(p);
	}
}

void CGesture::calculateOutput()
{
	m_output.set_used(0);
	for (int i = 0; i < m_input.size(); i++)
	{
		m_output.push_back( v2f(m_input[i].Point.X, m_input[i].Point.Y) );
	}

	//smoothOutput(16);

	m_dir.set_used(0);
	for (int i = 1; i < m_output.size(); i++)
	{
		v2f v = m_output[i] - m_output[i-1];
		int dir = (int)(atan2f(-v.Y, v.X) * RADTODEG);
		m_dir.push_back( dir );
	}

	m_angle.set_used(0);
	for (int i = 1; i < m_dir.size(); i++)
	{
		int angle = m_dir[i] - m_dir[i-1];
		if (angle > 180) angle -= 360;
		if (angle <= -180) angle += 360;
		m_angle.push_back( angle );
	}
}

void CGesture::calculateOutput2()
{
	int len = 0;
	int sum = 0;
	int mid_a = 0;
	int mid_i = -1;

	m_output2 = m_output;

	for (int i = m_angle.size() - 1; i >= 0; i--)
	{
		int a = m_angle[i];

		len += 1;
		sum += a;

		if (abs(a) > mid_a)
		{
			mid_a = abs(a);
			mid_i = i;
		}

		const int treshold = 70;

		if (abs(sum) >= treshold && abs(a) < treshold)
		{
			int offset = mid_i - i;
			i += offset;
			len -= offset;
			a = treshold;
		}
		
		if (abs(a) >= treshold)
		{
			if (len >= 2)
			{
				int p = (i + 2);
				int n = (len - 1);
				m_output2.erase(p, n);
			}
			len = 0;
			sum = 0;
			mid_a = 0;
			mid_i = -1;
		}
	}

	if (len >= 1)
	{
		int p = 1;
		int n = len;
		m_output2.erase(p, n);
	}

	m_len2.set_used(0);
	m_dir2.set_used(0);
	for (int i = 1; i < m_output2.size(); i++)
	{
		v2f v = m_output2[i] - m_output2[i-1];
		int len = (int)(v.getLength());
		int dir = (int)(atan2f(-v.Y, v.X) * RADTODEG);
		m_len2.push_back( len );
		m_dir2.push_back( dir );
	}

	m_angle2.set_used(0);
	for (int i = 1; i < m_dir2.size(); i++)
	{
		int angle = m_dir2[i] - m_dir2[i-1];
		if (angle > 180) angle -= 360;
		if (angle <= -180) angle += 360;
		m_angle2.push_back( angle );
	}
}

////////////////////////////////////////////////////////////////////////////////

bool angleNear(int angle1, int angle2, int treshold)
{
	int angle = angle1 - angle2;
	if (angle > 180) angle -= 360;
	if (angle <= -180) angle += 360;
	return abs(angle) <= treshold;
}

ESpellType CGesture::MatchGesture()
{
	calculateOutput();
	calculateOutput2();

	m_lastGesture = ST_None;

	if (MatchBoulder()) m_lastGesture = ST_Boulder;
	if (MatchTornado())	m_lastGesture =  ST_Tornado;
	if (MatchLightning()) m_lastGesture = ST_Lightning;
	if (MatchGhostArmy()) m_lastGesture = ST_GhostArmy;

	return m_lastGesture;
}

bool CGesture::MatchBoulder()
{
	/// Match a circle
	return MatchCircle();
}

bool CGesture::MatchTornado()
{
	/// Match a 'V' letter
	if (m_dir2.size() != 2)
		return false;

	if (!angleNear(m_dir2[0], -70, 25))
		return false;
	if (!angleNear(m_angle2[0], 180-60, 40))
		return false;
	if (m_len2[0] < 30)
		return false;
	if (m_len2[1] < 30)
		return false;

	return true;
}

bool CGesture::MatchLightning()
{
	/// Match a jagged line with turns from 70 to 170 degrees
	if (m_angle2.size() < 2)
		return false;

	for (int i = 0; i < m_angle2.size(); i++)
	{
		int dir = (i % 2) == 0 ? m_angle2[0] : -m_angle2[0];
		dir = dir > 0 ? 1 : -1;

		if (!angleNear(m_angle2[i], dir * 120, 50)) return false;
		if (i < 3 && m_len2[i] < 30) return false;
	}

	return true;
}

bool CGesture::MatchGhostArmy()
{
	/// Match a line right
	if (m_dir2.size() == 1 && 
		angleNear(m_dir2[0], -20, 20) && m_len2[0] > 200 )
		return true;

	/// Match a line down then right
	if (m_dir2.size() == 2 && 
		angleNear(m_dir2[0], -70, 30) && m_len2[0] < 150 &&
		angleNear(m_dir2[1], -15, 20) && m_len2[1] > 150 )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
