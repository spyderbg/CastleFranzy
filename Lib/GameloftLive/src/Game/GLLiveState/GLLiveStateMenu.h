#ifndef __GAME_STATE_MENU_H__
#define __GAME_STATE_MENU_H__

#include "GLLiveState.h"
#include "GLLiveMenu.h"


class TrophyInfo;

/**
 * The main menu.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GLLiveStateMenu :
	public GLLiveState
{
public:
//Sub State
	enum MainSubState
	{	
		MAIN_NORMAL,
		MAIN_PROCESS,	
		MAIN_ERROR,
		MAIN_NOTIFY,
		MAIN_REPUTATION_DETAIL,
		MAIN_BIND_CONFIRM
	};

	GLLiveStateMenu( GLLiveStateHandler * handler);
	~GLLiveStateMenu(void);

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_MENU;}
	bool IsProcess(){return m_sub_state == MAIN_PROCESS;}

    void Paint();

	void PaintMain();

	void PaintReputationDetail();

	void UpdateAnimation();

	void OnMenuItemClicked( int id );

// GLXPlayerObserver interface implement
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );

	virtual void PaintAvatar(CGLImage * img, int x, int y, int scalex = -1, int scaley = -1);

private:

	GLLiveMenu * m_menu;
	GLLiveMenu *m_confirmMenu;
	int GetNewMsgCount();

//Touch rect
	_TouchRect m_trAvatar;
	_TouchRect m_trUserInfo;     //name, reputation, score
	_TouchRect m_trFriend;
	_TouchRect m_trMessage;
	_TouchRect m_trAd;
	_TouchRect m_trNews;

	MainSubState        m_sub_state;
	bool                m_bLogout;
	bool                m_bPressedLogout;
	bool                m_isEnterFriendList;
	bool                m_bUpdating;
	bool                m_freshInfoForBindTrophy;
	bool                m_isEnterTrophy;
	bool                m_isEnterLeaderBoard;

	long                m_startTime;
	TrophyInfo         *m_trophyList;
	int                 m_trophyCount;
};


class GLLiveStateDisconnect : public GLLiveState
{
public:
	GLLiveStateDisconnect(GLLiveStateHandler * handler, GLLiveState *pLastState);
	~GLLiveStateDisconnect();
	
	//Game state interface
	void Paint();
	bool HandleEvent( _TOUCH_EVENT event );
    void SwitchState();
	void UpdateAnimation();
	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_DISCONNECT;}
	bool IsProcess(){return true;}
	
private:
	GLLiveState *m_lastState;
};

#endif // __GAME_STATE_MENU_H__