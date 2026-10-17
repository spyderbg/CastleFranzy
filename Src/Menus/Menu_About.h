///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlPane;

class CSpriteInstance;


///////////////////////////////////////////////////////////////////////
/// Menu_LevelSelect
///////////////////////////////////////////////////////////////////////

class Menu_About : public IMenu
{
public:
	Menu_About(IMenu* parent);
	virtual ~Menu_About();

	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void GainFocus();
	void LoseFocus();

	void ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);

private:
	void DrawBackground();
	void UpdateControlsOnFocus(); 

private:
	bool					m_Finish;
	VisualControlPane*		m_vpane;

	void PreprocessStrings();	
	//search for font name in the string
	irr::core::stringw EscapeFont( s32 s_id );
	//search for color in the string
	irr::core::stringw EscapeColor( s32 s_id );
	//get the text only
	irr::core::stringw EscapeTextLine ( s32 s_id );
	//convert hex color string to s32
	u32 ConvertColorFromString( stringw color );

	irr::core::stringw SetVersion( irr::core::stringw cline );
};