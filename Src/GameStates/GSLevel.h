///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IStateBase.h"

#include "Core/irrlicht.h"

#include "Game/HUD/HUD.h"

///////////////////////////////////////////////////////////////////////

struct SLevelInfo
{
	stringw levelName;
	stringc levelFileName;
};

///////////////////////////////////////////////////////////////////////

class HUD;
class GamePad;
class ASprite;
class ASpriteInstance;

class CGameConfig;
class CPlayfield;

class MenuLoading;
class Menu_Ingame;
class Menu_Help;

class CUnit;

///////////////////////////////////////////////////////////////////////

class GSLevel;
extern GSLevel* g_level;

///////////////////////////////////////////////////////////////////////
/// GSLevel
///////////////////////////////////////////////////////////////////////

class GSLevel : public IStateBase
{
	friend class CStateMachine;	

private:
	GSLevel();
public:
	virtual	~GSLevel();

public:

	virtual void		update( float dt );
	virtual void		draw();

	virtual void		sleep();
	virtual void		wakeup();

	HUD*				GetHUD()				{ return m_hud; }
	Menu_Ingame*		GetInGameMenu () { return m_pMenuIngame; }

private:

	virtual	void		ctor();
	virtual	void		dtor();

	IrrlichtDevice*		m_Device;
	ISceneManager*		m_SceneManager;
	IVideoDriver*		m_Driver;
	IGUIEnvironment*	m_Guienv;

	HUD*				m_hud;

	s32					m_MusicId;

	MenuLoading*		m_pLoadingMenu;
	Menu_Ingame*		m_pMenuIngame;
	Menu_Help*			m_pMenuHelp;

	ISceneNode*			m_dummy;
	CUnit*				m_rider;
};
