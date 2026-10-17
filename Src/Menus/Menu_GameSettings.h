///////////////////////////////////////////////////////////////////////
#pragma once

#include "Entities/IMenu.h"

#include "Game/Config/UserProfile.h"

///////////////////////////////////////////////////////////////////////

class MenuPopupMM;
class Menu_LevelSelect;
class Menu_Confirmation;

class VisualControlButton;

///////////////////////////////////////////////////////////////////////
/// Menu_GameSettings
///////////////////////////////////////////////////////////////////////

class Menu_GameSettings : public IMenu
{
public:
	Menu_GameSettings(IMenu* parent);
	~Menu_GameSettings();
	
	void Initialize();
	void Destroy();

	void Draw();
	void Update( float dt );

	void GainFocus();
	void LoseFocus();

	void DrawChapterInfo();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );
	void SetIsMenuIngame( bool is_ingame ) { m_isIngame = is_ingame; }

private:
	bool					m_isIngame;
	void					DrawAdditionalItems();
	bool					m_Finish;

	VisualControlButton*	m_buttonLock;

	Menu_Confirmation*		m_menuConfirmation;

	// confirm 
	s32						m_confirmStrId;
	IVisualControl*			m_confirmVC;
};
//---------------------------------------------------------------------------------------------------------
