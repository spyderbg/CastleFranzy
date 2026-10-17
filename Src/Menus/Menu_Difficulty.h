///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class MenuEngage;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;

class CSpriteInstance;


///////////////////////////////////////////////////////////////////////
/// Menu_Difficulty
///////////////////////////////////////////////////////////////////////

class Menu_Difficulty : public IMenu
{
public:
	Menu_Difficulty(IMenu* parent);
	virtual ~Menu_Difficulty();

	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);

private:
	void DrawBackground();

private:
	bool					m_Finish;
};