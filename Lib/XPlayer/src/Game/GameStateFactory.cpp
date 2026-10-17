#include "stdafx.h"
#include "GameStateFactory.h"

#include "../../include/Porting.h"
#include "GameStateLogo.h"
#include "GameStateMenu.h"
#ifdef MMO_CHAT
#include "GameStateMMO.h"
#endif
//#include "GameStateChat.h"
#include "GameStateOnline.h"
#if(MULTIPLAYER_TYPE == MULTIPLAYER_LOBBY)
#include "GameStateMultiplayerLobby.h"
#else
#include "GameStateMultiplayer.h"
#endif

#ifdef LOBBY_THREAD_TEST
#include "GameStateMultiplayerLobby.h"
#include "GameStateTestLobby.h"
#include "stresstest/StressThread.h"
static CRITICAL_SECTION cs1 = {0} ;


typedef struct _I_NAME
{
	GameState*  pState;
	int			iNum;
}i_Name;

DWORD WINAPI TestLobbyThread(LPVOID lp) ;

bool g_bTestLogin = false;
bool g_bTestCreate = false;
bool g_bTestFilter = false;
bool g_bTestJoin = false;
bool g_bRandom = true;

const int thread_runtype_length = 2;
bool runtype[2][4] = { //{ true, false, false, false },//0
	{ true, true, false, false },//1
	{ true, false, true, true },//2
};

extern bool g_bSaveLog;
extern char filetime[20];

int  g_iThreadCount = 0;
#endif

#ifdef ENABLE_UNIT_TEST
#include "../../../UnitTest/GameStateTest.h"
#endif //ENABLE_UNIT_TEST
#ifdef LOBBY_THREAD_TEST
StressThread st;
#endif
GameState * GameStateFactory::CreateGameState( GameStateHandler * handler, 
											  DEF::GAME_STATE_ENUM state )
{
	GameState * game_state = NULL;

	switch(state)
	{
	case DEF::GAME_STATE_LOGO:
		game_state = XP_API_NEW GameStateLogo(handler);
		break;
	case DEF::GAME_STATE_MENU:
		game_state = XP_API_NEW GameStateMenu(handler);
		break;
		#ifdef MMO_CHAT
	case DEF::GAME_STATE_MMO:
		game_state = XP_API_NEW GameStateMMO(handler);
		break;
#endif
	//case DEF::GAME_STATE_CHAT:
	//	game_state = XP_API_NEW GameStateChat(handler);
	//	break;
	case DEF::GAME_STATE_ONLINE:
		game_state = XP_API_NEW GameStateOnline(handler);
		break;	
	case DEF::GAME_STATE_TEST_LOBBY:
#ifdef LOBBY_THREAD_TEST
		game_state = XP_API_NEW GameStateTestLobby(handler);
#endif
		break;
	case DEF::GAME_STATE_MULTIPLAYER:
		{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_LOBBY)
#ifndef LOBBY_THREAD_TEST
			game_state = XP_API_NEW GameStateMultiplayerLobby(handler);
#else
			g_bSaveLog = false;
			game_state = XP_API_NEW GameStateMultiplayerLobby(handler, true, false, false, false, false, false);

			InitializeCriticalSectionAndSpinCount(&cs1 ,400) ;


			for (int i =  g_iThreadCount ; i > 0 ; i--)
			{
				int aa = i;
				if( aa % 100 != 0 )
					aa %= 100;
				aa /= 10;
				if( aa == 6)
				{i -= 10;
				g_iThreadCount-=10;
				}
				DWORD dr ;

				i_Name *ab = NULL;
				//ab = (i_Name*)malloc(sizeof(i_Name)) ;
				ab = XP_API_NEW i_Name() ;
				ab->iNum = i;
				ab->pState = game_state;
				CreateThread(0,0,TestLobbyThread, (LPVOID)ab,0,&dr) ;
			}

			st.start(g_iThreadCount);
#endif //LOBBY_THREAD_TEST
#else
			game_state = XP_API_NEW GameStateMultiplayer(handler);
#endif
		}
		break;
#ifdef ENABLE_UNIT_TEST
	case DEF::GAME_STATE_TEST:
		game_state = XP_API_NEW GameStateTest(handler);
		break;
#endif //ENABLE_UNIT_TEST	
	default:
		break;
	}

	return game_state;
}

#ifdef LOBBY_THREAD_TEST
const int TEXT_LENGTH = 35;
char text[TEXT_LENGTH+1] = "                                   ";

//web log
const int maxtest_length = 6;

const int webtest_length = 6;
const char webtest[][50] = { "LoginWeb send:",
"NoRegister:",
"Register send:",
"Register:",
"LoginWeb send again:",
"LoginWeb:"
};

const int lobbytest_length = 4;
const char lobbytest[][50] = { "ConnectLobby send:",
"ConnectLobby:",
"LoginLobby send:",
"LoginLobby:"
};

const int creatlobby_length = 2;
const char creatlobby[][50] = { "CreatLobby send:",
"CreatLobby:",
};

const int filterlobby_length = 2;
const char filterlobby[][50] = { "FilterLobby send:",
"FilterLobby:",
};

const int joinlobby_length = 2;
const char joinlobby[][50] = { "JoinLobby send:",
"JoinLobby:",
};

const char failed[] = "failed:";

enum LOGTYPE
{
	webtest_type = 0,
	lobbytest_type,
	creatlobby_type,
	filterlobby_type,
	joinlobby_type
};

const char filename[][30] = { "webfailed", "websuccess",
"lobbyfailed", "lobbysuccess",
"creatfailed", "creatsuccess",
"filterfailed", "filtersuccess",
"joinfailed", "joinsuccess",
};

void SaveLog( LOGTYPE logTypeI, int lengthType, GameStateMultiplayerLobby* game_state, int& logI, bool bSaveFinal );

DWORD WINAPI TestLobbyThread(LPVOID lp)
{
	i_Name* ab;
	ab = (i_Name*)lp;

	Sleep( ab->iNum );
	int index = XP_API_GET_TIME() % thread_runtype_length;
	GameStateMultiplayerLobby * game_state = XP_API_NEW GameStateMultiplayerLobby(0, false, runtype[index][0], runtype[index][1], runtype[index][2], runtype[index][3], true );

	char* name = game_state->GetUserName();
	memset( name, 0, 32 );
	strcpy( name, ((GameStateMultiplayerLobby*)(ab->pState))->GetUserName() );
	XP_API_SPRINTF( &name[XP_API_STRLEN(name)], "%d", ab->iNum );

	char* password = game_state->GetPassword();
	memset( password, 0, 32 );
	strcpy( password, ((GameStateMultiplayerLobby*)(ab->pState))->GetPassword() );

	_TOUCH_EVENT event; 
	event.isClicked = false;
	game_state->HandleEventLogin( event );

	while( !g_bSaveLog && game_state->GetSubState() != 0 && game_state->GetSubState() != 1 )
	{
		game_state->UpdateAnimation();
		game_state->HandleEvent(event);
		Sleep(10);
	}

	EnterCriticalSection(&cs1) ;
	
	queue.push(game_state->m_log);
	
	int logI = 1;
	SaveLog( webtest_type, webtest_length, game_state, logI, false );
	if( !g_bRandom )
	{
		SaveLog( lobbytest_type, lobbytest_length, game_state, logI, false );
		if( g_bTestCreate )
			SaveLog( creatlobby_type, creatlobby_length, game_state, logI, true );

		SaveLog( filterlobby_type, filterlobby_length, game_state, logI, false );
		if( g_bTestJoin )
			SaveLog( joinlobby_type, joinlobby_length, game_state, logI, true );
	}
	else
	{
		SaveLog( lobbytest_type, lobbytest_length, game_state, logI, false );
		if( game_state->m_bTestCreate )
			SaveLog( creatlobby_type, creatlobby_length, game_state, logI, true );

		SaveLog( filterlobby_type, filterlobby_length, game_state, logI, false );
		if( game_state->m_bTestJoin )
			SaveLog( joinlobby_type, joinlobby_length, game_state, logI, true );
	}

	LeaveCriticalSection(&cs1) ;

	XP_API_DEL( ab );
	XP_API_DEL(game_state);

	return 0;
}

void SaveLog( LOGTYPE logTypeI, int lengthType, GameStateMultiplayerLobby* game_state, int& logI, bool bSaveFinal )
{
	char logType[maxtest_length][50] = { 0 };

	if( logTypeI == webtest_type )
	{
		for( int ii = 0; ii < lengthType; ii++ )
			XP_API_STRCPY( logType[ii], webtest[ii] );

	}
	else if( logTypeI == lobbytest_type )
	{
		for( int ii = 0; ii < lengthType; ii++ )
			XP_API_STRCPY( logType[ii], lobbytest[ii] );
	}
	else if( logTypeI == creatlobby_type )
	{
		for( int ii = 0; ii < lengthType; ii++ )
			XP_API_STRCPY( logType[ii], creatlobby[ii] );
	}	
	else if( logTypeI == filterlobby_type )
	{
		for( int ii = 0; ii < lengthType; ii++ )
			XP_API_STRCPY( logType[ii], filterlobby[ii] );
	}
	else if( logTypeI == joinlobby_type )
	{
		for( int ii = 0; ii < lengthType; ii++ )
			XP_API_STRCPY( logType[ii], joinlobby[ii] );
	}

	char output[2048] = { 0 };
	XP_API_STRCPY( output, game_state->m_sSaveOnlineLog[0] );
	output[XP_API_STRLEN(output)] = ' ';
	output[XP_API_STRLEN(output)] = ' ';
	output[XP_API_STRLEN(output)] = ' ';

	char textLog[TEXT_LENGTH+1] = { 0 };
	int i = 0;
	bool bFailed = false;
	if( XP_API_STRLEN(game_state->m_sSaveOnlineLog[logI]) == 0 )
		return;//bFailed = true;
	while( i < lengthType /*&& XP_API_STRLEN(game_state->m_sSaveOnlineLog[logI]) != 0*/ )
	{		
		int len = XP_API_STRLEN( game_state->m_sSaveOnlineLog[logI] );
		int j = 0;
		for( j = 0; j < len; j++ )
		{
			if( game_state->m_sSaveOnlineLog[logI][j] == ':' )
				break;
		}

		XP_API_STRCPY( textLog, text );
		int lenLog = XP_API_STRLEN( logType[i] );
		if( len != 0 && XP_API_STRNCMP( (char*)logType[i], game_state->m_sSaveOnlineLog[logI], lenLog>(j+1)?(j+1):lenLog ) == 0 )
		{
			for( int k = 0; k<len && k<=TEXT_LENGTH; k++ )
				textLog[k] = game_state->m_sSaveOnlineLog[logI][k];

			logI++;

			int length = XP_API_STRLEN( output );
			XP_API_STRCPY( &output[length], textLog );
		}
		else
		{
			int lenFailed = XP_API_STRLEN( failed );
			if( i == lengthType - 1 //no success message
				|| XP_API_STRNCMP( (char*)failed, game_state->m_sSaveOnlineLog[logI], lenFailed>(j+1)?(j+1):lenFailed ) == 0 )
			{
				bFailed = true;
				int length = XP_API_STRLEN( output );
				XP_API_STRCPY( &output[length], game_state->m_sSaveOnlineLog[logI] );
				logI++;
				break;
			}
			int length = XP_API_STRLEN( output );
			XP_API_STRCPY( &output[length], textLog );
		}
		i++;
	}

	XP_FILE_HANDLE pFile;
	char name[50] = { 0 };

	if( bSaveFinal )
	{
		char specator[6] = {     };
		while( XP_API_STRLEN(game_state->m_sSaveOnlineLog[logI]) != 0 )
		{
			XP_API_STRCPY( &output[XP_API_STRLEN(output)], specator );
			XP_API_STRCPY( &output[XP_API_STRLEN(output)], game_state->m_sSaveOnlineLog[logI] );
			logI++;
		}
	}

	if( bFailed )
	{
		XP_API_SPRINTF( name, ".\\lobbytest\\%s%s.txt", filetime, filename[logTypeI*2] );
		pFile = XP_API_FILE_OPEN( name, "at+" );
	}
	else
	{
		XP_API_SPRINTF( name, ".\\lobbytest\\%s%s.txt", filetime, filename[logTypeI*2+1] );
		pFile = XP_API_FILE_OPEN( name, "at+" );
	}

	if( !pFile )
	{
		XP_DEBUG_OUT("/////////////////////////////////////save log error, Error creating file %s \n", name);
		return;
	}

	char newline[] = "\r\n";
	XP_API_FILE_WRITE((void*)output, 1, XP_API_STRLEN(output), pFile);
	XP_API_FILE_WRITE((void*)newline, 1, XP_API_STRLEN(newline), pFile);

	XP_API_FILE_CLOSE(pFile);
}

#endif