///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"
#include "Game/Config/Scores.h"

///////////////////////////////////////////////////////////////////////

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlPane;

class CSpriteInstance;


///////////////////////////////////////////////////////////////////////
/// Menu_LevelSelect
///////////////////////////////////////////////////////////////////////

class Menu_HighScore : public IMenu
{
public:
	Menu_HighScore(IMenu* parent);
	virtual ~Menu_HighScore();

	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void GainFocus();
	void LoseFocus();

	void ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);

	static void SetLevel (int level);
	static void SetEndless ();

private:
	void DrawBackground();

	static CampaignScore	m_campaignScore;
	static EndlessScore		m_endlessScore;

	ASprite*				m_highScoreIcons;

private:
	static bool m_endless;
	static int m_level;

	VisualControlButton		*m_submit_button;
};