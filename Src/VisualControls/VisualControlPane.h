//---------------------------------------------------------------------------------------------------------
// Copyright (C) 2009 Gameloft. All rights reserved.
//---------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "IVisualControl.h"


///////////////////////////////////////////////////////////////////////
/// VisualControlPane
///////////////////////////////////////////////////////////////////////

class VisualControlPane : public IVisualControl
{
public:
	static const irr::s32	CPT_AllowContMove	= 1;		// allow coninue moving ( as iPhone )
	static const irr::s32	CPT_AlignControls	= 1 << 1;	// align elments to pane border after sliding
	static const irr::s32	CPT_Vertical		= 1 << 2;
	static const irr::s32	CPT_Horizontal		= 1 << 3;
	static const irr::s32	CPT_Cycle			= 1 << 4;

protected:
	enum
	{
		CPS_WaitTouch,			// no touch
		CPS_Moving,				// touced and moving all controls
		CPS_Sliding,			// no touch but still moving
	} m_state;

public:
	VisualControlPane( s32 action_id, s32 click_sfx = -1 );
	~VisualControlPane();

			void	Draw() {}
			s32		Update( float dt );			// time in miliseconds
			void	Reset() {}

	/// Type
	inline	bool	CanChangeType() { return m_state == CPS_WaitTouch; }
			void	SetType( irr::s32 type );
			
	/// Controls
			void	AddControl( IVisualControl *ptr );
			void	RemoveControl( IVisualControl *ptr );			
			void	RemoveAllControls();
			void	ReturnToStartPos();

	/// Scrolling 
			void	SetScrollSize( d2i area );
			void	SetRows( int rows );
			void	SetCols( int cols );

			int		GetRows();
			int		GetCols();

			int		GetCurrentRow();
			int		GetCurrentCol();

			void	PushVertical(float rows, float dt);
			void	PushHorizontal(float cols, float dt);
			void	ScrollVertical(float rows);
			void	ScrollHorizontal(float cols);

			void	GoToRow(int row, bool instant = false);
			void	GoToCol(int col, bool instant = false);

			irr::core::array< IVisualControl * >	GetControls(){ return m_controls; }

protected:
			void	SetControlsPosition( bool check_bounds = true );

			void	StartMoving();
			void	StartSliding();

			void	UpdateMoving( float time );
			void	UpdateSliding( float time );

			void	constraintOffset( v2f & offset );
			void	constraintVelocity( f32 max );

			void	normalizeOffset ();

protected:
	irr::core::array< p2i >					m_startPos;
	irr::core::array< IVisualControl * >	m_controls;

private:
	irr::s32				m_type;

	/// dimention
	d2i						m_scrollSize;
	v2f						m_minOffset;
	v2f						m_maxOffset;

	/// moving / sliding
	v2f						m_offset;
	v2f						m_velocity;
	v2f						m_acceleration;
	v2f						m_autoVelocity;

	/// Snap
	v2f						m_snapOffset;
	int						m_rows;
	int						m_cols;
};

