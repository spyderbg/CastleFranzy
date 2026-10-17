#pragma once

#include "Game/ColladaControl.h"
#include "Game/Config/Upgrades.h"
#include "Game/Objects/Weapons/WeaponObject.h"

enum GriffinState
{
	GGS_Inactive,
	GGS_Active,
	GGS_Smashing,
	GGS_GoingUp,
	GGS_Up,
	GGS_FlyingToSpawnPosition,
	GGS_Shop,
	GGS_Disabled,
};

class CTouchElement;
class ParticleSystem;

class CGriffin : public CWeaponObject, public ColladaControl
{
public:
	explicit CGriffin();
	virtual	~CGriffin();

	virtual EWeaponType	getType() { return WT_Griffin; }

	virtual void		update (f32 time);
	virtual void		draw ();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		reset();
	virtual void		setShopState();
	virtual f32			getCooldown();

	void				setSpawnPosition(vector3df pos);

	void				setState(GriffinState state);
	GriffinState		getState();

	CTouchElement*		getTouch ();

	void				elfArrowHit ();

protected:
	virtual	void		adjustStats(IAttributes* attributes);

	float				m_stat_cooldown;
	float				m_stat_radius;
	int					m_stat_hits;
	float				m_stat_petrify;

private:
	void				activate();
	void				deactivate();
	void				flyToSpawnPosition();

	vector3df			m_spawnPosition;
	int					m_state;
	float				m_cooldown;
	float				m_smashTimer;
	CTouchElement *		m_touch;
	ColladaControl		m_brokenGround;
	ColladaControl		m_petrifyWave;
	float				m_removeBrokenGroundTimer;
	ISceneNode*			m_shadow;

	ISceneNode*			m_scaleDummy;
	ISceneNode*			m_scaleBip01;

	void				petrify();
	void				updatePetrify(f32 time);
	void				petrifyBlast();
	v3f					m_petrifyPosition;
	f32					m_petrifyCountdown;
	ParticleSystem*		m_petrifySparkles;
	f32					m_petrifySparklesTime;

	v3f					m_deactivatingStartPosition;
	float				m_deactivatingTimer;

	void				setShouldBeVisible(bool object, bool shadow, bool effect, bool petrify);
	bool				m_visible;
	bool				m_visibleObject;
	bool				m_visibleShadow;
	bool				m_visibleEffect;
	bool				m_visiblePetrify;
};
