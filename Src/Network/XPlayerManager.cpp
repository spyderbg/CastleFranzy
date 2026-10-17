#include "stdafx.h"

#include "Network/xplayermanager.h"
#include "Menus/Menu_Score.h"
#include "Game/Config/Scores.h"

#ifdef OS_IPHONE

	#include <Leaderboard/GLXPlayerLeaderboard.h>
	#include <Login/GLXPlayerLogin.h>

#endif


CXPlayerManager::CXPlayerManager()
{
#ifdef OS_IPHONE
	m_GLXPlayerLogin = new GLXPlayerLogin();
	m_GLXPlayerLogin->RegisterObserver( this );
	
	m_GLXPlayerLeaderboard = new GLXPlayerLeaderboard();
	m_GLXPlayerLeaderboard->RegisterObserver( this );
	
	m_state = NOT_INITIALISED;
	m_lastError = k_NOERROR;
#endif
}

CXPlayerManager::~CXPlayerManager()
{
#ifdef OS_IPHONE
	SAFE_DEL( m_GLXPlayerLogin );
	SAFE_DEL( m_GLXPlayerLeaderboard );
#endif
}

void CXPlayerManager::SwitchState(CXPlayerManagerState newState)
{
	m_state = newState;
}

void CXPlayerManager::Login(char* userName, char* userPassword)
{
#ifdef OS_IPHONE
	printf("XPlayerManager: sending login data\n");
	m_GLXPlayerLogin->SendLogin( userName, userPassword, 2); /// why this parameter is 2 ?!
	strcpy( m_playerName, userName );
	SwitchState( LOGGINGIN );
#endif
}

void CXPlayerManager::Logout()
{
#ifdef OS_IPHONE
	printf("XPlayerManager: sending logout\n");
	m_GLXPlayerLogin->SendLogout();
	SwitchState( LOGGINGOUT );
	m_lastError = k_NOERROR;
#endif
	
}

bool CXPlayerManager::IsReady()
{
	return m_state == IDLE;
}

bool CXPlayerManager::IsLoggedIn()
{
#ifdef OS_IPHONE
	return m_GLXPlayerLogin->IsLoggedIn() && m_state != SYNCHRONISE && m_state != PROCESSSYNCHRONISE && m_state !=SENDINGHIGHSCORE && m_state != PROCESSINGSENDHIGHSCORE;
#else
	return true;
#endif
}

void CXPlayerManager::Update()
{
#ifdef OS_IPHONE
	switch( m_state )
	{
		case NOT_INITIALISED:
			break;
		case LOGGINGIN:
		{
			m_GLXPlayerLogin->Update();
			if( m_GLXPlayerLogin->IsLoggedIn() )
			{
				printf("XPlayerManager: logged in UID = %d!\n",m_GLXPlayerLogin->GetUID());
				m_GLXPlayerLeaderboard->SetUID( m_GLXPlayerLogin->GetUID() );
				SwitchState( SYNCHRONISE );
			}
		}
			break;
		case LOGGINGOUT:
			m_GLXPlayerLogin->Update();
			break;
		case LOGGEDIN:
			printf("XPlayerManager: logged in\n");
			break;
		case SYNCHRONISE:
			m_GLXPlayerLeaderboard->sendRankGetAroundPlayer(1, 8);
			SwitchState(PROCESSSYNCHRONISE);
			break;
		case PROCESSSYNCHRONISE:
			m_GLXPlayerLeaderboard->Update();
			break;
		case SENDINGHIGHSCORE:
			printf("Sending high scores\n");
			printf("XPlayerManager: my score = %d\n",m_GLXPlayerLeaderboard->getCurrentPlayerLeaderboardScore());			
			m_GLXPlayerLeaderboard->sendHighScore( m_score, 1 );
			SwitchState(PROCESSINGSENDHIGHSCORE);
			break;
		case PROCESSINGSENDHIGHSCORE:
			m_GLXPlayerLeaderboard->Update();
			break;
		case GETTINGHIGHSCORE:
			//sendRankGet(int level, int start_pos = 0, int number_of_players = 10, int score_type = SCORE_TYPE_POINTS, bool isGetByCountry = true);	
			printf("Getting high scores\n");
			m_GLXPlayerLeaderboard->sendRankGet(1, 0, 8);
			SwitchState( PROCESSINGGETHIGHSCORE );
			break;
		case PROCESSINGGETHIGHSCORE:
			m_GLXPlayerLeaderboard->Update();
			break;
		case GETTINGMYSCORE:
			printf("Getting my scores\n");			
			m_GLXPlayerLeaderboard->sendRankGetAroundPlayer(1, 7);
			SwitchState( PROCESSINGGETMYSCORE );
			break;
		case PROCESSINGGETMYSCORE:
			m_GLXPlayerLeaderboard->Update();
			break;
		case IDLE:
			break;
		case NETWORKERROR:
			
			break;
	}
	
	if( m_GLXPlayerLogin->IsLoggedIn() )
    	m_GLXPlayerLogin->KeepAlive();
#endif
}

void CXPlayerManager::OnNetworkError() 
{
	printf("XPlayerManager: ERROR! NETWORK ERROR!\n");
	m_lastError = k_NETWORK_ERROR;
	SwitchState(NETWORKERROR);
}

void CXPlayerManager::OnRequestTimeout(int func_id) 
{
	printf("XPlayerManager: ERROR! REQUEST TIMEOUT %d\n",func_id); 
	m_lastError = k_NETWORK_ERROR;
	SwitchState(NETWORKERROR);
}

void CXPlayerManager::OnRequestSuccess( int func_id, char* data, int data_len ) 
{
#ifdef OS_IPHONE
	printf("XPlayerManager: REQUESTSUCCESS data = %s, data_len = %d\n",data, data_len);
	switch( m_state )
	{
		case LOGGINGIN:
			break;
		case PROCESSSYNCHRONISE:
		{
			printf("XPlayerManager: OnRequestSuccess + PROCESSSYNCHRONISE\n");
			m_GLXPlayerLeaderboard->Update();
			int leaderboardSize = m_GLXPlayerLeaderboard->getLeaderboardSize();
			printf("XPlayerManager: leaderboardsize = %d\n", leaderboardSize);
			stringc playerName ( m_playerName );
			for( int i=0;i<leaderboardSize + 1;i++)
			{
				stringc otherPlayerName ( m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName( i ) );
				printf("XPlayerManager: checking for player %s",  m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName( i ) );
				if( playerName.equals_ignore_case(otherPlayerName) )
				{
					m_score = m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore(i);
				}
			}

			printf("XPlayerManager: my score = %d\n",m_score);
			int totalScore = 0;
			CUserProfile myprofile = __PROFILE();
			if (HasEndlessScore(myprofile))
			{
				EndlessScore myscore;
				GetEndlessScore(myprofile, &myscore);
				totalScore = GetScoreTotal(&myscore);
			}
			UploadMyScore(totalScore);
		}
			break;
		case PROCESSINGSENDHIGHSCORE:
			SwitchState(IDLE);
			break;
		case PROCESSINGGETHIGHSCORE:
			SwitchState(IDLE);
			break;
		case PROCESSINGGETMYSCORE:
			SwitchState(IDLE);
			break;
		case LOGGINGOUT:
			SwitchState(NOT_INITIALISED);
			break;
		default:
			printf("XPlayerManager: Request Success action not implemented!\n");
	}
#endif
}

void CXPlayerManager::FillScore(Menu_Score* menuScore, MenuScore_InputType type)
{
#ifdef OS_IPHONE
	int leaderboardSize = m_GLXPlayerLeaderboard->getLeaderboardSize();
	if( leaderboardSize == -1 )
	{
		printf("XPlayerManager: ERROR! No leaderboard to display\n");
		return;
	}
	TCHAR** NamesPtr = NEW TCHAR*[leaderboardSize];
	for( int i=0; i<leaderboardSize; i++ )
	{
		NamesPtr[i] = NEW TCHAR[XPLAYER_MAX_USERNAME_LENGTH];
	}
	
	int* ScoresPtr = NEW int[leaderboardSize];
	int* PositionsPtr = NEW int[leaderboardSize];
	
	for( int i=0; i< leaderboardSize; i++ )
	{
		swprintf( NamesPtr[ i ], XPLAYER_MAX_USERNAME_LENGTH, L"%s", m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName( i ) );
		ScoresPtr[ i ] = m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore( i );
		PositionsPtr[ i ] = m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerPosition( i );
		printf("XPlayerManager: entry = name:%s, score:%d, pos:%d\n",m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName( i ), ScoresPtr[ i ], PositionsPtr[ i ] );
	}
	printf("XPlayerManager: leaderboard size = %d\n",leaderboardSize );
	menuScore->SetInputData( type, leaderboardSize, NamesPtr, ScoresPtr, PositionsPtr, 0, m_playerName );
	
	for( int i=0; i<leaderboardSize; i++ )
	{
		SAFE_DEL( NamesPtr[i] );
	}
	SAFE_DEL( NamesPtr );
	
	SAFE_DEL( ScoresPtr );
	SAFE_DEL( PositionsPtr );
#endif
}

void CXPlayerManager::RefreshScore(MenuScore_InputType type)
{
	if( type == MenuScoreTop10 )
	{
		SwitchState(GETTINGHIGHSCORE);
	}else if( type == MenuScoreMyRank )
	{
		SwitchState(GETTINGMYSCORE);
	}
}

void CXPlayerManager::UploadMyScore(int score)
{
	if( GetMyScore() < score )
	{
		printf("XPlayerManager: Uploading my score = %d because its higher than %d\n", score, m_score);
		SwitchState(SENDINGHIGHSCORE);
		m_score = score;
	}else
	{
		printf("XPlayerManager: NOT uploading my score = %d because its NOT higher than %d\n", score, m_score);
		SwitchState(IDLE);
	}
}

void CXPlayerManager::OnRequestFailure( int func_id, int error_code)
{
#ifdef OS_IPHONE
	printf("XPlayerManager: ERROR! REQUEST FAILURE func_id = %d, error_code = %d\n",func_id,error_code);
	switch( m_state )
	{
		case LOGGINGIN:
		{
			if( func_id != FUNCTION_LOGIN )
				printf("XPlayerManager: ERROR! LOGIN STATES MISSMATCH\n");				
			switch(error_code)
			{
				case ERROR_USERNAME_NOTEXISTS:
					m_lastError = k_INVALID_USERNAME;
					break;
				case ERROR_INVALID_USERNAMEORPASSWORD:
				case ERROR_NO_PASSWORD:
					m_lastError = k_USERNAME_AND_PASSWORD_MISSMATCH;
					break;
				case ERROR_NO_USERNAME:
					m_lastError = k_INVALID_USERNAME;
					break;
				default:
				m_lastError = k_NETWORK_ERROR;				
			}
		}
		break;
		default:
			m_lastError = k_NETWORK_ERROR;
	}
	SwitchState(NETWORKERROR);
#endif
}
