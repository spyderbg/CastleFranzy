///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class MenuEngage;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlPane;
class VisualControlText;

class Menu_Confirmation;
class CSpriteInstance;
class CTouchElement;

///////////////////////////////////////////////////////////////////////
/// Menu_LevelSelect
///////////////////////////////////////////////////////////////////////

class Menu_HighScoresLevelList : public IMenu
{
public:
	Menu_HighScoresLevelList(IMenu* parent);
	virtual ~Menu_HighScoresLevelList();

	void Initialize();
	void Destroy();

	virtual void    GainFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu( IMenu* menu, bool gain_focus = true );

private:
	void					DrawBackground();
	void					CreateLevelButtons();

	bool					m_Finish;

	s32						m_selected;

	VisualControlPane*		m_vpane;
	VisualControlText*		m_titleText;

	VisualControlButton*	m_arrowUp;
	VisualControlButton*	m_arrowDown;
};