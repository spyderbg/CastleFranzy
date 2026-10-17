#pragma once

#include "GLLiveState.h"
#include "GLLiveMenu.h"

class TrophyInfo;

class GLLiveStateLogin : public GLLiveState
{
public:
	GLLiveStateLogin(GLLiveStateHandler * handler);
	virtual ~GLLiveStateLogin(void);

//Sub State
	enum OnlineSubState
	{	
		ONLINE_NULL,
		ONLINE_LOGIN,
		ONLINE_REGISTER,
		ONLINE_FORGET,
		ONLINE_PROCESS,	
		ONLINE_PROTOCOL,
		ONLINE_ERROR		
	};
// GLLiveState

	virtual bool HandleEvent( _TOUCH_EVENT event );
    virtual void SwitchState();
	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_LOGIN;}
	bool IsProcess(){return m_sub_state == ONLINE_PROCESS;}
    virtual void Paint();
	void PaintLogin();
	void PaintRegister();
	void PaintForget();
	void PaintProtocol();
	void UpdateAnimation();
	virtual void OnMenuItemClicked( int id );

// GLXPlayerObserver interface implement

	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );

//  Chat observer interface
	void OnChatConnected();
	void OnChatDisconnect(int e);

	void SendCreate();

	void SetSubState(OnlineSubState newState);

	static int UploadTrophyData(TrophyInfo *& trophyList, int trophyCount);

private:

// Tools function
	bool LoadProfile();
	bool SaveProfile();

	void SetRememberMe(bool rem);
	void SetEmailFlag(bool flag);

	///@ User informations
	char m_username[MAX_USERNAME];
	char m_password[MAX_PASSWORD];
	char m_confirmPwd[MAX_PASSWORD];
	char m_email[MAX_EMAIL];
	bool m_emailFlag;

	//Timestamp
	int  m_timestamp;

	//Edit box rectangle
	//_TouchRect m_editLogin;
	//_TouchRect m_editPassword;
	//_TouchRect m_editConfirm;
	//_TouchRect m_editEmail;
	//_TouchRect m_emailFlagBox;      

	GLLiveMenu * m_loginMenu;
	GLLiveMenu * m_regMenu;
	GLLiveMenu * m_forgotMenu;
	GLLiveMenu * m_confirmMenu;
	GLLiveMenu * m_protocolMenu;

	bool   m_confirmLogin;
	bool   m_confirming;
	bool   m_retrievlNote;

	//Network data
	int m_errorCode;

	OnlineSubState m_sub_state;
	OnlineSubState m_last_state;

	//Data for upload trophy
	int m_trophyCount;
	TrophyInfo *m_trophyList;

	long m_totalLoginTime;

	bool m_isAcceptProtocol;
	char *m_gcContent;             //General Condition Content
	char *m_gcTime;
	bool m_isRereadGC;
	bool m_isLogin;

	//Scroll Bar
	int    m_totalHeight;
	int	   m_scrollBarH;
	int    m_scrollBarPos;
	bool   m_scrollMoving;
	//Animation data
	int        m_offset;
	int        m_oldOffset;
	int        m_touchPos;         //Y-
	bool       m_moving;
	bool       m_moved;
	int        m_speed;
	int        m_trophyScore;
};