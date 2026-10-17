#ifndef __GAME_STATE_CHAT_H__
#define __GAME_STATE_CHAT_H__

#include "gamestate.h"
#include "Online/Porting.h"
#include "Online/Chat/GLXPlayerChat.h"
#include "Online/Chat/GLXPlayerChatObserver.h"
#include "Online/Chat/GLXPlayerChatRosterItem.h"
#include "Online/Chat/GLXPlayerChatRoster.h"
#include "Online/Chat/GLXPlayerChatRoom.h"
#include "Online/Chat/GLXPlayerChatRoomList.h"
#include "Online/Chat/GLXPlayerChatRoomObserver.h"
#include "InputBox.h"

class GameStateChat :
	public GameState, GLXPlayerChatObserver, GLXPlayerChatRoomObserver
{
public:	

	GameStateChat( GameStateHandler * handler );

	~GameStateChat(void);

	enum ChatSubState
	{
		CHAT_STATE_WAITING,
		CHAT_STATE_MAIN,
		CHAT_STATE_CHAT,
		CHAT_STATE_BUDDY,
		CHAT_STATE_BUDDY_REQUEST,
		CHAT_STATE_ROOM_INVITATION,
		CHAT_STATE_ROOM_CHAT,
		CHAT_STATE_ROOM_MENU,
		CHAT_STATE_INPUT,
		CHAT_STATE_LOGIN,
		CHAT_STATE_BUDDY_ADD,
		CHAT_STATE_BUDDY_DEL,
		CHAT_STATE_ROOM_ADD,
		CHAT_STATE_ROOM_INVITE
	};

// GameState

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

    void Paint();

	void UpdateAnimation();

// GLOnlineChatObserver

	void OnChatConnected();

	void OnChatDisconnect( int e );

	void OnChatRoster( const char * jid, const char * name, const char * group, int state );

	void OnChatRosterPresence( const char * jid, const char * name, const char * group, int state );

	void OnChatSession( char * from );

	void OnChatMessage( char * from, char * message );

	void OnChatAddBuddy( const char * jid );

	void OnChatRemoveBuddy( const char * jid );

	void OnChatBuddyRequest( const char * jid, const char * message );

	void OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
		const char * password );

	void OnChatRoomMessage( const char * room, const char * nickname, const char * message );

	void OnChatRoomFound( const char * room, const char * nickname );

	void OnChatRoomPresence( const char * room_jid, const char * user_name, int state );

	void OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason );

	void OnChatRoomConfigResult( const char * room, bool success );

private:

	void SetSubState( ChatSubState state );

private:

	DEF::GAME_STATE_ENUM m_state;

	GLXPlayerChat * m_gl_xplayer_chat;

	ChatSubState m_sub_state;
	char m_username[64];
	char m_password[64];
	char m_domain[64];
	char m_domain_conference[64];
	char m_server[64];
	int m_server_port;

	GLXPlayerChatRoster * m_roster;
	int m_roster_index;

	char m_session_jid[64];
	char m_data[1024];
	char m_chat_send_data[256];

	_TouchRect m_SoftBuddy;
	_TouchRect m_SoftRoom;
	_TouchRect m_SoftMove;

	char m_buddy_request_jid[64];
	char m_buddy_request_msg[64];

	char m_chat_room_jid[64];
	char m_chat_room_invitor[64];
	char m_room_invite_reason[64];
	char m_room_invite_password[64];

	GLXPlayerChatRoom * m_chat_room;
	GLXPlayerChatRoomList * m_chat_room_list;
	int m_conference_index;

	char m_conference_data[1024];
	int m_conference_user_index;

	int m_buddy_type_index;

	InputBox * m_InputBox;
	char m_input_title[64];
	char m_input[256];
	ChatSubState m_pre_sub_state;

	int m_login_index;

	int m_add_buddy_index;
	char m_add_buddy_jid[64];
	char m_add_buddy_nickname[64];
	char m_add_buddy_group[64];
	char m_add_buddy_message[64];

	char m_del_buddy_jid[64];

	char m_add_conference_name[64];
	char m_conference_send_data[256];

	int m_conference_invite_index;
	char m_conference_invite_jid[64];
	char m_conference_invite_msg[64];

	int m_buddy_request_index;
	char m_buddy_request_nickname[64];
	char m_buddy_request_groupname[64];
};

#endif