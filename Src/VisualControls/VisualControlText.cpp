#include "stdafx.h"

#include "VisualControlText.h"
#include "Common/StringTable.h"


VisualControlText::VisualControlText(s32 control_id) : 
	IVisualControl( -1 , control_id , -1 ), 
	m_align( 0 ), 
	m_disabledTextAlpha( 0 ), 
	m_useForcedColor( false ),
	m_forcedTextColor( 0xFFFFFFFF ), 
	m_max_width ( 480 ), 
	m_max_height ( 320 ), 
	m_textId( -1 )
{	
	IVisualControl::SetEnabledTE(false);
	SetSelected(false);
}

VisualControlText::~VisualControlText() 
{

}

void VisualControlText::Draw()
{
	if (!m_bVisible)
		return;

	irr::core::stringw text; 
	if ( m_textId != -1 ) 
		text = _TXT_INT( m_textId );
	else 
		text = m_text;

	if (text.size() == 0)
		return;

	/// Visibility check
	r2i visibleRect = r2i(0, 0, GAME_SCREEN_W, GAME_SCREEN_H);
	visibleRect.clipAgainst(GetTextArea());
	if (visibleRect.getWidth() == 0 || visibleRect.getHeight() == 0)
		return;

	s32 w = m_max_width;
	int alpha = m_transition_alpha;
	if ( !m_bEnabled ) alpha = ( alpha * m_disabledTextAlpha ) >> 8;
	CLAMP_VAL( alpha, 0, 255 );				

	s32 x = m_pos.X;
	s32 y = m_pos.Y;		
	
	ASprite::PushClipRect( m_clip );

	if ( !m_useForcedColor )
	{
		
		__FONT( m_font )->DrawWrappedText( text.c_str(), x, y, w, m_align, alpha );
	}
	else
	{
		SColor oldColor = __FONT( m_font )->GetColor1(); 
		__FONT( m_font )->SetColor1( m_forcedTextColor );

		__FONT( m_font )->DrawWrappedText( text.c_str(), x, y, w, m_align, alpha );

		__FONT( m_font )->SetColor1( oldColor );
	}
	ASprite::PopClipRect( );
}

int	VisualControlText::Update( float dt )
{
	m_frame_counter++;
	Transition_Update();

	if ( (!m_bVisible) ) 
		return -1;

	if ( Transition_IsGoing() ) 
		return -1;	

	return -1; 
}

void VisualControlText::RecalculateSize()
{
	irr::core::stringw text; 
	if ( m_textId != -1 ) 
		text = _TXT_INT( m_textId );
	else 
		text = m_text;

	if (text.size() == 0) 
	{
		SetSize(d2i(0, 0));
	}
	else if (m_font.size() == 0 ) 
	{
		SetSize(d2i(0, 0));
	}
	else
	{
		d2f size = __FONT( m_font )->GetStringSize( text.c_str() );
		SetSize( d2i((int)size.Width, (int)size.Height) );
	}
}

void VisualControlText::SetText(const stringw & text )
{ 
	m_text = text;
	RecalculateSize();
} 

void VisualControlText::SetText( int textId )
{ 
	m_textId = textId; 
	RecalculateSize();
}

stringw VisualControlText::GetText()
{ 	
	if ( m_textId != -1 ) 
		return  stringw( _TXT_INT( m_textId ) ) ;
	else 
		return m_text;	
}

void VisualControlText::SetFont( const stringw & fontName ) 
{ 
	m_font = fontName ; 
	RecalculateSize();
}

void VisualControlText::SetRegion(int mwidth, int mheight) 
{ 
	m_max_width = mwidth; 
	m_max_height = mheight; 
	RecalculateSize();
}

void VisualControlText::SetFontColor( u32 forced_color ) 
{ 
	m_useForcedColor = true; 
	m_forcedTextColor = forced_color; 
}

void VisualControlText::SetAlign( s32 align ) 
{
	GX_ASSERT(!!(align & CFont::LEFT) + !!(align & CFont::RIGHT) + !!(align & CFont::HCENTER) <= 1);
	GX_ASSERT(!!(align & CFont::TOP) + !!(align & CFont::BOTTOM) + !!(align & CFont::VCENTER) <= 1);
	m_align = align; 
}

r2i VisualControlText::GetTextArea()
{
	p2i pos = GetPos();
	d2i size = GetSize();

	if(m_align & CFont::HCENTER)	pos.X -= size.Width/2;
	if(m_align & CFont::VCENTER)	pos.Y -= size.Height/2;
	if(m_align & CFont::RIGHT)		pos.X -= size.Width;
	if(m_align & CFont::BOTTOM)		pos.Y -= size.Height;

	return r2i(pos, size);
}