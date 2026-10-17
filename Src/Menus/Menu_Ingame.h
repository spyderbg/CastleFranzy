///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class CPlayfield;
class MenuEngage;
//class MenuOptions;
//class MenuChooseChapter;
//class MenuFullVersion;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlText;

class Menu_Confirmation;
class Menu_Ingame_Help;
class Menu_Help;
class Menu_GameSettings;
class Menu_SoundOptions;


class CSpriteInstance;

enum IngameMenuTypes {
	IngameMenuPause = 0, 
	IngameMenuVictory, 
	IngameMenuDefeated, 
	IngameMenuHighScore,
	//default
	IngameMenuNotSpecified,
} ;


///////////////////////////////////////////////////////////////////////
/// Menu_Ingame
///////////////////////////////////////////////////////////////////////

class Menu_Ingame : public IMenu
{
public:
	Menu_Ingame(IMenu* parent);
	virtual ~Menu_Ingame();

	void LoadResources();
	void FreeResources();
	
	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void UpdateMenuMusic();
		
	void CallMenu( IngameMenuTypes type ) {
					SetMenuType( type );
					ShowMenu( true );
					DoMenuInitializations();
					m_time = 0;
				}

	void SetEndlessScores (int kills, int days, int gold, int souls);
	void SetLevelScores (int gold, int souls, int difficulty);

	void LoseFocus(); 
	void GainFocus();

	bool IsOnFocus();

	Menu_Help			*GetHelpMenu() { return m_menuHelp; }
	Menu_GameSettings	*GetGameSettings(){ return m_menuGameSettings; }
	Menu_SoundOptions	*GetSoundOptions(){ return m_menuSoundOptions; }
	VisualControlText   *GetTitleTextControl() { return m_titleText; }


private:
	void DrawBackground();
	void DrawHighScore();

	void DoMenuInitializations();
	void ShowMenu( bool show ) { m_menuShow = show; }	
	void SetMenuType( IngameMenuTypes type ) { m_menuType = type; }

	void DeleteControls();
	void MakeControls();
	void ReturnControlToGame();

	void CreatePauseMenu(); 
	void CreateVictoryMenu(); 
	void CreateDefeatedMenu();
	void CreateHighScoreMenu();
	
	void DoActionPause( IVisualControl* visual_control, int action_id );
	void DoActionVictory( IVisualControl* visual_control, int action_id );
	void DoActionDefeated( IVisualControl* visual_control, int action_id );
	void DoActionHighScore( IVisualControl* visual_control, int action_id );

private:
	CPlayfield*				m_Playfield;

	bool					m_menuShow;
	bool					m_Finish;
	IngameMenuTypes			m_menuType;
	VisualControlText*		m_titleText;	

	Menu_Confirmation	    *m_menuConfirmation;
	Menu_Ingame_Help		*m_menuIngameHelp;
	Menu_Help				*m_menuHelp;
	Menu_GameSettings	    *m_menuGameSettings;
	Menu_SoundOptions		*m_menuSoundOptions; 
	bool					m_exit;
	ASprite*				m_highScoreIcons;
	float					m_time;

	bool					m_scoreEndless;
	int						m_scoreKills;
	int						m_scoreDays;
	int						m_scoreGold;
	int						m_scoreSouls;
	int						m_scoreDifficulty;


///////////////////////////////////////////////////////////////////////
/// Cheat codes
///////////////////////////////////////////////////////////////////////

private:
	CTouchElement		m_touch;

private:
			void		UpdateCheats();
	
};