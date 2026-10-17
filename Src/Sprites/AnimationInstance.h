///////////////////////////////////////////////////////////////////////
//
//	Animation instance
//
///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IUpdatable.h"

#include "Sprites/ASprite.h"

///////////////////////////////////////////////////////////////////////

#define	SPRITE_INSTANCE_DEFAULT_FPS			25

#define	ANIM_INSTANCE_LOOP					(1)

#define	SPRITE_INSTANCE_FLIP_X				(1<<1)
#define	SPRITE_INSTANCE_FLIP_Y				(1<<2)
#define	SPRITE_INSTANCE_FLIP_XY				( SPRITE_INSTANCE_FLIP_X | SPRITE_INSTANCE_FLIP_Y )

#define ANIM_INSTANCE_OPPOSITE_DIRECTION	(1<<3 )

///////////////////////////////////////////////////////////////////////

class C3DSKeyFrameObject;

///////////////////////////////////////////////////////////////////////
/// CAnimation
///////////////////////////////////////////////////////////////////////

class CAnimationInstance : public IUpdatable
{
/// Config constants
private:
	static bool _skip_anim_timer;

public:
	CAnimationInstance( u16 flags=0 );
	virtual ~CAnimationInstance();

public:
	virtual void update( TimeStamp timestamp );
	virtual void Render() = 0;
	virtual void Update() = 0;

	/// Animation methods
	virtual s16	 GetCurrAnimIdx() { return m_AnimIdx; }
	virtual void StartAnimation( u16 anim_idx=0 ) = 0;
	virtual void StopAnimation() = 0;
	virtual void ResetAnimation() = 0;
	virtual bool IsAnimExists( u16 anim_idx ) { return ( anim_idx >= 0 ); }

	inline bool IsPlaying() { return m_Playing == true; }

	/// FPS
	virtual void SetFPS( u16 fps );
	inline	u16	 GetFPS() { return m_FPS; }

	/// Flags
	inline u16	GetFlags() { return m_Flag; }
	inline void SetFlags( u16 flags ) { m_Flag |= flags; }
	inline void UnsetFlags( u16 flags ) { m_Flag &= ~flags; }

	/// Position
	inline void SetPosition( s32 posX, s32 posY, s32 posZ ) { m_Pos.X = posX; m_Pos.Y = posY; m_Pos.Z = posZ; }
	inline void GetPosition( s32 &posX, s32 &posY, s32 &posZ ) { posX = m_Pos.X; posY = m_Pos.Y; posZ = m_Pos.Z; }	

protected:
	vector3di		m_Pos;
	u16				m_AnimIdx;
	u16				m_Flag;

	u16				m_FPS;
	TimeStamp		m_UpdateStep;
	TimeStamp		m_SystemTime;
	TimeStamp		m_LastUpdateTime;

	bool			m_Playing;
};

///////////////////////////////////////////////////////////////////////
/// CSpriteInstance
///////////////////////////////////////////////////////////////////////

class CSpriteInstance : public CAnimationInstance
{
	friend class CSpriteControlAnim; // fast fix in last moment

public:
	CSpriteInstance( ASprite* sprite, u16 flags=0 );
	CSpriteInstance( ASprite* sprite, s32 posX, s32 posY, s32 posZ, u16 flags=0 );
	~CSpriteInstance();

	void Update();
	void Render();

	void	SetDefaultFrame( u32 frame_idx );
	u32		GetDefaultFrame() { return m_DefaultFrameIdx; }

	inline  ASprite* GetSprite() { return m_Sprite; }

	/// Animation methods
	virtual			void StartAnimation( u16 anim_idx );
	virtual inline	void StopAnimation() { m_Playing = false; }
	virtual inline	void ResetAnimation() { m_PlayedFrames = 0; }
	virtual			bool IsAnimExists( u16 anim_idx );
					void ChangeAnimation( u16 anim );

	s32		GetPlayedFrames() { return m_PlayedFrames; }
	s32		GetLastPlayedFrameIdx() { return m_LastPlayedFrameIdx; }
	s32		GetFramesNum();
	void	SetLastFrame();

	inline void SetAlpha( u8 alpha ) { m_Alpha = alpha; }
	inline u8	GetAlpha() { return m_Alpha; }

private:
	ASprite*	m_Sprite;
	s32			m_PlayedFrames;
	s16			m_AFrame;
	u8			m_Alpha;
	bool		m_EndlessFrame;

	u32			m_DefaultFrameIdx;
	s32			m_LastPlayedFrameIdx;
};
