#pragma once

#include "Game/ColladaControl.h"
#include "Game/Config/Upgrades.h"
#include "Game/Objects/Weapons/WeaponObject.h"
#include "IO/TouchScreen/TouchPad.h"

enum ESpellState
{
	SS_Disabled,
	SS_Ready,
	SS_Casting,
	SS_Active,
	SS_Cooldown,
};

class CTouchElement;
class ParticleSystem;

class ISpellBehaviour
{
public:
	virtual ~ISpellBehaviour () { }

	virtual void activate (p2i gestureEnd) = 0;

	// returns true if the spell is still active and false otherwise
	virtual bool update (f32 time) = 0;
	virtual void draw2D () { }
	virtual void draw3D () { }
	virtual void adjustStats (IAttributes* attr) { }
};

class CBoulderBehaviour : public ISpellBehaviour
{
public:
	CBoulderBehaviour ();
	~CBoulderBehaviour ();

	void				activate (p2i gestureEnd);
	bool				update (f32 time);

	void				adjustStats (IAttributes* attr);

private:
	f32					m_radius;
	f32					m_duration;

	ColladaControl		m_boulder;
	ISceneNode*			m_shadow;
	bool				m_falling;
	bool				m_breaking;
	f32					m_speed;
	f32					m_acceleration;
	f32					m_time;
	s32					m_soundIndex;
};

class CLightningBehaviour : public ISpellBehaviour
{
public:
	CLightningBehaviour ();
	~CLightningBehaviour ();

	void				activate (p2i gestureEnd);
	bool				update (f32 time);
	void				draw2D ();

	void				adjustStats (IAttributes* attr);

private:
	ASpriteInstance		m_sprite;
	ColladaControl		m_wave;
};

class CTornadoBehaviour : public ISpellBehaviour
{
public:
	CTornadoBehaviour ();
	~CTornadoBehaviour ();

	void				activate (p2i gestureEnd);
	bool				update (f32 time);

	void				adjustStats (IAttributes* attr);

private:
	void				affectUnit (f32 time, CUnit* unit, bool continueSpell, int* unitsHitByKind);
	void				affectBomb (f32 time, CBomb* unit, bool continueSpell);

	CTouchElement		m_touch;
	ColladaControl		m_tornado;
	float				m_time;
	ParticleSystem*		m_dust;
	ISceneNode*			m_bottom;

	float				m_speed;
	float				m_radius;
	float				m_maxTime;

	s32					m_soundIndex;
};

class CGhostArmyBehaviour : public ISpellBehaviour
{
public:
	CGhostArmyBehaviour ();
	~CGhostArmyBehaviour ();

	void				activate (p2i gestureEnd);
	bool				update (f32 time);
	void				draw3D ();

	void				adjustStats (IAttributes* attr);

private:
	enum { k_ghosts = 7 };
	v3f					m_positions[k_ghosts];
	v3f					m_velocities[k_ghosts];
	ASpriteInstance		m_sprites[k_ghosts];
	float				m_radius;
	int					m_kills;
	s32					m_soundIndex;
};

class CEarthquakeBehaviour : public ISpellBehaviour
{
public:
	CEarthquakeBehaviour ();
	~CEarthquakeBehaviour ();

	void				activate (p2i gestureEnd);
	bool				update (f32 time);

	void				adjustStats (IAttributes* attr);

private:
	float				m_time;
	float				m_maxTime;
	float				m_power;
};

class CSpell : public CWeaponObject
{
public:
	explicit CSpell();
	virtual	~CSpell();

	virtual EWeaponType	getType() { return WT_Spell; }

	virtual void		update(f32 time);
	virtual void		draw();
	void				draw3D();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		reset();
	virtual void		setShopState();
	virtual f32			getCooldown();

	void				setState(ESpellState state);
	ESpellState			getState();

	bool				canStartCast();
	bool				isCasting();
	void				startCast(CTouchElement* touch);

protected:
	virtual	void		adjustStats(IAttributes* attributes);
	f32					m_stat_cooldown;

private:
	void				drawGesture ();

	ISpellBehaviour*	m_behaviour;
	ESpellState			m_state;
	ColladaControl		m_shield;
	CTouchElement*		m_touch;

	ITexture*			m_fingerTrailTexture;
	float				m_fadeAlpha;
	array<v3f>			m_gesture;
	array<S3DVertex>	m_vertices;
	array<u16>			m_indices;

	float				m_cooldown;
	bool				m_visible;
};
