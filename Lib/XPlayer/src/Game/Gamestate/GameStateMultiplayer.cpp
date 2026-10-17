#include "GameStateMultiplayer.h"
#include "MyRoomQueryCondition.h"

#ifdef OS_IPHONE
extern void GetSaveFilePath(char* filePath, const char* filename);
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif


GameStateMultiplayer::GameStateMultiplayer(GameStateHandler * handler) : GameState( handler )
{
	m_multiPlayerMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);	
	m_multiPlayerMenu->AddMenuItem( MENU_MP_LOCAL, this );	
	m_multiPlayerMenu->AddMenuItem( MENU_MP_ONLINE, this );	
	
	m_multiPlayerMenuOnline = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_multiPlayerMenuOnline->AddMenuItem( MENU_MP_ONLINE_JOIN, this );	
	m_multiPlayerMenuOnline->AddMenuItem( MENU_MP_ONLINE_CREATE, this );	
	m_multiPlayerMenuOnline->AddMenuItem( MENU_MP_ONLINE_HISCORE, this );	
	
	m_multiPlayerMenuLocal = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);	
	m_multiPlayerMenuLocal->AddMenuItem( MENU_MP_LOCAL_BLUETOOTH, this );	
	m_multiPlayerMenuLocal->AddMenuItem( MENU_MP_LOCAL_WIFI, this );	
	
	m_multiPlayerMenuHiscore = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_multiPlayerMenuHiscore->AddMenuItem( MENU_RANK, this );
	m_multiPlayerMenuHiscore->AddMenuItem( MENU_TOP_RANK, this );
	m_multiPlayerMenuHiscore->AddMenuItem( MENU_RANK_AROUND, this );
	m_multiPlayerMenuHiscore->AddMenuItem( MENU_TOP_FRIEND_RANK, this );
	
	m_GLXPlayerUser = XP_API_NEW GLXPlayerUser();
	m_GLXPlayerUser->RegisterObserver( this );
	
	m_GLXPlayerLeaderboard = XP_API_NEW GLXPlayerLeaderboard();
	m_GLXPlayerLeaderboard->RegisterObserver( this );
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPbyP2P(this);
#else
	m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPbyServer(this);	
#endif
	
	m_GLXPlayerLogin = XP_API_NEW GLXPlayerLogin();	
	m_GLXPlayerLogin->RegisterObserver( this );
	
	m_GLXPlayerMessage = XP_API_NEW GLXPlayerMessage();
	m_GLXPlayerMessage->RegisterObserver( this );
	
	m_GLXPlayerUserFriend = XP_API_NEW GLXPlayerUserFriend();
	m_GLXPlayerUserFriend->RegisterObserver( this );
	
	m_state = DEF::GAME_STATE_MULTIPLAYER;
	SetOnlineSubState(ONLINE_MP_MENU);	
	
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
	
	LoadProfile();
	m_nationData = XP_API_NEW char[32];
	XP_API_MEMSET( m_nationData, 0, 32 );
	XP_API_SPRINTF( m_nationData, "VietNam");
	
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
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_gamedata = 0;
	m_gamedata_old = 0;

	m_recv_user = NULL;
	m_recv_data = NULL;
#endif
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
}

GameStateMultiplayer::~GameStateMultiplayer()
{
	XP_API_DEL( m_GLXPlayerUser );
	XP_API_DEL( m_GLXPlayerMultiPlayer );
	XP_API_DEL( m_GLXPlayerLeaderboard );
	XP_API_DEL( m_GLXPlayerLogin );
	XP_API_DEL( m_multiPlayerMenu );
	XP_API_DEL( m_multiPlayerMenuOnline );
	XP_API_DEL( m_multiPlayerMenuLocal );
	XP_API_DEL( m_multiPlayerMenuHiscore );
	XP_API_DEL( m_InputBox );	
	XP_API_DEL( m_nationData );
	XP_API_DEL( m_email );
	XP_API_DEL( m_scoreArray );	
	XP_API_DEL( m_errMsg );
	XP_API_DEL( m_GLXPlayerMessage );
	XP_API_DEL( m_GLXPlayerUserFriend );
	XP_API_DEL_ARRAY( m_playerName , m_numberPlayer);
	XP_API_DEL( m_playerScore );
}


bool GameStateMultiplayer::LoadProfile()
{
	XP_FILE_HANDLE pFile;

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "rb" );	
#else
	pFile = XP_API_FILE_OPEN( PROFILE_FILE_NAME, "rb" );
#endif

	XP_API_MEMSET(m_username, 0, 15);
	XP_API_MEMSET(m_password, 0, 32);
		
	if(!pFile)
	{
		XP_DEBUG_OUT("LoadProfile(), File %s not found \n", PROFILE_FILE_NAME);
		return false;
	}
	signed   int nRead;	
	
	nRead = XP_API_FILE_READ((void*)m_username,	1, sizeof(m_username),	pFile);
	nRead = XP_API_FILE_READ((void*)m_password,	1, sizeof(m_password),	pFile);	

	XP_API_FILE_CLOSE(pFile);
	return true;
}

bool GameStateMultiplayer::SaveProfile()
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

void GameStateMultiplayer::Paint()
{
	switch(m_sub_state)
	{
		case ONLINE_PROCESS:		
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
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
		case ONLINE_RANKING:		
			m_handler->ClearScreen();
			DrawRanking(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;	
		case ONLINE_TOP_RANKING:		
			m_handler->ClearScreen();
			DrawTopRanking(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;	
		case ONLINE_RANKING_AROUND:		
			m_handler->ClearScreen();
			DrawRankingAround(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;		
		case ONLINE_TOP_FRIEND_RANKING:		
			m_handler->ClearScreen();
			DrawTopFriendRanking(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;	
			
		case ONLINE_MSG_LIST:		
			m_handler->ClearScreen();
			DrawMsgList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_MSG_READ:			
			DrawMessage(m_handler);		
			break;	
		case ONLINE_MSG_INFO:		
			m_handler->ClearScreen();
			DrawMsgInfo(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;				
		case ONLINE_MP_PROCESS:
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;
		case ONLINE_MP_IN_SESSION:
			m_handler->ClearScreen();
			DrawSession(m_handler);
			break;
		case ONLINE_MP_IN_SESSION_LIST:
			m_handler->ClearScreen();
			DrawSessionList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_JOIN_BACK);
			break;
		case ONLINE_MP_IN_GAME:
			m_handler->ClearScreen();
			DrawInGame(m_handler);
			DrawCommandBar(m_handler,GameState::CB_FINISH_LEAVE);		
			break;
		case ONLINE_MP_MENU:
			m_handler->ClearScreen();
			m_multiPlayerMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	
		case ONLINE_MP_MENU_ONLINE:
			m_handler->ClearScreen();
			m_multiPlayerMenuOnline->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	
		case ONLINE_MP_MENU_LOCAL:
			m_handler->ClearScreen();
			m_multiPlayerMenuLocal->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	
		case ONLINE_MP_MENU_HISCORE:
			m_handler->ClearScreen();
			m_multiPlayerMenuHiscore->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_MP_END_GAME:
			m_handler->ClearScreen();
			DrawMPEndGame(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_MP_EVALUATE_PLAYER:
			m_handler->ClearScreen();
			DrawEvaluatePlayer(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_MP_SESSION_BUDDY:
			m_handler->ClearScreen();
			DrawMPSessionBuddy(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_MP_SESSION_BUDDY_PLAY:
			DrawInviteToPlayGame(m_handler);
			break;
			
		default:
			break;
	}	
}

bool GameStateMultiplayer::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;
	switch(m_sub_state)
	{
		case ONLINE_LOGIN:
			bHandled = HandleEventLogin(event);
			break;		
		case ONLINE_REGISTER:
			bHandled = HandleEventRegister(event);
			break;			
		case ONLINE_ERROR:	
		case ONLINE_MP_ERROR:		
			bHandled = HandleEventError( event );
			break;
		case ONLINE_PROCESS:
		case ONLINE_MP_PROCESS:
			bHandled = HandleEventProcess( event );
			break;		
		case ONLINE_MP_MENU:		
			bHandled = HandleEventMPMenu( event );
			break;			
		case ONLINE_MP_MENU_ONLINE:		
			bHandled = HandleEventMPMenuOnline( event );
			break;			
		case ONLINE_MP_MENU_LOCAL:		
			bHandled = HandleEventMPMenuLocal( event );
			break;			
		case ONLINE_MP_MENU_HISCORE:		
			bHandled = HandleEventMPMenuHiscore( event );
			break;			
		case ONLINE_RANKING:		
		case ONLINE_TOP_RANKING:		
		case ONLINE_RANKING_AROUND:		
		case ONLINE_TOP_FRIEND_RANKING:		
			bHandled = HandleEventRanking( event );
			break;
		case ONLINE_MSG_INFO:		
			bHandled = HandleMsgInfo( event);
			break;	
		case ONLINE_MSG_LIST:		
			bHandled = HandleMsgList( event );
			break;
		case ONLINE_MSG_READ:		
			bHandled = HandleViewMessage( event );
			break;
			
		case ONLINE_MP_IN_SESSION:		
			bHandled = HandleEventSession( event );
			break;	
		case ONLINE_MP_IN_SESSION_LIST:		
			bHandled = HandleEventSessionList( event );
			break;	
		case ONLINE_MP_IN_GAME:		
			bHandled = HandleEventInGame( event );
			break;	
		case ONLINE_MP_END_GAME:		
			bHandled = HandleEventMPEndGame( event );
			break;	
		case ONLINE_MP_EVALUATE_PLAYER:		
			bHandled = HandleEventEvaluatePlayer( event );
			break;	
		case ONLINE_MP_SESSION_BUDDY:		
			bHandled = HandleEventMPSessionBuddy( event );
			break;
		case ONLINE_MP_SESSION_BUDDY_PLAY:
			bHandled = HandleEventInviteToPlayGame( event );
			break;

		default:
			break;
	}
	return bHandled;
}

void GameStateMultiplayer::DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool isDrawUpDown )
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
	case CB_JOIN_BACK:
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
	default:
		break;
	}

	GameState::DrawCommandBar(handler, state, isDrawUpDown);
}

void GameStateMultiplayer::UpdateAnimation( )
{
	m_GLXPlayerLogin->Update();
	m_GLXPlayerUser->Update();	
	m_GLXPlayerLeaderboard->Update();	
	m_GLXPlayerMessage->Update();
	m_GLXPlayerUserFriend->Update();

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->Update();
	if(m_GLXPlayerMultiPlayer->mpIsInGame())
	{
		UpdateGame();
	}
#else
	if(m_sub_state == ONLINE_MP_PROCESS || m_GLXPlayerMultiPlayer->mpIsConnected())
	{
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerMultiPlayer->Update();
		}
	}
#endif

	m_frame++;
}

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
void GameStateMultiplayer::UpdateGame()
{
	// Game completed
	if ( m_GLXPlayerMultiPlayer->mpIsGameFinished() )
	{
		//SetOnlineSubState( ONLINE_MP_MENU );
		return;
	}

	// Exit game
	if ( !m_GLXPlayerMultiPlayer->mpIsInGame() )
	{
		//SetOnlineSubState( ONLINE_MP_MENU );
		//m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
		return;
	}

	if(m_GLXPlayerMultiPlayer->mpIsGameFinishConfirm())
	{
		SetOnlineSubState(ONLINE_MP_FINISH_GAME_CONFIRM);
		return;
	}

	char       * player_name = NULL;
	DataPacket * player_data = NULL;

	// Handle all the received data
	while ( m_GLXPlayerMultiPlayer->mpCheckGameData( player_name, player_data) )
	{
		// Deal with the received data
		char buffer[1024] = { 0 };
		char * temp = NULL;
		BYTE len = 0;

		BYTE c = 0;
		short s = 0;
		int i = 0;
		float f = 0;

		player_data->getByteLenString( temp, len );
		XP_DEBUG_OUT( "<= [%s]\n", temp );
		XP_API_DEL( temp );
		temp = NULL;

		player_data->getByte( c );
		player_data->getShort( s );
		player_data->getInt( i );
		player_data->getFloat( f );

		XP_API_MEMSET( buffer, 0, 1024 );
		XP_API_SPRINTF( buffer, "%c %d %d %f", c, s, i, f );
		SaveData( player_name, buffer );

		player_data->getByteLenString( temp, len );
		XP_DEBUG_OUT( "<= [%s]\n", temp );
		XP_API_DEL( temp );
		temp = NULL;

		player_data->getByteLenString( temp, len );
		XP_DEBUG_OUT( "<= [%s]\n", temp );
		XP_API_DEL( temp );
		temp = NULL;

		player_data->getByteLenString( temp, len );
		XP_DEBUG_OUT( "<= [%s]\n", temp );
		XP_API_DEL( temp );
		temp = NULL;

		XP_API_DEL( player_name );
		XP_API_DEL( player_data );
		player_name = NULL;
		player_data = NULL;			
	}

	//m_gamedata += 1;
	if ( m_gamedata == 100 )
	{
		m_gamedata = 0;
	}

	// Send data
	if ( m_gamedata != m_gamedata_old )
	{
		char buffer[256] = { 0 };

		bool bReliable = false;

		if ( m_gamedata % 4 == 0 )
		{
			bReliable = true;
		}

		char c = 'A';
		short s = 10;
		int i = 50000;
		float f = 1234.56f;

		DataPacket * outgoing = XP_API_NEW DefaultDataPacket();

		char temp[64] = {0};
		XP_API_MEMSET(temp, 0, 64);

		if(bReliable)
		{
			XP_API_SPRINTF(temp, "%d ======TTTTTT======", m_gamedata);
			outgoing->addByteLenString(temp, XP_API_STRLEN(temp));
		}
		else
		{
			XP_API_SPRINTF(temp, "%d =====UUUUUU======", m_gamedata);
			outgoing->addByteLenString(temp, XP_API_STRLEN(temp));
		}
		outgoing->addByte(c);
		outgoing->addShort(s);
		outgoing->addInt(m_gamedata);
		outgoing->addFloat(f);
		XP_API_MEMSET( buffer, 'a', 240 );
		outgoing->addByteLenString( (char *)buffer, 240 );
		XP_API_MEMSET( buffer, 'b', 240 );
		outgoing->addByteLenString( (char *)buffer, 240 );
		XP_API_MEMSET( buffer, 'c', 240 );
		outgoing->addByteLenString( (char *)buffer, 240 );
		outgoing->packMessage();
		m_GLXPlayerMultiPlayer->mpSendGameData( outgoing, bReliable );
		m_gamedata_old = m_gamedata;
		XP_API_DEL( outgoing );
		outgoing = NULL;
	}
}
#endif //MULTIPLAYER_TYPE == MULTIPLAYER_P2P

void GameStateMultiplayer::SwitchState( )
{
	SetGameState(m_state);
}

void GameStateMultiplayer::SetOnlineSubState( OnlineSubState state )
{
	if (m_sub_state != ONLINE_PROCESS)	
	{
		m_pre_sub_state = m_sub_state;
	}
	m_sub_state = state;
}

void GameStateMultiplayer::OnMenuItemClicked( char * menu_name )
{
	if ( XP_API_STRCMP( menu_name, MENU_MP_LOCAL ) == 0 )
	{		
		SetOnlineSubState(ONLINE_MP_MENU_LOCAL);			
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_MP_ONLINE ) == 0 )
	{					
		SetOnlineSubState(ONLINE_LOGIN);		
	}			
	else if ( XP_API_STRCMP( menu_name, MENU_MP_ONLINE_JOIN ) == 0 )
	{		
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		char name[] = "Old Trafford";
		int value = 5;
		CMyRoomQueryCondition * query = XP_API_NEW CMyRoomQueryCondition();
		DefaultDataPacket * packet = XP_API_NEW DefaultDataPacket();
		//query->And( "pitch", ONLINE_QUERY_EQUAL, &name );
		query->And( "substitue", ONLINE_QUERY_LESS, &value );
		query->ToPacket( packet );
		//m_GLXPlayerMultiPlayer->mpSendListRoomByData( 5, 0, packet );
		//m_GLXPlayerMultiPlayer->mpSendListRoomByName( 5, 0, "playername" );
		//m_GLXPlayerMultiPlayer->mpSendListRoom( 5, 0, NULL );
		m_GLXPlayerMultiPlayer->mpSendListAllRoom( 5, 0 );
		XP_API_DEL( packet );
		XP_API_DEL( query );	
#else
		m_GLXPlayerMultiPlayer->mpSendListSession(MAX_LIST_SIZE,0);	
#endif
		SetOnlineSubState(ONLINE_MP_PROCESS);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_MP_ONLINE_CREATE ) == 0 )
	{		
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		char pitch[] = "Old Trafford";
		int substitute = 3;
		float  half_time = 4.5;
		BYTE choose_same_team = 'Y';
		char data_play[] = "ManUtd";

		DataPacket * packet_room = XP_API_NEW DefaultDataPacket();
		packet_room->addByteLenString( pitch, XP_API_STRLEN( pitch ) );
		packet_room->addInt( substitute );
		packet_room->addFloat( half_time );
		packet_room->addByte( choose_same_team );
		packet_room->packMessage();

		DataPacket * packet_play = XP_API_NEW DefaultDataPacket();
		packet_play->addByteLenString( data_play, XP_API_STRLEN( data_play ) );
		packet_play->packMessage();

		m_GLXPlayerMultiPlayer->mpSendCreateRoom(m_username, packet_room, packet_play, 2, 2);

		XP_API_DEL( packet_room );
		XP_API_DEL( packet_play );
#else
		m_GLXPlayerMultiPlayer->mpSendCreateSession(m_username, "datta");
#endif
		SetOnlineSubState(ONLINE_MP_PROCESS);		
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_MP_ONLINE_HISCORE ) == 0 )
	{			
		SetOnlineSubState(ONLINE_MP_MENU_HISCORE);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_RANK ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_isRankAround = 0;
		m_GLXPlayerLeaderboard->sendHighScore(m_score,1);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_TOP_RANK ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_isRankAround = 1;
		//By this way, you can send multiScore or/and with supplemental data...
		m_GLXPlayerLeaderboard->addScoreEntry(m_scoreArray, m_score, 1);
		m_GLXPlayerLeaderboard->sendHighScore(m_scoreArray);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_RANK_AROUND ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_isRankAround = 2;
		//This way is traditional, send basic score only...
		m_GLXPlayerLeaderboard->sendHighScore(m_score,1);
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_TOP_FRIEND_RANK ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_isRankAround = 3;
		//This way is traditional, send basic score only...
		m_GLXPlayerLeaderboard->sendHighScore(m_score,1);
	}	
}

void GameStateMultiplayer::DrawWaiting( GameStateHandler * handler )
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

void GameStateMultiplayer::DrawLogin( GameStateHandler * handler )
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

void GameStateMultiplayer::DrawRegister( GameStateHandler * handler )
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
	XP_API_SPRINTF( tmp, "Email: %s", m_email);
	handler->DrawString(x,y,tmp);	
	
	y+=2*DEF::LINE_HEIGHT;
	XP_API_MEMSET( tmp, 0, 128 );
	XP_API_SPRINTF( tmp, "Receive Email: %d", m_emailFlag);
	handler->DrawString(x,y,tmp);	
	
	m_InputBox->showTextBox(handler);
}

void GameStateMultiplayer::DrawError( GameStateHandler * handler )
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

void GameStateMultiplayer::DrawMPError(GameStateHandler * handler)
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

void GameStateMultiplayer::DrawSession(GameStateHandler * handler)
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	int x = 20;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };

	if (m_GLXPlayerMultiPlayer->mpIsMaster())
	{
		DrawCommandBar(m_handler,GameState::CB_START_KICKOUT);
	}
	else
	{
		DrawCommandBar(m_handler,GameState::CB_LEAVE_NULL, false);
	}

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "Myname is %s",m_GLXPlayerMultiPlayer->GetUsername());
	handler->DrawString(x,y,line);

	y += DEF::LINE_HEIGHT;
	XP_API_MEMSET( line, 0, 128 );

	COnlineGameRoom * room = m_GLXPlayerMultiPlayer->mpGetCurrentGameRoom();

	if ( NULL != room->GetData() )
	{
		char * pitch = NULL;
		BYTE len = 0;
		int sub = 0;
		float time = 0;
		BYTE same_room = -1;

		DataPacket * packet = XP_API_NEW DefaultDataPacket( room->GetData() );
		packet->getByteLenString( pitch, len );
		packet->getInt( sub );
		packet->getFloat( time );
		packet->getByte( same_room );
		XP_API_DEL( packet );

		XP_API_SPRINTF( line, "%s %d %f %c",pitch,sub,time,same_room);
		XP_API_DEL( pitch );
		handler->DrawString(x,y,line);
	}

	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,"Player List:");

	int count = room->GetPlayerCount();

	for ( int i=0; i<count; i++ )
	{
		COnlinePlayer * player = room->GetPlayer( i );

		char * data = NULL;
		BYTE len = 0;
		if ( NULL != player->GetData() )
		{
			DataPacket * packet = XP_API_NEW DefaultDataPacket( player->GetData() );
			packet->getByteLenString( data, len );
			XP_API_DEL( packet );
		}

		XP_API_MEMSET( line, 0, 128 );
		if ( i == (m_selectedIndex+1) )
		{
			XP_API_SPRINTF( line, "> %d %s %s %s",player->GetIndex(),player->GetName(),player->GetNationality(), data );
		}
		else
		{
			XP_API_SPRINTF( line, "  %d %s %s %s",player->GetIndex(),player->GetName(),player->GetNationality(), data );
		}
		XP_API_DEL( data );

		y += DEF::LINE_HEIGHT;
		handler->DrawString(x,y,line);
	}
#else //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
	if (m_GLXPlayerMultiPlayer->mpIsMaster())
	{
		//DrawCommandBar(m_handler,GameState::CB_START_KICKOUT);
		m_handler->DrawButton(m_UpKey);
		m_handler->DrawButton(m_DownKey);
		m_handler->DrawButton(m_SoftLeft);
		m_handler->DrawButton(m_SoftRight);
		m_handler->DrawButton(m_SoftMid);
		m_handler->DrawButton(m_SoftAdd1);
		m_handler->DrawButton(m_SoftAdd2);

		m_handler->DrawString(m_UpKey.left,m_UpKey.top,"Up");
		m_handler->DrawString(m_DownKey.left,m_DownKey.top,"Down");
		m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Start");
		m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Kick");
		m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Leave");
		m_handler->DrawString(m_SoftAdd1.left,m_SoftAdd1.top,"Invite");
		m_handler->DrawString(m_SoftAdd2.left,m_SoftAdd2.top,"Send friend invitation");
	}
	else
	{
		DrawCommandBar(m_handler,GameState::CB_LEAVE_NULL, false);
	}
	
	#define SIZE_OF_TMP 256
	char tmp[SIZE_OF_TMP] = {0};
	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	
	handler->DrawString(x,y,"--- Session ---");
	y+=2*DEF::LINE_HEIGHT;
	
	if (m_GLXPlayerMultiPlayer->getNameList() == NULL)
	{
		return;
	}

	if (m_GLXPlayerMultiPlayer->getNumberOfItems() > 0)
	{
		XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
		XP_API_SPRINTF(tmp, "Name: %s", m_GLXPlayerMultiPlayer->getSessionName());
		
		handler->DrawString(x,y, tmp);
		y+=2*DEF::LINE_HEIGHT;

		char* sessiondata = m_GLXPlayerMultiPlayer->getSessionData();
		if (sessiondata != NULL)
		{
			XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
			XP_API_SPRINTF(tmp, "Data: %s", sessiondata);
			
			handler->DrawString(x,y, tmp);
			y+=2*DEF::LINE_HEIGHT;
		}

		for (int i = 0; i < m_GLXPlayerMultiPlayer->getNumberOfItems();i++)
		{
			XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
			
			if (m_selectedIndex == i)
			{
				XP_API_SPRINTF(tmp, "> %d. %s", m_GLXPlayerMultiPlayer->getByteDataList()[i],m_GLXPlayerMultiPlayer->getNameList()[i]);
			}
			else
			{
				XP_API_SPRINTF(tmp, " %d. %s", m_GLXPlayerMultiPlayer->getByteDataList()[i],m_GLXPlayerMultiPlayer->getNameList()[i]);
			}

			handler->DrawString(x,y, tmp);
			y+=DEF::LINE_HEIGHT;
		}		
	}
	else
	{
		handler->DrawString(x,y, "These is no data ...");
	}
#endif //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
}

void GameStateMultiplayer::DrawSessionList(GameStateHandler * handler)
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	int x = 20;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };

	handler->DrawString(x,y,"Room List:");

	COnlineGameRoomList * list = m_GLXPlayerMultiPlayer->mpGetGameRoomList();

	for ( int i=0; i<list->Count(); i++ )
	{
		XP_API_MEMSET( line, 0, 128 );
		COnlineGameRoom * room = list->Get( i );
		int index = room->GetIndex();
		long time = room->GetPingTime();
		char * name = room->GetName();
		char * country = room->GetHost()->GetNationality();
		int num = room->GetPlayerCount();
		char lock = ' ';
		if ( room->HasPassword() )
		{
			lock = '#';
		}

		if ( i == m_selectedIndex )
		{
			if ( time == 0 )
			{
				XP_API_SPRINTF( line, ">%d %s %s(%d) %c ...",index,name,country,num,lock);
			}
			else if ( time > 0 )
			{
				XP_API_SPRINTF( line, ">%d %s %s(%d) %c %dms",index,name,country,num,lock,time);
			}
			else
			{
				XP_API_SPRINTF( line, ">%d %s %s(%d) %c unreachable",index,name,country,num,lock);
			}
		}
		else
		{
			if ( time == 0 )
			{
				XP_API_SPRINTF( line, " %d %s %s(%d) %c ...",index,name,country,num,lock);
			}
			else if ( time > 0 )
			{
				XP_API_SPRINTF( line, " %d %s %s(%d) %c %dms",index,name,country,num,lock,time);
			}
			else
			{
				XP_API_SPRINTF( line, " %d %s %s(%d) %c unreachable",index,name,country,num,lock);
			}
		}		

		y += DEF::LINE_HEIGHT;
		handler->DrawString(x,y,line);		
	}
#else //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	#define SIZE_OF_TMP 256
	char tmp[SIZE_OF_TMP] = {0};

	handler->DrawString(x,y,"--- Room List ---");
	y+=2*DEF::LINE_HEIGHT;
	
	if (m_GLXPlayerMultiPlayer->getNumberOfItems() > 0)
	{
		// display the session list
		if (m_GLXPlayerMultiPlayer->getNameList() == NULL)
		{
			return;
		}				

		for (int i = 0; i < m_GLXPlayerMultiPlayer->getNumberOfItems();i++)
		{
			XP_API_MEMSET( tmp, 0, SIZE_OF_TMP );
			if (m_selectedIndex == i)
			{
				XP_API_SPRINTF(tmp, "> %d.%s-%d ", (m_GLXPlayerMultiPlayer->getFirstSessionIndex() + 1 + i),m_GLXPlayerMultiPlayer->getNameList()[i],m_GLXPlayerMultiPlayer->getByteDataList()[i]);
			}
			else
			{
				XP_API_SPRINTF(tmp, " %d. %s-%d ", (m_GLXPlayerMultiPlayer->getFirstSessionIndex() + 1 + i),m_GLXPlayerMultiPlayer->getNameList()[i],m_GLXPlayerMultiPlayer->getByteDataList()[i]);
			}

			// put the session name
			char* sessiondata = m_GLXPlayerMultiPlayer->getDataList()[i];

			if (sessiondata != NULL)
			{
				// put the session data
				if (SIZE_OF_TMP >= (STRLEN(tmp) + STRLEN(sessiondata)))
				{
					XP_API_STRCAT(tmp, sessiondata);
				}
			}

			handler->DrawString( x,y, tmp);
			y+=DEF::LINE_HEIGHT;
		}		
	}
	else
	{
		handler->DrawString( x,y, "These is no session ...");
	}
#endif //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
}

void GameStateMultiplayer::DrawInGame(GameStateHandler * handler)
{	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	int x = 20;
	int y = DEF::START_POS_Y;
	char msg[256];

	COnlineGameRoom * room = m_GLXPlayerMultiPlayer->mpGetCurrentGameRoom();
	COnlinePlayer * myself = room->GetMyself();

	XP_API_MEMSET( msg, 0, 256 );
	if ( room->AmITheHost() )
	{
		XP_API_SPRINTF( msg, "Host: %s %s", myself->GetName(), myself->GetNationality() );
	}
	else
	{
		XP_API_SPRINTF( msg, "Client: %s %s", myself->GetName(), myself->GetNationality() );
	}
	handler->DrawString(x,y,msg);
	
	XP_API_MEMSET( msg, 0, 256 );
	char * temp = NULL;
	BYTE len = 0;
	DataPacket * packet = XP_API_NEW DefaultDataPacket( myself->GetData() );
	packet->getByteLenString( temp, len );
	XP_API_DEL( packet );
	XP_API_SPRINTF( msg, "Info %s", temp );
	XP_API_DEL( temp );
	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,msg);

	XP_API_MEMSET( msg, 0, 256 );
	XP_API_SPRINTF( msg, "%s %d %d", myself->GetExternalIP(), myself->GetExternalTcpPort(), myself->GetExternalUdpPort() );
	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,msg);

	XP_API_MEMSET( msg, 0, 256 );
	XP_API_SPRINTF( msg, "%s %d %d", myself->GetInternalIP(), myself->GetInternalTcpPort(), myself->GetInternalUdpPort() );
	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,msg);

	XP_API_MEMSET( msg, 0, 256 );
	XP_API_SPRINTF( msg, "MyData %d", m_gamedata );
	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,msg);

	XP_API_MEMSET( msg, 0, 256 );
	if ( m_recv_data != NULL )
	{
		XP_API_SPRINTF( msg, "OpData < %s %s", m_recv_user, m_recv_data );
	}
	else
	{	
		XP_API_SPRINTF( msg, "OpData", m_recv_user, m_recv_data );
	}
	
	y += 2*DEF::LINE_HEIGHT;
	handler->DrawString(x,y,msg);

	y = SCREEN_HEIGHT - 2*DEF::LINE_HEIGHT;
	handler->DrawString(0,y,"Key UP +10 DOWN -10");
#else //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
	if (m_GLXPlayerMultiPlayer->getNumberOfItems() > 0)
	{		
		#define SIZE_OF_TMP 256
		char tmp[SIZE_OF_TMP] = {0};
		
		for (int i = 0; i < m_GLXPlayerMultiPlayer->getNumberOfItems();i++)
		{
			XP_API_SPRINTF(tmp,m_GLXPlayerMultiPlayer->getNameList()[i]);

			if (XP_API_STRCMP(tmp, m_username) == 0)
			{
				handler->DrawString( gameX, gameY, tmp);
			}
			else
			{
				DataPacket* nextGamePacket = m_GLXPlayerMultiPlayer->getNextInGameMessage();

				 if (nextGamePacket != NULL)
				 {
					nextGamePacket->getInt(oppX);
					nextGamePacket->getInt(oppY);
				 }
				 
				// this is opponent
				if (m_GLXPlayerMultiPlayer->mpHasOpponnentFinished())
				{
					m_isWin = false;
					if (SIZE_OF_TMP >= (XP_API_STRLEN(tmp) + XP_API_STRLEN("-finished")))
					{
						XP_API_STRCAT(tmp, "-finished");
					}
				}
				handler->DrawString( oppX, oppY, tmp);
			}
		}		
	}
	else
	{
		handler->DrawString(DEF::START_POS_X, DEF::START_POS_Y, "These is no data ... ");
	}		
#endif //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
}

void GameStateMultiplayer::DrawMPEndGame(GameStateHandler * handler)
{		
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	
	handler->DrawString(x,y,"--- Debriefing ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char tmp[128] = {0};	
	
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp,"My score: %d", m_score);	
	handler->DrawString(x,y,tmp);
	y += 2*DEF::LINE_HEIGHT;
	
	int i = 0;	
	for(i = 0; i < m_numberPlayer; i++)
	{
		XP_API_MEMSET(tmp, 0, 128);
		XP_API_SPRINTF(tmp,"%s - %d",m_playerName[i], m_playerScore[i]);
		if(m_selectedIndex == i)
		{
			handler->DrawString(x - 20, y, ">");
		}
		handler->DrawString(x,y,tmp);
		y+=DEF::LINE_HEIGHT;
	}
	if(m_selectedIndex == i)
	{
		handler->DrawString(x - 20, y, ">");
	}
	handler->DrawString(x,y,"Rematch");	
}

void GameStateMultiplayer::DrawMPSessionBuddy(GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- Friends List ---");
	y+=2*DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"No - Name");
	y+=DEF::LINE_HEIGHT;

	char line[256] = { 0 };
	
	for(int i=0; i < m_GLXPlayerUserFriend->GetFriendsCount(); i++) 
	{
		if(m_selectedIndex == i)
		{
			handler->DrawString(x - 20 , y, ">");
		}
		XP_API_SPRINTF( line, "%d %s", i+1 , m_GLXPlayerUserFriend->GetName(i));				
		handler->DrawString(x,y,line);
		
		y += DEF::LINE_HEIGHT;
	}
}

void GameStateMultiplayer::DrawEvaluatePlayer(GameStateHandler * handler)
{		
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	
	handler->DrawString(x,y,"--- Evaluate Player ---");
	y+=2*DEF::LINE_HEIGHT;
	
	char tmp[128] = {0};	
	
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp,"Player: %s      Score: %d", m_playerName[m_currentUserIndex], m_playerScore[m_currentUserIndex]);	
	handler->DrawString(x,y,tmp);
	y += 2*DEF::LINE_HEIGHT;	
	
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20, y, ">");
	}
	handler->DrawString(x,y,"Good");
	y+=DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20, y, ">");
	}
	handler->DrawString(x,y,"Bad");
	y+=DEF::LINE_HEIGHT;
}

void GameStateMultiplayer::DrawRanking( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- User Ranking ---");
	y += 2*DEF::LINE_HEIGHT;
	
	char tmp[32] = {0};	
	XP_API_SPRINTF(tmp, "My rank: %d", m_GLXPlayerLeaderboard->getCurrentPlayerLeaderboardPosition());
	handler->DrawString(x,y,tmp);
	y+= DEF::LINE_HEIGHT;
	
	XP_API_MEMSET(tmp, 0, 32);
	XP_API_SPRINTF(tmp, "My score: %d", m_score);
	handler->DrawString(x,y,tmp);
}

void GameStateMultiplayer::DrawTopRanking( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- Top Ranking ---");
	y += DEF::LINE_HEIGHT;
	
	int count = m_GLXPlayerLeaderboard->getLeaderboardSize();
	char line[128] = { 0 };
	int i = 0;
	for (i = 0; i < count; i++)
	{
		XP_API_MEMSET( line, 0, 128 );
		XP_API_SPRINTF( line, "%d %s %s %d",m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerPosition(i),
			m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName(i), m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerCountry(i),
			m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore(i));

		y += DEF::LINE_HEIGHT;
		handler->DrawString(x,y,line);
		if(i == 4)
			break;
	}
	
	y += 2*DEF::LINE_HEIGHT;	
	x = DEF::START_POS_X;
	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "My Rank: %d", m_GLXPlayerLeaderboard->getCurrentPlayerLeaderboardPosition());
	
	handler->DrawString(x,y,line);

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "My Score: %d",m_GLXPlayerLeaderboard->getCurrentPlayerLeaderboardScore());
	y += DEF::LINE_HEIGHT;
	handler->DrawString(x,y,line);
	
	if(i >= 4)
	{
		i++;
		x = 220;
		y = DEF::START_POS_Y + DEF::LINE_HEIGHT;
		for (; i<count; i++)
		{
			XP_API_MEMSET( line, 0, 128 );
			XP_API_SPRINTF( line, "%d %s %s %d",m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerPosition(i),
				m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName(i), m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerCountry(i),
				m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore(i));

			y += DEF::LINE_HEIGHT;
			handler->DrawString(x,y,line);
		}		
	}	
}

void GameStateMultiplayer::DrawRankingAround( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- Ranking Around Player ---");
	y += DEF::LINE_HEIGHT;
	
	int count = m_GLXPlayerLeaderboard->getLeaderboardSize();
	char line[128] = { 0 };
	int i = 0;
	for (i = 0; i < count; i++)
	{
		XP_API_MEMSET( line, 0, 128 );
		XP_API_SPRINTF( line, "%d %s %d",m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerPosition(i),
			m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName(i), m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore(i));

		y += DEF::LINE_HEIGHT;
		handler->DrawString(x,y,line);
		if(i == 4)
			break;
	}
	
	if(i >= 4)
	{
		i++;
		x = 220;
		y = DEF::START_POS_Y + DEF::LINE_HEIGHT;
		for (; i<count; i++)
		{
			XP_API_MEMSET( line, 0, 128 );
			XP_API_SPRINTF( line, "%d %s %d",m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerPosition(i),
				m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerName(i),m_GLXPlayerLeaderboard->getLeaderboardEntryPlayerScore(i));

			y += DEF::LINE_HEIGHT;
			handler->DrawString(x,y,line);
		}		
	}	
}

void GameStateMultiplayer::DrawTopFriendRanking( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"---Top Friend Ranking ---");
	y += DEF::LINE_HEIGHT;	
	
}

void GameStateMultiplayer::DrawMsgList( GameStateHandler * handler)
{
		int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	
	char line[128] = { 0 };
	
	handler->DrawString(x,y,"--- Message List ---");
	y += 2*DEF::LINE_HEIGHT;			
	
	for(int i=0; i < m_GLXPlayerMessage->getMsgListCount(); i++) 
	{
		if(m_selectedIndex == i)
		{
			handler->DrawString(x - 20 , y, ">");
		}
		XP_API_SPRINTF( line, "%d %d %s %s", i+1 , m_GLXPlayerMessage->getMsgID(i), m_GLXPlayerMessage->getMsgTitle(i),  m_GLXPlayerMessage->getMsgSender(i));				
		handler->DrawString(x,y,line);
		
		y += DEF::LINE_HEIGHT;
	}
}

void GameStateMultiplayer::DrawMsgInfo( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- Message notice ---");
	y += 2*DEF::LINE_HEIGHT;	
	
	char line[128] = { 0 };
	
	XP_API_MEMSET(line, 0, 128);
	XP_API_SPRINTF( line, "Game Message: %d.",m_gameMsgCount);
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	handler->DrawString(x,y,line);	
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET(line, 0, 128);
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	XP_API_SPRINTF( line, "User Message: %d.",m_userMsgCount);
	handler->DrawString(x,y,line);	
	y += DEF::LINE_HEIGHT;
		XP_API_MEMSET(line, 0, 128);
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	XP_API_SPRINTF( line, "Friend invitation: %d.",m_friendInvitedMsgCount);
	handler->DrawString(x,y,line);	
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET(line, 0, 128);
	if(m_selectedIndex == 3)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	XP_API_SPRINTF( line, "Play game invitation: %d.",m_gameInvitedMsgCount);
	handler->DrawString(x,y,line);	
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET(line, 0, 128);
	if(m_selectedIndex == 4)
	{
		handler->DrawString(x - 20 , y, ">");
	}
	XP_API_SPRINTF( line, "Chat invitation: %d.",m_chatInvitedMsgCount);
	handler->DrawString(x,y,line);	
	y += DEF::LINE_HEIGHT;	
		
}

void GameStateMultiplayer::DrawMessage( GameStateHandler * handler )
{
	m_handler->ClearScreen();
	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- View Message ---");
	y += 2*DEF::LINE_HEIGHT;	
	
	char line[128] = { 0 };
	
	int i = m_GLXPlayerMessage->getCurrentMsgPosition();	
	
	if( i >= 0 )
	{		
		XP_API_SPRINTF( line, "MessageID: %d", m_GLXPlayerMessage->getMsgID(i));
		handler->DrawString(x,y,line);			
		y += DEF::LINE_HEIGHT;
		
		XP_API_MEMSET(line, 0, 128);
		XP_API_SPRINTF( line, "Subject: %s", m_GLXPlayerMessage->getMsgTitle(i));
		handler->DrawString(x,y,line);	
		y += DEF::LINE_HEIGHT;
		
		XP_API_MEMSET(line, 0, 128);
		XP_API_SPRINTF( line, "From: %s", m_GLXPlayerMessage->getMsgSender(i));				
		handler->DrawString(x,y,line);		
		y += 2*DEF::LINE_HEIGHT;
		
		handler->DrawString(x,y, "Content: ");
		y += DEF::LINE_HEIGHT;
		handler->DrawString(x,y, m_GLXPlayerMessage->getMsgBody());
			
		int t = m_GLXPlayerMessage->getMsgType(i);	
		
		if( t ==  USER_MESSAGE_TYPE_GAMEMESSAGE || 
			t ==  USER_MESSAGE_TYPE_USERMESSAGE )
		{
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
		}
		else if(t ==  USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION || 
				t ==  USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION || 
				t == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION )
		{
			DrawCommandBar(m_handler,GameState::CB_ACCEPT_IGNORE, false);
		}
	}	
}

bool GameStateMultiplayer::HandleEventLogin( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
		//m_GLXPlayerLogin->SendLogin(m_username,m_password);
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login_Game);
#endif
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
				XP_API_MEMSET( m_username, 0, 15 );
				XP_API_STRCPY( m_username, tmp );						
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

bool GameStateMultiplayer::HandleEventRegister( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
		m_GLXPlayerUser->sendRegister(m_username, m_password, m_email, m_nationData, m_emailFlag );
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


bool GameStateMultiplayer::HandleEventMPMenu( _TOUCH_EVENT event )
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

bool GameStateMultiplayer::HandleEventMPMenuOnline( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_multiPlayerMenuOnline->HandleEvent( event ) )
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

bool GameStateMultiplayer::HandleEventMPMenuLocal( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_multiPlayerMenuLocal->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_MENU);			
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventMPMenuHiscore( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_multiPlayerMenuHiscore->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_MENU_ONLINE);			
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventError( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		if(m_errorCode == ERROR_USER_HAS_LOGIN || m_errorCode == ERROR_HAS_NOT_LOGIN || m_errorCode == ERROR_NO_UUID )
		{
			SetOnlineSubState(ONLINE_MP_MENU);
		}
		else if(m_errorCode == 3) // Error when login to game server fail.
		{
			m_GLXPlayerLogin->SendLogout();
			SetOnlineSubState(ONLINE_PROCESS);
		}
		else if(m_sub_state == ONLINE_MP_MENU_ONLINE && m_errorCode == ERROR_CONNECTION)
		{
			SetOnlineSubState(ONLINE_MP_MENU);
		}
		else
		{
			SetOnlineSubState(ONLINE_MP_MENU_ONLINE);			
		}
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventProcess( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		if(m_GLXPlayerLogin->IsLoggedIn() == false)
		{
			m_GLXPlayerLogin->Cancel();
			SetOnlineSubState(ONLINE_MP_MENU);
		}
		else
		{
			m_GLXPlayerUser->Cancel();
			//add by heyi
			//cancel the thread
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			m_GLXPlayerMultiPlayer->CloseUpnpThread();
#endif
			SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
		}	
		
		bHandled = true;
	}

//#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
//	if(isStartJoin)
//	{
//		if(m_GLXPlayerMultiPlayer->mpGetConnectState() == GLXPlayerMPBase::CONNECT_STATE_WAITING)
//		{
//			m_GLXPlayerMultiPlayer->mpSendJoinRoom( NULL, NULL);
//		}
//
//		if(m_GLXPlayerMultiPlayer->mpGetConnectState() == GLXPlayerMPBase::CONNECT_STATE_SUCCESS)
//		{
//			m_GLXPlayerMultiPlayer->mpSetConnectState(GLXPlayerMPBase::CONNECT_STATE_START);
//			isStartJoin = false;
//		}
//		else if(m_GLXPlayerMultiPlayer->mpGetConnectState() == GLXPlayerMPBase::CONNECT_STATE_FAILED)
//		{
//			m_GLXPlayerMultiPlayer->mpSetConnectState(GLXPlayerMPBase::CONNECT_STATE_START);
//			isStartJoin = false;
//			SetOnlineSubState( ONLINE_ERROR );
//		}
//	}
//#endif

	return bHandled;
}


bool GameStateMultiplayer::HandleEventRanking( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_MENU_HISCORE);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleMsgList( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MSG_INFO);
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{			
		if(m_selectedIndex > -1)
		{
			m_GLXPlayerMessage->SendReadMessage(m_GLXPlayerMessage->getMsgID(m_selectedIndex));
		}
		SetOnlineSubState(ONLINE_PROCESS);
		m_selectedIndex = -1;		
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{			
		
		if (m_selectedIndex < 0)
		{
			m_selectedIndex = m_GLXPlayerMessage->getMsgListCount() - 1;
		}
		else
		{
			m_selectedIndex--;
		}
		bHandled = true;
					
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{			
		if (m_selectedIndex > m_GLXPlayerMessage->getMsgListCount() - 2)
		{
			m_selectedIndex = -1;
		}
		else
		{
			m_selectedIndex++;
		}
		bHandled = true;
				
	}
	return bHandled;
}

bool GameStateMultiplayer::HandleViewMessage( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	int i = m_GLXPlayerMessage->getCurrentMsgPosition();	
	
	if( i >= 0 )
	{		
		int t = m_GLXPlayerMessage->getMsgType(i);	
		
		if( t ==  USER_MESSAGE_TYPE_GAMEMESSAGE || 
			t ==  USER_MESSAGE_TYPE_USERMESSAGE )
		{
			if ( event.isClicked && m_SoftRight.Contain(event.position) )
			{				
				SetOnlineSubState(ONLINE_MSG_LIST);				
				bHandled = true;
			}
		}
		else
		{		
			if(t ==  USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION)				
			{
				if ( event.isClicked && m_SoftRight.Contain(event.position) )
				{			
#ifdef USE_USER_NUMBER
					m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSenderNumber(i), 0);
#else
					m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSender(i), 0);
#endif //USE_USER_NUMBER
					SetOnlineSubState(ONLINE_PROCESS);
					bHandled = true;
				}
				else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
				{			
#ifdef USE_USER_NUMBER
					m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSenderNumber(i), 1);
#else
					m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSender(i), 1);
#endif //USE_USER_NUMBER
					SetOnlineSubState(ONLINE_PROCESS);

					bHandled = true;
				}
			}
			else if ( t == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION )
			{
				if ( event.isClicked && m_SoftRight.Contain(event.position) )
				{				
					SetOnlineSubState(ONLINE_MSG_LIST);
					bHandled = true;
				}
				else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
				{			
					//m_playInvitedGame = true;
					//m_isInvitor = false;
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
					char name[] = "Old Trafford";
					int value = 5;
					CMyRoomQueryCondition * query = XP_API_NEW CMyRoomQueryCondition();
					DefaultDataPacket * packet = XP_API_NEW DefaultDataPacket();
					//query->And( "pitch", ONLINE_QUERY_EQUAL, &name );
					query->And( "substitue", ONLINE_QUERY_LESS, &value );
					query->ToPacket( packet );
					//m_GLXPlayerMultiPlayer->mpSendListRoomByData( 5, 0, packet );
					//m_GLXPlayerMultiPlayer->mpSendListRoomByName( 5, 0, "playername" );
					m_GLXPlayerMultiPlayer->mpSendListRoom( 5, 0, NULL );
					//m_GLXPlayerMultiPlayer->mpSendListAllRoom( 5, 0 );
					XP_API_DEL( packet );
					XP_API_DEL( query );	
#else
					m_GLXPlayerMultiPlayer->mpSendListSession(MAX_LIST_SIZE,0);	
#endif
					SetOnlineSubState(ONLINE_MP_PROCESS);		

					//SetOnlineSubState(ONLINE_MESSAGE_INBOX);
					bHandled = true;
				}
			}
			else if ( t == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION )
			{
				if ( event.isClicked && m_SoftRight.Contain(event.position) )
				{				
					SetOnlineSubState(ONLINE_MSG_LIST);
					bHandled = true;
				}
				else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
				{	
					//To Do
					SetOnlineSubState(ONLINE_MSG_LIST);
					//m_xp_chat->Login( m_username, m_password );
					//XP_API_MEMSET( m_chat_send_data, 0, 256 );
					//XP_API_MEMSET( m_chat_data, 0, 1024 );
					//XP_API_MEMSET( m_chat_friend_name, 0, 64 );
					//XP_API_STRCPY( m_chat_friend_name, m_GLXPlayerMessage->getMsgSender(i) );
					//SetOnlineSubState(ONLINE_PROCESS);
					bHandled = true;
				}
			}

			if ( event.isClicked && m_SoftMid.Contain(event.position) )
			{						
				SetOnlineSubState(ONLINE_MSG_LIST);				
				bHandled = true;
			}
		}
	}
		
	return bHandled;
}

bool GameStateMultiplayer::HandleMsgInfo( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MP_MENU_ONLINE);		
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{			
		if(m_selectedIndex == 0 && m_gameMsgCount > 0)
		{
			m_currentMsgType = USER_MESSAGE_TYPE_GAMEMESSAGE;
			m_GLXPlayerMessage->SendGetMessageHeader(m_currentMsgType, USER_MESSAGE_READTYPE_NEW, 0, m_gameMsgCount);
			SetOnlineSubState(ONLINE_PROCESS);	
			m_selectedIndex = -1;			
		}
		else if(m_selectedIndex == 1 && m_userMsgCount > 0)
		{
			m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE;
			m_GLXPlayerMessage->SendGetMessageHeader(m_currentMsgType, USER_MESSAGE_READTYPE_NEW, 0, m_userMsgCount);
			SetOnlineSubState(ONLINE_PROCESS);	
			m_selectedIndex = -1;
		}	
		else if(m_selectedIndex == 2)
		{
			m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION;
			m_GLXPlayerMessage->SendGetMessageHeader(m_currentMsgType, USER_MESSAGE_READTYPE_NEW, 0, m_friendInvitedMsgCount);
			SetOnlineSubState(ONLINE_PROCESS);	
			m_selectedIndex = -1;
		}
		else if(m_selectedIndex == 3)
		{
			m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION;
			m_GLXPlayerMessage->SendGetMessageHeader(m_currentMsgType, USER_MESSAGE_READTYPE_NEW, 0, m_gameInvitedMsgCount);
			SetOnlineSubState(ONLINE_PROCESS);	
			m_selectedIndex = -1;
		}
		else if(m_selectedIndex == 4)
		{
			m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION;
			m_GLXPlayerMessage->SendGetMessageHeader(m_currentMsgType, USER_MESSAGE_READTYPE_NEW, 0, m_chatInvitedMsgCount);
			SetOnlineSubState(ONLINE_PROCESS);	
			m_selectedIndex = -1;
		}			
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{	
		if (m_selectedIndex < 0)
		{
			m_selectedIndex = 4;
		}
		else
		{
			m_selectedIndex--;
		}
		bHandled = true;
					
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{			
		if (m_selectedIndex > 4)
		{
			m_selectedIndex = -1;
		}
		else
		{
			m_selectedIndex++;
		}
		bHandled = true;
				
	}
	return bHandled;
}


bool GameStateMultiplayer::HandleEventSession( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if (m_GLXPlayerMultiPlayer->mpIsMaster())
	{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		int count = 0;
		COnlineGameRoom * room = m_GLXPlayerMultiPlayer->mpGetCurrentGameRoom();
		if(room != NULL)
		{
			count = room->GetPlayerCount();
			m_numberPlayer = count;
		}
#else
		int count = m_GLXPlayerMultiPlayer->getNumberOfItems();
		m_numberPlayer = count;
#endif
		if ( event.isClicked && m_SoftRight.Contain(event.position) )
		{
			SetOnlineSubState(ONLINE_MP_PROCESS);	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			m_GLXPlayerMultiPlayer->mpSendLeaveRoom();
#else
			m_GLXPlayerMultiPlayer->mpSendLeaveSession();
#endif
			bHandled = true;
		}	
		else if ( event.isClicked && m_SoftMid.Contain(event.position) )
		{
			if(count > 1 && m_selectedIndex >= 0)
			{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
				if ( m_selectedIndex < count-1 )
				{
					COnlinePlayer * player = room->GetPlayer( m_selectedIndex + 1 );
					m_GLXPlayerMultiPlayer->mpSendKickOutPlayer( player->GetName() );
				}
#else
				SetOnlineSubState( ONLINE_MP_PROCESS );
				m_GLXPlayerMultiPlayer->mpSendKickOutPlayer(m_GLXPlayerMultiPlayer->getNameList()[m_selectedIndex]);
#endif
				m_selectedIndex = -1;
			}			
			bHandled = true;
		}
		else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
		{
			if(count > 1)
			{
				SetOnlineSubState(ONLINE_MP_PROCESS);
				m_GLXPlayerMultiPlayer->mpSendStartGame();
				m_selectedIndex = -1;
			}
			bHandled = true;
		}
		else if( event.isClicked && m_UpKey.Contain(event.position) )
		{			
			
			if (m_selectedIndex < 0)
			{
				m_selectedIndex = count - 1;
			}
			else
			{
				m_selectedIndex--;
			}
			bHandled = true;
						
		}
		else if( event.isClicked && m_DownKey.Contain(event.position) )
		{			
			if (m_selectedIndex > count - 2)
			{
				m_selectedIndex = -1;
			}
			else
			{
				m_selectedIndex++;
			}
			bHandled = true;
					
		}
		else if ( event.isClicked && m_SoftAdd1.Contain(event.position) )
		{
			m_GLXPlayerUserFriend->sendGetUserFriends();
			SetOnlineSubState(ONLINE_PROCESS);
			bHandled = true;
		}
		else if ( event.isClicked && m_SoftAdd2.Contain(event.position) )
		{
			char tmpHeader[128] = {0};
			char tmpContent[128] = {0};
			
			if ( m_selectedIndex < 0 )
			{
				bHandled = true;
			}
			else
			{
				char* receiver = NULL;

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
				if(room != NULL)
				{
					COnlinePlayer * player = room->GetPlayer( m_selectedIndex );
					receiver = XP_API_STRNEW(player->GetName());
				}
#else
				receiver = XP_API_STRNEW(m_GLXPlayerMultiPlayer->getNameList()[m_selectedIndex]);
#endif

				if ( XP_API_STRCMP( receiver, m_username ) == 0 )
				{
					bHandled = false;
				}
				else
				{
					XP_API_SPRINTF(tmpHeader, "Friend invitation from %s", m_username);
					XP_API_SPRINTF(tmpContent, "%s want to add you as a friend. You can accept or deny this invitation.", m_username );
				#ifdef USE_USER_NUMBER
					m_GLXPlayerUserFriend->sendAddUserFriend(receiver, NULL, false);
				#else
					m_GLXPlayerUserFriend->sendAddUserFriend(receiver);
				#endif				
					SetOnlineSubState(ONLINE_PROCESS);
					bHandled = true;
				}
				
				XP_API_DEL(receiver);
				
			}
		}
	}
	else
	{
//		if(!m_GLXPlayerMultiPlayer->mpIsInSession())
//		{			
//			SetOnlineSubState(ONLINE_MP_MENU_ONLINE);			
//			bHandled = true;
//		}	
//		else
//		{
//			if(m_GLXPlayerMultiPlayer->mpIsInGame())
//			{
//				gameX = 0;
//				gameY = 0;
//				
//				oppX = 0;
//				oppY = 0;
//
//#if(MULTIPLAYER_TYPE == MULTIPLAYER_BY_SERVER)
//				if(m_currentSessionIndex > -1)
//				{	
//					m_numberPlayer = m_GLXPlayerMultiPlayer->getNumberOfPlayers(m_currentSessionIndex);
//					m_playerName = new char* [m_numberPlayer];
//					m_playerScore = new int [m_numberPlayer];
//					for(int i = 0; i < m_numberPlayer; i++)
//					{
//						m_playerName[i] = XP_API_STRNEW(m_GLXPlayerMultiPlayer->getNameList()[i]);
//						m_playerScore[i] = 0;
//					}
//				}
//#endif
//				SetOnlineSubState(ONLINE_MP_IN_GAME);				
//			}
//		}
		
		if ( event.isClicked && m_SoftLeft.Contain(event.position) )
		{
			SetOnlineSubState(ONLINE_MP_PROCESS);	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			m_GLXPlayerMultiPlayer->mpSendLeaveRoom();
#else
			m_GLXPlayerMultiPlayer->mpSendLeaveSession();
#endif
			bHandled = true;
		}				
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventSessionList( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	int count = 0;
	COnlineGameRoomList * list = m_GLXPlayerMultiPlayer->mpGetGameRoomList();
	if(list != NULL)
		count = list->Count();
#else
	int count = m_GLXPlayerMultiPlayer->getNumberOfItems();
#endif
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		if(m_selectedIndex >= 0)
		{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			m_GLXPlayerMultiPlayer->mpSetConnectState(GLXPlayerMPBase::CONNECT_STATE_START);

			char data_play[] = "Chelsea";

			DataPacket * packet_play = XP_API_NEW DefaultDataPacket();
			packet_play->addByteLenString( data_play, XP_API_STRLEN( data_play ) );
			packet_play->packMessage();

			COnlineGameRoom * room = m_GLXPlayerMultiPlayer->mpGetGameRoomList()->Get( m_selectedIndex );
			m_GLXPlayerMultiPlayer->mpSendJoinRoom( room->GetName(), packet_play );
			XP_API_DEL( packet_play );
#else
			m_currentSessionIndex = m_selectedIndex;
			m_GLXPlayerMultiPlayer->mpSendJoinSession(m_GLXPlayerMultiPlayer->getNameList()[m_selectedIndex]);
#endif
			SetOnlineSubState( ONLINE_MP_PROCESS );
		}
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_MP_MENU_ONLINE );
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{
		
		if (m_selectedIndex < 0)
		{
			m_selectedIndex = count - 1;
		}
		else
		{
			m_selectedIndex--;
		}
		bHandled = true;
					
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{		
		if (m_selectedIndex > count - 2)
		{
			m_selectedIndex = -1;
		}
		else
		{
			m_selectedIndex++;
		}
		bHandled = true;
				
	}

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->mpHandlePing();
#endif

	return bHandled;
}

bool GameStateMultiplayer::HandleEventInGame( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		m_GLXPlayerMultiPlayer->mpSendFinishGame();
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		SetOnlineSubState(ONLINE_MP_END_GAME);
#else
		SetOnlineSubState(ONLINE_MP_PROCESS);
#endif
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		m_GLXPlayerMultiPlayer->mpSendLeaveGame();
		if(m_GLXPlayerMultiPlayer->mpIsMaster())
		{
			m_GLXPlayerMultiPlayer->mpClearRoomData();
			m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
		}
		SetOnlineSubState( ONLINE_MP_PROCESS );
#else
		m_GLXPlayerMultiPlayer->mpSendLeaveSession();
		SetOnlineSubState( ONLINE_MP_PROCESS );
		//SetOnlineSubState( ONLINE_MP_MENU_ONLINE );
#endif
		
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		m_gamedata--;
#else
		gameX--;
		gameY--;
		
		if (gameX < 0)
		{
			gameX = 250;
		}
		if (gameY < 0)
		{
			gameY = 250;
		}
#endif
		bHandled = true;					
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		m_gamedata++;
#else
		gameX++;
		gameY++;
		
		if (gameX > 250)
		{
			gameX = 0;
		}
		if (gameY > 250)
		{
			gameY = 0;
		}
#endif
		bHandled = true;				
	}
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_BY_SERVER)
	if (oldX != gameX || oldY != gameY)
	{
		DataPacket* outgoing = m_GLXPlayerMultiPlayer->mpPrepareGameData();

		outgoing->addInt(gameX);
		outgoing->addInt(gameY);
		
		m_GLXPlayerMultiPlayer->mpSendGameData(outgoing);

		oldX = gameX;
		oldY = gameY;
	}
#endif
	return bHandled;
}

bool GameStateMultiplayer::HandleEventMPEndGame( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		if(m_selectedIndex == m_numberPlayer)
		{
			//TODO: rematch
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			if(m_GLXPlayerMultiPlayer->mpIsMaster())
			{
				m_GLXPlayerMultiPlayer->mpStartGame();
				OnMPStartGameSuccess();
			}
#else
#endif
		}
		else if(m_selectedIndex > -1)
		{
			//TODO: evaluate
			m_currentUserIndex = m_selectedIndex;
			SetOnlineSubState(ONLINE_MP_EVALUATE_PLAYER);
		}
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		m_GLXPlayerMultiPlayer->mpSendLeaveGame();
		m_GLXPlayerMultiPlayer->mpClearRoomData();
		m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
		SetOnlineSubState(ONLINE_PROCESS);
#else
		SetOnlineSubState( ONLINE_MP_MENU_ONLINE );		
#endif
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{
		m_selectedIndex --;
		if(m_selectedIndex < -1)					
		{
			m_selectedIndex = m_numberPlayer;
		}
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > m_numberPlayer)					
		{
			m_selectedIndex = -1;
		}	
	}
	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventMPSessionBuddy( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_MP_IN_SESSION;
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if((m_selectedIndex > -1) && (m_selectedIndex < m_GLXPlayerUserFriend->GetFriendsCount()))
		{
			m_currentFriendIndex = m_selectedIndex;
			XP_API_MEMSET( m_invite_play_username, 0, 64 );
			XP_API_STRCPY( m_invite_play_username, m_GLXPlayerUserFriend->GetName(m_currentFriendIndex) );
			
			XP_API_MEMSET( m_invite_play_usernumber, 0, 64 );
			XP_API_STRCPY( m_invite_play_usernumber, m_GLXPlayerUserFriend->GetUserNumber(m_currentFriendIndex) );
			SetOnlineSubState(ONLINE_MP_SESSION_BUDDY_PLAY);
		}		 
		m_selectedIndex = -1;
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		if ( m_GLXPlayerUserFriend->GetFriendsCount() > 0 )
		{
			m_selectedIndex --;		
			if(m_selectedIndex < 0)
			{			
				m_selectedIndex = m_GLXPlayerUserFriend->GetFriendsCount() - 1;			
			}			
			bHandled = true;
		}
	 }	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		if ( m_GLXPlayerUserFriend->GetFriendsCount() > 0 )
		{
			m_selectedIndex ++;
			if(m_selectedIndex > m_GLXPlayerUserFriend->GetFriendsCount() - 1)
			{	
				m_selectedIndex = 0;			
			}
			bHandled = true;
		}
	}	
	return bHandled;
}

bool GameStateMultiplayer::HandleEventEvaluatePlayer( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		if(m_selectedIndex == 0)
		{
			m_GLXPlayerUser->sendPointUserReputation(m_playerName[m_currentUserIndex], 5);
			SetOnlineSubState(ONLINE_PROCESS);
		}
		else if(m_selectedIndex == 1)
		{
			m_GLXPlayerUser->sendPointUserReputation(m_playerName[m_currentUserIndex], 1);
			SetOnlineSubState(ONLINE_PROCESS);
		}
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_MP_END_GAME );		
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{
		m_selectedIndex --;
		if(m_selectedIndex < -1)					
		{
			m_selectedIndex = 1;
		}
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex > 1)					
		{
			m_selectedIndex = -1;
		}	
	}
	
	return bHandled;
}

//GLXPlayerMultiPlayerObserver

void GameStateMultiplayer::OnMPNetworkError(int errorCode)
{
	SetOnlineSubState(ONLINE_MP_ERROR);
	m_errorCode = errorCode;
	XP_DEBUG_OUT( "[GameStateMultiplayer] OnMPNetworkError: %d\n", m_errorCode);
}

void GameStateMultiplayer::OnMPFunctionError( char* errMsg, int error_code )
{
	SetOnlineSubState(ONLINE_MP_ERROR);
	m_errorCode = error_code;
	m_errMsg = XP_API_STRNEW(errMsg);
	XP_DEBUG_OUT( "[GameStateMultiplayer] OnMPFunctionError: %d\n", m_errorCode);
}

void GameStateMultiplayer::OnMPuPnPTestSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
	SetOnlineSubState(ONLINE_MP_PROCESS);
#endif
}
void GameStateMultiplayer::OnMPEstablishConnectionSuccess()
{	
	char* uid = m_GLXPlayerLogin->GetUID();
	m_GLXPlayerMultiPlayer->mpSendLogin(uid);
	SetOnlineSubState(ONLINE_MP_PROCESS);
}

void GameStateMultiplayer::OnMPLoginSuccess(DataPacket* currentPacket)
{	
	m_GLXPlayerMessage->SendGetMessageCount();			
	SetOnlineSubState(ONLINE_PROCESS);				
}

void GameStateMultiplayer::OnMPCreateSessionSuccess(DataPacket* currentPacket)
{
	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateMultiplayer::OnMPListSessionSuccess(DataPacket* currentPacket)
{
	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
}

void GameStateMultiplayer::OnMPJoinSessionSuccess(DataPacket* currentPacket)
{
	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateMultiplayer::OnMPLeaveSessionSuccess()
{
	SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
}

void GameStateMultiplayer::OnMPLeaveGameSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
    XP_DEBUG_OUT("====mpSendEstablishConnection()====");
	SetOnlineSubState(ONLINE_MP_PROCESS);
#else
	SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
#endif

}

void GameStateMultiplayer::OnMPKickOutPlayerSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
	SetOnlineSubState(ONLINE_MP_PROCESS);
#else
	SetOnlineSubState(ONLINE_MP_IN_SESSION);
#endif
}

void GameStateMultiplayer::OnMPStartGameSuccess(DataPacket* currentPacket)
{
#if (MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	XP_API_DEL_ARRAY( m_playerName , m_numberPlayer);

	int count = 0;
	COnlineGameRoom * room = m_GLXPlayerMultiPlayer->mpGetCurrentGameRoom();
	if(room != NULL)
	{
		count = room->GetPlayerCount();
		m_numberPlayer = count;
	}
#else
	int count = m_GLXPlayerMultiPlayer->getNumberOfItems();
	m_numberPlayer = count;
#endif

	m_playerName = new char* [m_numberPlayer];
	m_playerScore = new int [m_numberPlayer];

#if (MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_gamedata = 0;

	for(int i = 0; i < m_numberPlayer; i++)
	{
		COnlineGameRoom *room = NULL;
		COnlinePlayer *player = NULL;
		room = m_GLXPlayerMultiPlayer->mpGetCurrentGameRoom();
		if(room != NULL)
		{
			player = room->GetPlayer( i );
			if(player != NULL)
			{
				m_playerName[i] = XP_API_STRNEW(player->GetName());
			}
		}
		m_playerScore[i] = 0;
	}
#else
	gameX = 0;
	gameY = 0;
		
	oppX = 0;
	oppY = 0;
	for(int i = 0; i < m_numberPlayer; i++)
	{
		m_playerName[i] = XP_API_STRNEW(m_GLXPlayerMultiPlayer->getNameList()[i + 1]);
		m_playerScore[i] = 0;
	}
#endif
	SetOnlineSubState(ONLINE_MP_IN_GAME);
}

void GameStateMultiplayer::OnMPDisconnectSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	if(m_sub_state != ONLINE_MP_IN_GAME)
	{
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		SetOnlineSubState(ONLINE_MP_MENU);
#else
		m_GLXPlayerLogin->SendLogout();
	SetOnlineSubState(ONLINE_PROCESS);	
#endif
	}
#else //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
	SetOnlineSubState(ONLINE_MP_MENU);
#else
	m_GLXPlayerLogin->SendLogout();
	SetOnlineSubState(ONLINE_PROCESS);
#endif
#endif //MULTIPLAYER_TYPE == MULTIPLAYER_P2P
}

void GameStateMultiplayer::OnMPFinishGameSuccess()
{
	SetOnlineSubState(ONLINE_MP_END_GAME);
	if(m_isWin == true)
	{
		m_score = 99;
		for(int i = 0; i < m_numberPlayer; i++)
		{
			m_playerScore[i] = 0;
		}
	}
	else
	{
		m_score = 0;
		for(int i = 0; i < m_numberPlayer; i++)
		{
			m_playerScore[i] = 999;
		}
	}
}



void GameStateMultiplayer::OnNetworkError()
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_CONNECTION;
	XP_DEBUG_OUT( "[GameStateMultiplayer] OnUserNetworkError: %d\n", m_errorCode);
}

void GameStateMultiplayer::OnRequestTimeout( int func_id )
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_REQUEST_TIME_OUT;
	XP_DEBUG_OUT( "[GameStateMultiplayer] OnRequestTimeout: Function: %d - Error: %d\n", func_id, m_errorCode);
}

void GameStateMultiplayer::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
		case GL_XPLAYER_FUNCTION_LOGIN:	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_BY_SERVER)
			SaveProfile();
#endif
			m_GLXPlayerUser->SetUID( m_GLXPlayerLogin->GetUID() );			
			m_GLXPlayerLeaderboard->SetUID( m_GLXPlayerLogin->GetUID() );			
			m_GLXPlayerMessage->SetUID( m_GLXPlayerLogin->GetUID() );	
			m_GLXPlayerUserFriend->SetUID( m_GLXPlayerLogin->GetUID() );	

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
			m_GLXPlayerMultiPlayer->SetUsername(m_username);
			//m_GLXPlayerLogin->SendGetCountry();
			m_GLXPlayerUser->sendGetCountry(m_username);
#else
			m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
#endif
			SetOnlineSubState(ONLINE_MP_PROCESS);
			break;

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		case GL_XPLAYER_FUNCTION_GET_COUNTRY:
			SaveProfile();
			//m_GLXPlayerMultiPlayer->SetCountry(m_GLXPlayerLogin->GetCountry());
			m_GLXPlayerMultiPlayer->SetCountry(m_GLXPlayerUser->getCountry());
			m_GLXPlayerMultiPlayer->mpSendUPnPPortMapping();
			break;
#endif
		case GL_XPLAYER_FUNCTION_LOGOUT:
			SetOnlineSubState(ONLINE_MP_MENU);	
			break;
		case FUNCTION_GET_MESSAGE_COUNT:			
			m_gameMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_GAMEMESSAGE, USER_MESSAGE_READTYPE_NEW);
			m_userMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE, USER_MESSAGE_READTYPE_NEW);
			m_friendInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION, USER_MESSAGE_READTYPE_NEW);
			m_gameInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, USER_MESSAGE_READTYPE_NEW);
			m_chatInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, USER_MESSAGE_READTYPE_NEW);
			if(m_gameMsgCount > 0 || m_userMsgCount > 0 || m_friendInvitedMsgCount > 0 || m_gameInvitedMsgCount > 0 || m_chatInvitedMsgCount > 0)
			{				
				SetOnlineSubState(ONLINE_MSG_INFO);
			}
			else
			{
				SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
			}			
			break;
		case FUNCTION_GET_MESSAGE_HEADER:
			SetOnlineSubState(ONLINE_MSG_LIST);
			break;
		case FUNCTION_READ_MESSAGE:
			SetOnlineSubState(ONLINE_MSG_READ);
			break;
			
		case FUNCTION_REGISTER:
			//m_GLXPlayerLogin->SendLogin(m_username,m_password);
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
		m_GLXPlayerLogin->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login_Game);
#endif
			SetOnlineSubState(ONLINE_PROCESS);			
			break;
		case FUNCTION_UPLOAD_SCORE:
			if(m_isRankAround == 0)
			{				
				SetOnlineSubState(ONLINE_RANKING);
			}
			else if(m_isRankAround == 1)
			{
				m_GLXPlayerLeaderboard->sendRankGet(1);
				SetOnlineSubState(ONLINE_PROCESS);
			}
			else if(m_isRankAround == 2)
			{
				m_GLXPlayerLeaderboard->sendRankGetAroundPlayer(1);
				SetOnlineSubState(ONLINE_PROCESS);
			}						
			else if(m_isRankAround == 3)
			{
				m_GLXPlayerLeaderboard->sendRankGetFriends(1);
				SetOnlineSubState(ONLINE_PROCESS);
				//SetOnlineSubState(ONLINE_TOP_FRIEND_RANKING);
			}						
			break;
		case FUNCTION_GET_RANKINGS:			
			SetOnlineSubState(ONLINE_TOP_RANKING);			
			break;
		case FUNCTION_GET_RANKINGS_AROUND_PLAYER:			
			SetOnlineSubState(ONLINE_RANKING_AROUND);			
			break;
		case FUNCTION_GET_RANKINGS_FRIENDS:			
			SetOnlineSubState(ONLINE_TOP_FRIEND_RANKING);			
			break;
		case FUNCTION_CONFIRM_USER_FRIEND:				
			m_GLXPlayerMessage->SendGetMessageCount();
			SetOnlineSubState(ONLINE_PROCESS);				
			break;
		case FUNCTION_POINT_USER_REPUTATION:				
			SetOnlineSubState(ONLINE_MP_END_GAME);
			break;
		case FUNCTION_GET_USER_FRIENDS:
			SetOnlineSubState(ONLINE_MP_SESSION_BUDDY);
			break;
		case FUNCTION_SEND_MESSAGE:
			if( m_currentMsgType == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION )
			{
				SetOnlineSubState( ONLINE_MP_SESSION_BUDDY );
			}
			else if ( m_currentMsgType ==  USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION )
			{
				SetOnlineSubState( ONLINE_MP_IN_SESSION );
			}
			break;
		case FUNCTION_KEEP_ALIVE:
			break;
		default:	
			SetOnlineSubState(ONLINE_MP_MENU_ONLINE);		
			break;			
	}
}

/**
 * The wrong response or the operation is failed on server side.
 *
 */
void GameStateMultiplayer::OnRequestFailure( int func_id, int error_code )
{
	switch(func_id)
	{
		case GL_XPLAYER_FUNCTION_LOGIN:	
			if(error_code == ERROR_USERNAME_NOTEXISTS)
			{
				SetOnlineSubState(ONLINE_REGISTER); 
			}
			else
			{
				SetOnlineSubState(ONLINE_ERROR);
				m_errorCode = error_code;
				XP_DEBUG_OUT( "[GameStateMultiplayer] OnUserFunctionError: %d\n", error_code);
			}
		break;

		default:
			SetOnlineSubState(ONLINE_ERROR);
			m_errorCode = error_code;			
			XP_DEBUG_OUT( "[GameStateMultiplayer] OnUserFunctionError: %d\n", error_code);	
			break;
	}
}

void GameStateMultiplayer::DrawInviteToPlayGame( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->ClearScreen();
	handler->DrawString( x, y, "--- Invite to play game ---" );
	y += 2 * DEF::LINE_HEIGHT;
	
	char tmp[128] = {0};
	
	XP_API_SPRINTF( tmp, "Send multiplayer invitation to: %s?", m_invite_play_username );	
	handler->DrawString( x, y, tmp );	

	DrawCommandBar(m_handler,GameState::CB_GO_BACK);	
}

bool GameStateMultiplayer::HandleEventInviteToPlayGame( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		char message[256] = { 0 };

		XP_API_MEMSET( message, 0, 256 );
		XP_API_SPRINTF( message, "%s invite you to play game.", m_username );
		m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION;
	#ifdef USE_USER_NUMBER
		m_GLXPlayerMessage->SendOnlineMessage( m_invite_play_usernumber, m_currentMsgType, "Invite to play game", message, XP_API_STRLEN( message ) );
	#else
		m_GLXPlayerMessage->SendOnlineMessage( m_invite_play_username, m_currentMsgType, "Invite to play game", message, XP_API_STRLEN( message ) );
	#endif //USE_USER_NUMBER
		SetOnlineSubState(ONLINE_PROCESS);
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_MP_SESSION_BUDDY;
		bHandled = true;
	}
	return bHandled;
}