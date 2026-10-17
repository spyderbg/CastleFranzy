////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Sprite instance
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Sprites/ASprite.h"
#include "Sprites/AnimationInstance.h"

///////////////////////////////////////////////////////////////////////
/// CAnimationInstance
///////////////////////////////////////////////////////////////////////

bool CAnimationInstance::_skip_anim_timer = false;

CAnimationInstance::CAnimationInstance( u16 flags ) :
	m_SystemTime( 0.0 ),
	m_Flag( 0 ),
	m_Playing( false )
{
	m_Flag = flags;
}

CAnimationInstance::~CAnimationInstance()
{
}

///////////////////////////////////////////////////////////////////////

void CAnimationInstance::update( TimeStamp timestamp )
{
	/// Update timer
	m_SystemTime += timestamp;

	if( !_skip_anim_timer )
	{
		if( m_SystemTime < m_LastUpdateTime + m_UpdateStep )
			return;

		m_LastUpdateTime = m_SystemTime;
	}

	/// Actual update 
	Update();
}

///////////////////////////////////////////////////////////////////////

void CAnimationInstance::SetFPS( u16 fps )
{
	m_FPS = fps;
	/// Calculate step in milliseconds and use fixed point
	m_UpdateStep = ( (f32)1000 / fps );
}

///////////////////////////////////////////////////////////////////////
/// CSpriteInstance
///////////////////////////////////////////////////////////////////////

CSpriteInstance::CSpriteInstance( ASprite* sprite, u16 flags ) : CAnimationInstance( flags ),
	m_Sprite( sprite ),
	m_PlayedFrames( 0 ),
	m_AFrame( 0 ),
	m_Alpha( DEFAULT_FRAME_ALPHA ),
	m_EndlessFrame( false )
{
	SetFPS( SPRITE_INSTANCE_DEFAULT_FPS );
}

CSpriteInstance::CSpriteInstance( ASprite* sprite, s32 posX, s32 posY, s32 posZ, u16 flags ) : CAnimationInstance( flags ),
	m_Sprite( sprite ),
	m_PlayedFrames( 0 ),
	m_AFrame( 0 ),
	m_Alpha( DEFAULT_FRAME_ALPHA ),
	m_EndlessFrame( false )
{
	m_Flag = flags;

	m_Pos.X = posX;
	m_Pos.Y = posY;
	m_Pos.Z = posZ;

	m_DefaultFrameIdx		= 0;
	m_LastPlayedFrameIdx	= -1;

	SetFPS( SPRITE_INSTANCE_DEFAULT_FPS );
}

///////////////////////////////////////////////////////////////////////

CSpriteInstance::~CSpriteInstance()
{
}

///////////////////////////////////////////////////////////////////////

void CSpriteInstance::Update()
{	
	if( !m_Playing )
		return;

	if( m_Sprite == NULL )
	{
		DBG_ERROR( "Update: sprite not loaded" );
		return;
	}

	/// If no anims -> nothing to update
	if( m_Sprite->GetAnimsNum() == 0 )
	{
		m_Playing = false;
		return;
	}

	/// Endless AFrame
	if( (m_EndlessFrame == false) && (m_Sprite->GetAFrames( m_AnimIdx ) >= m_AFrame) && (m_Sprite->GetAFrameTime( m_AnimIdx, m_AFrame ) == 0) )
	{
		m_EndlessFrame = true;
		m_Playing = false;
		return;
	}

	/// Update 
	const s32 frames_num = GetFramesNum();

	if( m_Flag & ANIM_INSTANCE_OPPOSITE_DIRECTION ) 
	{
		if( --m_PlayedFrames <= 0 )
		{
			if( m_Flag & ANIM_INSTANCE_LOOP )
			{
				m_PlayedFrames = GetFramesNum();
			}
			else
			{
				m_Playing = false;
				m_PlayedFrames = 0;
			}
		}
	}
	else
	{
		if( ++m_PlayedFrames > frames_num ) 
		{
			if( m_Flag & ANIM_INSTANCE_LOOP )
			{
				m_PlayedFrames = 1;
			}
			else
			{
				m_Playing = false;
				m_PlayedFrames = frames_num;
			}
		}
	}

	GX_ASSERT( m_PlayedFrames >= 0 && m_PlayedFrames <= GetFramesNum() );

	/// Set appropriate frame
	m_AFrame = 0;
	s32 frames = m_Sprite->GetAFrameTime( m_AnimIdx, m_AFrame );
	while( m_PlayedFrames > frames )
	{
		if( (m_AFrame + 1) >= m_Sprite->GetAFrames( m_AnimIdx ) )
			break;			

		s32 frame_time = m_Sprite->GetAFrameTime( m_AnimIdx, ++m_AFrame );

		if( frame_time == 0 ) 
			frame_time = 1;			

		frames += frame_time;
	}

	//DBG_OUT( "%d	%d		%d\n", m_AFrame, m_PlayedFrames, (s32)(m_Flag & ANIM_INSTANCE_OPPOSITE_DIRECTION) );
}

void CSpriteInstance::Render()
{
	if( m_Sprite == NULL )
	{
		DBG_ERROR( "Render: sprite not loaded" );
		return;
	}

	if( m_Sprite->GetAnimsNum() == 0 || m_AnimIdx > m_Sprite->GetAnimsNum() )
	{
		m_LastPlayedFrameIdx = m_DefaultFrameIdx;

		m_Sprite->PaintFrame( m_DefaultFrameIdx, m_Pos.X, m_Pos.Y, 0 );
	}
	else if( m_AFrame >= 0 )
	{
		GX_ASSERT( m_AFrame < m_Sprite->GetAFrames( m_AnimIdx ) );

		m_LastPlayedFrameIdx = m_AFrame;

		m_Sprite->PaintAFrame( m_AnimIdx, m_AFrame, m_Pos.X, m_Pos.Y, m_Flag, 0, 0, m_Alpha );
	}
}

///////////////////////////////////////////////////////////////////////

void CSpriteInstance::SetDefaultFrame( u32 frame_idx )
{
	if( m_Sprite == NULL || m_Sprite->GetFramesNum() < frame_idx ) 
		return;

	m_DefaultFrameIdx = frame_idx;
}

///////////////////////////////////////////////////////////////////////

void CSpriteInstance::StartAnimation( u16 anim )
{
	if( !IsAnimExists( anim ) )
	{
		//ERROR_MESSAGE( "Animation doesn't exists IDX=%d", anim );
		m_AnimIdx = -1;
		return;
	}
		
	m_Playing = true;
	m_LastUpdateTime = 0;
	m_EndlessFrame = false;

	if( anim != m_AnimIdx )
	{
		m_AnimIdx = anim;
		m_PlayedFrames = 0;
		m_AFrame = 0;
	}
}

///////////////////////////////////////////////////////////////////////

void CSpriteInstance::ChangeAnimation( u16 anim )
{
	if( !IsAnimExists( anim ) )
	{
		//ERROR_MESSAGE( "Animation doesn't exists IDX=%d", anim );
		m_AnimIdx = -1;
		return;
	}
		
	if( anim == m_AnimIdx )
	{
		return;
	}

	if( m_Sprite == NULL )
	{
		DBG_ERROR( "Render: sprite not loaded" );
		return;
	}

	// change current animation only if the new one has exactly the same aframes
	int oldAFrames = m_Sprite->GetAFrames( m_AnimIdx );
	int newAFrames = m_Sprite->GetAFrames( anim );
	if( oldAFrames != newAFrames )
	{
		return;
	}

	for( int i = 0; i < oldAFrames; i++ )
	{
		int oldAFrameTime = m_Sprite->GetAFrameTime( m_AnimIdx, i );
		int newAFrameTime = m_Sprite->GetAFrameTime( anim     , i );
		if (oldAFrameTime != newAFrameTime)
		{
			return;
		}
	}

	m_AnimIdx = anim;
}

///////////////////////////////////////////////////////////////////////

bool CSpriteInstance::IsAnimExists( u16 anim_idx )
{ 
	if( m_Sprite == NULL || anim_idx >= m_Sprite->GetAnimsNum() )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////

s32	CSpriteInstance::GetFramesNum()
{
	if( !IsAnimExists( m_AnimIdx ) )
	{
		//ERROR_MESSAGE( "Animation doesn't exists IDX=%d", anim );
		m_AnimIdx = -1;
		return 0;
	}

	if( m_Sprite == NULL )
	{
		DBG_ERROR( "GetFramesNum: sprite not loaded" );
		return 0;
	}

	s32 frames_num = 0;
	s32 frames = m_Sprite->GetAFrames( m_AnimIdx );
	s32 frame_time = 0;
	while( frames > 0 )
	{
		frame_time = m_Sprite->GetAFrameTime( m_AnimIdx, frames-1 );

		if( frame_time == 0 ) 
			frame_time = 1;			

		frames_num += frame_time;

		frames--;
	}

	return frames_num;
}

///////////////////////////////////////////////////////////////////////

void CSpriteInstance::SetLastFrame()
{
	m_PlayedFrames = GetFramesNum();
}
