#include "stdafx.h"

#include "VisualControlContainer.h" 



VisualControlContainer::VisualControlContainer( ) : 
IVisualControl( -1,-1,-1 )
{
	IVisualControl::SetEnabledTE(false);
	SetSelected(false);
}

VisualControlContainer::~VisualControlContainer( )
{
	ReleaseControls();
}

void VisualControlContainer::ReleaseControls()
{
	for ( int i = 0; i< m_controls.size() ; i++ ) 
		SAFE_DEL( m_controls[i] ); 

	m_controls.clear();	
}

void VisualControlContainer::AddControl( IVisualControl *ptr )
{
	m_controls.push_back( ptr ); 
	m_startPos.push_back( ptr->GetPos() );
}

void VisualControlContainer::RemoveControl( IVisualControl *ptr )
{
	for ( int i = 0; i< m_controls.size() ; i++ ) 
		if ( m_controls[i] == ptr ) 
		{
			SAFE_DEL(  m_controls[i] );
			m_controls.erase( i ); 
			m_startPos.erase( i );
			return;
		}
}

void VisualControlContainer::Draw()
{
	if ( !m_bVisible ) 
		return;
	
	//ASprite::PushClipRect( GetArea() ) ;

	for ( int i = 0; i< m_controls.size() ; i++ ) 
		m_controls[i]->Draw();

	//ASprite::PopClipRect( );
}

int  VisualControlContainer::Update( float dt )
{
	if ( !m_bVisible ) 
		return -1;

	//do not set something touchable - not implemented
	for ( int i = 0; i< m_controls.size() ; i++ )
	{
		m_controls[i]->Update( dt );
	}

	return -1;
}

void VisualControlContainer::Reset()
{
	for ( int i = 0; i< m_controls.size() ; i++ )
		m_controls[i]->Reset();
}

void VisualControlContainer::SetArea( p2i pos, d2i size )
{
	IVisualControl::SetArea( pos, size );

	//for ( int i = 0; i< m_controls.size() ; i++ )
	//{
	//	m_controls[i]->SetArea( pos + m_startPos[i], size );
	//}
}

void VisualControlContainer::SetArea( int x, int y, int w, int h )
{
	IVisualControl::SetArea( x, y, w, h );

	//for ( int i = 0; i< m_controls.size() ; i++ )
	//{
	//	m_controls[i]->SetArea( x + m_startPos[i].X, y + m_startPos[i].Y, w, h );
	//}
}

void VisualControlContainer::SetArea( r2i area )
{
	IVisualControl::SetArea( area );
}	

void VisualControlContainer::SetPos( p2i pos )
{
	IVisualControl::SetPos( pos ); 
	//m_pos = pos;
	//m_TE.SetScreenCoordinates(m_pos.X - m_size.Width/2, m_pos.Y - m_size.Height/2, m_size.Width, m_size.Height);

	//apply to subcontrols
	for ( int i = 0; i< m_controls.size() ; i++ )
	{
		m_controls[i]->SetPos( pos + m_startPos[i] );
	}
}

//void VisualControlContainer::SetEnabledTE(bool is_enabled)
//{
//	for ( int i = 0; i< m_controls.size() ; i++ )
//		m_controls[i]->SetEnabledTE( is_enabled ) ;
//}

void VisualControlContainer::SetEnabled(bool is_enabled) 
{
	for ( int i = 0; i< m_controls.size() ; i++ )
		m_controls[i]->SetEnabled( is_enabled ) ;
}