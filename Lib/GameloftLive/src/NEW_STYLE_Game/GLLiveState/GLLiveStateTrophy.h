#pragma once

#include "GLLiveState.h"
#include "GLLiveMenu.h"

class TrophyInfo
{
public:
	TrophyInfo()
	{
		id = 0;
		score= 0;
		icon = NULL;
		name = NULL;
		description = NULL;
		time = NULL;
	}

	~TrophyInfo()
	{
		XP_API_DEL(icon);
		XP_API_DELETE_ARRAY(name);
		XP_API_DELETE_ARRAY(description);
		XP_API_DELETE_ARRAY(time);
	}
	int		  id;
	int       score;
	CGLImage *icon;
	char     *name;
	char     *description;
	char     *time;
};

class GLLiveGameInfo
{
public:
	GLLiveGameInfo()
	{
		name[0] = 0;
		pid = 0;
		score = 0;
		icon = NULL;
		trophyList = NULL;
		trophyCount = 0;
	}
	~GLLiveGameInfo()
	{
		XP_API_DEL(icon);
		XP_API_DELETE_ARRAY(trophyList);
	}
	char name[64];
	int pid;
	int score;
	CGLImage   *icon;			
	TrophyInfo *trophyList;
	int         trophyCount;
};

class GLLiveStateTrophy : public GLLiveState
{
public:

	GLLiveStateTrophy(GLLiveStateHandler * handler);
	~GLLiveStateTrophy(void);

	bool HandleEvent( _TOUCH_EVENT event );
	bool HandleEventView( _TOUCH_EVENT event );

	void SwitchState(){};
	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_TROPHY;}
	bool IsProcess(){return false;}

	void Paint();
	void PaintCover(int index);
	void UpdateAnimation();

	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );
	
	static TrophyInfo* processTrophyList(char *data, int dataLen, int &trophyCount);

private:
	void LoadGameIcon();
	void LoadCurTrophyIcon();
	bool HasTheTrophy(TrophyInfo* trophylist, int length, int trophyId);

	int     GetPageLines(int page);

	int		m_touchBeginX;
	bool	m_draggingX;
	int		m_moveDistance;
	int		m_offset_x;

	bool    m_draggingScroll;

	int     m_touchBeginY;
	bool    m_draggingY;
	int   * m_offset_y;

	int      m_page;
	int*     m_select;

	bool    m_holding;

	GLLiveGameInfo  *m_pGameList;
	GLLiveGameInfo  *m_pGameListFull;
	int     m_updatedTrophyGame;                //The games count whose tropy list has been updated
	int     m_updatedIconGame;					//The games count whose game icon has been update
	int     m_updatedTrophyIconGame;			//The games count whose trophy icons have been update
	int     m_updatedTrophyIcons;                //The trophy icons updated
	bool	m_inited;
};