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


///////////////////////////////////////////////////////////////////////
/// Menu_SoundOptions
///////////////////////////////////////////////////////////////////////

class Menu_SoundOptions : public IMenu
{
public:
	Menu_SoundOptions(IMenu* parent);
	virtual ~Menu_SoundOptions();

	void Initialize();
	void Destroy();

	virtual void GainFocus();
	virtual void LoseFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);
	void SetIsMenuIngame( bool is_ingame ) { m_isIngame = is_ingame; }

	bool isBack() { return m_backClicked; }

protected:
	void DrawMenuBackground();

private:
	bool					m_isIngame;
	bool					m_Finish;

	//updated runtime
	VisualControlText*		m_sfxText;
	VisualControlText*		m_musicText;

	VisualControlSlider*	m_musicSlider;
	VisualControlSlider*	m_sfxSlider;
	void					UpdateText();

	bool m_backClicked;	
};