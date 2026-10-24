// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GSSplash
//
//		State that handle the showing of the game splash screen
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IStateBase.h"
#include "IO/TouchScreen/TouchPad.h"

///////////////////////////////////////////////////////////////////////

// ** Forward Declaration
// ////////////////////////////////////////////////////////
class ASprite;
class GS_Menu;
class VisualControlText;
class VisualControlSlider;
class Menu_LanguageSelection;

enum GS_SplashType { 
	GS_SplashType_FirstLoad = 0
};

enum GSSplashStates {
	GSSplashState_Initializing = 0,
	GSSplashState_Loading, 
	GSSplashState_LoadingDone
} ;

// ** Main Class
// ////////////////////////////////////////////////////////
class GS_Splash : public IStateBase
{
// ** Friendships
// ////////////////////////////////////////////////////////
	friend				class CStateMachine;
// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
public:
						 GS_Splash( GS_SplashType type = GS_SplashType_FirstLoad );
	virtual				~GS_Splash();
// ** Methods
// ////////////////////////////////////////////////////////
public:
	virtual void	update( float dt );
	virtual void	draw();

	virtual void	sleep();
	virtual void	wakeup();

// ** Members
// ////////////////////////////////////////////////////////
private:
	virtual void	ctor();
	virtual void	dtor();

	GS_SplashType		m_splashType;
	void				InitializeControls();

	GSSplashStates		m_state;
	int					m_loadingStep;

	//background
	ASprite*			m_BGR;	

	CTouchElement		m_TE;

	f32					m_loadingDoneTimer;

	VisualControlSlider *m_LoadingBar;

	GS_Menu*			m_nextState;

	int					m_loadingStepMin; 
	int					m_loadingStepMax;
};

