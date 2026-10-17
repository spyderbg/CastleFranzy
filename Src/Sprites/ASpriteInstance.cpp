
#include "stdafx.h"

#include "Core/irrlicht.h"
#include "ASpriteInstance.h"
#include "ASprite.h"

ASpriteInstance::ASpriteInstance()
{
	x = 0;
	y = 0;
	m_nCurrentSprite = NULL;

	Stop();
}

ASpriteInstance::~ASpriteInstance()
{

}

void ASpriteInstance::Stop()
{
	m_nCurrentAnim = -1;
	m_nCurrentAFrame = 0;
	m_nCurrentFTime = 0;
	//m_nLoop = 0xFFFFFF;
	m_nNextAnim = -1;
}

void ASpriteInstance::SetAnim(int anim, int loop)
{
	Stop();

	m_nCurrentAnim = anim;
	m_nLoop = loop;
}

void ASpriteInstance::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

void ASpriteInstance::SetASprite(ASprite* sprite)
{
	m_nCurrentSprite = sprite;
}

ASprite* ASpriteInstance::GetASprite () const
{
	return m_nCurrentSprite;
}

void ASpriteInstance::StartAnim(ASprite* sprite, int anim, int x, int y, int loop)
{
	Stop();

	m_nCurrentSprite = sprite;
	m_nCurrentAnim = anim;
	this->x = x;
	this->y = y;
	m_nLoop = loop;
}

void ASpriteInstance::SetTransitionAnim(int anim, int nextAnim)
{
	SetAnim(anim);
	m_nNextAnim = nextAnim;
}

void ASpriteInstance::Update(float dt)
{
	if(m_nCurrentSprite == NULL || m_nCurrentAnim < 0)
		return;

	int time = m_nCurrentSprite->GetAFrameTime(m_nCurrentAnim, m_nCurrentAFrame);

	if(time == 0)
	{
		return;
	}

	m_nCurrentFTime += 25 * dt;

	while (time > 0 && m_nCurrentFTime >= time)
	{
		m_nCurrentFTime -= time;
		m_nCurrentAFrame++;

		if(m_nCurrentAFrame >= m_nCurrentSprite->GetAFrames(m_nCurrentAnim))
		{		
			if(m_nNextAnim >= 0)
			{
				SetAnim(m_nNextAnim);
				return;
			}
			else
			{
				m_nLoop--;
				if(m_nLoop < 0)
				{
					Stop();
					return;
				}
				else
				{
					m_nCurrentAFrame = 0;
					m_nNextAnim = -1;
				}
			}
		}

		time = m_nCurrentSprite->GetAFrameTime(m_nCurrentAnim, m_nCurrentAFrame);
	}
}

bool ASpriteInstance::IsAnimEnded()
{
	return (m_nLoop < 0) && m_nCurrentAFrame == 0 && m_nCurrentFTime == 0; 
}

void ASpriteInstance::Draw(int alpha)
{
	if(m_nCurrentSprite == NULL || m_nCurrentAnim < 0)
		return;
		
	m_nCurrentSprite->PaintAFrame(m_nCurrentAnim, m_nCurrentAFrame, x, y, 0, 0, 0, alpha);	
}

void ASpriteInstance::Draw3D(const irr::core::vector3df& pos, float scale)
{
	if(m_nCurrentSprite == NULL || m_nCurrentAnim < 0)
		return;
		
	m_nCurrentSprite->PaintAFrame3D(m_nCurrentAnim, m_nCurrentAFrame, pos, 0, 0, 0, scale * SPRITE_SCALE_3D, scale * SPRITE_SCALE_3D);
}