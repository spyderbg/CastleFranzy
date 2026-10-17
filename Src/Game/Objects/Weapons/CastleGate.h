///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Game/ColladaControl.h"
#include "Game/Config/GameConfig.h"
#include "Game/Objects/Weapons/WeaponObject.h"

///////////////////////////////////////////////////////////////////////

class CCastleGate : public CWeaponObject, public ColladaControl
{
public:
	explicit CCastleGate();
	virtual ~CCastleGate();

	virtual void		update(f32 time);
	virtual void		draw();

	virtual EWeaponType	getType() { return WT_Gate; }

	virtual void		reset();
	virtual void		setShopState();

	void				damageGate(f32 damage);

	inline f32			getHealth() { return m_health; }
	inline f32			getSlowHealth() { return m_slowHealth; }
	inline f32			getMaxHealth() { return m_maxHealth; }
	inline f32			getHealthRatio() { return m_health / m_maxHealth; }
	inline f32			getSlowHealthRatio() { return m_slowHealth / m_maxHealth; }
	inline bool			isDestroyed() { return m_health <= 0.0f && !CGameConfig::Invulnerable; }

protected:
	virtual	void		adjustStats(IAttributes* attributes);

private:
	f32				m_maxHealth;
	f32				m_health;
	f32				m_slowHealth;
	f32				m_slowHealthVelocity;

	ColladaControl	m_gate;

};