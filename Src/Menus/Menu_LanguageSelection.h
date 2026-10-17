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


class CSpriteInstance;
class CTouchElement;

///////////////////////////////////////////////////////////////////////
/// Menu_LevelSelect
///////////////////////////////////////////////////////////////////////

class Menu_LanguageSelection : public IMenu
{
public:
	Menu_LanguageSelection(IMenu* parent);
	virtual ~Menu_LanguageSelection();

	void Initialize();
	void Destroy();

	virtual void    GainFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu( IMenu* menu, bool gain_focus = true );

private:
	void						DrawBackground();
	void						CreateLanguageButtons();

	bool						m_Finish;	

	VisualControlPane*			m_vpane;
	VisualControlText*			m_titleText;

	VisualControlButton*		m_arrowUp;
	VisualControlButton*		m_arrowDown;

	VisualControlButton*		m_backButton;
};