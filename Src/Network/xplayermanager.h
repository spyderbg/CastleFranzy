#pragma once

#include "Common/Singleton.h"
#include "Menus/Menu_ScoreInputType.h"

#ifdef OS_IPHONE

	#include "Common/GLXPlayerObserver.h"
	#include <Leaderboard/GLXPlayerLeaderboard.h>
	#include <Login/GLXPlayerLogin.h>

#endif


typedef enum CXPlayerManagerState
{
	NOT_INITIALISED = 0,
	LOGGINGIN,
	LOGGINGOUT,
	LOGGEDIN,
	SYNCHRONISE,
	PROCESSSYNCHRONISE,
	SENDINGHIGHSCORE,
	PROCESSINGSENDHIGHSCORE,
	GETTINGHIGHSCORE,
	PROCESSINGGETHIGHSCORE,
	GETTINGMYSCORE,
	PROCESSINGGETMYSCORE,
	IDLE,
	NETWORKERROR
};

typedef enum CXPlayerERROR
{
	k_NOERROR = 0,
	k_INVALID_USERNAME,
	k_USERNAME_AND_PASSWORD_MISSMATCH,
	k_NETWORK_ERROR
};

class Menu_Score;

class CXPlayerManager : public CSingleton<CXPlayerManager>
#ifdef OS_IPHONE
	, public GLXPlayerObserver 
#endif
{
protected:
	friend class CSingleton<CXPlayerManager>;
	CXPlayerManager();
	~CXPlayerManager();
private:
	
#ifdef OS_IPHONE
	GLXPlayerLogin* m_GLXPlayerLogin;
	GLXPlayerLeaderboard* m_GLXPlayerLeaderboard;
#endif
	
	CXPlayerManagerState m_state;
	void SwitchState(CXPlayerManagerState newState);
	int m_score;
#ifdef OS_IPHONE
	char m_playerName[3*XPLAYER_MAX_USERNAME_LENGTH + 1];
#endif
	CXPlayerERROR m_lastError;
public:
	void Login(char* userName, char* userPassword);
	void Logout();
	void ShutDown();
	void Update();
	bool IsReady();
	bool IsLoggedIn();
	int GetMyScore() { return m_score; };
	CXPlayerManagerState GetState() { return m_state; };
	CXPlayerERROR GetLastErrorCode() { return m_lastError; };
	bool HasError() { return m_state == NETWORKERROR; }
	void FillScore(Menu_Score* menuScore, MenuScore_InputType type );
	void RefreshScore(MenuScore_InputType type);
	void UploadMyScore(int score);
	void OnNetworkError();
	void OnRequestTimeout(int func_id);
	void OnRequestSuccess( int func_id, char* data, int data_len );
	void OnRequestFailure( int func_id, int error_code);
	void ClearError () { if (m_state == NETWORKERROR) m_state = IDLE; }
};