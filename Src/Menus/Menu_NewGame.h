///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class MenuEngage;
//class MenuOptions;
//class MenuChooseChapter;
//class MenuFullVersion;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;

class CSpriteInstance;


///////////////////////////////////////////////////////////////////////
/// Menu_LevelSelect
///////////////////////////////////////////////////////////////////////

class Menu_NewGame : public IMenu
{
public:
	Menu_NewGame(IMenu* parent);
	virtual ~Menu_NewGame();

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