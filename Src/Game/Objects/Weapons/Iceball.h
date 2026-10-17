#pragma once

#include "Core/irrlicht.h"

#include "Game/ColladaControl.h"
#include "Game/Config/Upgrades.h"
#include "Game/Objects/Weapons/WeaponObject.h"

#include "Sprites/ASpriteInstance.h"

class CTouchElement;

enum EIceballState
{
	IS_Respawning,
	IS_RespawningToReadyTransition,
	IS_Ready,
	IS_Dragged,
	IS_Dying,
	IS_Shop,
	IS_Intro,
	IS_Flying,
	IS_Disabled
};

class ASprite;
class CTouchElement;
class ParticleSystem;

class CIceball : public CWeaponObject
{
public:
	explicit CIceball();
	virtual ~CIceball();

	virtual EWeaponType	getType() { return WT_Iceball; }

	virtual void		update(f32 time);
	virtual void		draw();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		reset();
	virtual void		setShopState();
	virtual void		setIntroState();
	virtual void		setOldState();
	virtual void		setShopHighlight(f32 power);
	virtual f32			getCooldown();

	void				setSpawnPosition(ISceneNode * node);

	void				setState(EIceballState state);
	EIceballState		getState();

	bool				isDragable();
	void				startDrag(CTouchElement * touch);

	position2di			getScreenPosition();

	ASprite*			getSprite ();
	ASprite*			getSpriteGlow ();
	CTouchElement*		getTouch ();

	inline ISceneNode*	getPostament() { return m_postament; }

	void				elfArrowHit ();

	bool				isGroundSlippery (v3f pos);

	void				removeIce ();

	s32					PlaySfx(s32 soundId, bool loop = false, s32 fadeInTime = 0);

protected:
	virtual	void		adjustStats(IAttributes* attributes);

	float				m_stat_cooldown;
	float				m_stat_radius;
	int					m_stat_maxIcedUnits;
	float				m_stat_groundIceRadius;

private:
	void				explode ();
	void				calculateTail (v3f acceleration, v3f velocity);

	EIceballState		m_state;
	EIceballState		m_oldState;
	f32					m_stateTime;

	vector3df			m_spawnPosition;

	vector3df			m_velocity;
	vector3df			m_smoothAcceleration;
	vector3df			m_smoothVelocity;
	ISceneNode*			m_postament;
	ISceneNode*			m_shadow;
	ISceneNode*			m_light;
	ISceneNode*			m_groundIce;

	float				m_groundIceTimer;

	struct PosScale
	{
		PosScale (vector3df pos, float scale)
		{
			this->pos = pos;
			this->scale = scale;
		}
		vector3df pos;
		float scale;
	};
	array<PosScale>		m_drawItems;

	position2di			m_dragOffset;
	
	ASprite*			m_sprite;
	ASprite*			m_spriteGlow;

	ASpriteInstance		m_inst;
	ParticleSystem*		m_cometTail;
	CTouchElement*		m_touch;

	void				setShouldBeVisible(bool object, bool shadow, bool effect);
	bool				m_visible;
	bool				m_visibleObject;
	bool				m_visibleShadow;
	bool				m_visibleEffect;
};
