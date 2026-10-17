#include "GameStateOnline.h"
#include "MyRoomQueryCondition.h"
#include "memory.h"
#include "MyGraphics.h"
#include "Stream.h"


#ifdef OS_IPHONE
extern void GetSaveFilePath(char* filePath, const char* filename);
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

#define REFRESH_FRIEND_LIST_TIME_OUT 120000 // 2 minutes

GameStateOnline::GameStateOnline( GameStateHandler * handler ) : GameState( handler )
{
	m_onlineMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_onlineMenu->AddMenuItem( MENU_GL_USER_PROFILE, this );	
	m_onlineMenu->AddMenuItem( MENU_GL_USER_FRIEND, this );			
	m_onlineMenu->AddMenuItem( MENU_GL_MESSAGE, this );	
	m_onlineMenu->AddMenuItem( MENU_GL_CHAT, this );			
	m_onlineMenu->AddMenuItem( MENU_GL_NEW_GAME, this );
	m_onlineMenu->AddMenuItem( MENU_GL_UPLOAD_VIDEO, this );
	m_onlineMenu->AddMenuItem( MENU_GL_DOWNLOAD, this );
	
	m_InputBox = XP_API_NEW InputBox(155,205);
	m_selectedIndex = 0;
	
	m_errorCode = ERROR_NONE;

	m_state = DEF::GAME_STATE_ONLINE;

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPbyP2P(this);
#else
	m_GLXPlayerMultiPlayer = XP_API_NEW GLXPlayerMPbyServer(this);	
#endif
	m_gl_xplayer_user = XP_API_NEW GLXPlayerUser();	
	m_gl_xplayer_user->RegisterObserver( this );

	m_GLXPlayerDownload = XP_API_NEW GLXPlayerDownload();
	m_GLXPlayerDownload->RegisterObserver( this );
	
	m_pre_sub_state = ONLINE_NONE;
	m_next_sub_state = ONLINE_NONE;
	m_sub_state = ONLINE_NONE;
	SetOnlineSubState(ONLINE_LOGIN);	
	
	m_SoftMid2.left = DEF::BUTTON_MID_X;
	m_SoftMid2.top = DEF::BUTTON_MID_Y - DEF::BUTTON_MID_H - 5;
	m_SoftMid2.width = DEF::BUTTON_MID_W;
	m_SoftMid2.height = DEF::BUTTON_MID_H;

	m_SoftAdd1.left = DEF::BUTTON_RIGHT_X + 55;
	m_SoftAdd1.top = DEF::BUTTON_RIGHT_Y;
	m_SoftAdd1.width = DEF::BUTTON_RIGHT_W;
	m_SoftAdd1.height = DEF::BUTTON_RIGHT_H;

	m_SoftAdd2.left = DEF::BUTTON_RIGHT_X + 110;
	m_SoftAdd2.top = DEF::BUTTON_RIGHT_Y;
	m_SoftAdd2.width = DEF::BUTTON_RIGHT_W;
	m_SoftAdd2.height = DEF::BUTTON_RIGHT_H;

	// Init user information
	LoadProfile();
	
	m_oldPassword = NULL;
	
	m_nationData = XP_API_NEW char[32];
	XP_API_MEMSET( m_nationData, 0, 32 );
	XP_API_SPRINTF( m_nationData, "VietNam");
	
	m_email = XP_API_NEW char[32];
	XP_API_MEMSET( m_email, 0, 32 );
	XP_API_SPRINTF( m_email, "user@gameloft.com");		
	m_emailFlag = false;	
	
	m_avatar = NULL;
	
	m_frame = 0;	
	
	m_dataCenterMenu = NULL;
	
	m_dataGroup = NULL;
	m_dataValue = NULL;
	
	m_profileMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_NAME, this );	
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_PASS, this );			
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_EMAIL, this );			
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_EMAIL_FLAG, this );			
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_COUNTRY, this );			
	m_profileMenu->AddMenuItem( MENU_GL_USER_PF_AVATAR, this );
		
	
	m_friendMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_friendMenu->AddMenuItem( MENU_GL_USER_ADD_FRIEND, this );	
	m_friendMenu->AddMenuItem( MENU_GL_USER_LIST_FRIENDS, this );			
	
	m_currentFriendMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);		

	m_chatMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
	m_chatMenu->AddMenuItem( MENU_CHAT_WITH_FRIEND, this );
	m_chatMenu->AddMenuItem( MENU_CHAT_IN_CONFERENCE, this );

	m_messageMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y + 2*DEF::LINE_HEIGHT);
	m_messageMenu->AddMenuItem( MENU_MESSAGE_WRITE, this );
	m_messageMenu->AddMenuItem( MENU_MESSAGE_INBOX, this );
	
	m_msgReceiver = NULL;
	m_msgContent = NULL;
	m_msgTitle = NULL;
	m_msgCC = NULL;
	m_isUserNumber  = false;
	
	m_xp_login = XP_API_NEW GLXPlayerLogin();
	m_xp_login->RegisterObserver( this );
	
	m_xp_chat = XP_API_NEW GLXPlayerChat();
	m_xp_chat->RegisterChatObserver( this );
	m_xp_chat->RegisterChatRoomObserver( this );

	m_chat_room_list = XP_API_NEW GLXPlayerChatRoomList();
	m_chat_room = NULL;
	
	m_GLXPlayerMessage = XP_API_NEW GLXPlayerMessage();
	m_GLXPlayerMessage->RegisterObserver( this );

	m_GLXPlayerUserFriend = XP_API_NEW GLXPlayerUserFriend();	
	m_GLXPlayerUserFriend->RegisterObserver( this );

	m_GLXPlayerYoutube = XP_API_NEW GLXPlayerYoutube();	
	m_GLXPlayerYoutube->RegisterObserver( this );

	m_currentMsgType = USER_MESSAGE_TYPE_GAMEMESSAGE;
	m_gameMsgCount = 0;
	m_userMsgCount = 0;	
	m_friendInvitedMsgCount = 0;
	m_gameInvitedMsgCount = 0;	
	m_chatInvitedMsgCount = 0;
	
	m_isInbox = false;
	m_currentMsgPage = 0;
	m_msgInPageCount = 0;
	
	m_currentFriendIndex = 0;
	m_playInvitedGame = false;
	m_isInvitor = false;

	m_numberPlayer = 0;
	m_currentSessionIndex = -1;
	m_numberPlayer = 0;
	m_playerName = NULL;
	m_playerScore = NULL;
	m_isWin = true;

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
}

GameStateOnline::~GameStateOnline(void)
{
	XP_API_DEL( m_gl_xplayer_user );	
	XP_API_DEL( m_oldPassword );
	XP_API_DEL( m_nationData );
	XP_API_DEL( m_email );
	XP_API_DEL( m_avatar );
	XP_API_DEL( m_InputBox );
	XP_API_DEL( m_onlineMenu );		
	XP_API_DEL(m_dataCenterMenu);		
	XP_API_DEL(m_profileMenu);	
	XP_API_DEL(m_friendMenu);		
	XP_API_DEL(m_msgReceiver);	
	XP_API_DEL(m_msgContent);
	XP_API_DEL(m_msgTitle);
	XP_API_DEL(m_msgCC);
	XP_API_DEL( m_currentFriendMenu );
	XP_API_DEL( m_chatMenu );
	XP_API_DEL( m_messageMenu );	
	XP_API_DEL( m_chat_room_list );
	m_chat_room = NULL;
	XP_API_DEL( m_xp_login );
	XP_API_DEL( m_xp_chat );
	XP_API_DEL( m_GLXPlayerMessage );
	XP_API_DEL( m_GLXPlayerUserFriend );
	XP_API_DEL( m_GLXPlayerMultiPlayer );
	XP_API_DEL( m_GLXPlayerDownload );
	XP_API_DEL(m_GLXPlayerYoutube);
}

bool GameStateOnline::LoadProfile()
{
	XP_FILE_HANDLE pFile;

#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "rb" );	
#else
	pFile = XP_API_FILE_OPEN( PROFILE_FILE_NAME, "rb" );
#endif

	XP_API_MEMSET(m_username, 0,  sizeof(m_username));
	XP_API_MEMSET(m_password, 0,  sizeof(m_password));
		
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

bool GameStateOnline::SaveProfile()
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


bool GameStateOnline::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;
	switch(m_sub_state)
	{
		case ONLINE_LOGIN:
			bHandled = HandleEventLogin(event);
			break;
		case ONLINE_MSG_LIST:					
			bHandled = HandleMsgList( event );
			break;
		case ONLINE_MSG_INFO:		
			bHandled = HandleMsgInfo( event);
			break;		
		case ONLINE_MSG_READ:		
			bHandled = HandleViewMessage( event );
			break;
		case ONLINE_REGISTER:
			bHandled = HandleEventRegister(event);
			break;		
		case ONLINE_ERROR:		
			bHandled = HandleEventError( event );
			break;			
		case ONLINE_PROCESS:		
			bHandled = HandleEventProcess( event );
			break;			
		case ONLINE_MENU:		
			bHandled = HandleEventOnlineMenu( event );
			break;		
		case ONLINE_USER_GAME_LIST:		
		case ONLINE_USER_STATE:		
			bHandled = HandleEventUserGameList( event );
			break;		
		case ONLINE_USER_REPUTATION:		
			bHandled = HandleEventUserReputation( event );
			break;
		case ONLINE_USER_DATA_MENU:		
			bHandled = HandleEventDataMenu( event );
			break;		
		case ONLINE_USER_DATA_VIEW:		
			bHandled = HandleEventDataView( event );
			break;
		case ONLINE_USER_DATA_UPLOAD:		
			bHandled = HandleEventDataUpload( event );
			break;
		case ONLINE_PROFILE_MENU:		
			bHandled = HandleEventOnlineProfileMenu( event );
			break;		
		case ONLINE_USER_PROFILE:		
			bHandled = HandleEventOnlineUserProfile( event );
			break;		
		case ONLINE_FRIEND_MENU:		
			bHandled = HandleEventOnlineFriendMenu( event );
			break;		
		case ONLINE_ADD_FRIEND:		
			bHandled = HandleEventOnlineAddFriend( event );
			break;		
		case ONLINE_LIST_FRIENDS:		
			bHandled = HandleEventOnlineListFriend( event );
			break;		
		case ONLINE_CURRENT_FRIEND_MENU:		
			bHandled = HandleEventOnlineCurrentFriendMenu( event );
			break;		
		case ONLINE_SEND_MSG_TO_FRIEND:		
		case ONLINE_MESSAGE_WRITE:		
			bHandled = HandleEventOnlineSendMsg( event );
			break;		
		case ONLINE_COMPARE_FRIEND_GAME:		
			bHandled = HandleEventOnlineCompareGameWithFriend( event );
			break;		
		case ONLINE_CHAT_MENU:
			bHandled = HandleEventOnlineChatMenu( event );
			break;
		case ONLINE_CHAT_WITH_FRIEND:
			bHandled = HandleEventOnlineChatWithFriend( event );
			break;
		case ONLINE_CHAT_CONFERENCE_LIST:
			bHandled = HandleEventOnlineChatConferenceList( event );
			break;
		case ONLINE_CHAT_CONFERENCE:
			bHandled = HandleEventOnlineChatConference( event );
			break;
		case ONLINE_CHAT_ADD_CONFERENCE:
			bHandled = HandleEventOnlineChatAddConference( event );
			break;
		case ONLINE_INPUT:
			bHandled = HandleEventOnlineInput( event );
			break;
		case ONLINE_CHAT_CONFERENCE_INVITE:
			bHandled = HandleEventOnlineChatConferenceInvitation( event );
			break;
		case ONLINE_CHAT_SCREEN:
			bHandled = HandleEventOnlineChatScreen( event );
			break;
		case ONLINE_INVITE_CHAT:
			bHandled = HandleEventInviteChat( event );
			break;
		case ONLINE_CHAT_BE_INVITED:
			bHandled = HandleEventOnlineChatBeInvited( event );
			break;
		case ONLINE_MESSAGE_MENU:
			bHandled = HandleEventOnlineMessageMenu( event );
			break;	
		case ONLINE_MESSAGE_INBOX:
			HandleEventOnlineMessageInbox( event );
			break;
		case ONLINE_INVITE_PLAY:
			HandleEventOnlineInviteToPlayGame( event );
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
		default:
			break;
	}
	return bHandled;
}

void GameStateOnline::SwitchState()
{
	SetGameState(m_state);
}

void GameStateOnline::DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool drauUpDown )
{
	if(drauUpDown == true)
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
	case CB_REMOVE_BACK:
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		handler->DrawButton(m_SoftMid);
		break;
	case CB_ACCEPT_IGNORE:
		handler->DrawButton(m_SoftLeft);
		handler->DrawButton(m_SoftRight);
		break;
	default:
		break;
	}

	GameState::DrawCommandBar(handler, state, drauUpDown);
}

void GameStateOnline::Paint()
{
	switch(m_sub_state)
	{
		case ONLINE_PROCESS:		
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_LOGIN:		
			m_handler->ClearScreen();
			DrawLogin(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_MSG_INFO:		
			m_handler->ClearScreen();
			DrawMsgInfo(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_MSG_LIST:		
			m_handler->ClearScreen();
			DrawMsgList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;			
		case ONLINE_MSG_READ:				
			DrawMessage(m_handler);		
			break;
		case ONLINE_MENU:
			m_handler->ClearScreen();
			m_onlineMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;						
		case ONLINE_ERROR:
			m_handler->ClearScreen();
			DrawError(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;			
		case ONLINE_REGISTER:		
			m_handler->ClearScreen();
			DrawRegister(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;			
		case ONLINE_USER_GAME_LIST:		
			m_handler->ClearScreen();
			DrawUserGameList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;			
		case ONLINE_USER_STATE:		
			m_handler->ClearScreen();
			DrawUserState(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;	
		case ONLINE_USER_REPUTATION:		
			m_handler->ClearScreen();
			DrawUserReputation(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			break;			
		case ONLINE_USER_DATA_MENU:		
			m_handler->ClearScreen();
			m_dataCenterMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;	
		case ONLINE_USER_DATA_VIEW:		
			m_handler->ClearScreen();
			DrawUserDataView(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK, false);
			break;	
		case ONLINE_USER_DATA_UPLOAD:		
			m_handler->ClearScreen();
			DrawUserDataUpload(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_PROFILE_MENU:
			m_handler->ClearScreen();
			m_profileMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_USER_PROFILE:
			m_handler->ClearScreen();
			DrawUserProfile(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_FRIEND_MENU:
			m_handler->ClearScreen();
			m_friendMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_ADD_FRIEND:
			m_handler->ClearScreen();
			DrawAddFriend(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_LIST_FRIENDS:
			m_handler->ClearScreen();
			DrawFriendList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_REMOVE_BACK);
			break;		
		case ONLINE_CURRENT_FRIEND_MENU:
			m_handler->ClearScreen();
			m_currentFriendMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_SEND_MSG_TO_FRIEND:
		case ONLINE_MESSAGE_WRITE:
			m_handler->ClearScreen();
			DrawSendMsg(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;		
		case ONLINE_COMPARE_FRIEND_GAME:
			m_handler->ClearScreen();
			DrawCompareGameWithFriend(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK);
			break;		
		case ONLINE_CHAT_MENU:
			m_handler->ClearScreen();
			m_chatMenu->Repaint(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_CHAT_WITH_FRIEND:
			m_handler->ClearScreen();
			DrawFriendList(m_handler);
			DrawCommandBar(m_handler,GameState::CB_GO_BACK);
			break;
		case ONLINE_CHAT_CONFERENCE_LIST:
			DrawChatRoomMenu(m_handler);
			break;
		case ONLINE_CHAT_ADD_CONFERENCE:
			DrawCreateConference(m_handler);
			break;
		case ONLINE_INPUT:
			DrawInput(m_handler);
			break;
		case ONLINE_CHAT_CONFERENCE:
			DrawConference(m_handler);
			break;
		case ONLINE_CHAT_CONFERENCE_INVITE:
			DrawConferenceInvitation(m_handler);
			break;
		case ONLINE_CHAT_SCREEN:
			DrawChatScreen(m_handler);
			break;
		case ONLINE_INVITE_CHAT:
			DrawInviteChat(m_handler);
			break;
		case ONLINE_CHAT_BE_INVITED:
			DrawConferenceBeInvited(m_handler);
			break;
		case ONLINE_MESSAGE_MENU:
			DrawMessageMenu(m_handler);
			break;		
		case ONLINE_MESSAGE_INBOX:
			DrawMessageInbox(m_handler);
			break;
		case ONLINE_INVITE_PLAY:
			DrawInviteToPlayGame(m_handler);
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
		case ONLINE_MP_PROCESS:
			m_handler->ClearScreen();
			DrawWaiting(m_handler);
			DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
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
		default:
			break;
	}

	if ( m_sub_state != ONLINE_REGISTER && m_sub_state != ONLINE_LOGIN &&
		 m_sub_state != ONLINE_PROCESS && m_sub_state != ONLINE_ERROR &&
		 XP_API_STRLEN( m_username ) > 0 )
	{
		char line[128] = { 0 };
		XP_API_MEMSET( line, 0, 128 );
		XP_API_SPRINTF( line, "@%s@", m_username );
		m_handler->DrawString( 400, DEF::START_POS_Y, line );
	}
}

void GameStateOnline::DrawChatRoomMenu( GameStateHandler * handler )
{
	int x = 20;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };

	m_handler->ClearScreen();
	m_handler->DrawString(x,y,"## All the existed conference ##");

	if ( m_chat_room_list->Count() > 0 )
	{
		m_handler->DrawString(2,y+(m_conference_index+1)*2*DEF::LINE_HEIGHT,">");
		y += 2*DEF::LINE_HEIGHT;
	}

	for ( int i=0; i<m_chat_room_list->Count(); i++ )
	{
		GLXPlayerChatRoom * room = m_chat_room_list->Get( i );

		XP_API_MEMSET( line, 0, 128 );
		XP_API_SPRINTF( line, "%d name <%s>", i+1, room->GetName() );
		m_handler->DrawString(x,y,line);
		y += DEF::LINE_HEIGHT;

		XP_API_MEMSET( line, 0, 128 );
		XP_API_SPRINTF( line, "jid  <%s>", room->GetJid() );
		m_handler->DrawString(x,y,line);
		y += DEF::LINE_HEIGHT;
	}

	m_handler->DrawButton(m_UpKey);
	m_handler->DrawButton(m_DownKey);
	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawButton(m_SoftMid2);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_UpKey.left,m_UpKey.top,"Up");
	m_handler->DrawString(m_DownKey.left,m_DownKey.top,"Down");
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Create");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Join");
	m_handler->DrawString(m_SoftMid2.left,m_SoftMid2.top,"Refresh");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

void GameStateOnline::DrawLogin( GameStateHandler * handler )
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

void GameStateOnline::DrawRegister( GameStateHandler * handler )
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

void GameStateOnline::DrawWaiting( GameStateHandler * handler )
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

void GameStateOnline::DrawMsgList( GameStateHandler * handler)
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

void GameStateOnline::DrawMsgInfo( GameStateHandler * handler )
{
		int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- You have some message ---");
	y += 2*DEF::LINE_HEIGHT;	
	
	char line[128] = { 0 };
	
	XP_API_MEMSET(line, 0, 128);
	XP_API_SPRINTF( line, "Game Message : %d.",m_gameMsgCount);
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

void GameStateOnline::DrawMessage( GameStateHandler * handler )
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
			bool rt = m_GLXPlayerMessage->getMsgReadState(i); 
			if(rt == false)
			{
				DrawCommandBar(m_handler,GameState::CB_ACCEPT_IGNORE, false);
			}
			else
			{
				DrawCommandBar(m_handler,GameState::CB_NULL_BACK, false);
			}
		}
	}				
}

void GameStateOnline::DrawAddFriend( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Add Friend ---");
	y+=2*DEF::LINE_HEIGHT;
	
	handler->DrawString(x,y,"Press 'Go' to send invitation.");
	y+=2*DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20, y, ">");
	}
	char msg[256] = {0};
	XP_API_SPRINTF(msg, "Friends Name: %s", m_msgReceiver);
	handler->DrawString(x,y,msg);
	y+=2*DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20, y, ">");
	}
	XP_API_MEMSET(msg, 0, 256);
	XP_API_SPRINTF(msg, "Invite Message: %s", m_msgContent);
	handler->DrawString(x,y,msg);
#ifdef USE_USER_NUMBER
	y+=2*DEF::LINE_HEIGHT;
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20, y, ">");
	}
	XP_API_MEMSET(msg, 0, 256);
	XP_API_SPRINTF(msg, "Is User Number (1 Or 0): %d", m_isUserNumber);
	handler->DrawString(x,y,msg);
#endif // USE_USER_NUMBER
	m_InputBox->showTextBox(handler);
}

void GameStateOnline::DrawFriendList( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->DrawString(x,y,"--- Friends List ---");
	y+=2*DEF::LINE_HEIGHT;

	handler->DrawString(x,y,"No - Name - Country - Reputation - State");
	y+=DEF::LINE_HEIGHT;

	char line[256] = { 0 };
	
	for(int i=0; i < m_GLXPlayerUserFriend->GetFriendsCount(); i++) 
	{
		if(m_selectedIndex == i)
		{
			handler->DrawString(x - 20 , y, ">");
		}

		int state = m_GLXPlayerUserFriend->GetState(i);
		char * name = m_GLXPlayerUserFriend->GetName(i);
		char * country = m_GLXPlayerUserFriend->GetCountry(i);
		char * game = NULL;
		double reputation = m_GLXPlayerUserFriend->GetReputation(i);

		if ( state == XPLAYER_USER_STATE_ONLINE )
		{
			XP_API_MEMSET(line, 0, 256);
			XP_API_SPRINTF( line, "%d %s %s %f <online>", i+1 , name, country, reputation );
		}
		else if ( state == XPLAYER_USER_STATE_PLAYING )
		{
			XP_API_MEMSET(line, 0, 256);
			game = m_GLXPlayerUserFriend->GetCurrentGame(i);
			XP_API_SPRINTF( line, "%d %s %s %f <playing %s>", i+1 , name, country, reputation, game );
		}
		else
		{
			XP_API_MEMSET(line, 0, 256);
			XP_API_SPRINTF( line, "%d %s %s %f <offline>", i+1 , name, country, reputation );
		}		
		handler->DrawString(x,y,line);

		y += DEF::LINE_HEIGHT;
	}
}

void GameStateOnline::DrawCompareGameWithFriend( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Compare Game ---");
	y+=2*DEF::LINE_HEIGHT;

	char msg[128] = {0};

	XP_API_MEMSET(msg,0,128);
	XP_API_SPRINTF(msg, "User name: %s", m_gl_xplayer_user->getOtherUserName());
	handler->DrawString(x,y,msg);
	y+=2*DEF::LINE_HEIGHT;	

	XP_API_MEMSET(msg,0,128);
	XP_API_SPRINTF(msg, "Trophy: %d", m_gl_xplayer_user->getUserTrophy());
	handler->DrawString(x,y,msg);
	y+=2*DEF::LINE_HEIGHT;
	
}

void GameStateOnline::DrawSendMsg( GameStateHandler * handler)
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Send Message ---");	
	y+=2*DEF::LINE_HEIGHT;	
	
	char msg[128] = {0};

	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20, y, ">");
	}
	XP_API_MEMSET(msg,0,128);
	XP_API_SPRINTF(msg, "To: %s", m_msgReceiver);
	handler->DrawString(x,y,msg);	
	y+=DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20, y, ">");
	}
	
	XP_API_MEMSET(msg,0,128);
	XP_API_SPRINTF(msg, "CC: %s", m_msgCC);
	handler->DrawString(x,y,msg);
	y+=DEF::LINE_HEIGHT;
	handler->DrawString(x,y,"(Add more User, split by ',')");		
	y+=2*DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 2)
	{
		handler->DrawString(x - 20, y, ">");
	}
	XP_API_MEMSET(msg, 0, 128);
	XP_API_SPRINTF(msg, "Use User Numer(0/1): %d", m_isUserNumber);
	handler->DrawString(x,y,msg);	
	y+=DEF::LINE_HEIGHT;
	
	if(m_selectedIndex == 3)
	{
		handler->DrawString(x - 20, y, ">");
	}	
	XP_API_MEMSET(msg,0,128);
	XP_API_SPRINTF(msg, "Title: %s", m_msgTitle);
	handler->DrawString(x,y,msg);
	y+=DEF::LINE_HEIGHT;	

	if(m_selectedIndex == 4)
	{
		handler->DrawString(x - 20, y, ">");
	}
	XP_API_MEMSET(msg, 0, 128);
	XP_API_SPRINTF(msg, "Content: %s", m_msgContent);
	handler->DrawString(x,y,msg);
	
	m_InputBox->showTextBox(handler);
}

void GameStateOnline::DrawUserProfile( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	
	char msg[128] = {0};
	
	switch(m_profileMenu->getSelectedIndex())
	{
		case 0:
			XP_API_SPRINTF(msg, "--- Change User Name ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current Name: %s", m_username);
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "New Name: %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);						
			break;
		case 1:
			XP_API_SPRINTF(msg, "--- Change Password ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current Password: %s", m_password);
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "New Password: %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);
			break;
		case 2:		
			XP_API_SPRINTF(msg, "--- Change Email ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current Email: %s", m_email);
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "New Email: %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);
			break;
		case 3:			
			XP_API_SPRINTF(msg, "--- Change Email Flag ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current get email option: %d", m_emailFlag);
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128);
			XP_API_SPRINTF(msg, "Do you want to get email from Gameloft?");
			handler->DrawString(x,y,msg);
			y+= 2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Yes (1) / No (0): %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);
			break;
		case 4:
			XP_API_SPRINTF(msg, "--- Change Country ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current Country: %s", m_nationData);
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "New Country: %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);
			break;
		case 5:
			XP_API_SPRINTF(msg, "--- Change Avatar ---");
			handler->DrawString(x,y,msg);
			y+=2*DEF::LINE_HEIGHT;
			
			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "Current Avatar data: ");
			handler->DrawString(x,y,msg);
			y+=DEF::LINE_HEIGHT;

			if ( NULL != m_avatar )
			{
				handler->DrawString(x,y,m_avatar);
				y+=DEF::LINE_HEIGHT;
				XP_API_MEMSET( msg, 0, 128 );
				XP_API_SPRINTF(msg, "length: %d", XP_API_STRLEN(m_avatar));
				handler->DrawString(x,y,msg);
				y+=DEF::LINE_HEIGHT;
			}			

			XP_API_MEMSET( msg, 0, 128 );
			XP_API_SPRINTF(msg, "New Avatrar path: %s", m_InputBox->getInputBoxString());
			handler->DrawString(x,y,msg);
			break;
		
		default:
			break;
	}
	
	m_InputBox->showTextBox(handler);	
}

void GameStateOnline::DrawError( GameStateHandler * handler )
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

void GameStateOnline::DrawUserGameList( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- User Game List ---");
	y += 2*DEF::START_POS_Y;
	if(m_gl_xplayer_user->getNumberUserGameList() <= 0)
	{
		handler->DrawString(x,y,"These is no game ... ");
	}
	else
	{
		handler->DrawString(x, y, "Game");
		handler->DrawString(x + 80, y, "Game Name");
		handler->DrawString(x + 280, y, "Score");	
		y += DEF::START_POS_Y;
		handler->DrawString(x, y, "------------------------------------------------------------------");		
		y += DEF::START_POS_Y;	
		
		char tmp[32] = {0};			
		for(int i = 0; i < m_gl_xplayer_user->getNumberUserGameList(); i++)
		{
			handler->DrawString(x,y,m_gl_xplayer_user->getUserGamePID(i));
			handler->DrawString(x + 80 ,y,m_gl_xplayer_user->getUserGameName(i));
			XP_API_MEMSET(tmp,0,32);
			XP_API_SPRINTF(tmp, "%d", m_gl_xplayer_user->getUserGameTrophy(i));
			handler->DrawString(x + 280 ,y, tmp);				
			y +=DEF::START_POS_Y;
		}
	}
}

void GameStateOnline::DrawUserState( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- User State ---");
	y += 2*DEF::START_POS_Y;
	
	char tmp[32] = {0};
	if( m_gl_xplayer_user->getCurrentUserState() == true)
	{
		handler->DrawString(x,y,"User Status: Online");	
	}
	else
	{
		handler->DrawString(x,y,"User Status: Offline");	
	}			
	y += 2*DEF::START_POS_Y;
	
	if( m_gl_xplayer_user->getCurrentGamePID() != NULL)
	{
		handler->DrawString(x, y, "Game");
		handler->DrawString(x + 60, y, "Game Name");
		handler->DrawString(x + 220, y, "Score");	
		y += DEF::START_POS_Y;
		handler->DrawString(x, y, "----------------------------------------------------------");
		y += DEF::START_POS_Y;
		
		handler->DrawString(x,y,m_gl_xplayer_user->getCurrentGamePID());
		handler->DrawString(x + 60 ,y,m_gl_xplayer_user->getCurrentGameName());
		
		XP_API_MEMSET(tmp,0,32);
		XP_API_SPRINTF(tmp, "%d", m_gl_xplayer_user->getCurrentGameTrophy());
		handler->DrawString(x + 280 ,y, tmp);				
		y +=DEF::START_POS_Y;
	}	
}

void GameStateOnline::DrawUserReputation( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- User Reputation ---");
	y += 2*DEF::START_POS_Y;

	char tmp[128] = {0};
	if(m_gl_xplayer_user->getOtherUserName() != NULL)
	{
		XP_API_SPRINTF(tmp, "User Name: %s", m_gl_xplayer_user->getOtherUserName());
	}
	else
	{
		XP_API_SPRINTF(tmp, "User Name: %s", m_username);
	}	
	handler->DrawString(x,y,tmp);
	y += DEF::START_POS_Y;
	
	XP_API_MEMSET(tmp, 0 , 128);	
	XP_API_SPRINTF(tmp, "Reputation: %f", m_gl_xplayer_user->getUserReputation());
	handler->DrawString(x,y,tmp);
}

void GameStateOnline::DrawUserDataView( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Data Center ---");
	y += 2*DEF::START_POS_Y;
	
	handler->DrawString(x,y,"Please enter the tag below.");
	y += 2*DEF::START_POS_Y;
	
	char tmp[128] = {0};
	handler->DrawString(x - 20 , y, ">");
	XP_API_SPRINTF(tmp, "Tag Name: %s", m_dataGroup);
	handler->DrawString(x,y,tmp);
	y += DEF::START_POS_Y;
	
	XP_API_MEMSET(tmp,0, 128);
	XP_API_SPRINTF(tmp, "Data Value: %s", m_dataValue);
	handler->DrawString(x,y,tmp);
	y += DEF::START_POS_Y;
	
	m_InputBox->showTextBox(handler);
}


void GameStateOnline::DrawUserDataUpload( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	handler->DrawString(x,y,"--- Data Center ---");
	y += 2*DEF::START_POS_Y;
	
	handler->DrawString(x,y,"Please enter the tag and value.");
	y += 2*DEF::START_POS_Y;
	
	if(m_selectedIndex == 0)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	char tmp[128] = {0};
	XP_API_SPRINTF(tmp, "Data group: %s", m_dataGroup);
	handler->DrawString(x,y,tmp);
	y += DEF::START_POS_Y;
	
	if(m_selectedIndex == 1)
	{
		handler->DrawString(x - 20 , y, ">");	
	}
	XP_API_MEMSET(tmp, 0, 128);
	XP_API_SPRINTF(tmp, "Data Value: %s", m_dataValue);
	handler->DrawString(x,y,tmp);
	
	m_InputBox->showTextBox(handler);
}


void GameStateOnline::UpdateAnimation()
{
	m_xp_login->Update();
	m_gl_xplayer_user->Update();
	m_xp_chat->Update();
	m_GLXPlayerMessage->Update();	
	m_GLXPlayerUserFriend->Update();
	m_GLXPlayerDownload->Update();
	m_GLXPlayerYoutube->Update();

//	if(m_playInvitedGame)
//	{
//		if(m_sub_state == ONLINE_PROCESS || m_GLXPlayerMultiPlayer->mpIsConnected())
//		{
//			m_GLXPlayerMultiPlayer->Update();
//#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
//			if(m_GLXPlayerMultiPlayer->mpIsInGame())
//			{
//				UpdateGame();
//			}
//#endif
//		}
//	}

	if(m_playInvitedGame)
	{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		m_GLXPlayerMultiPlayer->Update();
		if(m_GLXPlayerMultiPlayer->mpIsInGame())
		{
			UpdateGame();
		}
#else
		if(m_sub_state == ONLINE_MP_PROCESS || m_GLXPlayerMultiPlayer->mpIsConnected())
		{
			if(m_xp_login->IsLoggedIn())
			{
				m_GLXPlayerMultiPlayer->Update();
			}
		}
#endif
	}

	m_frame++;
}

bool GameStateOnline::UpdateQuitAnimation()
{
	if(!m_xp_login->IsLoggedIn())
	{
		return true;
	}
	else
	{	
		m_xp_login->Update();
		return false;
	}
}

void GameStateOnline::BeginQuitAnimation()
{
	if(m_xp_login == NULL)
		return;
	m_xp_login->SendLogout();
}

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
void GameStateOnline::UpdateGame()
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

void GameStateOnline::SetOnlineSubState( OnlineSubState state )
{	
	if ( m_sub_state != ONLINE_NONE && m_sub_state != ONLINE_PROCESS)
	{
		m_pre_sub_state = m_sub_state;
	}

	m_sub_state = state;
}

void GameStateOnline::OnMenuItemClicked( char * menu_name )
{
	if ( XP_API_STRCMP( menu_name, MENU_GL_USER_PROFILE ) == 0 )
	{				
		SetOnlineSubState(ONLINE_PROFILE_MENU);		
	}	
	else if(
			(XP_API_STRCMP( menu_name, MENU_GL_USER_PF_NAME ) == 0) ||
			(XP_API_STRCMP( menu_name, MENU_GL_USER_PF_PASS ) == 0) ||
			(XP_API_STRCMP( menu_name, MENU_GL_USER_PF_EMAIL ) == 0) ||
			(XP_API_STRCMP( menu_name, MENU_GL_USER_PF_EMAIL_FLAG ) == 0) ||
			(XP_API_STRCMP( menu_name, MENU_GL_USER_PF_COUNTRY ) == 0)
		)
	{
		m_gl_xplayer_user->sendGetUserInfo();
		m_next_sub_state = ONLINE_USER_PROFILE;
		SetOnlineSubState(ONLINE_PROCESS);
	}	
	else if ((XP_API_STRCMP( menu_name, MENU_GL_USER_PF_AVATAR ) == 0))
	{		
		m_gl_xplayer_user->sendGetUserAvatar();
		m_next_sub_state = ONLINE_USER_PROFILE;
		SetOnlineSubState(ONLINE_PROCESS);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_FRIEND ) == 0 )
	{		
		SetOnlineSubState(ONLINE_FRIEND_MENU);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_ADD_FRIEND ) == 0 )
	{
		SetOnlineSubState(ONLINE_ADD_FRIEND);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_LIST_FRIENDS ) == 0 )
	{
		m_GLXPlayerUserFriend->sendGetUserFriends();
		SetOnlineSubState(ONLINE_PROCESS);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_INVITE_PLAY ) == 0 )
	{
		//TODO: Invite friend to play game
		//Create game room with set private to yes
		XP_API_MEMSET( m_invite_play_username, 0, 64 );
		XP_API_STRCPY( m_invite_play_username, m_GLXPlayerUserFriend->GetName(m_currentFriendIndex) );
		
		XP_API_MEMSET( m_invite_play_usernumber, 0, 64 );
		XP_API_STRCPY( m_invite_play_usernumber, m_GLXPlayerUserFriend->GetUserNumber(m_currentFriendIndex) );
		SetOnlineSubState(ONLINE_INVITE_PLAY);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_INVITE_CHAT ) == 0 )
	{
		//TODO: Invite friend to chat
		//Create chat screen
		XP_API_MEMSET( m_chat_send_data, 0, 256 );
		XP_API_MEMSET( m_chat_data, 0, 1024 );
		
		XP_API_MEMSET( m_chat_friend_name, 0, 64 );
		XP_API_STRCPY( m_chat_friend_name, m_GLXPlayerUserFriend->GetName(m_currentFriendIndex) );
		
		XP_API_MEMSET( m_chat_friend_number, 0, 64 );
		XP_API_STRCPY( m_chat_friend_number, m_GLXPlayerUserFriend->GetUserNumber(m_currentFriendIndex) );
		
		SetOnlineSubState(ONLINE_INVITE_CHAT);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_SEND_MSG ) == 0 )
	{		
		XP_API_DEL( m_msgReceiver );
		
		m_msgReceiver = XP_API_NEW char[64];
		XP_API_MEMSET(m_msgReceiver, 0, 64);
		XP_API_SPRINTF(m_msgReceiver, m_GLXPlayerUserFriend->GetName(m_currentFriendIndex));
		
		XP_API_MEMSET(m_msgReceiverNumber, 0, 64);
		XP_API_SPRINTF(m_msgReceiverNumber, m_GLXPlayerUserFriend->GetUserNumber(m_currentFriendIndex));
				
		XP_API_DEL( m_msgCC );
		m_msgCC = XP_API_NEW char[64];
		XP_API_MEMSET(m_msgCC, 0, 64);
				
		XP_API_DEL( m_msgTitle );
		m_msgTitle = XP_API_NEW char[64];
		XP_API_MEMSET(m_msgTitle, 0, 64);
		
		XP_API_DEL( m_msgContent );
		m_msgContent = XP_API_NEW char[256];
		XP_API_MEMSET(m_msgContent, 0, 256);
		
		m_isUserNumber = false;
		
		m_selectedIndex	= 0;
		SetOnlineSubState(ONLINE_SEND_MSG_TO_FRIEND);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_COMPARE_GAME ) == 0 )
	{
		//TODO: Compare game
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_USER_MORE_INFO ) == 0 )
	{
		//TODO: More information of friend
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_GL_NEW_GAME ) == 0 )
	{
		//Todo: New Game
	}
	else if ( XP_API_STRCMP( menu_name, MENU_USER_STATE ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_gl_xplayer_user->sendGetUserState();
	}
	else if ( XP_API_STRCMP( menu_name, MENU_USER_REPUTATION ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);		
		m_gl_xplayer_user->sendGetReputation();
	}	
	else if ( XP_API_STRCMP( menu_name, MENU_USER_DATA ) == 0 )
	{
		m_dataCenterMenu = XP_API_NEW Menu(DEF::START_POS_X,DEF::START_POS_Y);
		m_dataCenterMenu->AddMenuItem( MENU_USER_DATA_VIEW, this );	
		m_dataCenterMenu->AddMenuItem( MENU_USER_DATA_UPLOAD, this );	
		SetOnlineSubState(ONLINE_USER_DATA_MENU);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_USER_DATA_VIEW ) == 0 )
	{			
		SetOnlineSubState(ONLINE_USER_DATA_VIEW);			
	}
	else if ( XP_API_STRCMP( menu_name, MENU_USER_DATA_UPLOAD ) == 0 )
	{			
		XP_API_DEL(m_dataGroup);
		m_dataGroup = NULL;
		XP_API_DEL(m_dataValue);
		m_dataValue = NULL;
		SetOnlineSubState(ONLINE_USER_DATA_UPLOAD);			
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_CHAT ) == 0 )
	{		
		SetOnlineSubState(ONLINE_PROCESS);		
		m_xp_chat->Login(m_username,m_password);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_CHAT_WITH_FRIEND ) == 0 )
	{
		SetOnlineSubState(ONLINE_PROCESS);
		m_GLXPlayerUserFriend->sendGetUserFriends();
	}
	else if ( XP_API_STRCMP( menu_name, MENU_CHAT_IN_CONFERENCE ) == 0 )
	{
		SetOnlineSubState(ONLINE_CHAT_CONFERENCE_LIST);
		m_chat_room_list->Clear();
		m_conference_index = -1;
		m_xp_chat->ListChatRoom();
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_MESSAGE ) == 0 )
	{
		SetOnlineSubState(ONLINE_MESSAGE_MENU);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_MESSAGE_WRITE ) == 0 )
	{
		XP_API_MEMSET( m_message_receiver, 0, 64 );
		XP_API_MEMSET( m_message_title, 0, 64 );
		XP_API_MEMSET( m_message_content, 0, 64 );
		m_message_write_index = 0;
		SetOnlineSubState(ONLINE_MESSAGE_WRITE);
	}
	else if ( XP_API_STRCMP( menu_name, MENU_MESSAGE_INBOX ) == 0 )
	{
		m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE;
		m_GLXPlayerMessage->SendGetMessageCount();
		m_isInbox = true;
		SetOnlineSubState(ONLINE_PROCESS);		
	}
	else if ( XP_API_STRCMP( menu_name, MENU_GL_UPLOAD_VIDEO ) == 0 )
	{
		m_GLXPlayerYoutube->sendAddVideoInfo("guisu.huang@gmail.com", "huang123", "title", "comments");	
		SetOnlineSubState(ONLINE_PROCESS);
	}
	else if(XP_API_STRCMP(menu_name, MENU_GL_DOWNLOAD) == 0)
	{
		if( m_GLXPlayerDownload->SendDownloadFile(13214, "60ekli1dyufmxwcw2owa877783", "onlinequests") )
		{
			SetOnlineSubState(ONLINE_PROCESS);
		}
	}
}

bool GameStateOnline::HandleEventLogin( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		m_xp_login->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
		m_xp_login->SendLogin(m_username,m_password, GLXPlayerLogin::XPlayer_Login_Live);
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
				SetUsername( tmp );
			}
			else if(m_selectedIndex == 1)
			{
				SetPassword( tmp );
			}
		}
	}
	return bHandled;
}

void GameStateOnline::SetUsername( char * username )
{
	XP_API_MEMSET( m_username, 0, 15 );
	XP_API_STRCPY( m_username, username );
}

void GameStateOnline::SetPassword( char * password )
{
	XP_API_MEMSET( m_password, 0, 32 );
	XP_API_STRCPY( m_password, password );
}

bool GameStateOnline::HandleEventRegister( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState( ONLINE_PROCESS );
		m_gl_xplayer_user->sendRegister(m_username, m_password, m_email, m_nationData, m_emailFlag );
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
		if ( bHandled )
		{
			char* tmp = m_InputBox->getInputBoxString();
			if(m_selectedIndex == 0)
			{			
				SetUsername(tmp);
			}
			else if(m_selectedIndex == 1)
			{								
				SetPassword(tmp);
			}
			else if(m_selectedIndex == 2)
			{							
				XP_API_DEL(m_nationData);
				m_nationData = XP_API_STRNEW(tmp);
			}
		}
	}
	return bHandled;
}

bool GameStateOnline::HandleEventError( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		if(m_errorCode == ERROR_USER_HAS_LOGIN || m_errorCode == ERROR_HAS_NOT_LOGIN || m_errorCode == ERROR_NO_UUID )
		{
			m_state = DEF::GAME_STATE_MENU;
		}
		else if(m_errorCode == ERROR_CONNECTION && m_sub_state == ONLINE_MENU)
		{
			m_state = DEF::GAME_STATE_MENU;
		}
		else
		{
			SetOnlineSubState(ONLINE_MENU);
		}
		bHandled = true;
	}	
	return bHandled;
}


bool GameStateOnline::HandleEventProcess( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		if(m_xp_login->IsLoggedIn() == false)
		{
			m_xp_login->Cancel();
			m_state = DEF::GAME_STATE_MENU;	
		}
		else
		{
			m_gl_xplayer_user->Cancel();
			SetOnlineSubState(ONLINE_MENU);
		}
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleMsgList( _TOUCH_EVENT event)
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
			SetOnlineSubState(ONLINE_PROCESS);
		}		
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

bool GameStateOnline::HandleViewMessage( _TOUCH_EVENT event )
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
				if(m_isInbox == true)
				{
					//SetOnlineSubState(ONLINE_MESSAGE_INBOX);

					m_GLXPlayerMessage->SendGetMessageCount();
					SetOnlineSubState(ONLINE_PROCESS);		
				}
				else
				{
					SetOnlineSubState(ONLINE_MSG_LIST);
				}
				bHandled = true;
			}
		}
		else
		{
			bool rt = m_GLXPlayerMessage->getMsgReadState(i); 
			
			if (rt == false)
			{
				if(t ==  USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION)				
				{
					if ( event.isClicked && m_SoftRight.Contain(event.position) )
					{			
					#ifdef USE_USER_NUMBER
						m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSenderNumber(i), 0);
					#else
						m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSender(i), 0);
					#endif // USE_USER_NUMBER
						SetOnlineSubState(ONLINE_PROCESS);
						bHandled = true;
					}
					else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
					{			
					#ifdef USE_USER_NUMBER
						m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSenderNumber(i), 1);
					#else
						m_GLXPlayerUserFriend->sendConfirmUserFriend(m_GLXPlayerMessage->getMsgSender(i), 1);
					#endif // USE_USER_NUMBER
						SetOnlineSubState(ONLINE_PROCESS);
						bHandled = true;
					}
				}
				else if ( t == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION )
				{
					if ( event.isClicked && m_SoftRight.Contain(event.position) )
					{				
						if(m_isInbox == true)
						{
							//SetOnlineSubState(ONLINE_MESSAGE_INBOX);

							m_GLXPlayerMessage->SendGetMessageCount();
							SetOnlineSubState(ONLINE_PROCESS);
						}
						else
						{
							SetOnlineSubState(ONLINE_MSG_LIST);
						}
						bHandled = true;
					}
					else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
					{			
						m_xp_chat->Login( m_username, m_password );
						XP_API_MEMSET( m_chat_send_data, 0, 256 );
						XP_API_MEMSET( m_chat_data, 0, 1024 );
						XP_API_MEMSET( m_chat_friend_name, 0, 64 );
						XP_API_STRCPY( m_chat_friend_name, m_GLXPlayerMessage->getMsgSender(i) );
						SetOnlineSubState(ONLINE_PROCESS);
						bHandled = true;
					}
				}
				else if ( t == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION )
				{
					if ( event.isClicked && m_SoftRight.Contain(event.position) )
					{				
						if(m_isInbox == true)
						{
							//SetOnlineSubState(ONLINE_MESSAGE_INBOX);

							m_GLXPlayerMessage->SendGetMessageCount();
							SetOnlineSubState(ONLINE_PROCESS);
						}
						else
						{
							SetOnlineSubState(ONLINE_MSG_LIST);
						}
						bHandled = true;
					}
					else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
					{
						if(m_isInbox == true)
						{
							m_playInvitedGame = true;
							m_isInvitor = false;
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
							m_GLXPlayerMultiPlayer->SetUsername(m_username);
							//m_GLXPlayerLogin->SendGetCountry();
							m_gl_xplayer_user->sendGetCountry(m_username);
#else
							m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
#endif
							SetOnlineSubState(ONLINE_PROCESS);
							//SetOnlineSubState(ONLINE_MESSAGE_INBOX);
						}
						else
						{
							SetOnlineSubState(ONLINE_MSG_LIST);
						}
						bHandled = true;
					}
				}
			}
			else
			{
				if ( event.isClicked && m_SoftRight.Contain(event.position) )
				{				
					if(m_isInbox == true)
					{
						//SetOnlineSubState(ONLINE_MESSAGE_INBOX);

						m_GLXPlayerMessage->SendGetMessageCount();
						SetOnlineSubState(ONLINE_PROCESS);
					}
					else
					{
						SetOnlineSubState(ONLINE_MSG_LIST);
					}
					bHandled = true;
				}
			}
		}
	}
		
	return bHandled;
}


bool GameStateOnline::HandleMsgInfo( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
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


bool GameStateOnline::HandleEventUserGameList( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventUserReputation( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_onlineMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER		
		m_state = DEF::GAME_STATE_MENU;
#else
		SetOnlineSubState(ONLINE_PROCESS);			
		m_xp_login->SendLogout();		
#endif		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventDataMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_dataCenterMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineProfileMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_profileMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineFriendMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_friendMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineCurrentFriendMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_currentFriendMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_selectedIndex = -1;
		if ( m_GLXPlayerUserFriend->GetFriendsCount() > 0 )
		{
			m_selectedIndex = 0;
		}
		SetOnlineSubState(ONLINE_LIST_FRIENDS);		
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineAddFriend( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_FRIEND_MENU);	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{					
	#ifdef USE_USER_NUMBER
		m_GLXPlayerUserFriend->sendAddUserFriend(m_msgReceiver, m_msgContent, m_isUserNumber);	
	#else
		m_GLXPlayerUserFriend->sendAddUserFriend(m_msgReceiver, m_msgContent);	
	#endif // USE_USER_NUMBER
		SetOnlineSubState(ONLINE_PROCESS);
		
		m_InputBox->deleteTextInTextBox();
		XP_API_DEL(m_msgReceiver);
		XP_API_DEL(m_msgContent);
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{
		#ifdef USE_USER_NUMBER
			m_selectedIndex = 2;	
		#else			
			m_selectedIndex = 1;
		#endif //USE_USER_NUMBER
		}	
		m_InputBox->deleteTextInTextBox();		
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
	#ifdef USE_USER_NUMBER
		if(m_selectedIndex == 3)
	#else
		if(m_selectedIndex == 2)
	#endif //USE_USER_NUMBER
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
			if(m_selectedIndex == 0)
			{
				XP_API_DEL( m_msgReceiver );
				m_msgReceiver = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			else if(m_selectedIndex == 1)
			{
				XP_API_DEL( m_msgContent );
				m_msgContent = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			#ifdef USE_USER_NUMBER
			else if(m_selectedIndex == 2)
			{				
				m_isUserNumber = (XP_API_ATOI( m_InputBox->getInputBoxString() )!=0);
			}
			#endif // USE_USER_NUMBER
		}
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineCompareGameWithFriend( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_CURRENT_FRIEND_MENU);			
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineSendMsg( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		//SetOnlineSubState(ONLINE_CURRENT_FRIEND_MENU);	
		SetOnlineSubState(m_pre_sub_state);	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		m_InputBox->deleteTextInTextBox();
		m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE;
		char tmp[1024] = {0};
		XP_API_MEMSET(tmp, 0, 1024);
		
	#ifdef USE_USER_NUMBER	
		
		if(m_isUserNumber)
		{
			if(XP_API_STRLEN(m_msgCC)>0)
			{
				XP_API_SPRINTF(tmp, "%s,%s",m_msgReceiverNumber, m_msgCC);
			}
			else
			{
				XP_API_SPRINTF(tmp, "%s",m_msgReceiverNumber);
			}
		}
		else
		{
			if(XP_API_STRLEN(m_msgCC)>0)
			{
				XP_API_SPRINTF(tmp, "%s,%s",m_msgReceiver, m_msgCC);
			}
			else
			{
				XP_API_SPRINTF(tmp, "%s",m_msgReceiver);
			}			
		}
		m_GLXPlayerMessage->SendOnlineMessage(tmp,m_currentMsgType, m_msgTitle,m_msgContent,XP_API_STRLEN(m_msgContent), m_isUserNumber);
		
	#else
		m_isUserNumber = false;
		if(XP_API_STRLEN(m_msgCC)>0)
		{
			XP_API_SPRINTF(tmp, "%s,%s",m_msgReceiver, m_msgCC);
		}
		else
		{
			XP_API_SPRINTF(tmp, "%s",m_msgReceiver);
		}		
		m_GLXPlayerMessage->SendOnlineMessage(m_msgReceiver,m_currentMsgType, m_msgTitle,m_msgContent,XP_API_STRLEN(m_msgContent));
	#endif // USE_USER_NUMBER
		m_pre_sub_state = ONLINE_SEND_MSG_TO_FRIEND;
		SetOnlineSubState(ONLINE_PROCESS);
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		m_selectedIndex --;		
		if(m_selectedIndex < 0)
		{
			m_selectedIndex = 4;			
		}	
		m_InputBox->deleteTextInTextBox();		
		bHandled = true;
	}	
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{	
		m_selectedIndex ++;
		if(m_selectedIndex == 5)
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
			if(m_selectedIndex == 0)
			{
				XP_API_DEL( m_msgReceiver );
				m_msgReceiver = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			else if(m_selectedIndex == 1)
			{
				XP_API_DEL( m_msgCC );
				m_msgCC = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			else if(m_selectedIndex == 2)
			{				
				m_isUserNumber = (XP_API_ATOI( m_InputBox->getInputBoxString() ) != 0);
			}
			else if(m_selectedIndex == 3)
			{
				XP_API_DEL( m_msgTitle );
				m_msgTitle = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			else if(m_selectedIndex == 4)
			{
				XP_API_DEL( m_msgContent );
				m_msgContent = XP_API_STRNEW( m_InputBox->getInputBoxString() );
			}
			
		}
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineListFriend( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if((XP_API_GET_TIME() - m_startTime) > REFRESH_FRIEND_LIST_TIME_OUT)
	{
		m_startTime = XP_API_GET_TIME();
		m_GLXPlayerUserFriend->sendGetUserFriends();
	}
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{		
		SetOnlineSubState(ONLINE_FRIEND_MENU);	
		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if((m_selectedIndex > -1) && (m_selectedIndex < m_GLXPlayerUserFriend->GetFriendsCount()))
		{
			int state = m_GLXPlayerUserFriend->GetState(m_selectedIndex);
			if(state != XPLAYER_USER_STATE_OFFLINE)
			{
				if(m_currentFriendMenu->getItemCount() > 0)
				{
					m_currentFriendMenu->RemoveAllMenuItem();
				}
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_INVITE_PLAY, this );	
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_INVITE_CHAT, this );	
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_SEND_MSG, this );
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_COMPARE_GAME, this );					
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_MORE_INFO, this );				
			}
			else
			{
				if(m_currentFriendMenu->getItemCount() > 0)
				{
					m_currentFriendMenu->RemoveAllMenuItem();
				}
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_SEND_MSG, this );
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_COMPARE_GAME, this );					
				m_currentFriendMenu->AddMenuItem( MENU_GL_USER_MORE_INFO, this );	
			}
			m_currentFriendIndex = m_selectedIndex;
			SetOnlineSubState(ONLINE_CURRENT_FRIEND_MENU);
		}		 
		m_selectedIndex = -1;
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		if((m_selectedIndex > -1) && (m_selectedIndex < m_GLXPlayerUserFriend->GetFriendsCount()))
		{
			m_currentFriendIndex = m_selectedIndex;
			m_GLXPlayerUserFriend->sendDelUserFriend(m_GLXPlayerUserFriend->GetName(m_currentFriendIndex));
			SetOnlineSubState(ONLINE_PROCESS);
		}
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

bool GameStateOnline::HandleEventOnlineUserProfile( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_PROFILE_MENU);
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else if(event.isClicked && m_SoftLeft.Contain(event.position))
	{
		switch(m_profileMenu->getSelectedIndex())
		{
			case 0:
				m_gl_xplayer_user->sendChangeUserName(m_username, m_password, m_InputBox->getInputBoxString());			
				SetOnlineSubState(ONLINE_PROCESS);				
				break;
			case 1:
				m_oldPassword = XP_API_NEW char [32];
				XP_API_MEMSET(m_oldPassword, 0, 32);
				XP_API_SPRINTF(m_oldPassword, m_InputBox->getInputBoxString());
				m_gl_xplayer_user->sendChangePassword(m_username, m_password, m_oldPassword, m_oldPassword);
				SetOnlineSubState(ONLINE_PROCESS);
				break;				
			case 2:	
				m_gl_xplayer_user->sendChangeEmail(m_username, m_password, m_InputBox->getInputBoxString());
				SetOnlineSubState(ONLINE_PROCESS);
				break;
			case 3:	
			{
				int email_flag = XP_API_ATOI(m_InputBox->getInputBoxString());
				m_gl_xplayer_user->sendUpdateUserInfo(NULL,email_flag,NULL,NULL,NULL,-1,NULL);
				SetOnlineSubState(ONLINE_PROCESS);
			}
				break;
			case 4:	
				m_gl_xplayer_user->sendChangeCountry(m_username, m_password, m_InputBox->getInputBoxString());			
				SetOnlineSubState(ONLINE_PROCESS);
				break;
			case 5:	
				{
					CResourceStream *stream = XP_API_NEW CResourceStream(m_InputBox->getInputBoxString());
					if(stream != NULL)
					{
						BYTE* buffer = (BYTE*)(stream->GetBuffer());
						char* blob = SSEncDec_ByteArray2Blob(buffer, stream->GetLength());
						
						m_gl_xplayer_user->sendUploadUserAvatar(blob);
						SetOnlineSubState(ONLINE_PROCESS);			
						
						XP_API_DEL(blob);
						blob = NULL;			

						XP_API_DEL(stream);
						stream = NULL;						
					}				
				}
				break;
				
			default:
				break;
			
		}	
		m_InputBox->deleteTextInTextBox();
		bHandled = true;
	}
	else  
	{
		bHandled = m_InputBox->updateInputBox(event);				
	}		
	return bHandled;
}

bool GameStateOnline::HandleEventDataView( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_USER_DATA_MENU);	
		m_InputBox->deleteTextInTextBox();		
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		SetOnlineSubState(ONLINE_PROCESS);
		m_gl_xplayer_user->sendGetUserStoreData(m_dataGroup);		
		bHandled = true;
	}	
	else 
	{
		bHandled = m_InputBox->updateInputBox(event);
		XP_API_DEL(m_dataGroup);
		m_dataGroup = XP_API_STRNEW( m_InputBox->getInputBoxString() );			
	}
	return bHandled;
}

bool GameStateOnline::HandleEventDataUpload( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);	
		m_InputBox->deleteTextInTextBox();
		m_selectedIndex = -1;		
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_PROCESS);		
		char* tmp = SSEncDec_String2Blob( m_dataValue );
		m_gl_xplayer_user->sendSetUserStoreData(m_dataGroup, tmp);
		m_InputBox->deleteTextInTextBox();
		XP_API_DEL( tmp );
		XP_API_DEL(m_dataValue);
		m_dataValue = NULL;
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
		char* tmp = m_InputBox->getInputBoxString();
		if(m_selectedIndex == 0)
		{		
			XP_API_DEL( m_dataGroup );
			m_dataGroup = XP_API_STRNEW( tmp );						
		}
		else if(m_selectedIndex == 1)
		{			
			XP_API_DEL( m_dataValue );
			m_dataValue = XP_API_STRNEW( tmp );						
		}		
	}	
	return bHandled;
}

/**
 * Network is broken.
 *
 */
void GameStateOnline::OnNetworkError()
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_CONNECTION;
	XP_DEBUG_OUT( "[GameStateOnline] OnUserNetworkError: %d\n", m_errorCode);
}

/**
 * Timeout.
 *
 */
void GameStateOnline::OnRequestTimeout( int func_id )
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = ERROR_REQUEST_TIME_OUT;
	XP_DEBUG_OUT( "[GameStateOnline]  OnRequestTimeout: Function: %d - Error: %d\n", func_id, m_errorCode);
}

/**
 * The correct response.
 *
 */
void GameStateOnline::OnRequestSuccess( int func_id, char * data, int data_len )
{
	XP_DEBUG_OUT( "[GameStateOnline] OnUserFunctionOK\n" );
	switch(func_id)
	{
		case GL_XPLAYER_FUNCTION_LOGIN:	
			SaveProfile();
			m_gl_xplayer_user->SetUID( m_xp_login->GetUID() );
			m_gl_xplayer_user->setPassword(m_password);
			m_GLXPlayerMessage->SetUID( m_xp_login->GetUID() );	
			m_GLXPlayerUserFriend->SetUID(m_xp_login->GetUID());
			m_GLXPlayerYoutube->SetUID(m_xp_login->GetUID());
			m_next_sub_state = ONLINE_PROCESS;
			m_gl_xplayer_user->sendGetUserInfo();	

			SetOnlineSubState(ONLINE_PROCESS);
			break;

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
		case GL_XPLAYER_FUNCTION_GET_COUNTRY:
			//m_GLXPlayerMultiPlayer->SetCountry(m_GLXPlayerLogin->GetCountry());
			m_GLXPlayerMultiPlayer->SetCountry(m_gl_xplayer_user->getCountry());
			m_GLXPlayerMultiPlayer->mpSendUPnPPortMapping();
			break;
#endif
		case GL_XPLAYER_FUNCTION_LOGOUT:
			m_state = DEF::GAME_STATE_MENU;	
			break;
		case FUNCTION_GET_MESSAGE_COUNT:					
			if(m_isInbox == false)		
			{
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
					SetOnlineSubState(ONLINE_MENU);
				}
			}			
			else
			{
				m_userMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE, USER_MESSAGE_READTYPE_ALL);
				m_friendInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION, USER_MESSAGE_READTYPE_ALL);
				m_gameInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, USER_MESSAGE_READTYPE_ALL);
				m_chatInvitedMsgCount = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, USER_MESSAGE_READTYPE_ALL);
				m_allMsgCount = m_userMsgCount + m_friendInvitedMsgCount + m_gameInvitedMsgCount + m_chatInvitedMsgCount;

				if( m_allMsgCount <= 0 )
				{
					m_msgInPageCount = 0;
					SetOnlineSubState( ONLINE_MESSAGE_INBOX );
				}
				else 
				{
					if( m_allMsgCount <= 5 )
					{	
						m_msgInPageCount = m_allMsgCount;							
					}
					else
					{
						m_msgInPageCount = 5;
					}
					m_GLXPlayerMessage->SendGetMessageHeader(
						USER_MESSAGE_TYPE_USERMESSAGE | USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION | 
						USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION | USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, 
						USER_MESSAGE_READTYPE_ALL, 0, 5);

					SetOnlineSubState(ONLINE_PROCESS);
				}
			}		
			break;
		case FUNCTION_GET_MESSAGE_HEADER:
			if(m_isInbox == true)
			{
				SetOnlineSubState(ONLINE_MESSAGE_INBOX);
			}
			else
			{
				SetOnlineSubState(ONLINE_MSG_LIST);			
			}
			break;
		case FUNCTION_READ_MESSAGE:
			SetOnlineSubState(ONLINE_MSG_READ);
			break;
		case FUNCTION_SEND_MESSAGE:
			if(m_currentMsgType == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION)
			{
				m_playInvitedGame = true;
				m_isInvitor = true;
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
				m_GLXPlayerMultiPlayer->SetUsername(m_username);
				//m_GLXPlayerLogin->SendGetCountry();
				m_gl_xplayer_user->sendGetCountry(m_username);
				SetOnlineSubState(ONLINE_PROCESS);
#elif(MULTIPLAYER_TYPE == MULTIPLAYER_LOBBY)
				SetOnlineSubState(ONLINE_CURRENT_FRIEND_MENU);
#else
				m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
				SetOnlineSubState(ONLINE_PROCESS);
#endif
			}
			else if(m_currentMsgType == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION)
			{
				m_xp_chat->Login( m_username, m_password );
				SetOnlineSubState(ONLINE_PROCESS);
			}
			else
			{
				if(m_pre_sub_state == ONLINE_MESSAGE_WRITE)
				{
					SetOnlineSubState(ONLINE_MESSAGE_MENU);
				}
				else if(m_pre_sub_state == ONLINE_SEND_MSG_TO_FRIEND)
				{
					SetOnlineSubState(ONLINE_CURRENT_FRIEND_MENU);
				}
			}
			break;
		case FUNCTION_DEL_MESSAGE:
			m_GLXPlayerMessage->SendGetMessageCount();
			m_isInbox = true;
			SetOnlineSubState(ONLINE_PROCESS);
			break;
		case FUNCTION_REGISTER:
#ifdef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
			m_xp_login->SendLogin(m_username,m_password,GLXPlayerLogin::XPlayer_Login);
#else
			m_xp_login->SendLogin(m_username, m_password, GLXPlayerLogin::XPlayer_Login_Live);
#endif		
			SetOnlineSubState(ONLINE_PROCESS);			
			break;			
		case FUNCTION_GET_USER_GAMELIST:			
			SetOnlineSubState(ONLINE_USER_GAME_LIST);				
			break;		
		case FUNCTION_GET_USER_STATE:			
			SetOnlineSubState(ONLINE_USER_STATE);		
			break;	
		case FUNCTION_GET_REPUTATION:			
			SetOnlineSubState(ONLINE_USER_REPUTATION);		
			break;	
		case FUNCTION_GET_USER_STORE_DATA:
			
			//Data is stored as an blob, so it can be any type of data, Here we demo with string.
			m_dataValue = SSEncDec_Blob2String(data);
			SetOnlineSubState(ONLINE_USER_DATA_VIEW);
									
			break;		
		case FUNCTION_SET_USER_STORE_DATA:			
			SetOnlineSubState(ONLINE_USER_DATA_MENU);			
			break;	
			
	
		case FUNCTION_CHANGE_USERNAME:
			{
				char tmp[31] = {0};				
				getValue(data, tmp, 1, '|');	//irgnore |u|		
				SetUsername( tmp );
				SetOnlineSubState(ONLINE_PROFILE_MENU);			
			}
			break;			
		case FUNCTION_CHANGE_PASSWORD:
			SetPassword( m_oldPassword );
			m_gl_xplayer_user->setPassword(m_password);
			SetOnlineSubState(ONLINE_PROFILE_MENU);			
			break;
		case FUNCTION_CHANGE_EMAIL:		
		case FUNCTION_CHANGE_COUNTRY:					
		case FUNCTION_UPLOAD_USER_AVATAR:					
			SetOnlineSubState(ONLINE_PROFILE_MENU);
			break;	
		case FUNCTION_ADD_USER_FRIEND:					
			SetOnlineSubState(ONLINE_FRIEND_MENU);			
			break;	
		case FUNCTION_GET_USER_FRIENDS:		
			m_selectedIndex = -1;
			if ( m_GLXPlayerUserFriend->GetFriendsCount() > 0 )
			{
				m_selectedIndex = 0;
			}
			if ( m_pre_sub_state == ONLINE_CHAT_MENU )
			{
				SetOnlineSubState(ONLINE_CHAT_WITH_FRIEND);
			}
			else
			{
				m_startTime = XP_API_GET_TIME();
				SetOnlineSubState(ONLINE_LIST_FRIENDS);
			}
			break;	
		case FUNCTION_DEL_USER_FRIEND:	
			m_GLXPlayerUserFriend->sendGetUserFriends();
			SetOnlineSubState(ONLINE_PROCESS);			
			break;	
		case FUNCTION_CONFIRM_USER_FRIEND:	
			if(m_isInbox == true)
			{
				SetOnlineSubState(ONLINE_MESSAGE_INBOX);
			}
			else
			{
				m_GLXPlayerMessage->SendGetMessageCount();
				SetOnlineSubState(ONLINE_PROCESS);	
			}			
			break;	
		case FUNCTION_GET_USER_INFO:			
			XP_API_DEL(m_nationData);
			XP_API_DEL(m_email);
			
			XP_API_MEMSET(m_username, 0, sizeof(m_username));
			XP_API_MEMSET(m_password, 0, sizeof(m_password));
			
			if( m_gl_xplayer_user->getUserName() != NULL)
			{
				XP_API_SPRINTF(m_username, m_gl_xplayer_user->getUserName());
			}
			if(m_gl_xplayer_user->getPassword() != NULL)
			{
				XP_API_SPRINTF(m_password, m_gl_xplayer_user->getPassword());
			}
			if(m_gl_xplayer_user->getCountry() != NULL)
			{
				m_nationData = XP_API_STRNEW(m_gl_xplayer_user->getCountry());
			}
			if(m_gl_xplayer_user->getEmail() != NULL)
			{
				m_email = XP_API_STRNEW(m_gl_xplayer_user->getEmail());			
			}
			
			m_emailFlag = m_gl_xplayer_user->getEmailFlag();
			
			if(m_next_sub_state == ONLINE_PROCESS)
			{
				m_GLXPlayerMessage->SendGetMessageCount();			
			}
			SetOnlineSubState(m_next_sub_state);			
			break;	
		case FUNCTION_GET_USER_AVATAR:	
			XP_API_DEL(m_avatar);
			if(m_gl_xplayer_user->getUserAvatarKey() != NULL)
			{
				m_avatar = XP_API_STRNEW(m_gl_xplayer_user->getUserAvatarKey());
			}
			SetOnlineSubState(m_next_sub_state);
			break;	
		case FUNCTION_POINT_USER_REPUTATION:				
			SetOnlineSubState(ONLINE_MP_END_GAME);
			break;
		case FUNCTION_KEEP_ALIVE:
			break;
		case FUNCTION_ADD_VIDEO_INFO:
			{				
				CResourceStream *stream = XP_API_NEW CResourceStream("test.avi");
				if(stream != NULL)
				{
					BYTE* pData = (BYTE*)(stream->GetBuffer());
					int size = stream->GetLength();					

					m_GLXPlayerYoutube->sendAddVideoData((char *)pData, size);
					SetOnlineSubState(ONLINE_PROCESS);							
					
					XP_API_DEL(stream);										
				}
			}	
			break;
		case FUNCTION_UPLOAD_VIDEO_DATA:
			SetOnlineSubState(ONLINE_MENU);
			break;
		case FUNCTION_DOWNLOAD_ITEM:
			{
				XP_FILE_HANDLE file = XP_API_FILE_OPEN( "download_test", "w" );
				int writeLen = XP_API_FILE_WRITE( data, sizeof(char), data_len, file );
				XP_API_FILE_CLOSE( file );
				SetOnlineSubState(ONLINE_MENU);
			}
			break;
		default:	
			SetOnlineSubState(ONLINE_MENU);		
			break;			
	}
}

/**
 * The wrong response or the operation is failed on server side.
 *
 */
void GameStateOnline::OnRequestFailure( int func_id, int error_code )
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
				XP_DEBUG_OUT( "[GameStateOnline] OnRequestFailure: %d\n", error_code);
			}
		break;			
		case FUNCTION_GET_USER_STORE_DATA:	
			if(error_code == ERROR_DATA_NOT_STORE)
			{
				m_dataValue = XP_API_NEW char[64];
				XP_API_MEMSET(m_dataValue, 0, 64);
				XP_API_SPRINTF(m_dataValue, "#This tag does not contain any data.");					
				SetOnlineSubState(ONLINE_USER_DATA_VIEW);					
			}
			else			
			{
				SetOnlineSubState(ONLINE_ERROR);
				m_errorCode = error_code;			
				XP_DEBUG_OUT( "[GameStateOnline] OnRequestFailure: %d\n", error_code);
			}
			break;
		case FUNCTION_ADD_VIDEO_INFO:
			if(error_code == ERROR_YOUTUBE_ACCOUNT_NOT_EXIT)
			{			
				SetOnlineSubState(ONLINE_ERROR);
			}
			break;
		case FUNCTION_UPLOAD_VIDEO_DATA:
			SetOnlineSubState(ONLINE_ERROR);
			m_errorCode = error_code;			
			XP_DEBUG_OUT( "[GameStateOnline] OnRequestFailure: %d\n", error_code);
			break;
		case FUNCTION_DOWNLOAD_ITEM:
			SetOnlineSubState(ONLINE_ERROR);
			m_errorCode = error_code;			
			XP_DEBUG_OUT( "[GameStateOnline] OnRequestFailure: %d\n", error_code);
			break;
		default:
			SetOnlineSubState(ONLINE_ERROR);
			m_errorCode = error_code;			
			XP_DEBUG_OUT( "[GameStateOnline] OnRequestFailure: %d\n", error_code);	
			break;
	}
}

//============multiplayer observer================
void GameStateOnline::OnMPNetworkError(int errorCode)
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = errorCode;
	XP_DEBUG_OUT( "[GameStateOnline] OnMPNetworkError: %d\n", m_errorCode);
}

void GameStateOnline::OnMPFunctionError( char* errMsg, int error_code )
{
	SetOnlineSubState(ONLINE_ERROR);
	m_errorCode = error_code;
	//m_errMsg = errMsg;
	XP_DEBUG_OUT( "[GameStateOnline] OnMPFunctionError: %d\n", m_errorCode);
}

void GameStateOnline::OnMPuPnPTestSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	m_GLXPlayerMultiPlayer->mpSendEstablishConnection();
	SetOnlineSubState(ONLINE_MP_PROCESS);
#endif
}

void GameStateOnline::OnMPEstablishConnectionSuccess()
{
	char* uid = m_xp_login->GetUID();
	m_GLXPlayerMultiPlayer->mpSendLogin(uid);
	SetOnlineSubState(ONLINE_PROCESS);
}

void GameStateOnline::OnMPDisconnectSuccess()
{
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	if(m_sub_state != ONLINE_MP_IN_GAME)
	{
		if(m_isInvitor)
		{
			SetOnlineSubState(ONLINE_LIST_FRIENDS);
		}
		else
		{
			m_GLXPlayerMessage->SendGetMessageCount();
			SetOnlineSubState(ONLINE_PROCESS);		
		}

		m_playInvitedGame = false;
		m_isInvitor = false;
	}
#else
	if(m_isInvitor)
	{
		SetOnlineSubState(ONLINE_LIST_FRIENDS);
	}
	else
	{
		m_GLXPlayerMessage->SendGetMessageCount();
		SetOnlineSubState(ONLINE_PROCESS);		
	}

	m_playInvitedGame = false;
	m_isInvitor = false;
#endif
}

void GameStateOnline::OnMPLoginSuccess(DataPacket* currentPacket)
{
	if(m_isInvitor)
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
	}
	else
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
		m_GLXPlayerMultiPlayer->mpSendListRoom( 5, 0, NULL );
		//m_GLXPlayerMultiPlayer->mpSendListAllRoom( 5, 0 );
		XP_API_DEL( packet );
		XP_API_DEL( query );	
#else
		m_GLXPlayerMultiPlayer->mpSendListSession(MAX_LIST_SIZE,0);	
#endif
	}

	SetOnlineSubState(ONLINE_PROCESS);		
}

void GameStateOnline::OnMPCreateSessionSuccess(DataPacket* currentPacket)
{
	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateOnline::OnMPListSessionSuccess(DataPacket* currentPacket)
{
	m_selectedIndex = -1;
	SetOnlineSubState(ONLINE_MP_IN_SESSION_LIST);
}

void GameStateOnline::OnMPJoinSessionSuccess(DataPacket* currentPacket)
{
	SetOnlineSubState(ONLINE_MP_IN_SESSION);
}

void GameStateOnline::OnMPLeaveSessionSuccess()
{
	m_GLXPlayerMultiPlayer->mpSendDisconnect();
	SetOnlineSubState(ONLINE_MP_PROCESS);
}

void GameStateOnline::OnMPLeaveGameSuccess()
{
#if (MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	if(m_isInvitor)
	{
		SetOnlineSubState(ONLINE_LIST_FRIENDS);
	}
	else
	{
		m_GLXPlayerMessage->SendGetMessageCount();
		SetOnlineSubState(ONLINE_PROCESS);		
	}
#else
	m_GLXPlayerMultiPlayer->mpSendDisconnect();
	SetOnlineSubState(ONLINE_MP_PROCESS);
#endif
}

void GameStateOnline::OnMPStartGameSuccess(DataPacket* currentPacket)
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

void GameStateOnline::OnMPKickOutPlayerSuccess()
{	
	m_GLXPlayerMultiPlayer->mpSendDisconnect();
	SetOnlineSubState(ONLINE_MP_PROCESS);
}

void GameStateOnline::OnMPFinishGameSuccess()
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
//================================================

//////////////////////////////////////

/**
 * The method will be invoked after the connection to the chat server was established successfully.
 *
 */
void GameStateOnline::OnChatConnected()
{
	if ( m_sub_state == ONLINE_PROCESS )
	{
		if ( m_pre_sub_state == ONLINE_MENU )
		{
			SetOnlineSubState( ONLINE_CHAT_MENU );
		}
		else
		{
			SetOnlineSubState( ONLINE_CHAT_SCREEN );
		}		
	}
}

/**
 * The method will be invoked if the connection to the chat server has been broken.
 *
 */
void GameStateOnline::OnChatDisconnect( int e, int reason )
{
	m_errorCode = e;
	switch(e)
	{
	case ChatConnStreamError:
		{
			switch(reason)
			{
			case ChatStreamErrorConflict: //someone login in another place using the same username and password
				//notify user that he is kicked...
				break;
			default:
				break;
			}
		}
		break;
	case ChatConnAuthenticationFailed:
		{
			switch(reason)
			{
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

	SetOnlineSubState( ONLINE_ERROR );
}

/**
 * After the login, 
 *
 */
void GameStateOnline::OnChatRoster( const char * jid, const char * name, const char * group, int state )
{

}

void GameStateOnline::OnChatRosterPresence( const char * jid, const char * name, const char * group, int state )
{

}

void GameStateOnline::OnChatSession( char * from )
{

}

/**
 * The method will be invoked after a message was arrived.
 * 
 * @param
 *	char * from:	the jid of the sender, such as "username@gameloft.com"
 *	char * message:	the content of the message, such as "hello"
 *
 */
void GameStateOnline::OnChatMessage( char * from, char * message )
{
	if ( m_sub_state == ONLINE_CHAT_SCREEN )
	{
		char temp[1024] = { 0 };
		XP_API_MEMSET( temp, 0 , 1024 );
		XP_API_STRCPY( temp, m_chat_data );
		XP_API_MEMSET( m_chat_data, 0, 1024 );
		XP_API_SPRINTF( m_chat_data, "<%s>:%s\n%s", from, message, temp );
	}
	else
	{
		char tmpName[64];
		XP_API_MEMSET(tmpName, 0, 64);

		XP_API_MEMSET( m_chat_friend_name, 0, 64 );
		XP_API_MEMSET( m_chat_send_data, 0, 256 );
		XP_API_MEMSET( m_chat_data, 0, 1024 );
		XP_API_SPRINTF( m_chat_data, "<%s>:%s", from, message );

		XP_API_PARSE_DATA(from, tmpName, 0, '@');
		XP_API_STRCPY( m_chat_friend_name, tmpName );
		SetOnlineSubState( ONLINE_CHAT_SCREEN );
	}
}

void GameStateOnline::OnChatAddBuddy( const char * jid )
{

}

void GameStateOnline::OnChatRemoveBuddy( const char * jid )
{

}

/**
 * This method will be invoked after a user sent a buddy request to you.
 *
 * @param
 *	char * jid:		the jid of the applicant, such as "username@gameloft.com"
 *	char * message:	the appended information, such as "May I have a talk to you?"
 *
 */
void GameStateOnline::OnChatBuddyRequest( const char * jid, const char * message )
{

}

/**
 * This method will be invoked after a invitation was arrived to ask you join a conference.
 *
 * @param
 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
 *	char * invitor:		the jid of the sender, such as "username@gameloft.com"
 *	char * reason:		the appended information with the invitation, such as "come on"
 *	char * password:	the password will be appended if the chat room has been encrypted.
 *
 */
void GameStateOnline::OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
	const char * password )
{
	XP_API_MEMSET( m_chat_room_jid, 0, 64 );
	XP_API_MEMSET( m_chat_room_invitor, 0, 64 );
	XP_API_MEMSET( m_chat_room_invite_reason, 0, 64 );
	XP_API_MEMSET( m_chat_room_invite_password, 0, 64 );
	XP_API_STRCPY( m_chat_room_jid, room );
	XP_API_STRCPY( m_chat_room_invitor, invitor );
	XP_API_STRCPY( m_chat_room_invite_reason, reason );
	XP_API_STRCPY( m_chat_room_invite_password, password );
	SetOnlineSubState(ONLINE_CHAT_BE_INVITED);
}

/**
 * This method will be invoked after a conference message was arrived.
 *
 * @param
 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
 *	char * nickname:	the nickname of the sender
 *	char * message:		the content of the message
 *
 */
void GameStateOnline::OnChatRoomMessage( const char * room, const char * nickname, const char * message )
{
	if ( NULL == nickname || XP_API_STRLEN( nickname ) == 0 )
	{
		return;
	}

	// Append the message to the end of the data buffer
	if ( m_sub_state == ONLINE_CHAT_CONFERENCE )
	{
		char temp[1024] = { 0 };
		XP_API_MEMSET( temp, 0 , 1024 );
		XP_API_STRCPY( temp, m_conference_data );
		XP_API_MEMSET( m_conference_data, 0 , 1024 );
		XP_API_SPRINTF( m_conference_data, "%s:%s\n%s", nickname, message, temp );
	}
	else
	{
		SetOnlineSubState( ONLINE_CHAT_CONFERENCE );
		XP_API_MEMSET( m_conference_data, 0 , 1024 );
		XP_API_SPRINTF( m_conference_data, "%s:%s", nickname, message );
	}	
}

/**
 * This method will be invoked after a conference has been found.
 *
 * @param
 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
 *	char * nickname:	the nickname of the chat room
 *
 */
void GameStateOnline::OnChatRoomFound( const char * room, const char * nickname )
{
	std::string room_jid = room;
	std::string room_name = nickname;
	std::string my_name = m_username;

	GLXPlayerChatRoom * chat_room = m_chat_room_list->Find( room );

	if ( NULL == chat_room )
	{
		chat_room = XP_API_NEW GLXPlayerChatRoom( m_xp_chat, room_jid, room_name, my_name );
		m_chat_room_list->Add( room, chat_room );
	}

	if ( m_conference_index < 0 )
	{
		m_conference_index = 0;
	}
}

/**
 * This method will be invoked after a participant enter or leave the conference.
 *
 * @param
 *	char * room_jid:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
 *	char * user_name:		the nickname of the participant, such as "username"
 *	int state:			
 *
 */
void GameStateOnline::OnChatRoomPresence( const char * room_jid, const char * user_name, int state )
{
	GLXPlayerChatRoom * room = m_chat_room_list->Find( room_jid );
	if ( NULL != room )
	{
		room->OnParticipantPresence( user_name, state );
	}
}

/**
 * If you had sent a request to invite some guy join your confercence, 
 * this method will be invoked after the guy accept or reject your invitation.
 *
 * @param
 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
 *	char * invitor:		the jid of the sender, such as "username@gameloft.com"
 *	char * reason:		the appended information with the invitation, such as "come on"
 *
 */
void GameStateOnline::OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason )
{

}

void GameStateOnline::OnChatRoomConfigResult( const char * room, bool success )
{
	SetOnlineSubState(ONLINE_CHAT_CONFERENCE);
}

bool GameStateOnline::HandleEventOnlineChatMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_chatMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MENU);
		bHandled = true;
	}	
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineChatWithFriend( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		m_currentFriendIndex = m_selectedIndex;
		
		XP_API_MEMSET( m_chat_send_data, 0, 256 );
		XP_API_MEMSET( m_chat_data, 0, 1024 );
		
		XP_API_MEMSET( m_chat_friend_name, 0, 64 );		
		XP_API_STRCPY( m_chat_friend_name, m_GLXPlayerUserFriend->GetName(m_currentFriendIndex) );
		
		XP_API_MEMSET( m_chat_friend_number, 0, 64 );		
		XP_API_STRCPY( m_chat_friend_number, m_GLXPlayerUserFriend->GetUserNumber(m_currentFriendIndex) );
		
		SetOnlineSubState(ONLINE_CHAT_SCREEN);
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_CHAT_MENU);
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{					
		if ( m_GLXPlayerUserFriend->GetFriendsCount() > 0 )
		{
			m_selectedIndex --;		
			if( m_selectedIndex < 0 )
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
			if( m_selectedIndex == m_GLXPlayerUserFriend->GetFriendsCount() )
			{	
				m_selectedIndex = 0;			
			}
			bHandled = true;
		}		
	}

	return bHandled;
}

bool GameStateOnline::HandleEventOnlineChatConferenceList( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		// Create a new conference
		XP_API_MEMSET( m_conference_name, 0,  64 );
		XP_API_STRCPY( m_conference_name, m_username );
		SetOnlineSubState( ONLINE_CHAT_ADD_CONFERENCE );
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		// Join an existed conference
		m_chat_room = m_chat_room_list->Get( m_conference_index );
		if ( NULL != m_chat_room )
		{
			m_chat_room->Join();
			XP_API_MEMSET( m_conference_send_data, 0, 256 );
			XP_API_MEMSET( m_conference_data, 0, 1024 );
			SetOnlineSubState(ONLINE_CHAT_CONFERENCE);
		}
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid2.Contain(event.position) )
	{
		// Refresh all the existed conference
		m_chat_room_list->Clear();
		m_conference_index = -1;
		m_xp_chat->ListChatRoom();
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_CHAT_MENU);
		bHandled = true;
	}
	else if ( event.isClicked && m_UpKey.Contain(event.position) )
	{
		if ( m_chat_room_list->Count() > 1 )
		{
			m_conference_index --;
			if ( m_conference_index < 0 )
			{
				m_conference_index = m_chat_room_list->Count() - 1;
			}
			bHandled = true;
		}
	}
	else if ( event.isClicked && m_DownKey.Contain(event.position) )
	{
		if ( m_chat_room_list->Count() > 1 )
		{
			m_conference_index ++;
			if ( m_conference_index == m_chat_room_list->Count() )
			{
				m_conference_index = 0;
			}
			bHandled = true;
		}
	}

	return bHandled;
}

bool GameStateOnline::HandleEventOnlineChatConference( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if ( XP_API_STRLEN( m_conference_send_data ) > 0 )
		{
			m_chat_room->SendChatMessage( m_conference_send_data );
		}
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid2.Contain(event.position) )
	{
		XP_API_MEMSET( m_input_title, 0, 64 );
		XP_API_STRCPY( m_input_title, "--- Please input message ( max 256 ) ---" );
		XP_API_MEMSET( m_input, 0, 256 );
		XP_API_STRCPY( m_input, m_conference_send_data );
		m_InputBox->setInputBoxString( m_conference_send_data );
		SetOnlineSubState(ONLINE_INPUT);
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		m_conference_invite_index = 0;
		XP_API_MEMSET( m_conference_invite_name, 0, 64 );		
		XP_API_MEMSET( m_conference_invite_msg, 0, 64 );		
		SetOnlineSubState( ONLINE_CHAT_CONFERENCE_INVITE );
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_chat_room->Leave();
		SetOnlineSubState(ONLINE_CHAT_CONFERENCE_LIST);
		m_chat_room_list->Clear();
		m_conference_index = -1;
		m_xp_chat->ListChatRoom();
		bHandled = true;
	}
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineChatAddConference( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if ( XP_API_STRLEN( m_conference_name ) > 0 )
		{
			m_chat_room = m_xp_chat->CreateChatRoom( m_conference_name, m_username );
			m_chat_room_list->Add( m_chat_room->GetJid(), m_chat_room );
			XP_API_MEMSET( m_conference_send_data, 0, 256 );
			XP_API_MEMSET( m_conference_data, 0, 1024 );
			//SetOnlineSubState(ONLINE_CHAT_CONFERENCE);
			SetOnlineSubState(ONLINE_PROCESS);
			bHandled = true;
		}
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		XP_API_MEMSET( m_input_title, 0, 64 );
		XP_API_STRCPY( m_input_title, "--- Please input conference name ( max 64 ) ---" );
		XP_API_MEMSET( m_input, 0, 256 );
		XP_API_STRCPY( m_input, m_conference_name );
		m_InputBox->setInputBoxString( m_conference_name );
		SetOnlineSubState(ONLINE_INPUT);
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_CHAT_CONFERENCE_LIST);
		m_chat_room_list->Clear();
		m_conference_index = -1;
		m_xp_chat->ListChatRoom();
		bHandled = true;
	}
	return bHandled;
}

void GameStateOnline::DrawInput( GameStateHandler * handler )
{
	m_handler->ClearScreen();
	m_InputBox->showTextBox( m_handler );
	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	m_handler->DrawString(x,y,m_input_title);
	y += 2*DEF::LINE_HEIGHT;
	
	m_handler->DrawString(x,y,m_input);
	y += DEF::LINE_HEIGHT;

	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Done");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

bool GameStateOnline::HandleEventOnlineInput( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if ( m_pre_sub_state == ONLINE_CHAT_ADD_CONFERENCE )
		{
			XP_API_MEMSET( m_conference_name, 0, 64 );
			if ( NULL != m_InputBox->getInputBoxString() )
			{
				XP_API_STRCPY( m_conference_name, m_InputBox->getInputBoxString() );
			}
		}
		else if ( m_pre_sub_state == ONLINE_CHAT_CONFERENCE )
		{
			XP_API_MEMSET( m_conference_send_data, 0, 256 );
			if ( NULL != m_InputBox->getInputBoxString() )
			{
				XP_API_STRCPY( m_conference_send_data, m_InputBox->getInputBoxString() );
			}
		}
		else if ( m_pre_sub_state == ONLINE_CHAT_CONFERENCE_INVITE )
		{
			if ( m_conference_invite_index == 0 )
			{
				XP_API_MEMSET( m_conference_invite_name, 0, 64 );
				if ( NULL != m_InputBox->getInputBoxString() )
				{
					XP_API_STRCPY( m_conference_invite_name, m_InputBox->getInputBoxString() );
				}
			}
			else if ( m_conference_invite_index == 1 )
			{
				XP_API_MEMSET( m_conference_invite_msg, 0, 64 );
				if ( NULL != m_InputBox->getInputBoxString() )
				{
					XP_API_STRCPY( m_conference_invite_msg, m_InputBox->getInputBoxString() );
				}
			}
		}
		else if ( m_pre_sub_state == ONLINE_CHAT_SCREEN )
		{
			XP_API_MEMSET( m_chat_send_data, 0, 256 );
			if ( NULL != m_InputBox->getInputBoxString() )
			{
				XP_API_STRCPY( m_chat_send_data, m_InputBox->getInputBoxString() );
			}		
		}
		else if ( m_pre_sub_state == ONLINE_MESSAGE_WRITE )
		{
			if ( m_message_write_index == 0 )
			{
				XP_API_MEMSET( m_message_receiver, 0, 64 );
				if ( NULL != m_InputBox->getInputBoxString() )
				{
					XP_API_STRCPY( m_message_receiver, m_InputBox->getInputBoxString() );
				}
			}
			else if ( m_message_write_index == 1 )
			{
				XP_API_MEMSET( m_message_title, 0, 64 );
				if ( NULL != m_InputBox->getInputBoxString() )
				{
					XP_API_STRCPY( m_message_title, m_InputBox->getInputBoxString() );
				}
			}
			else if ( m_message_write_index == 2 )
			{
				XP_API_MEMSET( m_message_content, 0, 64 );
				if ( NULL != m_InputBox->getInputBoxString() )
				{
					XP_API_STRCPY( m_message_content, m_InputBox->getInputBoxString() );
				}
			}
		}
		else if ( m_pre_sub_state == ONLINE_INVITE_PLAY )
		{
			XP_API_MEMSET( m_invite_play_username, 0, 64 );
			if ( NULL != m_InputBox->getInputBoxString() )
			{
				XP_API_STRCPY( m_invite_play_username, m_InputBox->getInputBoxString() );
			}			
		}

		m_sub_state = m_pre_sub_state;
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = m_pre_sub_state;
		bHandled = true;
	}
	else
	{
		bHandled = m_InputBox->updateInputBox(event);
		if ( bHandled )
		{
			XP_API_MEMSET( m_input, 0, 256 );
			XP_API_STRCPY( m_input, m_InputBox->getInputBoxString() );
		}		
	}
	return bHandled;
}

void GameStateOnline::DrawConference( GameStateHandler * handler )
{
	int x = 0;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };

	m_handler->ClearScreen();

	m_handler->DrawString(x,y,"## User List ##");
	y += DEF::LINE_HEIGHT;

	for ( int i=0; i<m_chat_room->GetParticipantCount(); i++ )
	{
		GLXPlayerChatRoomParticipant * participant = m_chat_room->GetParticipant( i );
		XP_API_MEMSET( line, 0, 128 );
		switch( participant->state )
		{
		case GL_XPLAYER_PARTICIPANT_ONLINE:
			XP_API_SPRINTF( line, "%d %s <online>", i+1, participant->name );
			break;
		case GL_XPLAYER_PARTICIPANT_CHAT:
			XP_API_SPRINTF( line, "%d %s <chat>", i+1, participant->name );
			break;
		case GL_XPLAYER_PARTICIPANT_AWAY:
			XP_API_SPRINTF( line, "%d %s <away>", i+1, participant->name );
			break;
		case GL_XPLAYER_PARTICIPANT_BUSY:
			XP_API_SPRINTF( line, "%d %s <busyy>", i+1, participant->name );
			break;
		default:
			XP_API_SPRINTF( line, "%d %s", i+1, participant->name );
			break;
		}		
		m_handler->DrawString(x,y,line);
		y += DEF::LINE_HEIGHT;
	}
	
	y += DEF::LINE_HEIGHT;
	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "## Chat: <%s> ##", m_conference_send_data );
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;

	y += DEF::LINE_HEIGHT;
	m_handler->DrawString(x,y,"## Conference Message ##");
	y += DEF::LINE_HEIGHT;		

	char * h = m_conference_data;
	int length = XP_API_STRLEN( h );
	char * p = XP_API_STRSTR( h, "\n" );
	while( NULL != p )
	{
		XP_API_MEMSET( line, 0, 128 );
		XP_API_MEMCPY( line, h, p-h );
		m_handler->DrawString(x,y,line);
		y += DEF::LINE_HEIGHT;
		h = p + 1;
		if ( (h - m_conference_data) >= length )
		{
			break;
		}
		p = XP_API_STRSTR( h, "\n" );
	}

	if ( (h - m_conference_data) < length )
	{
		m_handler->DrawString(x,y,h);
	}

	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawButton(m_SoftMid2);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Send");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Invite");
	m_handler->DrawString(m_SoftMid2.left,m_SoftMid2.top,"Edit");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

void GameStateOnline::DrawCreateConference( GameStateHandler * handler )
{
	int x = 10;
	int y = 20;
	char line[128];

	m_handler->ClearScreen();

	m_handler->DrawString(x,y,"## Create Conference ##");
	y += 2*DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "Conference name: %s", m_conference_name );
	m_handler->DrawString(x,y,line);
	y += 10*DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "** <Conference name> must not be null **" );
	m_handler->DrawString(x,y,line);
	
	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Create");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Edit");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

void GameStateOnline::DrawConferenceInvitation( GameStateHandler * handler )
{
	int x = 10;
	int y = 20;
	char line[128];

	m_handler->ClearScreen();

	m_handler->DrawString(x,y,"## Conference Invitation ##");
	y += 2*DEF::LINE_HEIGHT;

	m_handler->DrawString(0,y+m_conference_invite_index*2*DEF::LINE_HEIGHT,">");

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "Username: %s", m_conference_invite_name );
	m_handler->DrawString(x,y,line);
	y += 2*DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "Message: %s", m_conference_invite_msg );
	m_handler->DrawString(x,y,line);
	y += 8*DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "** <Username> must not be null **" );
	m_handler->DrawString(x,y,line);
	
	m_handler->DrawButton(m_UpKey);
	m_handler->DrawButton(m_DownKey);
	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_UpKey.left,m_UpKey.top,"Up");
	m_handler->DrawString(m_DownKey.left,m_DownKey.top,"Down");
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Invite");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Edit");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

bool GameStateOnline::HandleEventOnlineChatConferenceInvitation( _TOUCH_EVENT event )
{
	if ( event.isClicked &&  m_SoftLeft.Contain(event.position) )
	{
		if ( XP_API_STRLEN( m_conference_invite_name ) == 0 )
		{
			return false;
		}
		m_chat_room->Invite( m_conference_invite_name, m_conference_invite_msg );
		m_sub_state = ONLINE_CHAT_CONFERENCE;
		return true;
	}
	else if ( event.isClicked &&  m_SoftMid.Contain(event.position) )
	{
		if ( m_conference_invite_index == 0 )
		{
			XP_API_MEMSET( m_input_title, 0, 64 );
			XP_API_STRCPY( m_input_title, "--- Please input user name ( max 64 ) ---" );
			XP_API_MEMSET( m_input, 0, 256 );
			XP_API_STRCPY( m_input, m_conference_invite_name );
			m_InputBox->setInputBoxString( m_conference_invite_name );
		}
		else if ( m_conference_invite_index == 1 )
		{
			XP_API_MEMSET( m_input_title, 0, 64 );
			XP_API_STRCPY( m_input_title, "--- Please input message ( max 64 ) ---" );
			XP_API_MEMSET( m_input, 0, 256 );
			XP_API_STRCPY( m_input, m_conference_invite_msg );
			m_InputBox->setInputBoxString( m_conference_invite_msg );
		}
		SetOnlineSubState(ONLINE_INPUT);
		return true;
	}
	else if ( event.isClicked &&  m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_CHAT_CONFERENCE;
		return true;
	}
	else if ( event.isClicked &&  m_UpKey.Contain(event.position) )
	{
		m_conference_invite_index --;
		if ( m_conference_invite_index < 0 )
		{
			m_conference_invite_index = 1;
		}
		return true;
	}
	else if ( event.isClicked &&  m_DownKey.Contain(event.position) )
	{
		m_conference_invite_index ++;
		if ( m_conference_invite_index == 2 )
		{
			m_conference_invite_index = 0;
		}
		return true;
	}
	return false;
}

void GameStateOnline::DrawChatScreen( GameStateHandler * handler )
{
	int x = 10;
	int y = 10;
	char line[128] = { 0 };

	m_handler->ClearScreen();

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF( line, "## Send Message To <%s> ##", m_chat_friend_name);
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;
	m_handler->DrawString(x,y,m_chat_send_data);
	y += 2*DEF::LINE_HEIGHT;

	m_handler->DrawString(x,y,"## Chat Message ##");
	y += DEF::LINE_HEIGHT;

	char * h = m_chat_data;
	int length = XP_API_STRLEN( h );
	char * p = XP_API_STRSTR( h, "\n" );
	while( NULL != p )
	{
		XP_API_MEMSET( line, 0, 128 );
		XP_API_MEMCPY( line, h, p-h );
		m_handler->DrawString(x,y,line);
		y += DEF::LINE_HEIGHT;
		h = p + 1;
		if ( (h - m_chat_data) >= length )
		{
			break;
		}
		p = XP_API_STRSTR( h, "\n" );
	}

	if ( (h - m_chat_data) < length )
	{
		m_handler->DrawString(x,y,h);
	}

	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Send");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Edit");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");
}

bool GameStateOnline::HandleEventOnlineChatScreen( _TOUCH_EVENT event )
{
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if ( XP_API_STRLEN( m_chat_send_data ) == 0 )
		{
			return false;
		}

		char temp[1024] = { 0 };
		XP_API_MEMSET( temp, 0 , 1024 );
		XP_API_STRCPY( temp, m_chat_data );
		XP_API_MEMSET( m_chat_data, 0 , 1024 );
		XP_API_SPRINTF( m_chat_data, "<%s>:%s\n%s", m_username, m_chat_send_data, temp );
		if ( XP_API_STRLEN( m_chat_friend_name ) > 0 )
		{
			m_xp_chat->SendChatMessage( m_chat_friend_name, m_chat_send_data );
		}
		return true;
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		XP_API_MEMSET( m_input_title, 0, 64 );
		XP_API_STRCPY( m_input_title, "--- Please input message ( max 256 ) ---" );
		XP_API_MEMSET( m_input, 0, 256 );
		XP_API_STRCPY( m_input, m_chat_send_data );
		m_InputBox->setInputBoxString( m_chat_send_data );
		SetOnlineSubState(ONLINE_INPUT);
		return true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_MENU;
		m_xp_chat->Logout();
		return true;
	}

	return false;
}

void GameStateOnline::DrawConferenceBeInvited( GameStateHandler * handler )
{
	int x = 10;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };

	m_handler->ClearScreen();
	m_handler->DrawString(x,y,"## Conference Invitation ##");
	y += 2*DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF(line,"<%s> invites you to join a conference.",m_chat_room_invitor);
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF(line,"Message <%s>",m_chat_room_invite_reason);
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF(line,"Password <%s>",m_chat_room_invite_password);
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;

	XP_API_MEMSET( line, 0, 128 );
	XP_API_SPRINTF(line,"RoomJID <%s>",m_chat_room_jid );
	m_handler->DrawString(x,y,line);
	y += DEF::LINE_HEIGHT;

	m_handler->DrawButton(m_SoftLeft);
	m_handler->DrawButton(m_SoftMid);
	m_handler->DrawButton(m_SoftRight);
	m_handler->DrawString(m_SoftLeft.left,m_SoftLeft.top,"Accept");
	m_handler->DrawString(m_SoftMid.left,m_SoftMid.top,"Reject");
	m_handler->DrawString(m_SoftRight.left,m_SoftRight.top,"Back");	
}

bool GameStateOnline::HandleEventOnlineChatBeInvited( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		// Accept and enter a new conference
		// m_chat_room will be used to send message
		m_chat_room = m_xp_chat->AcceptRoomInvitation( m_chat_room_jid, m_username );
		m_chat_room_list->Add( m_chat_room->GetJid(), m_chat_room );
		XP_API_MEMSET( m_conference_send_data, 0, 256 );
		XP_API_MEMSET( m_conference_data, 0, 1024 );
		SetOnlineSubState(ONLINE_CHAT_CONFERENCE);
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		// Reject a conference
		m_xp_chat->RejectRoomInvitation( m_chat_room_jid, m_chat_room_invitor, m_username );
		m_sub_state = m_pre_sub_state;
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = m_pre_sub_state;
		bHandled = true;
	}
	return bHandled;
}

void GameStateOnline::DrawMessageMenu( GameStateHandler * handler )
{
	handler->ClearScreen();
	handler->DrawString( DEF::START_POS_X, DEF::START_POS_Y, "--- Message ---" );
	m_messageMenu->Repaint( handler );
	DrawCommandBar(m_handler,GameState::CB_GO_BACK);
}

void GameStateOnline::DrawMessageInbox( GameStateHandler * handler )
{
	handler->ClearScreen();
	DrawCommandBar(m_handler,GameState::CB_REMOVE_BACK);
	
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;
	char line[128] = { 0 };
	
	handler->DrawString(x,y,"--- Inbox ---");
	y += 2*DEF::LINE_HEIGHT;	
	
	int total = 0;
	if((m_allMsgCount % 5) == 0 )
	{
		total = (m_allMsgCount / 5);
	}
	else
	{
		total = (m_allMsgCount / 5) + 1;
	}
	
	if(total == 0)
	{
		total = 1;
	}
	
	XP_API_SPRINTF(line, "Total: %d     Page: %d / %d", m_allMsgCount, m_currentMsgPage + 1, total);
	handler->DrawString(x,y,line);
	y += 2*DEF::LINE_HEIGHT;			
	
	
	for(int i=0; i < m_msgInPageCount; i++) 
	{
		if(m_selectedIndex == i)
		{
			handler->DrawString(x - 20 , y, ">");
		}
		XP_API_MEMSET(line, 0, 128);
		if ( m_GLXPlayerMessage->getMsgReadState(i) )
		{
			XP_API_SPRINTF( line, "%d %d %s <%s>", i+1 , m_GLXPlayerMessage->getMsgID(i), m_GLXPlayerMessage->getMsgTitle(i),  m_GLXPlayerMessage->getMsgSender(i));
		}
		else
		{
			XP_API_SPRINTF( line, "%d %d %s <%s> <Unread>", i+1 , m_GLXPlayerMessage->getMsgID(i), m_GLXPlayerMessage->getMsgTitle(i),  m_GLXPlayerMessage->getMsgSender(i));
		}
		handler->DrawString(x,y,line);
		
		y += DEF::LINE_HEIGHT;
	}
	
}

bool GameStateOnline::HandleEventOnlineMessageMenu( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( m_messageMenu->HandleEvent( event ) )
	{
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_MENU;
		bHandled = true;
	}
	return bHandled;
}

bool GameStateOnline::HandleEventOnlineMessageInbox( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		if(m_selectedIndex > -1)
		{
			m_isInbox = true;
			m_GLXPlayerMessage->SendReadMessage(m_GLXPlayerMessage->getMsgID(5*m_currentMsgPage + m_selectedIndex));
			SetOnlineSubState(ONLINE_PROCESS);
		}
		bHandled = true;
	}
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		SetOnlineSubState(ONLINE_MESSAGE_MENU);
		m_isInbox = false;
		bHandled = true;
	}
	
	else if ( event.isClicked && m_SoftMid.Contain(event.position) )
	{
		if(m_selectedIndex >= 0)
		{
			m_GLXPlayerMessage->SendDeleteMessage(m_GLXPlayerMessage->getMsgID(5*m_currentMsgPage + m_selectedIndex));
			SetOnlineSubState(ONLINE_PROCESS);
		}		
		bHandled = true;
	}
	else if ( event.isClicked &&  m_UpKey.Contain(event.position) )
	{
		m_selectedIndex --;		
		
		if(m_allMsgCount <= 5)
		{	
			if(m_selectedIndex < 0)
			{
				m_selectedIndex = m_allMsgCount;
			}
		}
		else
		{
			if(m_selectedIndex < 0)
			{
				m_currentMsgPage --;
				if(m_currentMsgPage < 0)
				{
					if((m_allMsgCount%5) == 0)
					{
						m_currentMsgPage = m_allMsgCount/5 - 1;
						m_msgInPageCount = 5;
					}
					else
					{
						m_currentMsgPage = m_allMsgCount/5;
						m_msgInPageCount = m_allMsgCount%5;
					}
				}
				else
				{
					m_msgInPageCount = 5;
				}		
				
				m_selectedIndex = m_msgInPageCount - 1;
				m_GLXPlayerMessage->SendGetMessageHeader(
					USER_MESSAGE_TYPE_USERMESSAGE | USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION | 
					USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION | USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, 
					USER_MESSAGE_READTYPE_ALL, 5*m_currentMsgPage, m_msgInPageCount);
				SetOnlineSubState(ONLINE_PROCESS);
			}
		}
		
	
	}
	else if ( event.isClicked &&  m_DownKey.Contain(event.position) )
	{
		m_selectedIndex ++;
			
		if(m_allMsgCount <= 5)
		{
			if(m_selectedIndex > m_allMsgCount)
			{
				m_selectedIndex = 0;
			}
		}
		else
		{				
			if(m_currentMsgPage == m_allMsgCount/5)
			{
				if(m_selectedIndex > m_msgInPageCount - 1)
				{
					m_currentMsgPage = 0;
					m_selectedIndex = 0;
					
					m_msgInPageCount = 5;
					m_GLXPlayerMessage->SendGetMessageHeader(
						USER_MESSAGE_TYPE_USERMESSAGE | USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION | 
						USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION | USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, 
						USER_MESSAGE_READTYPE_ALL, 5*m_currentMsgPage, m_msgInPageCount);
					SetOnlineSubState(ONLINE_PROCESS);
				}
			}	
			else					
			{	
				if(m_selectedIndex > 4)
				{
					 if((m_currentMsgPage == m_allMsgCount/5 - 1))
					 {
						if(m_allMsgCount%5 != 0)
						{							
							m_currentMsgPage ++;
							m_selectedIndex = 0;						
							m_msgInPageCount =  m_allMsgCount%5;								
						}
						else
						{
							m_currentMsgPage = 0;
							m_selectedIndex = 0;						
							m_msgInPageCount =  5;								
						}
					 }
					else
					{						
						m_currentMsgPage ++;
						m_selectedIndex = 0;
						m_msgInPageCount = 5;						
					}

				m_GLXPlayerMessage->SendGetMessageHeader(
					USER_MESSAGE_TYPE_USERMESSAGE | USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION | 
					USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION | USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, 
					USER_MESSAGE_READTYPE_ALL, 5*m_currentMsgPage, m_msgInPageCount);
				SetOnlineSubState(ONLINE_PROCESS);
				}			
			}
		}
	}
	return bHandled;
}

void GameStateOnline::DrawInviteToPlayGame( GameStateHandler * handler )
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

void GameStateOnline::DrawInviteChat( GameStateHandler * handler )
{
	int x = DEF::START_POS_X;
	int y = DEF::START_POS_Y;

	handler->ClearScreen();
	handler->DrawString( x, y, "--- Invite to chat ---" );
	y += 2 * DEF::LINE_HEIGHT;
	
	char tmp[128] = {0};
	
	XP_API_SPRINTF( tmp, "Send chat invitation to: %s?", m_chat_friend_name );	
	handler->DrawString( x, y, tmp );	

	DrawCommandBar(m_handler,GameState::CB_GO_BACK);	
}

void GameStateOnline::DrawSession(GameStateHandler * handler)
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

void GameStateOnline::DrawSessionList(GameStateHandler * handler)
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

void GameStateOnline::DrawInGame(GameStateHandler * handler)
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

void GameStateOnline::DrawMPEndGame(GameStateHandler * handler)
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

void GameStateOnline::DrawEvaluatePlayer(GameStateHandler * handler)
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

bool GameStateOnline::HandleEventOnlineInviteToPlayGame( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		char message[256] = { 0 };

		XP_API_MEMSET( message, 0, 256 );
		XP_API_SPRINTF( message, "%s invite you to play game.", m_username );
		m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION;
	#ifdef USE_USER_NUMBER
		m_GLXPlayerMessage->SendOnlineMessage( m_invite_play_usernumber, m_currentMsgType, "Invite to play game", message, XP_API_STRLEN( message ), true );
	#else
		m_GLXPlayerMessage->SendOnlineMessage( m_invite_play_username, m_currentMsgType, "Invite to play game", message, XP_API_STRLEN( message ) );
	#endif
		SetOnlineSubState(ONLINE_PROCESS);
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_CURRENT_FRIEND_MENU;
		bHandled = true;
	}
	return bHandled;
}

bool GameStateOnline::HandleEventInviteChat( _TOUCH_EVENT event )
{
	bool bHandled = false;
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{
		char message[256] = { 0 };

		XP_API_MEMSET( message, 0, 256 );
		XP_API_SPRINTF( message, "%s invite you to chat.", m_username );
		m_currentMsgType = USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION;
	#ifdef USE_USER_NUMBER
		m_GLXPlayerMessage->SendOnlineMessage( m_chat_friend_number, m_currentMsgType,"Invite to chat", message, XP_API_STRLEN( message ) );
	#else
		m_GLXPlayerMessage->SendOnlineMessage( m_chat_friend_name, m_currentMsgType,"Invite to chat", message, XP_API_STRLEN( message ) );
	#endif // USE_USER_NUMBER	
		SetOnlineSubState(ONLINE_PROCESS);
		bHandled = true;
	}	
	else if ( event.isClicked && m_SoftRight.Contain(event.position) )
	{
		m_sub_state = ONLINE_CURRENT_FRIEND_MENU;
		bHandled = true;
	}
	return bHandled;
}

bool GameStateOnline::HandleEventSession( _TOUCH_EVENT event )
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
		m_numberPlayer = count -1;
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
				#ifdef USE_USER_NUMBER
					m_GLXPlayerUserFriend->sendAddUserFriend(receiver, NULL, false);
				#else
					m_GLXPlayerUserFriend->sendAddUserFriend(receiver);
				#endif // USE_USER_NUMBER
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
//			//SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
//			m_GLXPlayerMultiPlayer->mpSendDisconnect();
//			SetOnlineSubState(ONLINE_MP_PROCESS);
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

bool GameStateOnline::HandleEventSessionList( _TOUCH_EVENT event )
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
		m_GLXPlayerMultiPlayer->mpSendDisconnect();
		SetOnlineSubState(ONLINE_MP_PROCESS);

		m_selectedIndex = -1;
		bHandled = true;
	}	
	else if( event.isClicked && m_UpKey.Contain(event.position) )
	{
		m_selectedIndex--;
		
		if (m_selectedIndex < 0)
		{
			m_selectedIndex = count - 1;
		}
		
		bHandled = true;
					
	}
	else if( event.isClicked && m_DownKey.Contain(event.position) )
	{		
		m_selectedIndex++;

		if (m_selectedIndex > count - 1)
		{
			m_selectedIndex = 0;
		}
	
		bHandled = true;
				
	}
	return bHandled;
}

bool GameStateOnline::HandleEventInGame( _TOUCH_EVENT event )
{
	bool bHandled = false;

	//if(!m_GLXPlayerMultiPlayer->mpIsInGame())
	//{			
	//	//SetOnlineSubState(ONLINE_MP_MENU_ONLINE);
	//	m_GLXPlayerMultiPlayer->mpSendDisconnect();
	//	SetOnlineSubState(ONLINE_MP_PROCESS);
	//	bHandled = true;
	//}
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		m_GLXPlayerMultiPlayer->mpSendFinishGame();
		//SetOnlineSubState(ONLINE_MP_PROCESS);
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
		if(m_isInvitor)
		{
			m_GLXPlayerMultiPlayer->mpClearRoomData();
			m_playInvitedGame = false;
			m_isInvitor = false;
			SetOnlineSubState(ONLINE_LIST_FRIENDS);
		}
#else
		m_GLXPlayerMultiPlayer->mpSendLeaveSession();
		SetOnlineSubState(ONLINE_MP_PROCESS);
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

bool GameStateOnline::HandleEventMPEndGame( _TOUCH_EVENT event )
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
		if(m_isInvitor)
		{
			m_GLXPlayerMultiPlayer->mpClearRoomData();
			m_playInvitedGame = false;
			m_isInvitor = false;
			SetOnlineSubState(ONLINE_LIST_FRIENDS);
		}
#else
		//m_GLXPlayerMultiPlayer->mpSendLeaveSession();
		//SetOnlineSubState(ONLINE_PROCESS);

		m_GLXPlayerMultiPlayer->mpSendDisconnect();
		SetOnlineSubState(ONLINE_MP_PROCESS);
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

bool GameStateOnline::HandleEventEvaluatePlayer( _TOUCH_EVENT event )
{
	bool bHandled = false;	
	
	if ( event.isClicked && m_SoftLeft.Contain(event.position) )
	{		
		if(m_selectedIndex == 0)
		{
			m_gl_xplayer_user->sendPointUserReputation(m_playerName[m_currentUserIndex], 5);
			SetOnlineSubState(ONLINE_PROCESS);
		}
		else if(m_selectedIndex == 1)
		{
			m_gl_xplayer_user->sendPointUserReputation(m_playerName[m_currentUserIndex], 0);
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
