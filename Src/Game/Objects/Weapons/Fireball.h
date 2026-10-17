#pragma once

#include "Core/irrlicht.h"

#include "Game/ColladaControl.h"
#include "Game/Config/Upgrades.h"
#include "Game/Objects/Weapons/WeaponObject.h"

#include "Sprites/ASpriteInstance.h"

class CTouchElement;

enum EFireballState
{
	FS_Respawning,
	FS_RespawningToReadyTransition,
	FS_Ready,
	FS_Dragged,
	FS_Dying,
	FS_Shop,
	FS_Flying,
	FS_Disabled
};

class ASprite;
class CTouchElement;
class ParticleSystem;

class CFireball : public CWeaponObject
{
public:
	explicit CFireball();
	virtual ~CFireball();

	virtual EWeaponType	getType() { return WT_Fireball; }

	virtual void		update(f32 time);
	virtual void		draw();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		reset();
	virtual void		setShopState();
	virtual void		setShopHighlight(f32 power);
	virtual f32			getCooldown();

	void				setSpawnPosition(ISceneNode * node);

	void				setState(EFireballState state);
	EFireballState		getState();

	bool				isDragable();
	void				startDrag(CTouchElement * touch);

	position2di			getScreenPosition();

	ASprite*			getSprite ();
	ASprite*			getSpriteGlow ();
	CTouchElement*		getTouch ();

	inline ISceneNode*	getPostament() { return m_postament; }

	void				elfArrowHit ();

	s32					PlaySfx(s32 soundId, bool loop = false, s32 fadeInTime = 0);

protected:
	virtual	void		adjustStats(IAttributes* attributes);

	float				m_stat_cooldown;
	float				m_stat_radius;
	int					m_stat_maxDamage;
	int					m_stat_maxBurn;
	int					m_stat_bounce;

private:
	void				explode ();
	void				calculateTail (v3f acceleration, v3f velocity);

	EFireballState		m_state;
	f32					m_stateTime;
	int					m_bounce;
	s32					m_bounceUnitsHit;
	vector3df			m_hit_normal;


	vector3df			m_spawnPosition;

	vector3df			m_velocity;
	vector3df			m_smoothAcceleration;
	vector3df			m_smoothVelocity;
	ISceneNode*			m_postament;
	ISceneNode*			m_shadow;
	ISceneNode*			m_light;
	ColladaControl		m_burnedGround;
	float				m_burnedGroundTimer;

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
