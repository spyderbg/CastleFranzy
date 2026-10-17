///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class ASprite;
class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlPane;
class VisualControlText;


///////////////////////////////////////////////////////////////////////
/// Menu_Achievements
///////////////////////////////////////////////////////////////////////

class Menu_Achievements : public IMenu
{
public:
	Menu_Achievements(IMenu* parent);
	virtual ~Menu_Achievements();

	void Initialize();
	void Destroy();

	virtual void    LoadResources();
	virtual void    FreeResources();

	virtual void    GainFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu( IMenu* menu, bool gain_focus = true );

private:
	void					DrawBackground();
	void					CreateButtons();
	void					SetButtonsAimations();

	bool					m_Finish;	

	VisualControlPane*		m_vpane;
	VisualControlText*		m_titleText;

private:
	ASprite*				m_Icons;
	s32						m_LastDescription;
};