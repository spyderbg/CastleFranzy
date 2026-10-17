#pragma once

#include "Game/ColladaControl.h"
#include "Game/Config/Upgrades.h"
#include "Game/Objects/Weapons/WeaponObject.h"


enum EHydraState
{
	HS_Idle,

	HS_EatFromHand,
	HS_WaitForFallingUnit,
	HS_EatFromRoad,

	HS_WaitForHelp,
	HS_EatUnit,

	HS_Transition,

	HS_Shop,
	HS_Disabled,
};

enum EHydraHeadState
{
	HHS_Ready,
	HHS_Aiming,
	HHS_Eating,
	HHS_Cooldown,
	HHS_Transition,
};

class CTouchElement;
class CUnit;

class CHydraHead : public ColladaControl
{
public:
	f32				Cooldown;
	EHydraHeadState	State;
};

class CHydra : public CWeaponObject
{
public:
	explicit CHydra();
	virtual ~CHydra();

	virtual EWeaponType	getType() { return WT_Hydra; }

	virtual void		update (f32 time);
	virtual void		draw ();

	virtual void		setVisible(bool visible);
	virtual bool		isVisible();

	virtual void		reset();
	virtual void		setShopState();
	virtual f32			getCooldown();
	virtual void		freezeAnimation(bool freeze);

	void				setState(EHydraState state);
	EHydraState			getState();

	bool				isReadyToEat();
	bool				isReadyToEat(CUnit* unit);
	void				waitForUnit(CUnit* unit);

	void				attachVictimToHydra();

	void				eatUnitStart(CUnit* unit);
	void				eatUnitContinue();
	void				eatUnitEnd();

	void				eatFromHandStart(CUnit* unit);
	void				eatFromHandFail();
	void				eatFromHandSuccess();

	void				eatFromRoadStart();
	void				eatFromRoadCancel();
	void				eatFromRoadFail();
	void				eatFromRoadSuccess(CUnit* unit);

	void				eatFromRoadRotateHead();
	CUnit*				eatFromRoadGetTarget();

	CTouchElement*		getTouch();

	void				debug(int n);

protected:
	virtual	void		adjustStats(IAttributes* attributes);

	float				m_stat_cooldown;
	int					m_stat_heads;

private:
	void				startHeadTransition(int head);
	int					m_transitionHead;
	f32					m_transitionTimer;

	EHydraState			m_state;

	CUnit*				m_victim;
	ISceneNode *		m_foodDummy;
	ISceneNode *		m_victimDummy;

	ISceneNode *		m_hydraDummy[3];

	bool				m_visible;
	
	bool				m_aiming;
	CTouchElement*		m_touch;
	float				m_angle;
	v3f					m_ptToFace;
	float				m_refuseTimer;

	f32					m_animOldScales[3];

public:
	CHydraHead			Head[3];
};
