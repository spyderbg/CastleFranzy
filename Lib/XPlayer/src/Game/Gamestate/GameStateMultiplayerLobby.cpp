#include "GameStateMultiplayerLobby.h"

#ifdef OS_WIN32
#include "../../include/Win32Socket.h"
#endif // OS_WIN32

#ifdef OS_IPHONE
#include "IPhoneSocket.h"

extern void GetSaveFilePath(char* filePath, const char* filename);
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

#ifdef LOBBY_THREAD_TEST
extern bool g_bTestLogin;
extern bool g_bTestCreate;
extern bool g_bTestFilter;
extern bool g_bTestJoin;
extern bool g_bRandom;
#endif

#ifdef LOBBY_THREAD_TEST
GameStateMultiplayerLobby::GameStateMultiplayerLobby(GameStateHandler * handler, bool bReadName, 
													 bool bTestLogin, bool bTestCreate, bool bTestFilter, bool bTestJoin, bool bTestThread )
: GameState( handler )
, m_bReadName( bReadName )
, m_bTestLogin( bTestLogin )
, m_bTestCreate( bTestCreate )
, m_bTestFilter( bTestFilter )
, m_bTestJoin( bTestJoin )
, m_bTestThread( bTestThread )
, m_bSameAccountLogin(false)
#else
GameStateMultiplayerLobby::GameStateMultiplayerLobby(GameStateHandler * handler)
: GameState( handler )
, m_bSameAccountLogin(false)
#endif
{
	m_multiPlayerMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);	
	m_multiPlayerMenu->AddMenuItem( MENU_MP_ONLINE, this );
	m_multiPlayerMenu->AddMenuItem( MENU_LOBBY_LOGIN, this );	

	m_multiPlayerMenuLobby = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_multiPlayerMenuLobby->AddMenuItem( MENU_LOBBY_MENU_CREATE, this );	
	m_multiPlayerMenuLobby->AddMenuItem( MENU_LOBBY_MENU_JOIN, this );	
	m_multiPlayerMenuLobby->AddMenuItem( MENU_LOBBY_MENU_AUTOMATCH, this );	

#ifndef GET_SERVER_FROM_WEB
	m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPLobby(this, NULL);	
#else
	m_GLXPlayerMultiPlayer = NULL;
#endif

	m_GLXPlayerUserFriend = XP_API_NEW GLXPlayerUserFriend();
	m_GLXPlayerUserFriend->RegisterObserver( this );
	
	m_GLXPlayerUser = XP_API_NEW GLXPlayerUser();
	m_GLXPlayerUser->RegisterObserver( this );

	m_state = DEF::GAME_STATE_MULTIPLAYER;
	SetOnlineSubState(ONLINE_LOGIN);	
		
	///@Softkey position
	m_SoftLeft.left = DEF::BUTTON_LEFT_X;
	m_SoftLeft.top = DEF::BUTTON_LEFT_Y;
	m_SoftLeft.width = DEF::BUTTON_LEFT_W;
	m_SoftLeft.height = DEF::BUTTON_LEFT_H;

	m_SoftRight.left = DEF::BUTTON_RIGHT_X;
	m_SoftRight.top = DEF::BUTTON_RIGHT_Y;
	m_SoftRight.width = DEF::BUTTON_RIGHT_W;
	m_SoftRight.height = DEF::BUTTON_RIGHT_H;
	
	m_SoftMid.left = DEF::BUTTON_MID_X;
	m_SoftMid.top = DEF::BUTTON_MID_Y;
	m_SoftMid.width = DEF::BUTTON_MID_W;
	m_SoftMid.height = DEF::BUTTON_MID_H;
	
	m_SoftAdd1.left = DEF::BUTTON_RIGHT_X + 55;
	m_SoftAdd1.top = DEF::BUTTON_RIGHT_Y;
	m_SoftAdd1.width = DEF::BUTTON_RIGHT_W;
	m_SoftAdd1.height = DEF::BUTTON_RIGHT_H;

	m_SoftAdd2.left = DEF::BUTTON_RIGHT_X + 110;
	m_SoftAdd2.top = DEF::BUTTON_RIGHT_Y;
	m_SoftAdd2.width = DEF::BUTTON_RIGHT_W;
	m_SoftAdd2.height = DEF::BUTTON_RIGHT_H;
	
	///@Input Box
	m_InputBox = XP_API_NEW InputBox(155,205);
	m_selectedIndex = 0;	
	
#ifdef LOBBY_THREAD_TEST
	for( int i = 0; i < MAX_ONLINE_MESSAGE_COUNT; i++)
		XP_API_MEMSET( m_sSaveOnlineLog[i], 0, ONINE_MESSAGE_MAX_CHARACTOR);
	m_iCurrentLogIndex = 0;
	m_bSendConnect = true;
	m_log=new StressLog;
	if( m_bReadName )
#endif
		LoadProfile();

	m_nationData = XP_API_NEW char[32];
	XP_API_MEMSET( m_nationData, 0, 32 );
	XP_API_SPRINTF( m_nationData, "CN");
	
	m_email = XP_API_NEW char[32];
	XP_API_MEMSET( m_email, 0, 32 );
	XP_API_SPRINTF( m_email, "user@gameloft.com");
	m_emailFlag = false;
	
	///@LeaderBoard variable
	// Game score, see online design to calculate this number
	m_score = 350;
	m_isRankAround = 0;	
	// Array for store score of user (can store multiScore and/or supplemental data)...
	m_scoreArray = XP_API_NEW char[1024];
	XP_API_MEMSET( m_scoreArray, 0, 1024 );	
	
	m_errorCode = ERROR_NONE;
	
	m_frame = 0;
	
	m_errMsg = NULL;
	
	// Game data ...
	gameX = 0;
	gameY = 0;
	oppX = 0;
	oppY = 0;
	oldX = 0;
	oldY = 0;
	
	m_currentMsgType = USER_MESSAGE_TYPE_GAMEMESSAGE;
	m_gameMsgCount = 0;
	m_userMsgCount = 0;
	m_friendInvitedMsgCount = 0;
	m_gameInvitedMsgCount = 0;	
	m_chatInvitedMsgCount = 0;
	
	m_currentSessionIndex = -1;
	m_numberPlayer = 0;
	m_playerName = NULL;
	m_playerScore = NULL;
	
	m_isWin = true;

	bLobbyState = false;

	m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();	
	m_GLXPlayerLogin->RegisterObserver( this );

	XP_API_MEMSET( m_nationlity, 0, 8 );

	//m_pInGame = NULL;

	m_udp_socket = NULL;

	m_bHasReady = false;
	m_GameType = GAMETYPE_NONE;

#ifdef LOBBY_THREAD_TEST
	m_iMaxCount = 200;
#else
	m_iMaxCount = 2;
#endif
	m_bPrivateRoom = false;
	m_bLock = false;
	m_bTimer = true;
	m_bColor = true;

	m_LobbyInfo = NULL;

	m_iStartIndexLobby = 0;
	m_iCountQueryLobby = 10;
	m_iQueryTimer = 5;

	m_bLuanchedJoin = false;

	m_lobbyParamAndQuery = NULL;
	m_bVisibilityInFriendQuery = false;

	m_autoMatchType = AUTO_MATCH_NONE;
	m_sLobbyServerAddress = NULL;
}

GameStateMultiplayerLobby::~GameStateMultiplayerLobby()
{
	XP_API_DEL(m_sLobbyServerAddress);

	XP_API_DEL( m_lobbyParamAndQuery );

	LOBBY_LIST::iterator it;
	for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
	{
		LOBBY * lobby = *it;
		XP_API_DEL( lobby->name );
		XP_API_DEL( lobby );
	}

	m_lobby_list.clear();

	PLAYER_LIST::iterator it_p;
	for ( it_p=m_player_list.begin(); it_p!=m_player_list.end(); it_p++ )
	{
		PLAYER * player = *it_p;
		XP_API_DEL( player->name );
		XP_API_DEL( player->nationality );
		XP_API_DEL( player->userParameter );
		XP_API_DEL( player );
	}

	m_player_list.clear();

	if( m_LobbyInfo != NULL )
	{
		XP_API_DEL( m_LobbyInfo->name );
		XP_API_DEL( m_LobbyInfo->gameParam );
		XP_API_DEL( m_LobbyInfo->nationality );
		XP_API_DEL( m_LobbyInfo );
	}

	XP_API_DEL( m_GLXPlayerUser );
	XP_API_DEL( m_GLXPlayerLogin );
	XP_API_DEL( m_GLXPlayerMultiPlayer );
	XP_API_DEL( m_multiPlayerMenu );
	XP_API_DEL( m_multiPlayerMenuLobby );
	XP_API_DEL( m_InputBox );	
	XP_API_DEL( m_nationData );
	XP_API_DEL( m_email );
	XP_API_DEL( m_scoreArray );	
	XP_API_DEL( m_errMsg );
	XP_API_DEL( m_GLXPlayerUserFriend );
	XP_API_DEL_ARRAY( m_playerName , m_numberPlayer);
	XP_API_DEL( m_playerScore );


	DisGameServerSocket();
}


bool GameStateMultiplayerLobby::LoadProfile()
{
	XP_FILE_HANDLE pFile;

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "rb" );	
#else
	pFile = XP_API_FILE_OPEN( PROFILE_FILE_NAME, "rb" );
#endif

	XP_API_MEMSET(m_username, 0, 32);
	XP_API_MEMSET(m_password, 0, 32);
		
	if(!pFile)
	{
		XP_DEBUG_OUT("LoadProfile(), File %s not found \n", PROFILE_FILE_NAME);
		return false;
	}
	signed   int nRead;	
	
	nRead = XP_API_FILE_READ((void*)m_username,	1, sizeof(m_username),	pFile);
	nRead = XP_API_FILE_READ((void*)m_password,	1, sizeof(m_password),	pFile);	

//#ifdef LOBBY_THREAD_TEST
//	m_username[2] = '0';
//	m_username[3] = '\0';
//#endif

	XP_API_FILE_CLOSE(pFile);
	return true;
}

bool GameStateMultiplayerLobby::SaveProfile()
{
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "wb+" );	
#else
	pFile = XP_API_FILE_OPEN( PROFILE_FILE_NAME, "wb" );
#endif

	if(!pFile)
	{
		XP_DEBUG_OUT("SaveProfile(), Error creating file %s \n", PROFILE_FILE_NAME);
		return false;
	}

	signed   int nWritten;
	
	nWritten = XP_API_FILE_WRITE((void*)m_username,	1, sizeof(m_username),		pFile);
	nWritten = XP_API_FILE_WRITE((void*)m_password,	1, sizeof(m_password),		pFile);	

	XP_API_FILE_CLOSE(pFile);
	return true;
}

void GameStateMultiplayerLobby::Paint()
{
	switch(m_sub_state)
	{
		case ONLINE_LOGIN:
			m_handler->ClearScreen();
			DrawLogin(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_REGISTER:		
			m_handler->ClearScreen();
			DrawRegister(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	

		case ONLINE_PROCESS:		
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			//DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;	
		case ONLINE_ERROR:
			m_handler->ClearScreen();
			DrawError(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;			
		case ONLINE_MP_ERROR:
			m_handler->ClearScreen();
			DrawMPError(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;			
		//case ONLINE_LOBBY_LOGIN:
		//	m_handler->ClearScreen();
		//	DrawLogin(m_handler);
		//	DrawCommandBar(m_handler,GameState::CB_GO_BACK);
		//	break;

		case ONLINE_MP_PROCESS:
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			//DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;
		case ONLINE_MP_IN_SESSION:
			m_handler->ClearScreen();
			DrawSession(m_handler);
			break;
		case ONLINE_MP_IN_SESSION_LIST:
			m_handler->ClearScreen();
			DrawSessionList(m_handler);
			//DrawCommandBar(m_handler,GameState::CB_JOIN_BACK);
			break;

		case ONLINE_MP_IN_GAME_LOADING:
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			break;
		case ONLINE_MP_IN_GAME_LOADING_TIMEOUT:
			{
				m_handler->ClearScreen();
				int x = DEF::START_POS_X;
				int y = DEF::START_POS_Y;
				m_handler->DrawString(x,y,"--- Launch Game Time Out ---");

				DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			}
			break;
		case ONLINE_MP_IN_GAME:
			m_handler->ClearScreen();
			DrawInGame(m_handler);
			//DrawCommandBar(m_handler,GameState::CB_FINISH_LEAVE);		
			break;

		case ONLINE_MP_MENU:
			m_handler->ClearScreen();
			m_multiPlayerMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	
		case ONLINE_LOBBY_MENU:
			m_handler->ClearScreen();
			m_multiPlayerMenuLobby->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	

		case ONLINE_LOBBY_CREAT:
			m_handler->ClearScreen();
			DrawetCreatLobbyParameter(m_handler);
			break;
		case ONLINE_LOBBY_SET_PARAMETER:
			m_handler->ClearScreen();
			DrawetSetLobbyParameter(m_handler);
			break;
		case ONLINE_LOBBY_START_FAILED:
			m_handler->ClearScreen();
			DrawetStartFailed(m_handler);
			break;

		case ONLINE_LOBBY_LIST:
			m_handler->ClearScreen();
			DrawListQuery(m_handler);
			break;
		case ONLINE_LOBBY_LIST_NAME_QUERY:
			m_handler->ClearScreen();
			DrawListNameQuery(m_handler);
			break;
		case ONLINE_LOBBY_LIST_CONDITION_QUERY:
			m_handler->ClearScreen();
			DrawListConditionQuery(m_handler);
			break;
		case ONLINE_LOBBY_LIST_FRIEND_QUERY:
			m_handler->ClearScreen();
			DrawListFriendQuery(m_handler);
			break;

		case ONLINE_LOBBY_LIST_INFO:
			m_handler->ClearScreen();
			DrawListInfo(m_handler);
			break;
		case ONLINE_LOBBY_JOIN:
			m_handler->ClearScreen();
			DrawJoinLobby(m_handler);
			break;

		default:
			break;
	}

	if( 
#ifdef GET_SERVER_FROM_WEB
		m_sLobbyServerAddress != NULL && 
#endif
		m_GLXPlayerMultiPlayer->GetSecondConnecting()
		&& (ONLINE_MP_IN_GAME != m_sub_state && ONLINE_MP_IN_GAME_LOADING != m_sub_state 
			&& ONLINE_ERROR != m_sub_state && ONLINE_MP_ERROR != m_sub_state) 
	   )
	{
		m_handler->ClearScreen();
		DrawWaiting(m_handler);
		DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top-50,"connect failed, waiting for connecting again");
	}

	//auto match
	if(AUTO_MATCH_WAITING == m_autoMatchType)
	{
		m_handler->ClearScreen();
		DrawWaiting(m_handler);
		DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top-50,"filtering other player");
	}
	else if(AUTO_MATCH == m_autoMatchType)
	{
		m_handler->ClearScreen();
		DrawWaiting(m_handler);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top-50,"wait, has found other player");
	}
}

bool GameStateMultiplayerLobby::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if( 
#ifdef GET_SERVER_FROM_WEB
		m_sLobbyServerAddress != NULL && 
#endif
		m_GLXPlayerMultiPlayer->GetSecondConnecting() 
		&& (ONLINE_MP_IN_GAME != m_sub_state && ONLINE_MP_IN_GAME_LOADING != m_sub_state
		    && ONLINE_ERROR != m_sub_state && ONLINE_MP_ERROR != m_sub_state)
	  )
	{
		bHandled = HandleEventError( event );
		return bHandled;
	}

	if(AUTO_MATCH_WAITING == m_autoMatchType || AUTO_MATCH == m_autoMatchType)
	{
		if ( event.isClicked && m_SoftRight.Contain(event.position) && AUTO_MATCH_WAITING == m_autoMatchType )
		{
			m_autoMatchType = AUTO_MATCH_NONE;
			m_GLXPlayerMultiPlayer->mpSendCancelAutoMatchRequest();
			return true;
		}
		return bHandled;
	}

	switch(m_sub_state)
	{
		case ONLINE_LOGIN:
			bHandled = HandleEventLogin(event);
			break;		
/*		case ONLINE_LOBBY_LOGIN:
			bHandled = HandleEventLogin(event);
			break;	*/	
		case ONLINE_REGISTER:
			bHandled = HandleEventRegister(event);
			break;			

		case ONLINE_ERROR:	
		case ONLINE_MP_ERROR:		
			bHandled = HandleEventError( event );
			break;

		case ONLINE_MP_PROCESS:
		case ONLINE_PROCESS:		
			bHandled = HandleEventProcess( event );
			break;		

		case ONLINE_MP_MENU:		
			bHandled = HandleEventMPMenu( event );
			break;			
		case ONLINE_LOBBY_MENU:
			bHandled = HandleEventMPMenuLobby( event );
			break;		

		case ONLINE_LOBBY_CREAT:
			bHandled = HandleEventCreatLobby( event );
			break;
		
		case ONLINE_LOBBY_LIST:
			bHandled = HandleEventListQuery(event);
			break;
		case ONLINE_LOBBY_LIST_NAME_QUERY:
			bHandled = HandleEventListNameQuery(event);
			break;
		case ONLINE_LOBBY_LIST_CONDITION_QUERY:
			bHandled = HandleEventListCondotionQuery(event);
			break;
		case ONLINE_LOBBY_LIST_FRIEND_QUERY:
			bHandled = HandleEventListFriendQuery(event);
			break;

		case ONLINE_MP_IN_SESSION_LIST:		
			bHandled = HandleEventSessionList( event );
			break;	
		case ONLINE_LOBBY_LIST_INFO:
			bHandled = HandleEventListInfo(event);
			break;
		case ONLINE_LOBBY_JOIN:
			bHandled = HandleEventJoinLobby(event);
			break;

		case ONLINE_MP_IN_SESSION:		
			bHandled = HandleEventSession( event );
			break;	
		case ONLINE_LOBBY_SET_PARAMETER:
			bHandled = HandleEventSetLobbyParameter(event);
			break;
		case ONLINE_LOBBY_START_FAILED:
			bHandled = HandleEventStartFailed(event);
			break;

		case ONLINE_MP_IN_GAME_LOADING:
			if( XP_API_GET_TIME() - m_GLXPlayerMultiPlayer->GetLaunchGameTime() > LAUNCH_GAME_TIME_OUT )
			{
				SetOnlineSubState(ONLINE_MP_IN_GAME_LOADING_TIMEOUT);
			}
			break;
		case ONLINE_MP_IN_GAME:		
			bHandled = HandleEventInGame( event );
			break;	
		case ONLINE_MP_IN_GAME_LOADING_TIMEOUT:
			{
				bool bHandle = false;

				if( event.isClicked && m_SoftRight.Contain(event.position) )
				{
					if(m_GLXPlayerMultiPlayer->GetAutoMatchStatus())
					{
						m_autoMatchType = AUTO_MATCH_NONE;
						m_GLXPlayerMultiPlayer->SetAutoMatchStatus(false);
						SetOnlineSubState(ONLINE_LOBBY_MENU);
						m_GLXPlayerMultiPlayer->SetMPConnectLevel(CONNECT_LEVEL_IN_CONNECTED);
					}
					else
					{
						SetOnlineSubState(ONLINE_MP_PROCESS);

						m_GLXPlayerMultiPlayer->SetSendRejoinAfterGameOver( true );

						if( ! m_GLXPlayerMultiPlayer->GetSecondConnecting() )
							m_GLXPlayerMultiPlayer->mpSendRejoinLobby( GLXPlayerMPLobby::NORMAL_RETURN_TO_LOBBY );
					}
					bHandle = true;
				}

				return bHandle;
			}
			break;

		default:
			break;
	}
	return bHandled;
}


void GameStateMultiplayerLobby::DrawRegister( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	char tmp[128] = {0};
	handler->DrawString(x,y,"--- Register ---");	
	y+=2*DEF::LINE_HEIGHT;	
	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Username: %s", m_username);
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Password: %s", m_password);
	handler->DrawString(x,y,tmp);	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Nationality: %s", m_nationData);
	handler->DrawString(x,y,tmp);	
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Nationality must be two characters");
	handler->DrawString(x,y,tmp);	

	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Email: %s", m_email);
	handler->DrawString(x,y,tmp);	
	
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Receive Email: %d", m_emailFlag);
	handler->DrawString(x,y,tmp);	
	
	m_InputBox->showTextBox(handler);
}

bool GameStateMultiplayerLobby::HandleEventRegister( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestLogin) || g_bTestLogin ) )
	{
		if( m_bSendConnect )
		{
			SetOnlineSubState( ONLINE_PROCESS );
			m_GLXPlayerUser->sendRegister(m_username, m_password, m_email, m_nationData, m_emailFlag, true );	
			XP_DEBUG_OUT("GameStateMultiplayerLobby::HandleEventRegister current user name %s\n", m_username);

			m_iSendTime = XP_API_GET_TIME();
			char* s = NULL;
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "Register send: %s", s );
			XP_API_DEL( s );

			m_bSendConnect = false;
			return true;
		}
		else
			return false;
	}
#endif

	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
		m_GLXPlayerUser->sendRegister(m_username, m_password, m_email, m_nationData, m_emailFlag, true );
		m_InputBox->deleteTextInTextBox();
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MENU;
		m_selectedIndex = -1;
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < -1)
		{			
			m_selectedIndex = 2;			
		}	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 2)
		{	
			m_selectedIndex = -1;			
		}
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else 
	{
		bHandled = m_InputBox->updateInputBox(event);
		char* tmp = m_InputBox->getInputBoxString();
		if(m_selectedIndex == 0)
		{
			XP_API_MEMSET( m_username, 0, 15 );
			XP_API_STRCPY( m_username, tmp );						
		}
		else if(m_selectedIndex == 1)
		{
			XP_API_MEMSET( m_password, 0, 32 );
			XP_API_STRCPY( m_password, tmp );
		}
		else if(m_selectedIndex == 2)
		{			
			XP_API_DEL( m_nationData );
			m_nationData = XP_API_STRNEW( tmp );				
		}		
	}
	return bHandled;
}

void GameStateMultiplayerLobby::DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown )
{
	if(isDrawUpDown == true)
	{
		handler->DrawButton(m_UpKey);
		handler->DrawButton(m_DownKey);
	}
	switch( state )
	{
	case CB_CANCEL:
	case CB_NULL_BACK:
		handler->DrawButton(m_SoftRight);
		break;
	case CB_LEAVE_NULL:
		handler->DrawButton(m_SoftLeft);
		break;
	case CB_GO_BACK:	
	case CB_FINISH_LEAVE:	
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		break;
	case CB_START_KICKOUT:
	case CB_ACCEPT_IGNORE:
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		handler->DrawButton(m_SoftMid);
		break;
	case CB_JOIN_BACK:
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		handler->DrawButton(m_SoftMid);
		break;
	default:
		break;
	}

	GameState::DrawCommandBar(handler, state, isDrawUpDown);
}

void GameStateMultiplayerLobby::OnMPGameOver()
{
	//if( m_GLXPlayerMultiPlayer->GetSecondConnecting() )
	//{
	//	XP_DEBUG_OUT("GameStateMultiplayerLobby::OnMPGameOver SetSendRejoinAfterGameOver to true\n");
	//	m_GLXPlayerMultiPlayer->SetSendRejoinAfterGameOver( true );
	//}
	//else
	//{
	//	XP_DEBUG_OUT("GameStateMultiplayerLobby::OnMPGameOver mpSendRejoinLobby\n");
	//	m_GLXPlayerMultiPlayer->mpSendRejoinLobby( GLXPlayerMPLobby::NORMAL_RETURN_TO_LOBBY );
	//}
	
	m_GLXPlayerMultiPlayer->SetSendRejoinAfterGameOver( true );
	if( ! m_GLXPlayerMultiPlayer->GetSecondConnecting() )
	{
		XP_DEBUG_OUT("GameStateMultiplayerLobby::OnMPGameOver mpSendRejoinLobby\n");
		m_GLXPlayerMultiPlayer->mpSendRejoinLobby( GLXPlayerMPLobby::NORMAL_RETURN_TO_LOBBY );
	}
	SetOnlineSubState(ONLINE_MP_PROCESS);
}

void GameStateMultiplayerLobby::UpdateAnimation( )
{
	m_GLXPlayerLogin->Update();
	m_GLXPlayerUser->Update();	
	m_GLXPlayerUserFriend->Update();

#ifdef GET_SERVER_FROM_WEB
	if(m_sLobbyServerAddress != NULL)
#endif	
	{
		if( m_sub_state == ONLINE_MP_PROCESS || m_GLXPlayerMultiPlayer->mpIsConnected() || m_GLXPlayerMultiPlayer->GetSecondConnecting() )
		{
			if(m_GLXPlayerLogin->IsLoggedIn())
			{
				m_GLXPlayerMultiPlayer->Update();
			}
		}
	}

	GameServerPlay();

	m_frame++;
}

void GameStateMultiplayerLobby::SwitchState( )
{
	SetGameState(m_state);
}

void GameStateMultiplayerLobby::SetOnlineSubState( OnlineSubState state )
{
	if (m_sub_state != ONLINE_PROCESS && m_sub_state != ONLINE_MP_PROCESS
		&& m_sub_state != ONLINE_MP_ERROR && m_sub_state != ONLINE_ERROR)	
	{
		m_pre_sub_state = m_sub_state;
	}
	m_sub_state = state;
}

void GameStateMultiplayerLobby::OnMenuItemClicked( char * menu_name )
{
	if( XP_API_STRCMP( menu_name, MENU_LOBBY_LOGIN ) == 0 )
	{
		//m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
		//SetOnlineSubState(ONLINE_MP_PROCESS);
		//SetOnlineSubState(ONLINE_LOBBY_LOGIN);
		//bLobbyState = true;
	}
	else if( XP_API_STRCMP( menu_name, MENU_LOBBY_MENU_CREATE ) == 0 )
	{
		//m_GLXPlayerMultiPlayer->mpSendCreateLobby(m_username, 0, 1);
		//SetOnlineSubState(ONLINE_MP_PROCESS);
		SetOnlineSubState( ONLINE_LOBBY_CREAT );
		m_selectedIndex = 0;
	}
	else if( XP_API_STRCMP( menu_name, MENU_LOBBY_MENU_JOIN ) == 0 )
	{		
		//SetOnlineSubState(ONLINE_LOBBY_LIST_CONDITION_QUERY);		
		SetOnlineSubState(ONLINE_LOBBY_LIST);
		m_selectedIndex = 0;
	}
	else if(XP_API_STRCMP(menu_name, MENU_LOBBY_MENU_AUTOMATCH) == 0)
	{
		char playerParameter[20] = {0};
		XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
		char gameParameter[20] = {0};
		XP_API_SPRINTF( gameParameter, "%d", m_bTimer ? 5 : 1 );

		m_GLXPlayerMultiPlayer->mpSendAutoMatchRequest(gameParameter, playerParameter, 10, 3);

		SetOnlineSubState(ONLINE_MP_PROCESS);
	}
}

void GameStateMultiplayerLobby::DrawWaiting( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Processing ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char wait[] = "Please Wait ";
	char msg[32];
	int len = 0;

	len = XP_API_STRLEN(wait);
	XP_API_MEMSET(msg,0,32);
	XP_API_MEMCPY(msg,wait,len);
	
	for (int i=0; i<m_frame/2%7; i++)
	{
		msg[len++] = '.';
	}
	msg[len] = 0;
	handler->DrawString(x,y,msg);
}

void GameStateMultiplayerLobby::DrawLogin( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	char tmp[128] = {0};
	handler->DrawString(x,y,"--- Login ---");	
	y+=2*DEF::LINE_HEIGHT;	
	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Username: %s", m_username);
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Password: %s", m_password);
	handler->DrawString(x,y,tmp);	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	m_InputBox->showTextBox(handler);
}

void GameStateMultiplayerLobby::DrawError( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Error ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char msg[32];
	XP_API_MEMSET( msg, 0, 32 );
	XP_API_SPRINTF( msg, "Error code = %d", m_errorCode);

	handler->DrawString(x,y,msg);	
}

void GameStateMultiplayerLobby::DrawMPError(GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Multi Player Error ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char msg[32];
	XP_API_MEMSET( msg, 0, 32 );
	XP_API_SPRINTF( msg, "Error code: %d", m_errorCode);
	handler->DrawString(x,y,msg);	
	
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( msg, 0, 32 );
	XP_API_SPRINTF( msg, "Error Message: %s", m_errMsg);
	handler->DrawString(x,y,msg);
}

void GameStateMultiplayerLobby::DrawSession(GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	char temp[128];

	handler->DrawString(x, y, "--- Game Lobby ---");
	y += 2*DEF::LINE_HEIGHT;
	
	XP_API_MEMSET( temp, 0, 128 );
	XP_API_SPRINTF( temp, "lobby name: %s", m_lobbyname);
	//XP_API_SPRINTF( temp, "lobby name: %s", m_lobbyname );
	m_handler->DrawString( x, y, temp );
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET( temp, 0, 128 );
	XP_API_SPRINTF( temp, "my name: %s", m_username);
	//XP_API_SPRINTF( temp, "my name: %s", m_username );
	m_handler->DrawString( x, y, temp );
	y += 2*DEF::LINE_HEIGHT;

	if (m_GLXPlayerMultiPlayer->mpIsMaster())
	{
		m_handler->DrawButton(m_SoftLeft);
		m_handler->DrawButton(m_SoftRight);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Launch");
		m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Leave");
		
		m_handler->DrawButton(m_SoftMid);
		m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"kickout");

		m_handler->DrawButton(m_UpKey);
		m_handler->DrawButton(m_DownKey);
		m_handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
		m_handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

		m_handler->DrawButton(m_SoftAdd1);
		m_handler->DrawButton(m_SoftAdd2);
		m_handler->DrawString(m_SoftAdd1.left,m_SoftAdd1.top,"Ready");
		m_handler->DrawString(m_SoftAdd2.left,m_SoftAdd2.top,"Parameter");
	}
	else
	{
		if ( m_player_myself->status == USER_SPECTATOR )
		{
			m_handler->DrawButton(m_SoftRight);
			m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Leave");			
		}
		else
		{
			//m_handler->DrawButton(m_UpKey);
			//m_handler->DrawButton(m_DownKey);
			//m_handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
			//m_handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

			m_handler->DrawButton(m_SoftRight);
			m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Leave");
			
			m_handler->DrawButton(m_SoftAdd1);
			m_handler->DrawButton(m_SoftAdd2);
			m_handler->DrawString(m_SoftAdd1.left,m_SoftAdd1.top,"Ready");
			m_handler->DrawString(m_SoftAdd2.left,m_SoftAdd2.top,"Parameter");
		}
	}

	int count = m_player_list.size();	
	
	#define SIZE_OF_TMP 256
	char tmp[SIZE_OF_TMP] = {0};	

	if ( count > 0 )
	{
		XP_API_SPRINTF(tmp, "index name nationality role status parameter");
		handler->DrawString( x,y, tmp);
		y += DEF::LINE_HEIGHT;

		PLAYER_LIST::iterator it;
		int i = 1;
		for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
		{
			PLAYER * player = *it;

			char* name = player->name;

			XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
			if(m_GLXPlayerMultiPlayer->mpIsMaster())
			{
				if(m_curr_player_it == it)
					tmp[0] = '>';
			}
			if ( player->status == USER_PLAYER_NOT_READY )
			{
				XP_API_SPRINTF(&tmp[XP_API_STRLEN(tmp)], " %d. %s  %s  <player> unready %s",i,name,player->nationality,(player->userParameter[0]==48)?"BLUE":"GREEN");
			}
			else if ( player->status == USER_PLAYER_READY )
			{
				XP_API_SPRINTF(&tmp[XP_API_STRLEN(tmp)], " %d. %s  %s  <player>  ready  %s",i,name,player->nationality,(player->userParameter[0]==48)?"BLUE":"GREEN");
			}
			else if ( player->status == USER_SPECTATOR )
			{
				XP_API_SPRINTF(&tmp[XP_API_STRLEN(tmp)], " %d. %s  %s  <spectator>",i,name,player->nationality);
			}	
			else if ( player->status == USER_PLAYER_OFFLINE )
			{
				XP_API_SPRINTF(&tmp[XP_API_STRLEN(tmp)], " %d. %s  %s  <player> offline %s",i,name,player->nationality,(player->userParameter[0]==48)?"BLUE":"GREEN");
			}
			else if ( player->status == USER_SPECTATOR_OFFLINE )
			{
				XP_API_SPRINTF(&tmp[XP_API_STRLEN(tmp)], " %d. %s  %s  <spectator> offline",i,name,player->nationality);
			}

			handler->DrawString( x,y, tmp);
			y+=DEF::LINE_HEIGHT;
			i++;
		}		
	}
	else
	{
		XP_DEBUG_OUT( " init lobby failed");
	}

	if( m_bDrawGameParamChanged || m_bDrawUserParamChanged )
	{
		m_handler->DrawString(m_SoftLeft.left, m_SoftLeft.top - DEF::LINE_HEIGHT, m_szParamString);
		
		if( XP_API_GET_TIME() - timer > 1500 )
		{
			m_bDrawGameParamChanged = false;
			m_bDrawUserParamChanged = false;
		}
	}
}

void GameStateMultiplayerLobby::DrawSessionList(GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	#define SIZE_OF_TMP 256
	char tmp[SIZE_OF_TMP] = {0};

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");


	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Query");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Leave");

	handler->DrawString(x,y,"--- Lobby List ---");
	y+=2*DEF::LINE_HEIGHT;
	int count = m_lobby_list.size();	

	if ( count > 0 )
	{
		XP_API_SPRINTF(tmp, "index lobbyname nationality conut status");
		handler->DrawString( x,y, tmp);
		y += DEF::LINE_HEIGHT;

		LOBBY_LIST::iterator it;
		int i = 1;
		for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
		{
			LOBBY * lobby = *it;

			char* name = lobby->name;

			XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
			if ( it == m_curr_lobby_it )
			{
				XP_API_SPRINTF(tmp, "> %d. %s %s %d %s", i,name,lobby->nationality,lobby->conut,(lobby->status == 0)?"UNLAUNCHED":"LAUNCHED");
			}
			else
			{
				XP_API_SPRINTF(tmp, "  %d. %s %s %d %s", i,name,lobby->nationality,lobby->conut,(lobby->status == 0)?"UNLAUNCHED":"LAUNCHED");
			}

			handler->DrawString( x,y, tmp);
			y+=DEF::LINE_HEIGHT;
			i++;
		}		
	}
	else
	{
		handler->DrawString( x,y, "These is no lobby created ...");
	}
}

void GameStateMultiplayerLobby::DrawListQuery( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"--- Select query list ---");	
	y+=DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"SELECT");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );

	XP_API_SPRINTF( tmp, "Name query" );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_SPRINTF( tmp, "Condition query" );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;

	XP_API_SPRINTF( tmp, "Friend query" );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
}

void GameStateMultiplayerLobby::DrawListNameQuery( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"--- Name query ---");	
	y+=DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"QUERY");

	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Name: %s", m_sQueryName );
	handler->DrawString(x,y,tmp);
	handler->DrawString(x - 20 , y, ">");	

	m_InputBox->showTextBox(handler);
}

void GameStateMultiplayerLobby::DrawListConditionQuery( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"--- Query list Parameter ---");	
	y+=DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"QUERY");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "start index: %d", m_iStartIndexLobby);
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "count: %d", m_iCountQueryLobby);
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}

	y+=DEF::LINE_HEIGHT;	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "timer: %d", m_iQueryTimer );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "  notice: timer must be 5 or 1" );
	handler->DrawString(x,y,tmp);

	m_InputBox->showTextBox(handler);
}

void GameStateMultiplayerLobby::DrawListFriendQuery( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"--- Frind list Parameter ---");	
	y+=DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"QUERY");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"SELECT");

	char tmp[128] = {0};	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Visibility: %s", m_bVisibilityInFriendQuery ? "true" : "false" );
	handler->DrawString(x,y,tmp);
}

void GameStateMultiplayerLobby::DrawJoinLobby( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"SELECT");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "player" );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "spectator");
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	handler->DrawString(x,y,tmp);
}

void GameStateMultiplayerLobby::DrawListInfo( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"JOIN");

	char tmp[128] = {0};	

	XP_API_MEMSET( tmp, 0, 128 );
	char* name = NULL;
	XP_API_SPRINTF( tmp, "lobby name: %s", m_LobbyInfo->name);
	handler->DrawString(x,y,tmp);
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "game param: %s", m_LobbyInfo->gameParam);
	handler->DrawString(x,y,tmp);
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "private: %s", m_LobbyInfo->visibility == 0 ? "YES" : "NO");
	handler->DrawString(x,y,tmp);
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "locked: %s", m_LobbyInfo->locked == 0 ? "NO" : "YES");
	handler->DrawString(x,y,tmp);
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "userNum: %d", m_LobbyInfo->userNum);
	handler->DrawString(x,y,tmp);
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "spectatorNum: %d", m_LobbyInfo->spectatorNum);
	handler->DrawString(x,y,tmp);
}

void GameStateMultiplayerLobby::DrawetStartFailed( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::LINE_HEIGHT;	
	handler->DrawString(x, y, "not all player ready");	

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
}

void GameStateMultiplayerLobby::DrawetSetLobbyParameter( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = 2;
	handler->DrawString(x,y,"--- press select to change the parameter ---");	
	y+=DEF::LINE_HEIGHT;	
	handler->DrawString(x,y,"--- when ready, can not change the user parameter ---");	
	y+=DEF::LINE_HEIGHT;
	handler->DrawString(x,y,"--- but master can change the game parameter ---");	
	y+=DEF::LINE_HEIGHT;

	//draw button
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftMid);
	handler->DrawString(m_SoftMid.left,m_SoftMid.top,"SELECT");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};

	if ( m_GLXPlayerMultiPlayer->mpIsMaster() )
	{
		//Game Parameter
		y+=DEF::LINE_HEIGHT;
		handler->DrawString(x,y,"--- Game Parameter ---");	
		y+=DEF::LINE_HEIGHT;	

		XP_API_MEMSET( tmp, 0, 128 );
		XP_API_SPRINTF( tmp, "Timer: %d", m_bTimer ? 5 : 1 );
		handler->DrawString(x,y,tmp);
		if(m_selectedIndex == 0)
		{
			handler->DrawString(x - 20 , y, ">");	
		}
		y+=DEF::LINE_HEIGHT;

		//player Parameter
		y+=DEF::LINE_HEIGHT;
		handler->DrawString(x,y,"--- player Parameter ---");	
		y+=DEF::LINE_HEIGHT;	

		XP_API_MEMSET( tmp, 0, 128 );
		XP_API_SPRINTF( tmp, "Color: %s", m_bColor ? "BLUE" : "GREEN" );
		handler->DrawString(x,y,tmp);
		if(m_selectedIndex == 1)
		{
			handler->DrawString(x - 20 , y, ">");	
		}
	}
	else
	{
		//player Parameter
		y+=DEF::LINE_HEIGHT;
		handler->DrawString(x,y,"--- player Parameter ---");	
		y+=DEF::LINE_HEIGHT;	

		XP_API_MEMSET( tmp, 0, 128 );
		XP_API_SPRINTF( tmp, "Color: %s", m_bColor ? "BLUE" : "GREEN" );
		handler->DrawString(x,y,tmp);
		if(m_selectedIndex == 0)
		{
			handler->DrawString(x - 20 , y, ">");	
		}
	}
	y+=DEF::LINE_HEIGHT;

	if( m_bDrawGameParamChanged || m_bDrawUserParamChanged )
	{
		m_handler->DrawString(m_SoftLeft.left, m_SoftLeft.top - DEF::LINE_HEIGHT, m_szParamString);
		
		if( XP_API_GET_TIME() - timer > 1500 )
		{
			m_bDrawGameParamChanged = false;
			m_bDrawUserParamChanged = false;
		}
	}
}

void GameStateMultiplayerLobby::DrawetCreatLobbyParameter( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- press select to change the parameter ---");	
	y+=DEF::LINE_HEIGHT;	
	y+=DEF::LINE_HEIGHT;

	//draw create button
	handler->DrawButton(m_SoftLeft);
	handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"CREAT");
	handler->DrawButton(m_SoftRight);
	handler->DrawString(m_SoftRight.left,m_SoftRight.top,"LEAVE");
	handler->DrawButton(m_SoftMid);
	handler->DrawString(m_SoftMid.left,m_SoftMid.top,"SELECT");

	handler->DrawButton(m_UpKey);
	handler->DrawString(m_UpKey.left,m_UpKey.top,"UP");
	handler->DrawButton(m_DownKey);
	handler->DrawString(m_DownKey.left,m_DownKey.top,"DOWN");

	char tmp[128] = {0};

	//Lobby Parameter
	handler->DrawString(x,y,"--- Lobby Parameter ---");	
	y+=DEF::LINE_HEIGHT;	
	
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "MaxCount: %d", m_iMaxCount);
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "PrivateRoom: %s", m_bPrivateRoom ? "YES" : "NO");
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Lock: %s", m_bLock ? "YES" : "NO");
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	//Game Parameter
	y+=DEF::LINE_HEIGHT;
	handler->DrawString(x,y,"--- Game Parameter ---");	
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Timer: %d", m_bTimer ? 5 : 1 );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 3)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;	

	//player Parameter
	y+=DEF::LINE_HEIGHT;
	handler->DrawString(x,y,"--- player Parameter ---");	
	y+=DEF::LINE_HEIGHT;	

	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Color: %s", m_bColor ? "BLUE" : "GREEN" );
	handler->DrawString(x,y,tmp);
	if(m_selectedIndex == 4)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	y+=DEF::LINE_HEIGHT;
}

void GameStateMultiplayerLobby::DrawInGame(GameStateHandler * handler)
{	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	if(!m_bHasReady)
	{
		m_handler->DrawButton(m_SoftLeft);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"READY");
		return;
	}

	if( m_GameType == GAMETYPE_OVER )
	{
		m_handler->DrawButton(m_SoftLeft);
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"QUIT");
	}

	if( (*m_pGameMyself)->m_GameOverType == CInGame::GAMEOVERTYPE_LOSE )
	{
		m_handler->DrawString( x, y, " YOU LOST" );
		return;
	}
	else if( (*m_pGameMyself)->m_GameOverType == CInGame::GAMEOVERTYPE_WIN )
	{
		m_handler->DrawString( x, y, "YOU WIN" );
		return;
	}
	else if( (*m_pGameMyself)->m_GameOverType == CInGame::GAMEOVERTYPE_QUIT )
	{
		m_handler->DrawString( x, y, "GAME OVER" );
		return;
	}

	_TouchRect rect;

	//draw main rect 
	rect.left = SQUARE_SIZE;
	rect.top = y;
	rect.width = SQUARE_SIZE * MAX_SQUARE_COUNT;
	rect.height = SQUARE_SIZE * MAX_SQUARE_COUNT;
	m_handler->DrawRect(rect);

	//draw another rect 
	rect.left = SQUARE_SIZE + rect.width + SQUARE_SIZE;
	rect.top = y;
	rect.width = SQUARE_SIZE * MAX_SQUARE_COUNT * 8 / 10;
	rect.height = SQUARE_SIZE * MAX_SQUARE_COUNT * 8 / 10;
	m_handler->DrawRect(rect);

	if( m_pInGame.size() == 0 )
	{
		return;
	}

	//draw key
	if ( m_player_myself->status != USER_SPECTATOR )
	{
		m_handler->DrawButton(m_SoftLeft);
		m_handler->DrawButton(m_SoftRight);
		m_handler->DrawButton(m_UpKey);

		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"LEFT");
		m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"RIGHT");
		m_handler->DrawString(m_UpKey.left,m_UpKey.top,"CHANGE");
	}
	m_handler->DrawButton(m_DownKey);
	m_handler->DrawString(m_DownKey.left,m_DownKey.top,"QUIT");

	//draw game UI
	for(GAME_LIST::iterator it = m_pInGame.begin(); it != m_pInGame.end(); it++)
	{
		int		 numPlayer = 0;
		CInGame* pInGame = *it;

		int sizeSquare;
		if( pInGame == *m_pGameMyself ) 
		{
			numPlayer  = 0;
			sizeSquare = SQUARE_SIZE;
		}
		else					
		{
			numPlayer  = 1;
			sizeSquare = SQUARE_SIZE * 8 /10;
		}

		rect.left = SQUARE_SIZE + numPlayer * ( SQUARE_SIZE * MAX_SQUARE_COUNT + SQUARE_SIZE );
		rect.top = y;
		rect.width = sizeSquare;
		rect.height = sizeSquare;

		int j = 0;
		for(j = 0; j < MAX_SQUARE_COUNT; j++)
		{
			for(int i = 0; i < MAX_SQUARE_COUNT; i++)
			{
				if(pInGame->m_bFilledSquare[j][i])
					m_handler->DrawButton(rect);
				rect.left += sizeSquare;
			}
			rect.left = SQUARE_SIZE + numPlayer * ( SQUARE_SIZE * MAX_SQUARE_COUNT + SQUARE_SIZE );
			rect.top += sizeSquare;
		}

		for(j = 0; j < 4; j++)
		{
			rect.left = SQUARE_SIZE + numPlayer * ( SQUARE_SIZE * MAX_SQUARE_COUNT + SQUARE_SIZE )
				        + pInGame->sq.ps[j].x * sizeSquare;
			rect.top = y + pInGame->sq.ps[j].y * sizeSquare;
			m_handler->DrawButton(rect);
		}
	}
}

bool GameStateMultiplayerLobby::HandleEventLogin( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
#ifdef LOBBY_THREAD_TEST
	m_iSendTime = XP_API_GET_TIME();

	if( m_bTestThread && ( (g_bRandom && m_bTestLogin) || g_bTestLogin ) )
	{
		if( m_bSendConnect )
		{
			SetOnlineSubState( ONLINE_PROCESS );

			m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login_Game, NULL, true);
			XP_DEBUG_OUT("HandleEventLogin current user name %s\n", m_username);

			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "$LQGUO$: %s", m_username );
			char* s = NULL;
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "LoginWeb send: %s", s );
			XP_API_DEL( s );
			m_log->setUserName(m_username);
			m_bSendConnect = false;
			return true;
		}
		else
			return false;
	}
#endif

	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login_Game, NULL, true);
#endif
		//m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
		//SetOnlineSubState(ONLINE_MP_PROCESS);

		m_InputBox->deleteTextInTextBox();
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MENU;
		m_selectedIndex = -1;
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < -1)
		{			
			m_selectedIndex = 1;			
		}	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 1)
		{	
			m_selectedIndex = -1;			
		}
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else 
	{
		bHandled = m_InputBox->updateInputBox(event);
		if ( bHandled )
		{
			char* tmp = m_InputBox->getInputBoxString();
			if(m_selectedIndex == 0)
			{
				XP_API_MEMSET( m_username, 0, 32 );
				XP_API_MEMCPY( m_username, tmp, XP_API_STRLEN( tmp ) );
			}
			else if(m_selectedIndex == 1)
			{
				XP_API_MEMSET( m_password, 0, 32 );
				XP_API_STRCPY( m_password, tmp );
			}
		}
	}
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventMPMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_multiPlayerMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_state = DEF::GAME_STATE_MENU;		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventMPMenuLobby( _TOUCH_EVENT event )
{
	bool bHandled = false;

#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestCreate) || g_bTestCreate ) )
	{
		if( m_bSendConnect )
		{
			BYTE bVisibility = 1, bLock = 0;
			if( m_bPrivateRoom )  bVisibility = 0;
			if( m_bLock )		  bLock = 1;

			char playerParameter[20] = {0};
			XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
			char gameParameter[20] = {0};
			XP_API_SPRINTF( gameParameter, "%d", m_bTimer ? 5 : 1 );

			XP_API_DEL( m_lobbyParamAndQuery );
			m_lobbyParamAndQuery = XP_API_NEW CLobbyParameterAndQuery();
			m_lobbyParamAndQuery->SetParameterValue( 0, gameParameter );

			char* s = NULL;
			m_iSendTime = XP_API_GET_TIME();
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "CreatLobby send: %s", s );
			XP_API_DEL( s );
			m_GLXPlayerMultiPlayer->mpSendCreateLobby( m_username, bVisibility, bLock, m_iMaxCount, gameParameter, playerParameter, m_lobbyParamAndQuery );
			m_log->setSendLobbyCreate();
			XP_API_DEL( m_lobbyParamAndQuery );

			SetOnlineSubState(ONLINE_MP_PROCESS);

			m_bSendConnect = false;

			bHandled = true;
		}
	}
	if( m_bTestThread && ( (g_bRandom && m_bTestFilter) || g_bTestFilter ) )
	{
		if( m_bSendConnect )
		{
			char gameParameter[20] = {0};
			XP_API_SPRINTF( gameParameter, "%d", m_iQueryTimer );

			XP_API_DEL( m_lobbyParamAndQuery );
			m_lobbyParamAndQuery = XP_API_NEW CLobbyParameterAndQuery();
			m_lobbyParamAndQuery->AddQueryCondition( 0, CMP_EQUAL, gameParameter );

			char* s = NULL;
			GetCurrentSystemTime( &s );
			m_iSendTime = XP_API_GET_TIME();
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "FilterLobby send: %s", s );
			XP_API_DEL( s );
			m_GLXPlayerMultiPlayer->mpSendGetLobbyList( m_iUserNumber, m_iStartIndexLobby, m_iCountQueryLobby, m_lobbyParamAndQuery );
			m_log->setSendLobbyFilter();
			SetOnlineSubState(ONLINE_MP_PROCESS);	

			XP_API_DEL( m_lobbyParamAndQuery );

			m_bSendConnect = false;

			bHandled = true;
		}
	}
#endif

	if ( m_multiPlayerMenuLobby->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_PROCESS);
		m_GLXPlayerMultiPlayer->mpSendDisconnect();
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventError( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		if(m_errorCode == ERROR_USER_HAS_LOGIN || m_errorCode == ERROR_HAS_NOT_LOGIN || m_errorCode == ERROR_NO_UUID )
		{
			m_state = DEF::GAME_STATE_MENU;
			//SetOnlineSubState(ONLINE_MP_MENU);
		}
		else if(m_sub_state == ONLINE_LOBBY_MENU && m_errorCode == ERROR_CONNECTION)
		{
			m_state = DEF::GAME_STATE_MENU;
			//SetOnlineSubState(ONLINE_MP_MENU);
		}
		else
		{
			if( m_errorCode == ERROR_CONNECTION || ERROR_REQUEST_TIME_OUT == m_errorCode 
				|| ERROR_LOBBY_TIME_OUT == m_errorCode || ERROR_SAMEACCOUNT_LOGIN == m_errorCode
				)
			{
				//for make sure not second connect when disconnect
				if(m_GLXPlayerMultiPlayer != NULL)
					m_GLXPlayerMultiPlayer->SetSecondConnecting( false );
				m_state = DEF::GAME_STATE_MENU;
			}
			else
			{
	#ifdef LOBBY_THREAD_TEST
				//add by heyi
				if( m_bTestThread && ( (g_bRandom && m_bTestLogin) || g_bTestLogin ) )
					m_state = DEF::GAME_STATE_MENU;
				else
	#endif
				{	
					if(ERROR_LOBBY_TIME_OUT == m_errorCode)
						SetOnlineSubState(ONLINE_LOBBY_MENU);
					else
						SetOnlineSubState(m_pre_sub_state);
				}
			}			
		}
		bHandled = true;
	}

	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventProcess( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	return false;

	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		if(m_GLXPlayerLogin->IsLoggedIn() == false || m_GLXPlayerMultiPlayer->GetMPConnectOrNot() )
		{
			m_GLXPlayerLogin->Cancel();
			m_state = DEF::GAME_STATE_MENU;
		}
		else
		{
			m_GLXPlayerUser->Cancel();
			SetOnlineSubState(ONLINE_LOBBY_MENU);
		}	
		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventSession( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	int iCountPlayer = m_player_list.size();

	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;

		if( player->status == USER_SPECTATOR)
			iCountPlayer--;
	}

	if (m_GLXPlayerMultiPlayer->mpIsMaster())
	{
		if ( event.isClicked && m_SoftRight.Contain(event.position) )
		{
			SetOnlineSubState(ONLINE_MP_PROCESS);	

			m_GLXPlayerMultiPlayer->mpSendLeaveLobby( /*m_lobby_id*/ );
			bHandled = true;
		}	
		else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
		{
			if(iCountPlayer < 3 && iCountPlayer > 0)
			{
				SetOnlineSubState(ONLINE_MP_PROCESS);

				m_GLXPlayerMultiPlayer->mpSendLunchLobbyGame();

				m_selectedIndex = -1;
	
			}
			else
			{
				m_handler->DrawString(m_SoftLeft.left, m_SoftLeft.top - 2 * DEF::START_POS_Y, "the count of player should be 1 or 2");
			}
			bHandled = true;
		}
		else if( event.isClicked && m_UpKey.Contain(event.position) )
		{			
			if ( m_player_list.size() > 0 )
			{
				if ( m_curr_player_it == m_player_list.begin() )
				{
					m_curr_player_it = m_player_list.end();
				}
				//else
				{
					m_curr_player_it--;
				}
				bHandled = true;
			}
		}
		else if( event.isClicked && m_DownKey.Contain(event.position) )
		{	
			if ( m_player_list.size() > 0 )
			{
				m_curr_player_it++;
				if ( m_curr_player_it == m_player_list.end() )
				{
					m_curr_player_it = m_player_list.begin();
				}
				//else
				//{
				//	m_curr_player_it++;
				//}
				bHandled = true;
			}
		}
		else if(event.isClicked && m_SoftAdd1.Contain(event.position))
		{
			if( m_player_myself->status == USER_PLAYER_NOT_READY )
				m_GLXPlayerMultiPlayer->mpSendSetPlayerStatus(USER_PLAYER_READY);
			//else if(m_player_myself->status == USER_PLAYER_READY)
			//	m_GLXPlayerMultiPlayer->mpSendSetPlayerStatus(USER_PLAYER_NOT_READY);

			bHandled = true;						
		}
		else if(event.isClicked && m_SoftAdd2.Contain(event.position))
		{
			SetOnlineSubState(ONLINE_LOBBY_SET_PARAMETER);
			m_selectedIndex = 0;
			bHandled = true;					
		}
		else if(event.isClicked && m_SoftMid.Contain(event.position))
		{
			if(XP_API_STRCMP(m_username, (*m_curr_player_it)->name) != 0)
			{
				m_GLXPlayerMultiPlayer->mpSendKickOut((*m_curr_player_it)->name);
				SetOnlineSubState(ONLINE_MP_PROCESS);
				bHandled = true;
			}
		}
	}
	else
	{
		//if(!m_GLXPlayerMultiPlayer->mpIsInSession())
		//{			
		//	SetOnlineSubState(ONLINE_LOBBY_MENU);
		//	bHandled = true;
		//}	
		//
		if ( event.isClicked && m_SoftRight.Contain(event.position) )
		{
			SetOnlineSubState(ONLINE_MP_PROCESS);	

			m_GLXPlayerMultiPlayer->mpSendLeaveLobby( /*m_lobby_id*/ );

			bHandled = true;
		}				
		else if( event.isClicked && m_SoftAdd1.Contain(event.position) )
		{			
			if( m_player_myself->status == USER_PLAYER_NOT_READY )
				m_GLXPlayerMultiPlayer->mpSendSetPlayerStatus(USER_PLAYER_READY);
			//lse if(m_player_myself->status == USER_PLAYER_READY)
			//	m_GLXPlayerMultiPlayer->mpSendSetPlayerStatus(USER_PLAYER_NOT_READY);

			bHandled = true;						
		}
		else if( event.isClicked && m_SoftAdd2.Contain(event.position) )
		{			
			SetOnlineSubState(ONLINE_LOBBY_SET_PARAMETER);
			m_selectedIndex = 0;
			bHandled = true;					
		}
	}	
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventSessionList( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
	int count = m_lobby_list.size();	
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) && count > 0 )
	{		
		if ( NULL != *m_curr_lobby_it )
		{
			m_GLXPlayerMultiPlayer->mpSendGetLobbyInfo( (*m_curr_lobby_it)->id );
			bHandled = true;
		}
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_LOBBY_MENU);
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) && count > 0 )
	{		
		if ( m_lobby_list.size() > 1 )
		{
			if ( m_curr_lobby_it == m_lobby_list.begin() )
			{
				m_curr_lobby_it = m_lobby_list.end();
			}
			else
			{
				m_curr_lobby_it--;
			}
			bHandled = true;
		}			
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) && count > 0 )
	{		
		if ( m_lobby_list.size() > 1 )
		{
			if ( m_curr_lobby_it == m_lobby_list.end() )
			{
				m_curr_lobby_it = m_lobby_list.begin();
			}
			else
			{
				m_curr_lobby_it++;
			}
			bHandled = true;
		}		
	}
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventJoinLobby( _TOUCH_EVENT event )
{
	bool bHandled = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_LOBBY_LIST_INFO);

		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{			
			m_selectedIndex = 1;			
		}	
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 1)
		{	
			m_selectedIndex = 0;			
		}
		bHandled = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		char playerParameter[20] = {0};
		XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
		if( m_selectedIndex == 0 )
		{		
			if ( NULL != *m_curr_lobby_it )
			{

				m_user_status = USER_PLAYER_NOT_READY;
				m_GLXPlayerMultiPlayer->mpSendJoinLobby( (*m_curr_lobby_it)->id, m_user_status, playerParameter );
				bHandled = true;
				SetOnlineSubState(ONLINE_MP_PROCESS);		
			}
		}
		else if( m_selectedIndex == 1 )
		{		
			if ( NULL != *m_curr_lobby_it )
			{
				m_user_status = USER_SPECTATOR;
				m_GLXPlayerMultiPlayer->mpSendJoinLobby( (*m_curr_lobby_it)->id, m_user_status, playerParameter );
				bHandled = true;
				SetOnlineSubState(ONLINE_MP_PROCESS);		
			}
		}
	}

	return bHandled;

}

bool GameStateMultiplayerLobby::HandleEventListQuery( _TOUCH_EVENT event )
{
	bool bHandled = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_LOBBY_MENU);
		bHandled = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if( m_selectedIndex == 0 )
		{
			m_selectedIndex = 0;
			XP_API_MEMSET( m_sQueryName, 0, NAME_QUERY_MAX_CHARACTOR );
			SetOnlineSubState(ONLINE_LOBBY_LIST_NAME_QUERY);
		}
		else if( m_selectedIndex == 1 )
		{
			m_selectedIndex = 0;
			SetOnlineSubState(ONLINE_LOBBY_LIST_CONDITION_QUERY);
		}
		else if( m_selectedIndex == 2 )
			SetOnlineSubState(ONLINE_LOBBY_LIST_FRIEND_QUERY);

		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{			
			m_selectedIndex = 2;			
		}	
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 2)
		{	
			m_selectedIndex = 0;			
		}
		bHandled = true;
	}
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventListNameQuery( _TOUCH_EVENT event )
{
	bool bHandled = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_selectedIndex = 0;
		SetOnlineSubState(ONLINE_LOBBY_LIST);//(ONLINE_LOBBY_MENU);
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if( m_sQueryName[0] != '\0' )
		{
#ifdef LOBBY_LIST_WITH_GAME_PARAM
			m_GLXPlayerMultiPlayer->mpSendGetLobbyForNameWithGameParam( m_sQueryName );
#else
			m_GLXPlayerMultiPlayer->mpSendGetLobbyForName( m_sQueryName );
#endif

			SetOnlineSubState(ONLINE_MP_PROCESS);	
			m_InputBox->deleteTextInTextBox();
			bHandled = true;
		}
	}
	else
	{
		bHandled = m_InputBox->updateInputBox(event);
		if ( bHandled )
		{
			char* tmp = m_InputBox->getInputBoxString();
			if( XP_API_STRLEN(tmp) < NAME_QUERY_MAX_CHARACTOR )
			{
				XP_API_MEMSET( m_sQueryName, 0, NAME_QUERY_MAX_CHARACTOR );
				XP_API_STRCPY( m_sQueryName, tmp );
			}
		}
	}

	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventListCondotionQuery( _TOUCH_EVENT event )
{
	bool bHandled = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_selectedIndex = 0;
		SetOnlineSubState(ONLINE_LOBBY_LIST);//(ONLINE_LOBBY_MENU);
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		char gameParameter[20] = {0};
		XP_API_SPRINTF( gameParameter, "%d", m_iQueryTimer );

		XP_API_DEL( m_lobbyParamAndQuery );
		m_lobbyParamAndQuery = XP_API_NEW CLobbyParameterAndQuery();
		m_lobbyParamAndQuery->AddQueryCondition( 0, CMP_EQUAL, gameParameter );

#ifdef LOBBY_LIST_WITH_GAME_PARAM
		m_GLXPlayerMultiPlayer->mpSendGetLobbyListWithGameParam( m_iUserNumber, m_iStartIndexLobby, m_iCountQueryLobby, m_lobbyParamAndQuery );
#else
		m_GLXPlayerMultiPlayer->mpSendGetLobbyList( m_iUserNumber, m_iStartIndexLobby, m_iCountQueryLobby, m_lobbyParamAndQuery );
#endif	
		SetOnlineSubState(ONLINE_MP_PROCESS);	
		
		XP_API_DEL( m_lobbyParamAndQuery );

		m_InputBox->deleteTextInTextBox();

		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{			
			m_selectedIndex = 2;			
		}	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 2)
		{	
			m_selectedIndex = 0;			
		}
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else
	{
		bHandled = m_InputBox->updateInputBox(event);
		if ( bHandled )
		{
			char* tmp = m_InputBox->getInputBoxString();

			int len = XP_API_STRLEN( tmp );
			if( len > 0 )
			{
				if( tmp[len-1] >= 48 && tmp[len-1] <= 48 + 9 )
				{
					if(m_selectedIndex == 0)
					{
						m_iStartIndexLobby = XP_API_ATOI( tmp );
					}
					else if(m_selectedIndex == 1)
					{
						m_iCountQueryLobby = XP_API_ATOI( tmp );
					}
					else if(m_selectedIndex == 2)
					{
						int timer = XP_API_ATOI( tmp );
						if( timer == 5 || timer == 1)
						{
							m_iQueryTimer = timer;
						}
						m_InputBox->deleteTextInTextBox();
					}
				}
				else
				{
					m_InputBox->delLastInputBoxCharactor();
				}
			}
		}
	}
	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventListFriendQuery( _TOUCH_EVENT event )
{
	bool bHandled = false;
    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_selectedIndex = 0;
		SetOnlineSubState(ONLINE_LOBBY_LIST);//(ONLINE_LOBBY_MENU);
		bHandled = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		m_GLXPlayerUserFriend->sendGetUserFriends();
//		m_GLXPlayerMultiPlayer->mpSendGetLobbyForFriend( m_iUserNumber, m_bVisibilityInFriendQuery ? 1 : 0, m_GLXPlayerUserFriend );
	
		SetOnlineSubState(ONLINE_MP_PROCESS);
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_bVisibilityInFriendQuery = !m_bVisibilityInFriendQuery;
	}

	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventListInfo( _TOUCH_EVENT event )
{
	bool bHandle = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);

		bHandle = true;
	}
	else if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_LOBBY_JOIN);
		m_selectedIndex = 0;
		bHandle = true;
	}

	return bHandle;
}

bool GameStateMultiplayerLobby::HandleEventStartFailed( _TOUCH_EVENT event )
{
	bool bHandle = false;

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_IN_SESSION);

		bHandle = true;
	}

	return bHandle;
}

bool GameStateMultiplayerLobby::HandleEventSetLobbyParameter( _TOUCH_EVENT event )
{
	bool bHandled = false;	

    if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_IN_SESSION);

		bHandled = true;
	}
	else if( event.isClicked && m_SoftMid.Contain(event.position) )
	{

		if ( m_GLXPlayerMultiPlayer->mpIsMaster() )
		{
			if( m_selectedIndex == 0 )
			{
				m_bTimer = !m_bTimer;

				char gameParameter[20] = {0};
				XP_API_SPRINTF( gameParameter, "%d", m_bTimer ? 5 : 1 );

				XP_API_DEL( m_lobbyParamAndQuery );
				m_lobbyParamAndQuery = XP_API_NEW CLobbyParameterAndQuery();
				m_lobbyParamAndQuery->SetParameterValue( 0, gameParameter );

				m_GLXPlayerMultiPlayer->mpSendSetGameParameter( gameParameter, XP_API_STRLEN( gameParameter ), m_lobbyParamAndQuery );
				//SetOnlineSubState(ONLINE_MP_PROCESS);

				XP_API_DEL( m_lobbyParamAndQuery );
			}	
			else if( m_selectedIndex == 1 )
			{
				if( m_player_myself->status == USER_PLAYER_NOT_READY )
				{
					m_bColor = !m_bColor;

					char playerParameter[20] = {0};
					XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );

					m_GLXPlayerMultiPlayer->mpSendSetUserParameter( playerParameter, XP_API_STRLEN( playerParameter ) );
					//SetOnlineSubState(ONLINE_MP_PROCESS);
				}
			}
		}
		else
		{
			if( m_selectedIndex == 0 )
			{
				if( m_player_myself->status == USER_PLAYER_NOT_READY )
				{
					m_bColor = !m_bColor;

					char playerParameter[20] = {0};
					XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );

					m_GLXPlayerMultiPlayer->mpSendSetUserParameter( playerParameter, XP_API_STRLEN( playerParameter ) );
					//SetOnlineSubState(ONLINE_MP_PROCESS);
				}
			}
		}
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		

		if ( m_GLXPlayerMultiPlayer->mpIsMaster() )
		{
			if(m_selectedIndex < 0)
			{			
				m_selectedIndex = 1;			
			}	
		}
		else
		{
			if(m_selectedIndex < 0)
			{			
				m_selectedIndex = 0;			
			}	
		}

		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;

		if ( m_GLXPlayerMultiPlayer->mpIsMaster() )
		{
			if(m_selectedIndex > 1)
			{	
				m_selectedIndex = 0;			
			}
		}
		else
		{
			if(m_selectedIndex > 0)
			{			
				m_selectedIndex = 0;			
			}
		}

		bHandled = true;
	}

	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventCreatLobby( _TOUCH_EVENT event )
{
	bool bHandled = false;	

	if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		BYTE bVisibility = 1, bLock = 0;
		if( m_bPrivateRoom )  bVisibility = 0;
		if( m_bLock )		  bLock = 1;

		char playerParameter[20] = {0};
		XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
		char gameParameter[20] = {0};
		XP_API_SPRINTF( gameParameter, "%d", m_bTimer ? 5 : 1 );

		XP_API_DEL( m_lobbyParamAndQuery );
		m_lobbyParamAndQuery = XP_API_NEW CLobbyParameterAndQuery();
		m_lobbyParamAndQuery->SetParameterValue( 0, gameParameter );

		m_GLXPlayerMultiPlayer->mpSendCreateLobby( m_username, bVisibility, bLock, m_iMaxCount, gameParameter, playerParameter, m_lobbyParamAndQuery );
		
		XP_API_DEL( m_lobbyParamAndQuery );

		SetOnlineSubState(ONLINE_MP_PROCESS);

		bHandled = true;
	}
	else if( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_LOBBY_MENU);

		bHandled = true;
	}
	else if( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		if( m_selectedIndex == 0 )
			;
		else if( m_selectedIndex == 1 )
		{
			m_bPrivateRoom = !m_bPrivateRoom;
		}
		else if( m_selectedIndex == 2 )
		{
			m_bLock = !m_bLock;
		}
		else if( m_selectedIndex == 3 )
		{
			m_bTimer = !m_bTimer;
		}	
		else if( m_selectedIndex == 4 )
		{
			m_bColor = !m_bColor;
		}

		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{			
			m_selectedIndex = 4;			
		}	
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 4)
		{	
			m_selectedIndex = 0;			
		}
		bHandled = true;
	}

	return bHandled;
}

bool GameStateMultiplayerLobby::HandleEventInGame( _TOUCH_EVENT event )
{
	bool bHandled = false;	

	if( m_GameType == GAMETYPE_OVER )
	{
		if( event.isClicked && m_SoftLeft.Contain(event.position) )
		{
			DisGameServerSocket();
			//m_state = DEF::GAME_STATE_MENU;

			if(m_GLXPlayerMultiPlayer->GetAutoMatchStatus())
			{
				m_GLXPlayerMultiPlayer->SetAutoMatchStatus(false);
				SetOnlineSubState(ONLINE_LOBBY_MENU);
				m_autoMatchType = AUTO_MATCH_NONE;
				m_GLXPlayerMultiPlayer->SetMPConnectLevel(CONNECT_LEVEL_IN_CONNECTED);
			}
			else
			{
				OnMPGameOver();
			}
		}
	}

	if(!m_bHasReady || m_player_myself->status == USER_SPECTATOR)
	{
		if( event.isClicked && m_SoftLeft.Contain(event.position) || (!m_bHasReady && m_player_myself->status == USER_SPECTATOR) )
		{
			DataPacket * packet = XP_API_NEW DataPacketLobby();

			if( m_bLuanchedJoin )
				packet->InitMessage( JOIN );
			else
				packet->InitMessage( READY );

			packet->addShortLenString( m_username, XP_API_STRLEN( m_username ) );

			if( m_bLuanchedJoin )
			{
				packet->addByte( m_user_status );

				char playerParameter[20] = {0};
				XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
				packet->addBlob(playerParameter, XP_API_STRLEN(playerParameter));

				m_bLuanchedJoin = false;
			}

			packet->packMessage();
			if ( NULL != m_udp_socket )
			{
				m_udp_socket->SendTo( (const char *)packet->getMessageBody(), packet->getMessageLen(), m_input_server, m_udp_port);
			}
			XP_API_DEL(packet);

			m_bHasReady = true;		

			bHandled = true;
		}
		else if( event.isClicked && m_DownKey.Contain(event.position) && m_bHasReady )
		{	
			(*m_pGameMyself)->m_GameOverType = (CInGame::GAMEOVERTYPE)CInGame::GAMEOVERTYPE_QUIT;
			m_GameType = GAMETYPE_OVER;

			bHandled = true;
		}

		return bHandled;
	}

	if( m_pInGame.size() == 0 ) return bHandled;

	if( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		DataPacket * packet = XP_API_NEW DataPacketLobby();
		packet->InitMessage( KEY );

		packet->addShortLenString( m_username, XP_API_STRLEN( m_username ) );

		packet->addInt( 1 );
		packet->packMessage();
		if ( NULL != m_udp_socket )
		{
			m_udp_socket->SendTo( (const char *)packet->getMessageBody(), packet->getMessageLen(), m_input_server, m_udp_port);
		}
		XP_API_DEL(packet);

		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		DataPacket * packet = XP_API_NEW DataPacketLobby();
		packet->InitMessage( KEY );

		packet->addShortLenString( m_username, XP_API_STRLEN( m_username ) );

		packet->addInt( 2 );
		packet->packMessage();
		if ( NULL != m_udp_socket )
		{
			m_udp_socket->SendTo( (const char *)packet->getMessageBody(), packet->getMessageLen(), m_input_server, m_udp_port);
		}
		XP_API_DEL(packet);

		bHandled = true;
	}
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{	
		DataPacket * packet = XP_API_NEW DataPacketLobby();
		packet->InitMessage( KEY );

		packet->addShortLenString( m_username, XP_API_STRLEN( m_username ) );

		packet->addInt( 3 );
		packet->packMessage();
		if ( NULL != m_udp_socket )
		{
			m_udp_socket->SendTo( (const char *)packet->getMessageBody(), packet->getMessageLen(), m_input_server, m_udp_port);
		}
		XP_API_DEL(packet);

		bHandled = true;
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		(*m_pGameMyself)->m_GameOverType = (CInGame::GAMEOVERTYPE)CInGame::GAMEOVERTYPE_LOSE;
		m_GameType = GAMETYPE_OVER;

		bHandled = true;
	}

	return bHandled;
}

void GameStateMultiplayerLobby::OnMPNetworkError(int errorCode)
{
	if( m_GLXPlayerMultiPlayer->GetSecondConnecting() 
		&& ONLINE_PROCESS != m_sub_state && ONLINE_MP_PROCESS != m_sub_state
		&& ONLINE_ERROR != m_sub_state && ONLINE_MP_ERROR != m_sub_state//make sure no message send before lobby crash and alive send
		&& !m_bSameAccountLogin
		)
	{
		m_errorCode = errorCode;
		XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnMPNetworkError: %d\n", m_errorCode);

#ifdef LOBBY_THREAD_TEST
		m_iSendTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "second connect send: %s", s );
		XP_API_DEL( s );
#endif
		m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
	}
	else if( ONLINE_MP_IN_GAME != m_sub_state && ONLINE_MP_IN_GAME_LOADING != m_sub_state )//no ui opeator when player has entioned the game server
	{
		SetOnlineSubState(ONLINE_MP_ERROR);

		if(m_bSameAccountLogin)
			m_errorCode = ERROR_SAMEACCOUNT_LOGIN;
		else
			m_errorCode = errorCode;
		XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnMPNetworkError: %d\n", m_errorCode);

#ifdef LOBBY_THREAD_TEST
		m_iRequestTime = XP_API_GET_TIME();
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: Network error ID: %d time: %d", m_errorCode, m_iRequestTime - m_iSendTime );
		
		SaveTestLog();
#endif
	}
}

void GameStateMultiplayerLobby::OnMPFunctionError( char* errMsg, int error_code )
{
	SetOnlineSubState(ONLINE_MP_ERROR);
	m_errorCode = error_code;
	m_errMsg = errMsg;
	XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnMPFunctionError: %d\n", m_errorCode);

#ifdef LOBBY_THREAD_TEST
	m_iRequestTime = XP_API_GET_TIME();
	XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: Function error ID: %d  time: %d  msg: %s ", m_errorCode, m_iRequestTime - m_iSendTime, errMsg );
	SaveTestLog();
#endif
}

void GameStateMultiplayerLobby::OnMPEstablishConnectionSuccess()
{	
#ifdef LOBBY_THREAD_TEST
	m_iRequestTime = XP_API_GET_TIME();
	char* s = NULL;
	GetCurrentSystemTime( &s );
	XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "ConnectLobby: %s %d ", s, m_iRequestTime - m_iSendTime );
	m_log->setlobbyConnectTime(m_iRequestTime - m_iSendTime);
	m_iSendTime = XP_API_GET_TIME();
	XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "LoginLobby send: %s", s );
	XP_API_DEL( s );
#endif
	if(m_GLXPlayerMultiPlayer->GetSecondConnecting())
		m_GLXPlayerMultiPlayer->mpSendLobbyLogin(m_ggi,m_uid,XP_API_STRLEN(m_uid), KEEP_LOBBY_STATUS);
	else
		m_GLXPlayerMultiPlayer->mpSendLobbyLogin(m_ggi,m_uid,XP_API_STRLEN(m_uid), SET_LOBBY_STATUS);

	if( !m_GLXPlayerMultiPlayer->GetSecondConnecting() )
		SetOnlineSubState(ONLINE_MP_PROCESS);
}

void GameStateMultiplayerLobby::OnMPLoginSuccess( DataPacket* currentPacket )
{	
	if ( NULL != currentPacket )
	{
		short len = 0;
		char * name = 0;
		char * nationlity = 0;
		currentPacket->getShortLenString( name, len );
		XP_API_DEL( name );
		currentPacket->getShortLenString( nationlity, len );
		XP_API_MEMSET( m_nationlity, 0, 8 );
		XP_API_STRCPY( m_nationlity, nationlity );
		XP_API_DEL( nationlity );
	}

	//>>this must be set,it is used for second connect.
	m_GLXPlayerMultiPlayer->SetSecConStartTime( GLXPlayerMPLobby::SET_STATUS_TO_CONNECT_SUCCESS );
	//<<end

	if( m_GLXPlayerMultiPlayer->GetSecondConnecting() )
	{
		if( ONLINE_MP_IN_SESSION == m_sub_state || ONLINE_LOBBY_SET_PARAMETER == m_sub_state
			|| ONLINE_MP_IN_GAME_LOADING == m_sub_state || ONLINE_MP_IN_GAME == m_sub_state )
		{
			if( m_GLXPlayerMultiPlayer->GetSendRejoinAfterGameOver() )
			{
				XP_DEBUG_OUT("send rejoin in game, NORMAL_RETURN_TO_LOBBY\n");
				m_GLXPlayerMultiPlayer->mpSendRejoinLobby( GLXPlayerMPLobby::NORMAL_RETURN_TO_LOBBY );
			}
			else
			{
				XP_DEBUG_OUT("send rejoin in game and session , ERROR_RETURN_TO_LOBBY\n");
				m_GLXPlayerMultiPlayer->mpSendRejoinLobby( GLXPlayerMPLobby::ERROR_RETURN_TO_LOBBY );
			}
		}
		else
		{
			m_GLXPlayerMultiPlayer->SetSecondConnecting( false );
		}

#ifdef LOBBY_THREAD_TEST
		m_iRequestTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "second LoginLobby: %s %d", s, m_iRequestTime - m_iSendTime );
		XP_API_DEL( s );
		SaveTestLog();
#endif
	}
	else
	{
		SetOnlineSubState(ONLINE_LOBBY_MENU);
#ifdef LOBBY_THREAD_TEST
		m_iRequestTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "LoginLobby: %s %d", s, m_iRequestTime - m_iSendTime );
		m_log->setlobbyLoginTime(m_iRequestTime - m_iSendTime);
		XP_API_DEL( s );
		SaveTestLog();

		if( (m_bTestThread && ((g_bRandom && m_bTestCreate) || g_bTestCreate))
			|| (m_bTestThread && ((g_bRandom && m_bTestFilter) || g_bTestFilter))
		  )
		{
			m_bSendConnect = true;
		}
#endif
	}

}

void GameStateMultiplayerLobby::OnMPCreateSessionSuccess( DataPacket* currentPacket )
{
#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestCreate) || g_bTestCreate ) )
	{
		m_iRequestTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "CreatLobby: %s %d ", s, m_iRequestTime - m_iSendTime );
		m_log->setLobbyCreateTime(m_iRequestTime - m_iSendTime );
		XP_API_DEL( s );
	}
#endif

	XP_DEBUG_OUT( "Create lobby Success");	

	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;
		XP_API_DEL( player->name );
		XP_API_DEL( player->nationality );
		XP_API_DEL( player->userParameter );
		XP_API_DEL( player );
	}

	m_player_list.clear();

	PLAYER * player = XP_API_NEW PLAYER;

	player->name = XP_API_NEW char[XP_API_STRLEN(m_username)+1];
	XP_API_MEMSET(player->name, 0, XP_API_STRLEN(m_username)+1);
	XP_API_MEMCPY(player->name, m_username, XP_API_STRLEN( m_username ));

	player->nationality = XP_API_NEW char[8];
	XP_API_MEMSET(player->nationality, 0, 8);
	XP_API_STRCPY(player->nationality, m_nationlity);

	player->status = USER_PLAYER_NOT_READY;;

	char playerParameter[200] = {0};
	XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
	int len = XP_API_STRLEN(playerParameter);
	player->userParameter = XP_API_NEW char[len+1];
	player->userParameter[len] = 0;
	XP_API_MEMCPY( player->userParameter, playerParameter, len );

	m_player_list.push_back( player );
	m_player_myself = player;

	m_curr_player_it = m_player_list.begin();

	XP_API_MEMSET( m_lobbyname, 0, 32);
	XP_API_MEMCPY( m_lobbyname, m_username, XP_API_STRLEN( m_username ) );

	currentPacket->getInt( m_lobby_id );

	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

bool GameStateMultiplayerLobby::OnMPListGameParameter(DataPacket* currentPacket)
{
	return false;
}

void GameStateMultiplayerLobby::OnMPGetLobbyInfo( DataPacket* currentPacket )
{
	if( m_LobbyInfo == NULL )
		m_LobbyInfo = XP_API_NEW LOBBYINFO();
	else
	{
		XP_API_DEL( m_LobbyInfo->name );
		XP_API_DEL( m_LobbyInfo->gameParam );
		XP_API_DEL( m_LobbyInfo->nationality );
	}

	short lenByte = 0; short lenShort = 0;

	currentPacket->getShortLenString( m_LobbyInfo->name, lenByte );
	currentPacket->getBlob( m_LobbyInfo->gameParam, lenShort );
	currentPacket->getByte( m_LobbyInfo->visibility );
	currentPacket->getByte( m_LobbyInfo->locked );
	currentPacket->getInt( m_LobbyInfo->userNum );
	currentPacket->getInt( m_LobbyInfo->spectatorNum );

	currentPacket->getShortLenString( m_LobbyInfo->nationality, lenShort );
	currentPacket->getByte( m_LobbyInfo->conut );
	currentPacket->getByte( m_LobbyInfo->status );

	SetOnlineSubState(ONLINE_LOBBY_LIST_INFO);
}

void GameStateMultiplayerLobby::OnMPListSessionSuccessWithGameParam( DataPacket* currentPacket )
{
	LOBBY_LIST::iterator it;
	for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
	{
		LOBBY * lobby = *it;
		XP_API_DEL( lobby->name );
		XP_API_DEL( lobby->nationality );
		XP_API_DEL( lobby );
	}

	m_lobby_list.clear();

	//int total_lobby_count = 0;
	//currentPacket->getInt( total_lobby_count );
	int lobby_count = 0;
	currentPacket->getInt( lobby_count );

	if ( lobby_count > 0 )
	{
		for ( BYTE i=0; i<lobby_count; i++ )
		{
			short len  = 0;
			LOBBY * lobby = XP_API_NEW LOBBY;
			lobby->id = 0;
			lobby->name = 0;
			lobby->nationality = 0;
			lobby->conut = 0;
			lobby->status = 0;

			currentPacket->getInt( lobby->id );

			currentPacket->getShortLenString( lobby->name, len );
			currentPacket->getShortLenString( lobby->nationality, len );
			currentPacket->getByte( lobby->conut );
			currentPacket->getByte( lobby->status );

			currentPacket->getByte( lobby->visibility );
			currentPacket->getByte( lobby->locked );
			currentPacket->getInt( lobby->userNum );
			currentPacket->getInt( lobby->spectatorNum );

			char* gameParam = NULL; short gameParamLen = 0;
			currentPacket->getBlob( lobby->gameParam, gameParamLen );

			m_lobby_list.push_back( lobby );
		}
		m_curr_lobby_it = m_lobby_list.begin();
	}

	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
}

void GameStateMultiplayerLobby::OnMPGetLobbyForName( DataPacket* currentPacket )
{
	LOBBY_LIST::iterator it;
	for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
	{
		LOBBY * lobby = *it;
		XP_API_DEL( lobby->name );
		XP_API_DEL( lobby->nationality );
		XP_API_DEL( lobby );
	}

	m_lobby_list.clear();

	//int total_lobby_count = 0;
	//currentPacket->getInt( total_lobby_count );
	int lobby_count = 0;
	currentPacket->getInt( lobby_count );

	if ( lobby_count > 0 )
	{
		for ( BYTE i=0; i<lobby_count; i++ )
		{
			short len  = 0;
			LOBBY * lobby = XP_API_NEW LOBBY;
			lobby->id = 0;
			lobby->name = 0;
			lobby->nationality = 0;
			lobby->conut = 0;
			lobby->status = 0;

			currentPacket->getInt( lobby->id );

			currentPacket->getShortLenString( lobby->name, len );
			currentPacket->getShortLenString( lobby->nationality, len );
			currentPacket->getByte( lobby->conut );
			currentPacket->getByte( lobby->status );

			currentPacket->getByte( lobby->visibility );
			currentPacket->getByte( lobby->locked );
			currentPacket->getInt( lobby->userNum );
			currentPacket->getInt( lobby->spectatorNum );

			//char* gameParam = NULL; short gameParamLen = 0;
			//currentPacket->getBlob( lobby->gameParam, gameParamLen );

			m_lobby_list.push_back( lobby );
		}
		m_curr_lobby_it = m_lobby_list.begin();
	}

	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
}

void GameStateMultiplayerLobby::OnMPGetLobbyForFriend( DataPacket* currentPacket )
{
	LOBBY_LIST::iterator it;
	for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
	{
		LOBBY * lobby = *it;
		XP_API_DEL( lobby->name );
		XP_API_DEL( lobby->nationality );
		XP_API_DEL( lobby );
	}

	m_lobby_list.clear();

	//int total_lobby_count = 0;
	//currentPacket->getInt( total_lobby_count );
	int lobby_count = 0;
	currentPacket->getInt( lobby_count );

	if ( lobby_count > 0 )
	{
		for ( BYTE i=0; i<lobby_count; i++ )
		{
			short len  = 0;
			LOBBY * lobby = XP_API_NEW LOBBY;
			lobby->id = 0;
			lobby->name = 0;
			lobby->nationality = 0;
			lobby->conut = 0;
			lobby->status = 0;

			currentPacket->getInt( lobby->id );

			currentPacket->getShortLenString( lobby->name, len );
			currentPacket->getShortLenString( lobby->nationality, len );
			currentPacket->getByte( lobby->conut );
			currentPacket->getByte( lobby->status );

			currentPacket->getByte( lobby->visibility );
			currentPacket->getByte( lobby->locked );
			currentPacket->getInt( lobby->userNum );
			currentPacket->getInt( lobby->spectatorNum );

			//char* gameParam = NULL; short gameParamLen = 0;
			//currentPacket->getBlob( lobby->gameParam, gameParamLen );

			m_lobby_list.push_back( lobby );
		}
		m_curr_lobby_it = m_lobby_list.begin();
	}

	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
}

void GameStateMultiplayerLobby::OnMPAutoMatchRequeset( int errType )
{
	if(ERROR_NONE == errType)
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_autoMatchType = AUTO_MATCH;
	}
	else if(ERROR_AUTOMATCH_WAITING == errType)
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_autoMatchType = AUTO_MATCH_WAITING;
	}
}

void GameStateMultiplayerLobby::OnMPAutoMatchRequesetCancel()
{
	SetOnlineSubState(ONLINE_LOBBY_MENU);
}

void GameStateMultiplayerLobby::OnMPListSessionSuccess( DataPacket* currentPacket )
{
#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestFilter) || g_bTestFilter ) )
	{
		m_iRequestTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "FilterLobby: %s %d ", s, m_iRequestTime - m_iSendTime );
		m_log->setLobbyFilterTime(m_iRequestTime - m_iSendTime);
		XP_API_DEL( s );
	}
#endif

	LOBBY_LIST::iterator it;
	for ( it=m_lobby_list.begin(); it!=m_lobby_list.end(); it++ )
	{
		LOBBY * lobby = *it;
		XP_API_DEL( lobby->name );
		XP_API_DEL( lobby->nationality );
		XP_API_DEL( lobby );
	}

	m_lobby_list.clear();

	//int total_lobby_count = 0;
	//currentPacket->getInt( total_lobby_count );
	int lobby_count = 0;
	currentPacket->getInt( lobby_count );

	if ( lobby_count > 0 )
	{
		for ( BYTE i=0; i<lobby_count; i++ )
		{
			short len  = 0;
			LOBBY * lobby = XP_API_NEW LOBBY;
			lobby->id = 0;
			lobby->name = 0;
			lobby->nationality = 0;
			lobby->conut = 0;
			lobby->status = 0;

			currentPacket->getInt( lobby->id );

			currentPacket->getShortLenString( lobby->name, len );
			currentPacket->getShortLenString( lobby->nationality, len );
			currentPacket->getByte( lobby->conut );
			currentPacket->getByte( lobby->status );

			currentPacket->getByte( lobby->visibility );
			currentPacket->getByte( lobby->locked );
			currentPacket->getInt( lobby->userNum );
			currentPacket->getInt( lobby->spectatorNum );

			//char* gameParam = NULL; short gameParamLen = 0;
			//currentPacket->getBlob( lobby->gameParam, gameParamLen );

			m_lobby_list.push_back( lobby );
		}
		m_curr_lobby_it = m_lobby_list.begin();
	}

#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestJoin) || g_bTestJoin ) )
	{
		if( lobby_count == 0)
		{
			char* s = NULL;
			m_iSendTime = XP_API_GET_TIME();
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "no lobby created: %s", s );
			XP_API_DEL( s );
		}
		else
		{
			if( lobby_count == 1 )
				m_curr_lobby_it = m_lobby_list.begin();
			else
			{
				int index = XP_API_GET_TIME() % (lobby_count-1);
				int ii = 0;
				for ( it=m_lobby_list.begin(); it!=m_lobby_list.end() && ii != index; it++, ii++ )
				{
					m_curr_lobby_it = it;
				}
			}

			char playerParameter[20] = {0};
			XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
			if ( NULL != *m_curr_lobby_it )
			{
				char* s = NULL;
				m_iSendTime = XP_API_GET_TIME();
				GetCurrentSystemTime( &s );
				XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "JoinLobby send: %s", s );
				XP_API_DEL( s );
				m_log->setSendLobbyJoin();
				m_user_status = USER_PLAYER_NOT_READY;
				m_GLXPlayerMultiPlayer->mpSendJoinLobby( (*m_curr_lobby_it)->id, m_user_status, playerParameter );
			}
		}
	}
	else
	{
		SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
	}
#else
	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
#endif
}

void GameStateMultiplayerLobby::OnMPJoinSessionSuccess( DataPacket* currentPacket )
{
#ifdef LOBBY_THREAD_TEST
	if( m_bTestThread && ( (g_bRandom && m_bTestJoin) || g_bTestJoin ) )
	{
		m_iRequestTime = XP_API_GET_TIME();
		char* s = NULL;
		GetCurrentSystemTime( &s );
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "JoinLobby: %s %d ", s, m_iRequestTime - m_iSendTime );
		m_log->setLobbyJoinTime( m_iRequestTime - m_iSendTime);
		XP_API_DEL( s );
	}
#endif

	XP_DEBUG_OUT( "Join lobby Success");

	m_player_myself = NULL;

	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;
		XP_API_DEL( player->name );
		XP_API_DEL( player->nationality );
		XP_API_DEL( player->userParameter );
		XP_API_DEL( player );
	}
	m_player_list.clear();

	int count = 0;

	currentPacket->getInt( count );

	if ( count > 0 )
	{
		for ( BYTE i=0; i<count; i++ )
		{
			PLAYER * player = XP_API_NEW PLAYER;
			short len = 0;

			currentPacket->getShortLenString( player->name, len );
			currentPacket->getShortLenString( player->nationality, len );
			currentPacket->getByte( player->status );

			if ( _stricmp( player->name, m_username ) == 0 )
			{
				m_player_myself = player;
			}

			currentPacket->getShortLenString( player->userParameter, len );

			m_player_list.push_back( player );
		}

		m_lobby_id = (*m_curr_lobby_it)->id;
		XP_API_MEMSET( m_lobbyname, 0, 32 );
		XP_API_MEMCPY( m_lobbyname, (*m_curr_lobby_it)->name, XP_API_STRLEN( (*m_curr_lobby_it)->name ) );
	}

	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateMultiplayerLobby::OnMPProcessPushMessages(DataPacket* currentPacket, int messageID)
{
	switch(messageID)
	{
	case PMSG_JOIN_LOBBY_LAUNCHED:
		{
			m_bLuanchedJoin = true;

			//adding to player list
			PLAYER * player = XP_API_NEW PLAYER;

			short len = -1;
			currentPacket->getShortLenString( player->name, len );
			currentPacket->getShortLenString( player->nationality, len );
			currentPacket->getByte( player->status );
			currentPacket->getShortLenString( player->userParameter, len );
			
			m_player_list.push_back( player );
			
			//new the game data array
			if( player->status == USER_SPECTATOR)
				break;

			CInGame* pInGame = XP_API_NEW CInGame();
			m_pInGame.push_back( pInGame );

			int lenName = XP_API_STRLEN( player->name );
			pInGame->m_username = XP_API_NEW char[lenName + 1];
			XP_API_MEMSET( pInGame->m_username, 0, (lenName + 1)*sizeof(char) );
			XP_API_MEMCPY( pInGame->m_username, player->name, XP_API_STRLEN( player->name ) );
		}
		break;

	case PMSG_JOIN_LOBBY_UNLAUNCHED:
		{
			PLAYER * player = XP_API_NEW PLAYER;

			short len = -1;
			currentPacket->getShortLenString( player->name, len );
			currentPacket->getShortLenString( player->nationality, len );
			currentPacket->getByte( player->status );
			currentPacket->getShortLenString( player->userParameter, len );

			m_player_list.push_back( player );
		}
		break;
	case PMSG_AUTOMATCH_USER:
		{
			currentPacket->getInt( m_lobby_id );

			PLAYER_LIST::iterator it;
			for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
			{
				PLAYER * player = *it;
				XP_API_DEL( player->name );
				XP_API_DEL( player->nationality );
				XP_API_DEL( player->userParameter );
				XP_API_DEL( player );
			}

			m_player_list.clear();

			PLAYER * player = XP_API_NEW PLAYER;

			player->name = XP_API_NEW char[XP_API_STRLEN(m_username)+1];
			XP_API_MEMSET( player->name, 0, (XP_API_STRLEN(m_username)+1) );
			XP_API_MEMCPY(player->name, m_username, XP_API_STRLEN( m_username ));

			player->nationality = XP_API_NEW char[8];
			XP_API_MEMSET(player->nationality, 0, 8);
			XP_API_STRCPY(player->nationality, m_nationlity);

			player->status = USER_PLAYER_NOT_READY;;

			char playerParameter[200] = {0};
			XP_API_SPRINTF( playerParameter, "%d", m_bColor ? BLUE : GREEN );
			int len = XP_API_STRLEN(playerParameter);
			player->userParameter = XP_API_NEW char[len+1];
			player->userParameter[len] = 0;
			XP_API_MEMCPY( player->userParameter, playerParameter, len );

			m_player_list.push_back( player );
			m_player_myself = player;

			m_curr_player_it = m_player_list.begin();
		}
		break;
	case PMSG_SET_USER_STATUS:
		{
			short len = -1; 
			char* name = NULL;
			currentPacket->getShortLenString( name, len );

			int count = m_player_list.size();	

			if ( count > 0 )
			{
				PLAYER_LIST::iterator it;
				for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
				{
					PLAYER * player = *it;

					if( XP_API_STRCMP(player->name, name) == 0 )
					{
						XP_API_DEL(name);
						currentPacket->getShortLenString( name, len );
						XP_API_DEL(name);

						if( currentPacket->getByte(player->status) == false )
						{
							XP_DEBUG_OUT( "OnMPProcessPushMessages: error get status failed");
						}
						break;
					}
				}
			}
			else
			{
				XP_DEBUG_OUT( "OnMPProcessPushMessages: error get list failed");
			}

			XP_API_DEL(name);
			break;
		}
	case PMSG_LEAVE_LOBBY:
	case PMSG_KICK_OUT_PLAYER:
		{
			short len = -1; 
			char* name;
			currentPacket->getShortLenString( name, len );

			XP_API_MEMSET(m_lobbyname, 0, 32);
			XP_API_MEMCPY(m_lobbyname, name, XP_API_STRLEN(name));

			bool bChangeToMaster = false;
			if ( XP_API_STRCMP( m_lobbyname, m_username ) == 0 )
			{
				bChangeToMaster = true;
				m_GLXPlayerMultiPlayer->mpChangeToBeMaster();
			}

			XP_API_DEL(name);
			currentPacket->getShortLenString( name, len );

			//>>for kick out self
			if(XP_API_STRCMP( name, m_username ) == 0)
			{
				PLAYER_LIST::iterator itout;
				for ( itout=m_player_list.begin(); itout!=m_player_list.end(); itout++ )
				{
					PLAYER * player = *itout;
					XP_API_DEL( player->name );
					XP_API_DEL( player->nationality );
					XP_API_DEL( player->userParameter );
					XP_API_DEL( player );
				}

				m_player_list.clear();

				XP_API_DEL(name);

				SetOnlineSubState(ONLINE_LOBBY_MENU);
				m_GLXPlayerMultiPlayer->SetMPConnectLevel(CONNECT_LEVEL_IN_CONNECTED);
				return;
			}
			//<<end

			int count = m_player_list.size();	

			if ( count > 0 )
			{
				PLAYER_LIST::iterator it;
				for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
				{
					PLAYER * player = *it;

					if( XP_API_STRCMP(player->name, name) == 0 )
					{
						{
							XP_API_DEL( player->name );
							XP_API_DEL( player->nationality );
							XP_API_DEL( player->userParameter );
							XP_API_DEL( player );
						}
						m_player_list.erase( it );
						XP_API_DEL(name);
						break;
					}
				}
			}
			else
			{
				XP_DEBUG_OUT( "OnMPProcessPushMessages: error get list failed push leave");
			}

			//>>for kick out
			if(bChangeToMaster)
			{
				m_curr_player_it = m_player_list.begin();
			}
			//<<end

			XP_API_DEL(name);
			break;
		}

	case PMSG_START_GAME:
		{
			SetOnlineSubState(ONLINE_MP_IN_GAME_LOADING);
		}
		break;

	case PMSG_GAMESERVER_ADDR:
		{
			if( ONLINE_MP_IN_GAME_LOADING == m_sub_state )
			{
				m_autoMatchType = AUTO_MATCH_NONE;
				SetOnlineSubState(ONLINE_MP_IN_GAME);

				InitGameServerSocket();
				//init game server ip and port

				short len = 0;
				char* serverip = NULL;
				currentPacket->getShortLenString(serverip, len);
				TransToIp( serverip );
				if(serverip != NULL)
					XP_API_STRCPY(m_input_server, serverip);
				else
				{
					XP_DEBUG_OUT("GET SERVER IP FAILED");
				}
				XP_API_DEL(serverip);

				currentPacket->getShort(m_tcp_port);
				currentPacket->getShort(m_udp_port);

				int lobbyID = -1;
				currentPacket->getInt(lobbyID);

				if( lobbyID != m_lobby_id )
				{
					InitGameServerSocket();
					XP_API_DEL( m_udp_socket );
					break;
				}

				for ( GAME_LIST::iterator it = m_pInGame.begin(); it != m_pInGame.end(); it++ )
				{
					CInGame * pInGame = *it;
					XP_API_DEL( pInGame );
				}
				m_pInGame.clear();

				//>>set name
				PLAYER_LIST::iterator it;
				for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
				{
					PLAYER * player = *it;

					if( player->status == USER_SPECTATOR)
						break;

					CInGame* pInGame = XP_API_NEW CInGame();
					m_pInGame.push_back( pInGame );

					int lenName = XP_API_STRLEN( player->name );
					if( (m_player_myself->status == USER_SPECTATOR && XP_API_STRCMP( player->name, (*m_curr_lobby_it)->name ) == 0)
						|| (m_player_myself->status != USER_SPECTATOR && XP_API_STRCMP( player->name, m_username ) == 0) )
					{
						pInGame->m_username = XP_API_NEW char[lenName + 1];
						XP_API_MEMSET( pInGame->m_username, 0, (lenName + 1)*sizeof(char) );
						XP_API_MEMCPY( pInGame->m_username, player->name, XP_API_STRLEN( player->name ) );
						m_pGameMyself = m_pInGame.end();  m_pGameMyself--;
					}
					else
					{
						pInGame->m_username = XP_API_NEW char[lenName + 1];
						XP_API_MEMSET( pInGame->m_username, 0, (lenName + 1));
						XP_API_MEMCPY( pInGame->m_username, player->name, XP_API_STRLEN( player->name ) );
					}				
				}
			}
			break;
		}

	case PMSG_SET_GAME_PARAMETER:
		{
			m_bDrawGameParamChanged = true;
			timer = XP_API_GET_TIME();

			char* gameParam = NULL; short len = 0;
			currentPacket->getBlob( gameParam, len);

			XP_API_MEMSET( m_szParamString, 0, 256 );
			XP_API_SPRINTF( m_szParamString, "change Game Param to %s", gameParam );

			XP_API_DEL( gameParam );
			
			break;
		}

	case PMSG_SET_USER_PARAMETER:
		{
			m_bDrawUserParamChanged = true;
			timer = XP_API_GET_TIME();

			char* username = NULL; short lenS = 0;
			currentPacket->getShortLenString( username, lenS );

			char* nationality = NULL; lenS = 0;
			currentPacket->getShortLenString( nationality, lenS );
			XP_API_DEL( nationality );

			char* userParam = NULL; short len = 0;
			currentPacket->getBlob( userParam, len );

			bool bColor = false;
			if( userParam[0] == (int)BLUE + 48)
				bColor = true;
			else
				bColor = false;

			XP_API_MEMSET( m_szParamString, 0, 256 );
			XP_API_SPRINTF( m_szParamString, "%s change user Param to %s", username, bColor ? "BLUE" : "GREEN" );

			XP_API_DEL( username );
			XP_API_DEL( userParam );
			break;
		}

	case PMSG_REJOIN_GAME:
		{
			short len = -1;
			char* name = NULL;
			currentPacket->getShortLenString( name, len );

			bool bHasExist = false;
			PLAYER_LIST::iterator it;
			for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
			{
				PLAYER * player = *it;

				if( XP_API_STRCMP( name, player->name ) == 0 )
				{
					bHasExist = true;
					break;
				}				
			}

			if( bHasExist )
			{
				XP_API_DEL( ((PLAYER*)(*it))->nationality );
				currentPacket->getShortLenString( ((PLAYER*)(*it))->nationality, len );
				currentPacket->getByte( ((PLAYER*)(*it))->status );
				currentPacket->getShortLenString( ((PLAYER*)(*it))->userParameter, len );
			}
			else
			{
				PLAYER * player = XP_API_NEW PLAYER;
				
				int lenName = XP_API_STRLEN(name);
				player->name = XP_API_NEW char[lenName+1];
				XP_API_STRCPY(player->name, name);
				player->name[lenName] = '\0';

				currentPacket->getShortLenString( player->nationality, len );
				currentPacket->getByte( player->status );
				currentPacket->getShortLenString( player->userParameter, len );

				m_player_list.push_back( player );
			}

			XP_API_DEL( name );
		}
		break;

	case PMSG_SAMEACCOUNT_LOGIN:
		{
			char* ip = NULL; short lenIP = 0;
			currentPacket->getShortLenString(ip, lenIP);
			XP_API_DEL(ip);
			
			m_GLXPlayerMultiPlayer->mpDisconnect();
			m_bSameAccountLogin = true;
		}
		break;
	}
}

void GameStateMultiplayerLobby::OnMPSetParamSuccess( int result )
{
	switch( result )
	{
	case SMSG_SET_USER_STATUS:
		{
			if( m_player_myself->status == USER_PLAYER_NOT_READY )
				m_player_myself->status = USER_PLAYER_READY;
			else if(m_player_myself->status == USER_PLAYER_READY)
				m_player_myself->status = USER_PLAYER_NOT_READY;
		}	
		break;

	//case 
	}
}

void GameStateMultiplayerLobby::OnMPLeaveSessionSuccess()
{
	SetOnlineSubState(ONLINE_LOBBY_MENU);

	XP_DEBUG_OUT( "Leave lobby Success");
}

void GameStateMultiplayerLobby::OnMPKickOutPlayerSuccess()
{
	char nameNext[50] = { 0 };
	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;

		if( XP_API_STRCMP(player->name, (*m_curr_player_it)->name) == 0 )
		{
			PLAYER_LIST::iterator itTemp = it;
			itTemp++;
			if(m_player_list.size() != 0)
			{
				if(itTemp == m_player_list.end())
					itTemp = m_player_list.begin();
				XP_API_STRCPY(nameNext, (*itTemp)->name);
			}

			{
				XP_API_DEL( player->name );
				XP_API_DEL( player->nationality );
				XP_API_DEL( player->userParameter );
				XP_API_DEL( player );
			}
			m_player_list.erase( it );
			break;
		}
	}

	if(XP_API_STRLEN(nameNext) != 0)
	{
		for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
		{
			PLAYER * player = *it;

			if( XP_API_STRCMP(player->name, nameNext) == 0 )
			{
				m_curr_player_it = it;
				break;
			}
		}
	}
	else
	{
		//this player has left
		if(m_player_list.size() != 0)
			m_curr_player_it = m_player_list.begin();
	}

	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateMultiplayerLobby::OnMPStartGameFailed( int result )
{
	if( result == ERROR_START_GAME_NOT_ALLPLAYER_READY )
	{
		SetOnlineSubState(ONLINE_LOBBY_START_FAILED);
	}
}

void GameStateMultiplayerLobby::TransToIp( char* &name )
{
#ifdef OS_WIN32
	CWin32Socket::TransToIp(name);
#endif

#ifdef OS_IPHONE
	CIPhoneSocket::TransToIp(name);
#endif
}
void GameStateMultiplayerLobby::OnMPStartGameSuccess(DataPacket* currentPacket)
{		
	XP_DEBUG_OUT( "Lunch lobby Success");

	SetOnlineSubState(ONLINE_MP_IN_GAME_LOADING);
/*
	InitGameServerSocket();
	//init game server ip and port

	short len = 0;
	char* serverip = NULL;
	currentPacket->getShortLenString(serverip, len);
	TransToIp( serverip );
	if(serverip != NULL)
		XP_API_STRCPY(m_input_server, serverip);
	else
	{
		XP_DEBUG_OUT("GET SERVER IP FAILED");
	}
	XP_API_DEL(serverip);

	currentPacket->getShort(m_tcp_port);
	currentPacket->getShort(m_udp_port);

	//clear the game data
	for ( GAME_LIST::iterator it = m_pInGame.begin(); it != m_pInGame.end(); it++ )
	{
		CInGame * pInGame = *it;
		XP_API_DEL( pInGame );
	}
	m_pInGame.clear();

	//>>set name
	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;

		if( player->status == USER_SPECTATOR )
			break;

		CInGame* pInGame = XP_API_NEW CInGame();
		m_pInGame.push_back( pInGame );

		//>>player only
		int lenName = XP_API_STRLEN_UNICODE( player->name );
		if( XP_API_STRCMP_UNICODE( player->name, m_username ) == 0 )
		{
			pInGame->m_username = XP_API_NEW char[lenName + 1];
			XP_API_MEMSET( pInGame->m_username, 0, (lenName + 1)*sizeof(char) );
			XP_API_MEMCPY( pInGame->m_username, player->name, XP_API_STRLEN_UNICODE( player->name ) * sizeof( char ) );
			m_pGameMyself = m_pInGame.end(); m_pGameMyself--;
		}
		else
		{
			pInGame->m_username = XP_API_NEW char[lenName + 1];
			XP_API_MEMSET( pInGame->m_username, 0, (lenName + 1)*sizeof(char) );
			XP_API_MEMCPY( pInGame->m_username, player->name, XP_API_STRLEN_UNICODE( player->name ) * sizeof( char ) );
		}

		//<<end
	}	
*/
}

void GameStateMultiplayerLobby::OnMPDisconnectSuccess()
{
	m_GLXPlayerLogin->SendLogout();
}

void GameStateMultiplayerLobby::OnMPFinishGameSuccess()
{
}

void GameStateMultiplayerLobby::OnMPRejoinLobbySuccess( DataPacket* currentPacket, bool bNeedChangeStatus )
{
	PLAYER_LIST::iterator it;
	for ( it=m_player_list.begin(); it!=m_player_list.end(); it++ )
	{
		PLAYER * player = *it;
		XP_API_DEL( player->name );
		XP_API_DEL( player->nationality );
		XP_API_DEL( player->userParameter );
		XP_API_DEL( player );
	}
	m_player_list.clear();
	m_player_myself = NULL;

	m_GLXPlayerMultiPlayer->SetSecondConnecting( false );

	//>>just for demo run error
	if(m_lobby_list.size() == 0)
	{
		LOBBY * lobby = XP_API_NEW LOBBY;
		m_lobby_list.push_back(lobby);
		m_curr_lobby_it = m_lobby_list.begin();
	}
	if(m_LobbyInfo == NULL)
	{
		m_LobbyInfo = XP_API_NEW LOBBYINFO();
	}
	//<<end

	//>>for new rejoin protocol
	//the current room name
	char* lobbyName = NULL; short lenLobbyName = 0;
	currentPacket->getShortLenString( lobbyName, lenLobbyName );
	//if lobbyName is NULL, there is an error in this response and should set the client status to lobby menu
	if(lobbyName == NULL)
	{
		return;
	}
	if(XP_API_STRCMP(lobbyName, m_lobbyname) != 0)
	{
		XP_API_MEMSET( m_lobbyname, 0, 32 );
		XP_API_MEMCPY( m_lobbyname, lobbyName, lenLobbyName );

		XP_API_DEL((*m_curr_lobby_it)->name);
		(*m_curr_lobby_it)->name = XP_API_NEW char[lenLobbyName+1];
		XP_API_MEMCPY( (*m_curr_lobby_it)->name, lobbyName, lenLobbyName );

		if(m_LobbyInfo != NULL)
		{
			XP_API_DEL(m_LobbyInfo);
			m_LobbyInfo = XP_API_NEW LOBBYINFO();
			
			m_LobbyInfo->name = XP_API_NEW char[lenLobbyName+1];
			XP_API_MEMCPY( m_LobbyInfo->name, lobbyName, lenLobbyName );
		}
	}
	XP_API_DEL(lobbyName);

	//be visible or not
	if(m_lobby_list.size() != 0)
	currentPacket->getByte( (*m_curr_lobby_it)->visibility );
	m_LobbyInfo->visibility = (*m_curr_lobby_it)->visibility;
    
	//be lock or not
	currentPacket->getByte( (*m_curr_lobby_it)->locked );
	m_LobbyInfo->locked = (*m_curr_lobby_it)->locked;
	
	//the max player number
	currentPacket->getByte( (*m_curr_lobby_it)->conut );
	m_LobbyInfo->conut = (*m_curr_lobby_it)->conut;

	//the game param
	XP_API_DEL(m_LobbyInfo->gameParam); short lenGameParam = 0;
	currentPacket->getBlob( m_LobbyInfo->gameParam, lenGameParam );
    
	//this room launched or not
	currentPacket->getByte( (*m_curr_lobby_it)->status );
	m_LobbyInfo->status = (*m_curr_lobby_it)->status;

	//the game server's ip what this room get 
	char* sServerIP = NULL; short lenServerIP = 0;
	currentPacket->getBlob( sServerIP, lenServerIP );

	//game server's tcp port
	short tcpPort = 0;
	currentPacket->getShort( tcpPort );

	//game server's udp port
	short udpPort = 0;
	currentPacket->getShort( udpPort );
	//<<end

	if(m_LobbyInfo->status != 0
		//>>checking your lobby status is in game or not, if the current status is in game, the below is not need
		&& m_GLXPlayerMultiPlayer->GetMPConnectLevel() != CONNECT_LEVEL_IN_GAME
		//<<end
		)
	{
		//should set game status to game loading and wait for getting the game server ip at here

		//<<end

		if( lenServerIP != 0 && XP_API_STRLEN(sServerIP) != 0 
			&& (tcpPort != 0 || udpPort != 0)
			)
		{
			//if this, should set game status to game has launched, and connect to game server in next frame
		}
	}

	int count = 0;
	currentPacket->getInt( count );

	if ( count > 0 )
	{
		for ( BYTE i=0; i<count; i++ )
		{
			PLAYER * player = XP_API_NEW PLAYER;
			short len = 0;

			currentPacket->getShortLenString( player->name, len );
			currentPacket->getShortLenString( player->nationality, len );
			currentPacket->getByte( player->status );
			currentPacket->getShortLenString( player->userParameter, len );
			
			if ( _stricmp( player->name, m_username ) == 0 )
			{
				m_player_myself = player;
			}

			m_player_list.push_back( player );
		}
	}

	if( bNeedChangeStatus )
	{
		m_curr_player_it = m_player_list.begin();
		SetOnlineSubState(ONLINE_MP_IN_SESSION);
	}
}

void GameStateMultiplayerLobby::OnNetworkError()
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_CONNECTION;
	XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnUserNetworkError: %d\n", m_errorCode);

#ifdef LOBBY_THREAD_TEST
	XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: OnNetworkError: m_errorCode: %d", m_errorCode );
#endif
}

void GameStateMultiplayerLobby::OnRequestTimeout( int func_id )
{
	//>>this must be set,it is used for disconnect
	if( m_GLXPlayerMultiPlayer != NULL )
		m_GLXPlayerMultiPlayer->SetSecConStartTime( GLXPlayerMPLobby::RESET_STATUS_TO_INIT );
	//<<end

	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_REQUEST_TIME_OUT;
	XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnRequestTimeout: Function: %d - Error: %d\n", func_id, m_errorCode);

#ifdef LOBBY_THREAD_TEST

	XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: OnRequestTimeout: func_id: %d m_errorCode: %d", func_id, m_errorCode );
#endif
}

void GameStateMultiplayerLobby::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case GL_XPLAYER_FUNCTION_LOGIN:	
		{
#ifdef LOBBY_THREAD_TEST
			m_iRequestTime = XP_API_GET_TIME();
			char* s = NULL;
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "LoginWeb: %s %d", s, m_iRequestTime - m_iSendTime );
			XP_API_DEL( s );
			m_log->setWebTime(m_iRequestTime - m_iSendTime);
			if( !m_bTestThread )
#endif
			SaveProfile();
			XP_API_MEMSET( m_uid, 0, 128 );
			XP_API_STRCPY( m_uid, m_GLXPlayerLogin->GetUID() );
			m_ggi = m_GLXPlayerLogin->GetGGI();
			m_iUserNumber = XP_API_ATOI(m_GLXPlayerLogin->getUserNumber());
			m_GLXPlayerUserFriend->SetUID(m_uid);

#ifdef LOBBY_THREAD_TEST
			m_iSendTime = XP_API_GET_TIME();
			s = NULL;
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "ConnectLobby send: %s", s );
			XP_API_DEL( s );
#endif
#ifndef GET_SERVER_FROM_WEB
			m_GLXPlayerMultiPlayer->SetUsername(m_username);
			m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
#else
			m_GLXPlayerLogin->SendGetLobbyServer();
#endif	
			SetOnlineSubState(ONLINE_MP_PROCESS);
		}
		break;
	case FUNCTION_GET_USER_FRIENDS:
#ifdef LOBBY_LIST_WITH_GAME_PARAM
		m_GLXPlayerMultiPlayer->mpSendGetLobbyForFriendWithGameParam( m_iUserNumber, m_bVisibilityInFriendQuery ? 1 : 0, m_GLXPlayerUserFriend );
#else
		m_GLXPlayerMultiPlayer->mpSendGetLobbyForFriend( m_iUserNumber, m_bVisibilityInFriendQuery ? 1 : 0, m_GLXPlayerUserFriend );
#endif

		//SetOnlineSubState(ONLINE_LOBBY_MENU);
		break;
	case GL_XPLAYER_FUNCTION_LOGOUT:
		m_state = DEF::GAME_STATE_MENU;
		break;
	case FUNCTION_KEEP_ALIVE:
		break;
	case GL_XPLAYER_FUNCTION_REGISTER:
		{
			SetOnlineSubState( ONLINE_PROCESS );
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
			m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
#ifdef LOBBY_THREAD_TEST
			m_iRequestTime = XP_API_GET_TIME();
			char* s = NULL;
			GetCurrentSystemTime( &s );
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "Register: %s %d", s, m_iRequestTime - m_iSendTime );
			m_iSendTime = XP_API_GET_TIME();
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "LoginWeb send again: %s", s );
			XP_API_DEL( s );
#endif
			m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login_Game, NULL, true);
#endif
		}
		break;
	case FUNCTION_GET_LOBBY_SERVER:
	case FUNCTION_GAME_INVITATION_GET_LOBBY:
		{
			XP_API_DEL(m_sLobbyServerAddress);
			int lenHead = XP_API_STRLEN(LOBBY_SERVER_ADDRESS_HEAD);
			m_sLobbyServerAddress = XP_API_NEW char[lenHead + data_len + 1];
			XP_API_STRCPY(m_sLobbyServerAddress, LOBBY_SERVER_ADDRESS_HEAD);
			XP_API_STRCPY(&m_sLobbyServerAddress[XP_API_STRLEN(m_sLobbyServerAddress)], data);

			m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPLobby(this, m_sLobbyServerAddress);

			m_GLXPlayerMultiPlayer->SetUsername(m_username);

			m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
			SetOnlineSubState(ONLINE_MP_PROCESS);
		}
		break;
	default:
		//SetOnlineSubState(ONLINE_LOBBY_MENU);
		break;
	}
	//switch(func_id)
	//{
	//	case FUNCTION_POINT_USER_REPUTATION:				
	//		SetOnlineSubState(ONLINE_MP_END_GAME);
	//		break;
	//	case FUNCTION_GET_USER_FRIENDS:
	//		SetOnlineSubState(ONLINE_MP_SESSION_BUDDY);
	//		break;
	//	default:	
	//		SetOnlineSubState(ONLINE_LOBBY_MENU);
	//		break;			
	//}
//	SetOnlineSubState(ONLINE_LOBBY_MENU);
}

/**
 * The wrong response or the operation is failed on server side.
 *
 */
void GameStateMultiplayerLobby::OnRequestFailure( int func_id, int error_code )
{
	switch(func_id)
	{
	case GL_XPLAYER_FUNCTION_LOGIN:	
		if(error_code == ERROR_USERNAME_NOTEXISTS)
		{
			SetOnlineSubState(ONLINE_REGISTER); 
#ifdef LOBBY_THREAD_TEST
			m_iRequestTime = XP_API_GET_TIME();
			char* s = NULL;
			GetCurrentSystemTime( &s );
		
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "NoRegister: %s %d", s, m_iRequestTime - m_iSendTime );
			XP_API_DEL( s );
			if( m_bTestThread && ( (g_bRandom && m_bTestLogin) || g_bTestLogin ) )
				m_bSendConnect = true;
#endif
		}
		else
		{
			SetOnlineSubState(ONLINE_ERROR);
			m_errorCode = error_code;
			XP_DEBUG_OUT( "[GameStateMultiplayer] OnRequestFailure: func_id: %d errorid: %d\n", func_id, error_code);
#ifdef LOBBY_THREAD_TEST
			m_iRequestTime = XP_API_GET_TIME();
			
			XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: error fuction login: %d   ", m_errorCode );
#endif
		}
		break;
	case FUNCTION_GET_USER_FRIENDS:
		SetOnlineSubState(ONLINE_ERROR);
		m_errorCode = error_code;		
		XP_DEBUG_OUT( "[GameStateMultiplayer] OnRequestFailure: func_id: %d errorid: %d\n", func_id, error_code);
#ifdef LOBBY_THREAD_TEST
		m_iRequestTime = XP_API_GET_TIME();
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: error get friends: %d   ", m_iRequestTime - m_iSendTime );
		SaveTestLog();
#endif
		break;
	default:
		SetOnlineSubState(ONLINE_ERROR);
		m_errorCode = error_code;			
		XP_DEBUG_OUT( "[GameStateMultiplayerLobby] OnRequestFailure: %d, func_id: %d\n, Name: %s", error_code, func_id, m_username);	
#ifdef LOBBY_THREAD_TEST
		m_iRequestTime = XP_API_GET_TIME();
		XP_API_SPRINTF( m_sSaveOnlineLog[m_iCurrentLogIndex++], "failed: error fuc_id: %d  time: %d", func_id, m_iRequestTime - m_iSendTime );
		SaveTestLog();
#endif
		break;
	}
}

void GameStateMultiplayerLobby::OnMPuPnPTestSuccess()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void GameStateMultiplayerLobby::InitGameServerSocket()
{
	XP_API_DEL( m_udp_socket );
	m_udp_socket = GLXPlayerSocketFactory::GetSocket();
	m_udp_socket->CreateUdpSocket();

	XP_API_MEMSET( m_buffer, 0, 1024 );
	XP_API_MEMSET(m_send, 0, 256);
	XP_API_MEMSET(m_input_server, 0, 64);

	m_tcp_port = -1;
	m_udp_port = -1;

	m_GameType = GAMETYPE_NONE;
	m_bHasReady = false;
}

void GameStateMultiplayerLobby::DisGameServerSocket()
{
	XP_API_DEL( m_udp_socket );
	XP_API_MEMSET( m_buffer, 0, 1024 );
	XP_API_MEMSET(m_send, 0, 256);
	XP_API_MEMSET(m_input_server, 0, 64);

	m_tcp_port = -1;
	m_udp_port = -1;

	for ( GAME_LIST::iterator it = m_pInGame.begin(); it != m_pInGame.end(); it++ )
	{
		CInGame * pInGame = *it;
		XP_API_DEL( pInGame );
	}
	m_pInGame.clear();
}

void GameStateMultiplayerLobby::GameServerPlay()
{
	if ( NULL == m_udp_socket )
	{
		return;
	}

	int iRet = m_udp_socket->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );
	if ( iRet > 0 )
	{
		XP_API_MEMSET( m_buffer, 0, 1024 );
		int size = m_udp_socket->RecvFrom( m_buffer, 1024, m_input_server, m_udp_port );
		if( size <= 0 )
			return;

		DataPacket * packet = XP_API_NEW DataPacketLobby();
		packet->setMessageBody( (BYTE*)m_buffer, size );
		short len = 0;
		packet->getShort(len);
		int type = 0;
		packet->getInt( type );

		short stringLen = 0;
		char* gameData = NULL;
		packet->getShortLenString( gameData, stringLen );

		XP_API_DEL( packet );

		switch( type )
		{
		case KEY:
			{
				const char NAME = '@';
				int index = 0;

				int userCount = 0;	

				while( userCount < m_pInGame.size() )		
				{
					if( gameData[index] == 0 )
						break;

					index++;
					int record = index;

					while( gameData[index] != NAME )
					{
						index++;
					}
					char username[20] = {0};
					XP_API_MEMCPY( username, &gameData[record], index - record);
					index++;

					CInGame* pInGame = NULL;;
					for( GAME_LIST::iterator it = m_pInGame.begin(); it != m_pInGame.end(); it++ )
					{
						if( XP_API_STRCMP( (*it)->m_username, username ) == 0 )
						{
							pInGame = (*it);
							break;
						}
					}

					for (int i = 0; i < MAX_SQUARE_COUNT; i++)
					{
						for (int j = 0; j < MAX_SQUARE_COUNT; j++)
						{								
							pInGame->m_bFilledSquare[i][j] = ( gameData[index] - 48 ) == 1;  index++;
							XP_DEBUG_OUT("%d ", pInGame->m_bFilledSquare[i][j] ? 1 : 0);
						}
						XP_DEBUG_OUT("\n");
					}	
					XP_DEBUG_OUT("\n");

					for( int k = 0; k < 4; k++)
					{
						pInGame->sq.ps[k].x = gameData[index] - 48;   index++;
						pInGame->sq.ps[k].y = gameData[index] - 48;   index++;
					}
					userCount++;
				}
			}
			break;

		case DIE:
			{
				const char NAME = '@';
				int index = 0;

				int userCount = 0;	

				while( userCount < m_player_list.size() )
				{
					if( gameData[index] == 0 )
						break;

					index++;
					int record = index;

					while( gameData[index] != NAME )
					{
						index++;
					}
					char username[20] = {0};
					XP_API_MEMCPY( username, &gameData[record], index - record);

					index++;

					if( XP_API_STRCMP(m_username, username ) == 0 )
					{
						(*m_pGameMyself)->m_GameOverType = (CInGame::GAMEOVERTYPE)(gameData[index] - 48);   index++;
						m_GameType = GAMETYPE_OVER;
					}

					userCount++;
				}
			}
			break;
		}
		XP_API_DEL( gameData );
	}
}

#ifdef LOBBY_THREAD_TEST
bool GameStateMultiplayerLobby::SaveTestLog()
{
	return false;

	static CRITICAL_SECTION cs0 = {0} ;
	if (cs0.SpinCount != 400)
	{
		InitializeCriticalSectionAndSpinCount( &cs0 ,400 ) ;
	}

	EnterCriticalSection( &cs0 ) ;

	XP_FILE_HANDLE pFile;

	char name[50] = { 0 };
	XP_API_SPRINTF( name, "%s_%s.txt", LOBBY_TEST_FILE_NAME, m_username );

	pFile = XP_API_FILE_OPEN( name, "wb" );

	if(!pFile)
	{
		XP_DEBUG_OUT("SaveTestLog(), Error creating file %s \n", name);
		return false;
	}

	signed   int nWritten;
	
	int i = 0;
	char newline[] = "\r\n";
	while( XP_API_STRLEN(m_sSaveOnlineLog[i]) != 0 )
	{
		nWritten = XP_API_FILE_WRITE((void*)m_sSaveOnlineLog[i],	1, XP_API_STRLEN(m_sSaveOnlineLog[i]),		pFile);
		nWritten = XP_API_FILE_WRITE((void*)newline,	1, XP_API_STRLEN(newline),		pFile);

		i++;
	}

	XP_API_FILE_CLOSE(pFile);

	LeaveCriticalSection( &cs0 ) ;

	return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
CInGame::CInGame()
{
	for (int i = 0; i < MAX_SQUARE_COUNT; i++)
	{
		for (int j = 0; j < MAX_SQUARE_COUNT; j++)
		{
			m_bFilledSquare[i][j] = false;
		}
	}	

	for (int i = 0; i < 4; i++)
	{
		sq.ps[i].x = 0;
		sq.ps[i].y = 0;
	}	

	m_username = NULL;

	m_GameOverType = GAMEOVERTYPE_NONE;
}
