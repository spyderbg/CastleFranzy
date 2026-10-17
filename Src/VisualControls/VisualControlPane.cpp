///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "VisualControlPane.h"

#include "Game/Misc.h"

#include <math.h>

///////////////////////////////////////////////////////////////////////

#define MASK_CHECK( x, mask )				( ( x & mask ) == mask )


///////////////////////////////////////////////////////////////////////
/// VisualControlPane
///////////////////////////////////////////////////////////////////////

VisualControlPane::VisualControlPane( s32 action_id, s32 click_sfx ) : IVisualControl( action_id, -1, click_sfx ), 
	m_state( CPS_WaitTouch )
{
	SetArea( 0, 0, GAME_SCREEN_W, GAME_SCREEN_H );
	SetType( CPT_AllowContMove | CPT_Vertical | CPT_AlignControls );

	m_rows = 1;
	m_cols = 1;
}


///////////////////////////////////////////////////////////////////////

VisualControlPane::~VisualControlPane()
{
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::AddControl( IVisualControl *ptr )
{
	m_startPos.push_back( ptr->GetPos() );
	m_controls.push_back( ptr );
}

void VisualControlPane::RemoveControl( IVisualControl *ptr )
{		
	for( s32 i=0; i<m_controls.size(); i++ )
	{
		if( m_controls[i] == ptr )
		{
			m_controls.erase( i );
			m_startPos.erase( i );
			break;
		}
	}
}

void VisualControlPane::RemoveAllControls() 
{
	m_controls.clear();
	m_startPos.clear();
}

void VisualControlPane::ReturnToStartPos()
{
	m_offset = v2f(0, 0);
	m_snapOffset = m_offset;
	m_velocity = v2f(0, 0);

	SetControlsPosition();
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::SetType( s32 type )
{
	if( CanChangeType() )
		m_type = type;
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::SetScrollSize( d2i size )
{
	m_scrollSize = size;

	r2i screenArea = GetArea();

	m_minOffset.X = -MAX(0, m_scrollSize.Width - screenArea.getWidth());
	m_maxOffset.X = 0;

	m_minOffset.Y = -MAX(0, m_scrollSize.Height - screenArea.getHeight());
	m_maxOffset.Y = 0;
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::SetControlsPosition( bool check_bounds )
{
	/// Check bounds
	if( check_bounds )
	{
		if( m_offset.X <= m_minOffset.X ) m_offset.X = m_minOffset.X;
		if( m_offset.X >= m_maxOffset.X ) m_offset.X = m_maxOffset.X;
		if( m_offset.Y <= m_minOffset.Y ) m_offset.Y = m_minOffset.Y;
		if( m_offset.Y >= m_maxOffset.Y ) m_offset.Y = m_maxOffset.Y;
	}

	r2i screenArea = GetArea();

	/// Set new positions
	if( MASK_CHECK(m_type, CPT_Cycle) )
	{
		for (s32 i = 0; i < m_controls.size(); i++)
		{		
			const p2i offset = p2i((int)m_offset.X, (int)m_offset.Y);
			p2i bla = m_startPos[i] + offset;
			if (bla.Y < -m_controls[i]->GetArea().getHeight())
				bla.Y += m_scrollSize.Height;
			if (bla.X < -m_controls[i]->GetArea().getWidth())
				bla.X += m_scrollSize.Width;
			const p2i pos = screenArea.UpperLeftCorner + bla;

			m_controls[i]->SetPos( pos );
		}
	}
	else
	{
		for (s32 i = 0; i < m_controls.size(); i++)
		{		
			const p2i offset = p2i((int)m_offset.X, (int)m_offset.Y);
			const p2i pos = screenArea.UpperLeftCorner + m_startPos[i] + offset;

			m_controls[i]->SetPos( pos );
		}
	}
}

///////////////////////////////////////////////////////////////////////

s32 VisualControlPane::Update( float dt )
{
	f32 time = dt / 1000;

	m_frame_counter++;

	Transition_Update();

	if( !m_bVisible || Transition_IsGoing() )
		return -1;

	if (m_TE.IsActive() )
	{
		if (m_TE.IsNowActivated())
			m_TE.DisableAllOtherElements(true);

		StartMoving();
	}
	else if( m_TE.IsNowReleased() )
	{
		StartSliding();

		m_TE.DisableAllOtherElements(false);
		m_TE.ResetInput();
	}

	switch (m_state)
	{
	case CPS_WaitTouch:
		break;

	case CPS_Moving:
		UpdateMoving( time );
		break;

	case CPS_Sliding:
		UpdateSliding( time );
		break;
	}

	normalizeOffset();

	return (m_is_selected) ? m_action_id : -1;
}

void VisualControlPane::UpdateMoving( float time )
{
	int dx, dy;
	m_TE.GetTouchPos_DT( dx, dy );
	v2f offset = v2f(dx, dy);

	constraintOffset(offset);
	m_offset += offset;
	m_snapOffset = m_offset;

	m_velocity = offset / time;

	bool checkBounds = !MASK_CHECK(m_type, CPT_AllowContMove);
	SetControlsPosition( checkBounds );
}

void VisualControlPane::StartMoving()
{
	/** /
	CLAMP(m_offset.X, m_minOffset.X, m_maxOffset.X);
	CLAMP(m_offset.Y, m_minOffset.Y, m_maxOffset.Y);
	m_snapOffset = m_offset;
	/**/

	m_state = CPS_Moving;
}

void VisualControlPane::StartSliding()
{
	const f32 maxVelocity = 2 * GAME_SCREEN_H; // 2 screens per second
	constraintVelocity( maxVelocity );

	m_snapOffset = m_offset + m_velocity / 3;
	if (!MASK_CHECK(m_type, CPT_Cycle))
	{
		CLAMP(m_snapOffset.X, m_minOffset.X, m_maxOffset.X);
		CLAMP(m_snapOffset.Y, m_minOffset.Y, m_maxOffset.Y);
	}

	if (MASK_CHECK(m_type, CPT_AlignControls))
	{
		f32 colWidth = (float)m_scrollSize.Width / m_cols;
		f32 rowHeight = (float)m_scrollSize.Height / m_rows;

		f32 col = floorf(m_snapOffset.X / colWidth + 0.5f);
		f32 row = floorf(m_snapOffset.Y / rowHeight + 0.5f);

		m_snapOffset.X = colWidth * col;
		m_snapOffset.Y = rowHeight * row;
	}

	m_state = CPS_Sliding;
}

void VisualControlPane::UpdateSliding( float time )
{
	float d = 10.0f;
	float k = 30.0f;

	v2f dist = m_offset - m_snapOffset;
	m_acceleration = -k * dist - d * m_velocity;
	m_velocity += m_acceleration * time;
	m_offset += m_velocity * time;

	if (dist.getLength() <= 1.0f && m_velocity.getLength() < 30.0f)
	{
		m_offset = m_snapOffset;
		m_state = CPS_WaitTouch;
	}

	SetControlsPosition(false);
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::constraintOffset( v2f & offset )
{
	if (MASK_CHECK(m_type, CPT_Horizontal))
		offset.Y = 0;
	if (MASK_CHECK(m_type, CPT_Vertical))
		offset.X = 0;
	if (MASK_CHECK(m_type, CPT_Cycle))
		return;

	if (m_offset.Y <= m_minOffset.Y || m_offset.Y >= m_maxOffset.Y)
		offset.Y *= 0.5f;
	else if (m_offset.X <= m_minOffset.X || m_offset.X >= m_maxOffset.X)
		offset.X *= 0.5f;
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::constraintVelocity( f32 max )
{
	f32 len = m_velocity.getLength();
	if (len > max)
		m_velocity *= max / len;
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::normalizeOffset ()
{
	if (MASK_CHECK(m_type, CPT_Cycle))
	{
		float width = m_scrollSize.Width;
		float height = m_scrollSize.Height;
		if (width > 0.0f)
		{
			while (m_offset.X < -width)
			{
				m_offset.X += width;
				m_snapOffset.X += width;
			}
			while (m_offset.X > 0)
			{
				m_offset.X -= width;
				m_snapOffset.X -= width;
			}
		}
		if (height > 0.0f)
		{
			while (m_offset.Y < -height)
			{
				m_offset.Y += height;
				m_snapOffset.Y += height;
			}
			while (m_offset.Y > 0)
			{
				m_offset.Y -= height;
				m_snapOffset.Y -= height;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::SetRows( int rows )
{
	m_rows = rows;
}

void VisualControlPane::SetCols( int cols )
{
	m_cols = cols;
}

///////////////////////////////////////////////////////////////////////

int VisualControlPane::GetRows()
{
	return m_rows;
}

int VisualControlPane::GetCols()
{
	return m_cols;
}

int	VisualControlPane::GetCurrentRow()
{
	f32 rowHeight = (float)m_scrollSize.Height / m_rows;
	f32 row = floorf(m_snapOffset.Y / rowHeight + 0.5f);

	return (int)-row;
}

int	VisualControlPane::GetCurrentCol()
{
	f32 colWidth = (float)m_scrollSize.Width / m_cols;
	f32 col = floorf(m_snapOffset.X / colWidth + 0.5f);

	return (int)-col;
}

///////////////////////////////////////////////////////////////////////

void VisualControlPane::PushVertical(float rows, float dt)
{
	float rowHeight = MASK_CHECK(m_type, CPT_AlignControls) ? m_scrollSize.Height / m_rows : 30;
	float power = -12.0f * rows * rowHeight;
	float friction = 4.0f;
	float accel = power - friction * m_velocity.Y;
	m_velocity.Y += accel * dt;

	StartSliding();
}

void VisualControlPane::PushHorizontal(float cols, float dt)
{
	float colWidth = MASK_CHECK(m_type, CPT_AlignControls) ? m_scrollSize.Width / m_cols : 30;
	float power = -12.0f * cols * colWidth;
	float friction = 4.0f;
	float accel = power - friction * m_velocity.X;
	m_velocity.X += accel * dt;

	StartSliding();
}

void VisualControlPane::ScrollVertical(float rows)
{
	if (MASK_CHECK(m_type, CPT_AlignControls))
		m_velocity.Y = -3.0f * rows * m_scrollSize.Height / m_rows;
	else
		m_velocity.Y = -3.0f * 30 * rows;

	StartSliding();
}

void VisualControlPane::ScrollHorizontal(float cols)
{
	if (MASK_CHECK(m_type, CPT_AlignControls))
		m_velocity.X = -3.0f * cols * m_scrollSize.Width / m_cols;
	else
		m_velocity.X = -3.0f * 30 * cols;

	StartSliding();
}

void VisualControlPane::GoToRow(int row, bool instant)
{
	GX_ASSERT( MASK_CHECK(m_type, CPT_AlignControls) );

	f32 rowHeight = (float)m_scrollSize.Height / m_rows;
	if (!instant)
	{	
		m_snapOffset.Y = rowHeight * -row;
		m_state = CPS_Sliding;
	}
	else
	{
		m_offset.Y = rowHeight * -row;
		m_state = CPS_WaitTouch;
	}
}
			
void VisualControlPane::GoToCol(int col, bool instant)
{
	f32 colWidth = (float)m_scrollSize.Width / m_cols;
	if (!instant)
	{
		m_snapOffset.X = colWidth * -col;
		m_state = CPS_Sliding;
	}
	else
	{
		m_offset.X = colWidth * -col;
		m_state = CPS_WaitTouch;
	}
}

///////////////////////////////////////////////////////////////////////