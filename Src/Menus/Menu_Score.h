#pragma once 

//---------------------------------------------------------------------------------------------------------
#include "Entities/IMenu.h"
#include "Menus/Menu_ScoreInputType.h"
//---------------------------------------------------------------------------------------------------------

class VisualControlEditBox;
class VisualControlButton;
class VisualControlContainer;
class VisualControlPane;

typedef enum MenuScoreState
{
	NEED_REFRESH,
	REFRESHING,
	REFRESHED
};

class Menu_Score : public IMenu
{
public:
	Menu_Score( IMenu* parent );
	virtual ~Menu_Score();

	void Initialize();
	void Destroy();

	void GainFocus();
	void LoseFocus();

	void Draw();
	void Update( float dt );

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	// copies input data from array to visual controls - only one call is needed, the controls will be auto recreated 
	// type - where to store controls
	// numFields - number of input fields 
	// NamesPtr and ScoresPtr - numFields elements will be copied
	// positionsPtr is needed when auto_increment is false 
	// initialPosition - rank start position	
	void SetInputData( MenuScore_InputType type, int numFields, TChar **NamesPtr, int *ScoresPtr, int *positionsPtr, int initialPosition, stringw playerName );

	static void SetBackGoesToHighScoreList () { m_backToHighScoreList = true; }
	static void SetBackGoesToSurvivalScore () { m_backToHighScoreList = false; }
	static bool BackGoesToHighScoreList () { return m_backToHighScoreList; }

private:
	void					SetMenu( ) ;
	VisualControlContainer *m_TopRankings;
	VisualControlContainer *m_MyRankings;
	VisualControlPane	   *m_paneTop;
	VisualControlPane	   *m_paneMy;
	MenuScoreState			m_MenuScoreState;
	MenuScore_InputType		m_currentMenuInputType;
	static bool				m_backToHighScoreList;
};