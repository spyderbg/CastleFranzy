///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class MenuEngage;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlText;


class CSpriteInstance;
class CTouchElement;

///////////////////////////////////////////////////////////////////////
/// Menu_Ingame_Help
///////////////////////////////////////////////////////////////////////

class Menu_Ingame_Help : public IMenu
{
public:
	Menu_Ingame_Help(IMenu* parent);
	virtual ~Menu_Ingame_Help();

	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu( IMenu* menu, bool gain_focus = true );

	void LoseFocus();
	void GainFocus();

	bool isBack() { return m_backClicked; }

private:
	void DrawBackground();
	void CreateLevelButtons();

	bool m_Finish;	
	bool m_backClicked;	
};