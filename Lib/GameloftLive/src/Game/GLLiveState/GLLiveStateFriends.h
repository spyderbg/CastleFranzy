#pragma once
#include "GLLiveState.h"
#include "GLLiveStateTrophy.h"

//Friends avatar
class AvatarInfo
{
public:
	AvatarInfo()
	{
		id     = 0;
		pImage = NULL;
		modifyTime = NULL;
	}

	~AvatarInfo()
	{
		XP_API_DEL(pImage);
		XP_API_DELETE_ARRAY(modifyTime);
	}

	int       id;
	CGLImage *pImage;
	char     *modifyTime;
};

class GLLiveStateFriends :
	public GLLiveState
{
public:

	enum FriendsSubState
	{	
		FRIENDS_NULL,
		FRIENDS_LIST,
		FRIENDS_DETAIL,
		FRIENDS_PROCESS,
		FRIENDS_DELETE,
		FRIENDS_EVALUATE,
		FRIENDS_COMPARE,
		FRIENDS_COMPARE_DETAIL,
		FRIENDS_GAME_SET,
		FRIENDS_ERROR		
	};

	enum FriendScroll
	{
		SCR_LIST = 0,
		SCR_COMPARE,
		SCR_COMPARE_DETAIL
	};

	GLLiveStateFriends(GLLiveStateHandler * handler, char * receiver = NULL);
	virtual ~GLLiveStateFriends(void);

	virtual void SwitchState();
	virtual DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_FRIENDS;}
	bool IsProcess(){return m_sub_state == FRIENDS_PROCESS;}

	bool HandleEvent( _TOUCH_EVENT event );
	bool HandleEventList( _TOUCH_EVENT event , FriendScroll type = SCR_LIST);
	bool HandleEventDetail( _TOUCH_EVENT event );
	bool HandleEventDelete( _TOUCH_EVENT event );
	bool HandleEventEvaluate( _TOUCH_EVENT event );
	bool HandleEventCompare( _TOUCH_EVENT event );

	void Paint();
	void PaintList();
	void PaintDetail();
	void PaintDelete();
	void PaintEvaluate();
	void PaintCompare();
	void PaintCompareDetail();
	void PaintScrollBar(GLLiveStateFriends::FriendScroll type);
	void PaintGameSet();

	void UpdateAnimation();
	void UpdateAnimationList(FriendScroll type = SCR_LIST);
	void UpdateAnimationCompare();

	void OnMenuItemClicked( int id );

// GLXPlayerObserver interface implement
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );

private:
	void ChangeOffset(int newOffset, FriendScroll type);
	void ChangeFriendCount(int count);
	void Evaluate(int posX);

	void LoadAllFriendAvatar();
	void UpdateFriendAvatar();
	void LoadGameIcon();
	void LoadTrophyIcon();
	void SortCommonGameList();

	bool  m_updating;                  //is updating friend avatar
	int   m_updateFriend;              //the friend index whose avatar is updating
	bool  m_updated;                   //all avatars are updated

	GLLiveMenu  *m_menu;
	GLLiveMenu  *m_detailMenu;
	GLLiveMenu  *m_deleteMenu;
	GLLiveMenu  *m_gameSetMenu;
	//GLLiveMenu  *m_evaluateMenu;

	_TouchRect m_trEvaluate;
	int    m_startEvaluate;
	int    m_evaluate;
	bool   m_evaluating;

	char   m_addUser[MAX_USERNAME];

	int    m_itemsCount[3];
	int    m_itemsPerPage[3];
	int    m_lineHeight[3];

	//visible area
	int    m_startY[3];
	int    m_height[3];

	//Scroll Bar
	int    m_barX[3];
	int    m_scrollBarH[3];
	int    m_scrollBarPos[3];
	//Animation data
	int        m_offset[3];
	int        m_oldOffset[3];
	int        m_touchPos[3];         //Y-
	bool       m_moving[3];
	bool       m_moved[3];
	int        m_speed[3];

	FriendsSubState m_sub_state;
	FriendsSubState m_last_state;

	//Friends detail
	int        m_select;
	int        m_selectedFriend;
	char *	m_receiver;

	CGLImage** m_friendAvatar;



	//my game list
	GLLiveGameInfo*		m_myGameList;
	int					m_myGameCount;
	//friend's game list
	GLLiveGameInfo*		m_friendGameList;
	int					m_friendGameCount;
	//common game list
	int*				m_commonGamePIDList;
	GLLiveGameInfo*		m_commonGameList;
	int					m_commonGameCount;
	//common trophy count
	int*				m_totalTrophyCount;
	int*				m_commonTrophyCount;
	//counts
	int					m_getGameTrophyCount;
	int					m_getTrophyIconCount;
	bool				m_isGettingMyTrophyList;
	
	GLLiveGameInfo*		GetCommonGameInfo(GLLiveGameInfo* gamelist, int length, int PID);
	bool				HasTheTrophy(TrophyInfo* trophylist, int length, int trophyId);
	
	bool                m_isInviteGame;
	bool                m_isInviteChat;
	bool                m_isEvaluate;
	int                 m_gameType;   //1 - Quick play 3 - Costum play

	bool                m_isLogout;
	char                m_inviteFriend[MAX_USERNAME];
};
