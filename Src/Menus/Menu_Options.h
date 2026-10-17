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
/// Menu_Options
///////////////////////////////////////////////////////////////////////

class Menu_Options : public IMenu
{
public:
	Menu_Options(IMenu* parent);
	virtual ~Menu_Options();

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