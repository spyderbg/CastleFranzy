///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Game/Objects/GameObject.h"
#include "Game/Config/Upgrades.h"

///////////////////////////////////////////////////////////////////////

class CWeaponObject : public CGameObject
{
public:
	CWeaponObject(IAttributes* attributes);
	virtual ~CWeaponObject();

	virtual void			update(f32 time);

	virtual EWeaponType		getType() = 0;

	virtual void			reset();
	virtual void			setShopState() = 0;
	virtual void			setShopHighlight(f32 power);

	virtual f32				getCooldown();

	void					setLevel(int level);
	int						getLevel();

	void					setScale(f32 scale);

	void					drawCooldown(p2i pos);

protected:
	virtual	void			adjustStats(IAttributes* attributes) = 0;
	int						m_level;

	void					setBaseScale(f32 scale);
	f32						m_scale;
	f32						m_baseScale;

	f32						m_readyTime;
	f32						m_cooldownTime;

};