#ifndef __GAME_STATE_ONLINE_H__
#define __GAME_STATE_ONLINE_H__

#include "gamestate.h"
#include "../../include/Porting.h"
#include "../../include/User/GLXPlayerUser.h"
#include "../../include/Message/GLXPlayerMessage.h"
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Chat/GLXPlayerChat.h"
#include "../../include/Chat/GLXPlayerChatRoomList.h"
#include "../../include/Buddy/GLXPlayerUserFriend.h"
#include "../../include/Download/GLXPlayerDownload.h"
#include "../../include/YouTube/GLXPlayerYoutube.h"

//#include "GameStateChat.h"

#include "InputBox.h"
#include "Menu.h"

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
#include "../../include/Multiplayer/P2P/GLXPlayerMPbyP2P.h"
#else
#include "../../include/Multiplayer/ForwardbyServer/GLXPlayerMPByServer.h"
#endif

/*@ define the max size of session list*/
#define MAX_LIST_SIZE 5


class GameStateOnline :	public GameState, public MenuItemObserver, GLXPlayerObserver,
	GLXPlayerChatObserver, GLXPlayerChatRoomObserver, GLXPlayerMPBaseObserver
{
public:	

	GameStateOnline( GameStateHandler * handler );

	~GameStateOnline(void);

	/**
	 * Network is broken.
	 *
	 */
	void OnNetworkError();

	/**
	 * Timeout.
	 *
	 */
	void OnRequestTimeout( int func_id );

	/**
	 * The correct response.
	 *
	 */
	void OnRequestSuccess( int func_id, char * data, int data_len );

	/**
	 * The wrong response or the operation is failed on server side.
	 *
	 */
	void OnRequestFailure( int func_id, int error_code );

//============multiplayer observer================
	//Error
	void OnMPNetworkError(int error);
	void OnMPFunctionError( char* errMsg, int error_code );
	
	//Success
	void OnMPuPnPTestSuccess();
	void OnMPEstablishConnectionSuccess();
	void OnMPDisconnectSuccess();
	void OnMPLoginSuccess(DataPacket* currentPacket = NULL);
	void OnMPCreateSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPListSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPJoinSessionSuccess(DataPacket* currentPacket = NULL);
	void OnMPLeaveSessionSuccess();
	void OnMPLeaveGameSuccess();
	void OnMPStartGameSuccess(DataPacket* currentPacket = NULL);	
	void OnMPKickOutPlayerSuccess();
	void OnMPFinishGameSuccess();
//===================================================

	enum OnlineSubState
	{
		ONLINE_NONE = 0				,
		ONLINE_PROCESS				,	
		ONLINE_ERROR				,		
		ONLINE_MENU					,	
		
		ONLINE_LOGIN				,			
		ONLINE_REGISTER				,	
		ONLINE_PROFILE_MENU			,		
		ONLINE_USER_PROFILE			,		
		ONLINE_USER_GAME_LIST		,
		ONLINE_USER_STATE			,
		ONLINE_USER_REPUTATION		,
		ONLINE_USER_AVATAR			,
		ONLINE_USER_AVATAR_UPLOAD	,
		ONLINE_USER_DATA_MENU		,
		ONLINE_USER_DATA_VIEW		,		
		ONLINE_USER_DATA_UPLOAD		,	
		
		ONLINE_MSG_LIST,		
		ONLINE_MSG_INFO,
		ONLINE_MSG_READ,
		
		ONLINE_FRIEND_MENU			,		
		ONLINE_ADD_FRIEND			,		
		ONLINE_LIST_FRIENDS			,		
		ONLINE_CURRENT_FRIEND_MENU	,		
		ONLINE_SEND_MSG_TO_FRIEND	,			
		ONLINE_COMPARE_FRIEND_GAME	,
		ONLINE_INVITE_PLAY			,
		ONLINE_INVITE_CHAT			,
		
		ONLINE_CHAT_MENU			,
		ONLINE_CHAT_WITH_FRIEND		,
		ONLINE_CHAT_CONFERENCE_LIST	,
		ONLINE_CHAT_CONFERENCE		,
		ONLINE_CHAT_ADD_CONFERENCE	,
		ONLINE_INPUT				,
		ONLINE_CHAT_CONFERENCE_INVITE,
		ONLINE_CHAT_SCREEN			,
		ONLINE_CHAT_BE_INVITED		,
		ONLINE_MESSAGE_MENU			,
		ONLINE_MESSAGE_WRITE		,
		ONLINE_MESSAGE_INBOX		,

		ONLINE_MP_PROCESS,
		ONLINE_MP_IN_SESSION,	
		ONLINE_MP_IN_SESSION_LIST,	
		ONLINE_MP_IN_GAME,	
		ONLINE_MP_END_GAME,	
		ONLINE_MP_FINISH_GAME_CONFIRM,
		ONLINE_MP_EVALUATE_PLAYER,	
		ONLINE_MP_ERROR,
	};

// GameState

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

    void Paint();

	void UpdateAnimation();

	bool UpdateQuitAnimation();

	void BeginQuitAnimation();

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	void UpdateGame();
#endif

	void DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool drawUpDown = true );

	void SetOnlineSubState(OnlineSubState state);
	
	void DrawLogin( GameStateHandler * handler );
	void DrawRegister( GameStateHandler * handler );
	
	void DrawMsgList( GameStateHandler * handler);
	void DrawMessage( GameStateHandler * handler );
	void DrawMsgInfo( GameStateHandler * handler );
	
	void DrawError( GameStateHandler * handler );
	void DrawUserProfile( GameStateHandler * handler );
	void DrawAddFriend( GameStateHandler * handler );
	void DrawCompareGameWithFriend( GameStateHandler * handler );
	void DrawFriendList( GameStateHandler * handler );
	void DrawSendMsg( GameStateHandler * handler);
	
	void DrawWaiting( GameStateHandler * handler );	
	void DrawUserGameList( GameStateHandler * handler );	
	void DrawUserState( GameStateHandler * handler );
	void DrawUserReputation( GameStateHandler * handler );	
	void DrawUserDataView( GameStateHandler * handler );
	void DrawUserDataUpload( GameStateHandler * handler );
	
	bool HandleEventError( _TOUCH_EVENT event );
	bool HandleEventProcess( _TOUCH_EVENT event );
	
	bool HandleMsgList( _TOUCH_EVENT event);
	bool HandleViewMessage( _TOUCH_EVENT event );
	bool HandleMsgInfo( _TOUCH_EVENT event );
	
	bool HandleEventLogin( _TOUCH_EVENT event );
	bool HandleEventRegister( _TOUCH_EVENT event );		
	bool HandleEventOnlineMenu( _TOUCH_EVENT event);
	bool HandleEventOnlineProfileMenu( _TOUCH_EVENT event );			
	bool HandleEventOnlineUserProfile( _TOUCH_EVENT event );
	bool HandleEventOnlineFriendMenu( _TOUCH_EVENT event );	
	bool HandleEventOnlineAddFriend( _TOUCH_EVENT event );	
	bool HandleEventOnlineCompareGameWithFriend( _TOUCH_EVENT event );	
	bool HandleEventOnlineListFriend( _TOUCH_EVENT event );	
	bool HandleEventOnlineCurrentFriendMenu( _TOUCH_EVENT event );	
	bool HandleEventOnlineSendMsg( _TOUCH_EVENT event );	
	
	bool HandleEventUserGameList( _TOUCH_EVENT event );			
	bool HandleEventUserReputation( _TOUCH_EVENT event );	
	bool HandleEventDataMenu( _TOUCH_EVENT event );		
	
	bool HandleEventDataView( _TOUCH_EVENT event );	
	bool HandleEventDataUpload( _TOUCH_EVENT event );		
	
	void OnMenuItemClicked( char * manu_name );

//////
	/**
	 * The method will be invoked after the connection to the chat server was established successfully.
	 *
	 */
	void OnChatConnected();

	/**
	 * The method will be invoked if the connection to the chat server has been broken.
	 *
	 */
	void OnChatDisconnect( int e, int reason );

	/**
	 * After the login, 
	 *
	 */
    void OnChatRoster( const char * jid, const char * name, const char * group, int state );

	void OnChatRosterPresence( const char * jid, const char * name, const char * group, int state );

	void OnChatSession( char * from );

	/**
	 * The method will be invoked after a message was arrived.
	 * 
	 * @param
	 *	char * from:	the jid of the sender, such as "username@gameloft.com"
	 *	char * message:	the content of the message, such as "hello"
	 *
	 */
	void OnChatMessage( char * from, char * message );

	void OnChatAddBuddy( const char * jid );

	void OnChatRemoveBuddy( const char * jid );

	/**
	 * This method will be invoked after a user sent a buddy request to you.
	 *
	 * @param
	 *	char * jid:		the jid of the applicant, such as "username@gameloft.com"
	 *	char * message:	the appended information, such as "May I have a talk to you?"
	 *
	 */
	void OnChatBuddyRequest( const char * jid, const char * message );

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
	void OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
		const char * password );

	/**
	 * This method will be invoked after a conference message was arrived.
	 *
	 * @param
	 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 *	char * nickname:	the nickname of the sender
	 *	char * message:		the content of the message
	 *
	 */
	void OnChatRoomMessage( const char * room, const char * nickname, const char * message );

	/**
	 * This method will be invoked after a conference has been found.
	 *
	 * @param
	 *	char * room:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 *	char * nickname:	the nickname of the chat room
	 *
	 */
	void OnChatRoomFound( const char * room, const char * nickname );

	/**
	 * This method will be invoked after a participant enter or leave the conference.
	 *
	 * @param
	 *	char * room_jid:		the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 *	char * user_name:		the nickname of the participant, such as "username"
	 *	int state:			
	 *
	 */
	void OnChatRoomPresence( const char * room_jid, const char * user_name, int state );

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
	void OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason );

	void OnChatRoomConfigResult( const char * room, bool success );

private:	
	
	bool LoadProfile();
	bool SaveProfile();	

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	void SaveData( char * name, char * data )
	{
		XP_API_DEL( m_recv_user );
		XP_API_DEL( m_recv_data );

		m_recv_user = XP_API_STRNEW( name );
		m_recv_data = XP_API_STRNEW( data );
	}
#endif

	
	void SetUsername( char * username );
	void SetPassword( char * password );	
	
	void DrawChatRoomMenu( GameStateHandler * handler );
	void DrawCreateConference( GameStateHandler * handler );
	void DrawConference( GameStateHandler * handler );
	void DrawConferenceInvitation( GameStateHandler * handler );
	void DrawInput( GameStateHandler * handler );
	void DrawChatScreen( GameStateHandler * handler );
	void DrawConferenceBeInvited( GameStateHandler * handler );
	void DrawMessageMenu( GameStateHandler * handler );	
	void DrawMessageInbox( GameStateHandler * handler );
	void DrawInviteToPlayGame( GameStateHandler * handler );
	void DrawInviteChat( GameStateHandler * handler );
	void DrawSession(GameStateHandler * handler);
	void DrawSessionList(GameStateHandler * handler);
	void DrawInGame(GameStateHandler * handler);
	void DrawMPEndGame(GameStateHandler * handler);
	void DrawEvaluatePlayer(GameStateHandler * handler);

	bool HandleEventOnlineChatMenu( _TOUCH_EVENT event );
	bool HandleEventOnlineChatWithFriend( _TOUCH_EVENT event );
	bool HandleEventOnlineChatConferenceList( _TOUCH_EVENT event );
	bool HandleEventOnlineChatConference( _TOUCH_EVENT event );
	bool HandleEventOnlineChatAddConference( _TOUCH_EVENT event );
	bool HandleEventOnlineChatConferenceInvitation( _TOUCH_EVENT event );
	bool HandleEventOnlineChatScreen( _TOUCH_EVENT event );
	bool HandleEventOnlineChatBeInvited( _TOUCH_EVENT event );	
	bool HandleEventOnlineInput( _TOUCH_EVENT event );
	bool HandleEventOnlineMessageMenu( _TOUCH_EVENT event );	
	bool HandleEventOnlineMessageInbox( _TOUCH_EVENT event );
	bool HandleEventOnlineInviteToPlayGame( _TOUCH_EVENT event );
	bool HandleEventInviteChat( _TOUCH_EVENT event );
	bool HandleEventSession( _TOUCH_EVENT event );
	bool HandleEventSessionList( _TOUCH_EVENT event );
	bool HandleEventInGame( _TOUCH_EVENT event );
	bool HandleEventMPEndGame( _TOUCH_EVENT event );
	bool HandleEventEvaluatePlayer( _TOUCH_EVENT event );
	
	DEF::GAME_STATE_ENUM m_state;
	
	OnlineSubState m_sub_state;
	OnlineSubState m_pre_sub_state;
	OnlineSubState m_next_sub_state;

	///@ User informations
	char m_username[15];
	char m_password[32];
	char* m_nationData;
	char* m_email;
	bool m_emailFlag;
	char* m_avatar;
	int m_score;
	
	//Temp variable for function change password
	char * m_oldPassword;
	
	InputBox* m_InputBox;
	int m_selectedIndex;
	
	Menu * m_onlineMenu;	
	Menu * m_profileMenu;
	Menu * m_friendMenu;
	Menu * m_currentFriendMenu;
	Menu * m_dataCenterMenu;	
	Menu * m_chatMenu;
	Menu * m_messageMenu;

	int m_errorCode;	
	int m_frame;		
	
	///@Send message
	char* m_msgReceiver;
	char m_msgReceiverNumber[64];
	char* m_msgContent;
	char* m_msgTitle;	
	char* m_msgCC;	
	bool m_isUserNumber;
	
	//For download and upload data
	char* m_dataGroup;
	char* m_dataValue;

#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	GLXPlayerMPbyP2P * m_GLXPlayerMultiPlayer;
#else
	GLXPlayerMPbyServer * m_GLXPlayerMultiPlayer;
#endif
	GLXPlayerUser * m_gl_xplayer_user;
	GLXPlayerMessage * m_GLXPlayerMessage;	
	GLXPlayerLogin * m_xp_login;
	GLXPlayerChat * m_xp_chat;
	GLXPlayerChatRoom * m_chat_room;
	GLXPlayerChatRoomList * m_chat_room_list;
	GLXPlayerUserFriend * m_GLXPlayerUserFriend;
	GLXPlayerDownload *m_GLXPlayerDownload;
	GLXPlayerYoutube *m_GLXPlayerYoutube;

	int m_conference_index;
	
	_TouchRect m_SoftMid2;
	_TouchRect m_SoftAdd1;
	_TouchRect m_SoftAdd2;

	char m_conference_name[64];
	char m_conference_data[1024];
	char m_conference_send_data[256];
	char m_input_title[64];
	char m_input[256];
	int m_conference_invite_index;
	char m_conference_invite_name[64];
	char m_conference_invite_msg[64];
	
	char m_chat_friend_name[64];
	char m_chat_friend_number[64];
	
	char m_chat_data[1024];
	char m_chat_send_data[256];
	char m_chat_room_jid[64];
	char m_chat_room_invitor[64];
	char m_chat_room_invite_reason[64];
	char m_chat_room_invite_password[64];
	char m_message_receiver[64];
	char m_message_title[64];
	char m_message_content[64];
	char m_message_write_index;
	
	char m_invite_play_username[64];
	char m_invite_play_usernumber[64];
	
	int m_currentMsgType;
	
	int m_gameMsgCount;
	int m_userMsgCount;		
	int m_friendInvitedMsgCount;
	int m_gameInvitedMsgCount;	
	int m_chatInvitedMsgCount;
	int m_allMsgCount;
	
	bool m_isInbox;
	int m_currentMsgPage;
	int m_msgInPageCount;
	
	int m_currentFriendIndex;

	bool m_bGetMyTrophy;
	long m_startTime;
	bool m_playInvitedGame;
	bool m_isInvitor;

	
	int m_currentSessionIndex;
	int m_numberPlayer;
	char** m_playerName;
	int* m_playerScore;
	bool m_isWin;
	int m_currentUserIndex;

///@ Game data ... 
	int gameX;
	int gameY;
	int oppX;
	int oppY;
	int oldX;
	int oldY;	
	
#if(MULTIPLAYER_TYPE == MULTIPLAYER_P2P)
	char * m_recv_user;
	char * m_recv_data;

	int m_gamedata;
	int m_gamedata_old;
#endif
};

#endif