///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "Entities/IMenu.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

//class MenuOptions;
//class MenuChooseChapter;

class Menu_Engage;
class Menu_NewGame;
class Menu_Options;
class Menu_SoundOptions;
class Menu_Campaign;
class Menu_Info;
class Menu_GameSettings;
class Menu_Achievements;
class Menu_Difficulty;
class Menu_About;
class Menu_Help;
class Menu_LanguageSelection;
class Menu_HighScoresLevelList;
class Menu_HighScore;
class Menu_Login;
class Menu_Score;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;

//---------------------------------------------------------------------------------------------------------
// Actions ids
//---------------------------------------------------------------------------------------------------------

#define MENU_ENGAGE_ACTION_BASE			( 10 )
#define MENU_OPTIONS_ACTION_BASE		( 40 )
#define MENU_NEWGAME_ACTION_BASE		( 60 )
#define MENU_SOUNDOPTIONS_ACTION_BASE	( 90 )
#define MENU_CAMPAIGN_ACTION_BASE		( 150 )
#define MENU_DAY_SELECT_ACTION_BASE		( 250 )
#define MENU_SHOP_ACTION_BASE			( 300 )
#define MENU_INFO_ACTION_BASE			( 350 )
#define MENU_GAME_SETTINGS_ACTION_BASE  ( 400 )
#define MENU_ACHIEVEMENTS_ACTION_BASE	( 450 )
#define MENU_CONFIRMATION_ACTION_BASE   ( 500 )
#define MENU_DIFFICULTY_ACTION_BASE		( 550 )
#define MENU_HELP_ACTION_BASE			( 600 )
#define MENU_INGAME_HELP_ACTION_BASE	( 700 )
#define MENU_LANGUAGE_SELECT			( 750 )
#define MENU_LOGIN_BASE					( 800 )
#define MENU_HIGHSCORES_BASE			( 850 )
#define MENU_HIGHSCORESLIST_BASE		( 900 )
#define MENU_SCORE_BASE					( 950 ) 


#define MENU_ABOUT_ACTION_BASE			( 1000 )

//common for every submenu
#define k_button_back_frame							( MENU_F_MENU_BACK_BUTTON )
#define k_button_back_game_x						( 41 )
#define k_button_back_game_y						( 70 )
#define k_button_back_game_w						( m_sprite->GetFrameWidth ( k_button_back_frame ) )
#define k_button_back_game_h						( m_sprite->GetFrameHeight( k_button_back_frame ) )
#define k_button_back_game_anim_normal				( MENU_A_MENU_BACK_BUTTON_NORMAL )
#define k_button_back_game_anim_pressed				( MENU_A_MENU_BACK_BUTTON_PRESSED )
#define k_button_back_game_anim_blink				( MENU_A_MENU_BACK_BUTTON_BLINK )
#define	k_button_back_text							( "" )
#define	k_button_back_font1							( "MM_Normal" )
#define	k_button_back_font2							( "MM_Pressed" )

//title button
#define k_button_main_frame							( MENU_F_MENU_BASE_BUTTON )
#define k_button_main_title_w						( m_sprite->GetFrameWidth ( k_button_main_frame ) )
#define k_button_main_title_h						( m_sprite->GetFrameHeight( k_button_main_frame ) )
#define k_button_main_title_x						( GAME_SCREEN_W2 )
#define k_button_main_title_y						( 16 )
#define k_button_main_anim_normal					( MENU_A_MENU_MAIN )
#define	k_button_main_anim_pressed					( MENU_A_MENU_MAIN )
#define	k_button_main_anim_blink					( MENU_A_MENU_MAIN )
#define	k_button_main_text							( "" )
#define	k_button_main_font1							( "MM_Normal" )
#define	k_button_main_font2							( "MM_Normal" )
#define k_button_main_font_title					( "MM_Title" )
#define k_button_main_textFlag						( CFont::LEFT | CFont::VCENTER )

//back btn sfx
#define k_back_button_sfx							( SFX_MENU_BACK )

//---------------------------------------------------------------------------------------------------------
// Handles bottom bar and works as container for all main menus
//---------------------------------------------------------------------------------------------------------
class MenuMain : public IMenu
{
public:
	MenuMain(IMenu* parent);
	virtual ~MenuMain();

	void LoadResources();
	void FreeResources();
	
	void Initialize();
	void Destroy();

	void GainFocus();
	void LoseFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );
	

	VisualControlButton *GetBackButton( ) { return m_back; }
	VisualControlButton *GetTitle( )	  { return m_title; }
	Menu_Help			*GetHelpMenu()	  { return m_menuHelp; }
	Menu_Score			*GetMenuScore()   { return m_menuScore; }

private:
	void DrawBackground();
	void UpdateBottomBarButtonsAnimTypes();

private:
	Menu_Engage					*m_menuEngage;
	Menu_NewGame				*m_menuNewGame; 
	Menu_Options				*m_menuOptions;
	Menu_SoundOptions			*m_menuSoundOptions;
	Menu_Campaign				*m_menuCampaign;
	Menu_Info					*m_menuInfo;
	Menu_GameSettings			*m_menuGameSettings;
	Menu_Achievements			*m_menuAhievements;
	Menu_Difficulty				*m_menuDifficulty;
	Menu_About					*m_menuAbout;
	Menu_Help					*m_menuHelp;
	Menu_LanguageSelection		*m_menuLanguageSelect;
	Menu_HighScoresLevelList	*m_menuHighScoresLevelList;
	Menu_HighScore				*m_menuHighScore;
	Menu_Login					*m_menuLogin;
	Menu_Score					*m_menuScore;

	IMenu					*m_LastSubMenuPtr;
	/// Visual constorls
	VisualControlButton     *m_title;
	VisualControlButton		*m_back;

	ASprite*				m_BGR;
};