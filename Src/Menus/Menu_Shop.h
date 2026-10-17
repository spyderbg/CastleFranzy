///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Entities/IMenu.h"

#include "Game/Config/Upgrades.h"

///////////////////////////////////////////////////////////////////////

class Menu_Confirmation;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlText;
class ASpriteInstance;
class CSpriteInstance;

class CWeaponObject;
class CCastleGate;
class CHydra;
class CFireball;
class CIceball;
class CGriffin;
class CSpell;

class ParticleSystem;

//---------------------------------------------------------------------------------------------------------
// Shop Menu
//---------------------------------------------------------------------------------------------------------
class Menu_Shop : public IMenu
{
public:
	explicit				Menu_Shop(IMenu* parent);
	virtual					~Menu_Shop();

	virtual void			LoadResources();
	virtual void			FreeResources();

	virtual void			Initialize();
	virtual void			Destroy();

	virtual void			GainFocus();
	virtual void			LoseFocus();

	virtual void			Draw();
	virtual void			Update( float dt );

	virtual void			VisualControlDoAction( IVisualControl* visual_control, int action_id );

public:
	void					ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);

private:
	void					DrawTitle();
	void					DrawWeaponDescription();
	void					DrawAvailableMoney();

private:
	void					UpdateSliderInput(f32 time);
	void					UpdateScrollInput(f32 time);
	void					UpdateHighlight(f32 time);
	void					RotateWeapons(f32 angle);
	f32						GetWeaponSnapAngle();
	int						AngleToWeapon(f32 angle);
	f32						WeaponToAngle(int weapon);
	void					AdjustCurrentAngle(f32 offset);
	void					AdjustTargetAngle(f32 offset);

	void					SelectWeapon(int index);
	void					SelectLevel(int level);

	void					SetStartingWeapon();

	void					NextWeapon();
	void					PrevWeapon();

	bool					IsWeaponBought(	int item, int level );
	bool					CanBuyWeapon(	int item, int level );
	void					BuyWeapon(		int item, int level );

	int						m_selected;
	int						m_level;

	VisualControlButton*    m_btn_buy;
	VisualControlButton*    m_btn_left;
	VisualControlButton*    m_btn_right;
	VisualControlSlider*	m_slider_level;
	VisualControlText*		m_titleText;

	// Scroll
	CTouchElement*          m_TE_weapons;

	f32						m_angle_current;
	f32						m_angle_target;
	f32						m_angle_speed;
	bool					m_angle_skip;

	bool					m_exit;

	ICameraSceneNode*		m_camera;
	ILightSceneNode*		m_light;
	ISceneNode*				m_floor;

	ISceneNode*				m_hydraPool;

	CWeaponObject*			m_weapons[WT_Count];
	int						m_weaponCount;

	f32						m_highlight[WT_Count];

	int						m_nextCutscene;

	ASprite*				m_shopSprite;

	Menu_Confirmation*		m_menuConfirmation;

};
