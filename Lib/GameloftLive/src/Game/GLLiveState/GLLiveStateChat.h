#pragma once
#include "GLLiveState.h"
#include "GLXPlayerChatRoomList.h"

#define MAX_INVITATION_STACK         50

//#ifdef IPHONE
//#import <UIKit/UIKit.h>
//
//@synthesize textView;
//@synthesize textField;
//#endif

using namespace std;

class AvatarInfo;

class GLLiveStateChat :
	public GLLiveState//, public GLXPlayerChatObserver
{
public:
	GLLiveStateChat( GLLiveStateHandler * handler, GLXPlayerChatRoom *room = NULL);
	virtual ~GLLiveStateChat(void);

	enum ChatSubState
	{	
		CHAT_NONE = 0,
		CHAT_CHAT,
		CHAT_PROCESS,	
		CHAT_SETCOLOR,
		CHAT_EXIT_CONFIRM,
		CHAT_QUIT_ROOM_CONFIRM,
		CHAT_ADD_FRIEND_CONFIRM,
		CHAT_ERROR
	};

////// Game State interface
	bool HandleEvent(_TOUCH_EVENT event);
	bool HandleEventChat(_TOUCH_EVENT event);
	bool HandleEventSetColor(_TOUCH_EVENT event);

	void Paint();
	void PaintChat();
	void PaintSetColor();

	void UpdateAnimation();
	void UpdateAnimationChat();
	void SwitchState();
	virtual DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_CHAT;}
	bool IsProcess(){return false;}

	void OnMenuItemClicked(int id);
//////  Chat observer
	void OnChatDisconnect( int e ); 

//////  ChatRoom Observer interface
	void OnChatRoomMessage( const char * room, const char * nickname, const char * message );
	void OnChatRoomPresence(const char *room_jid, const char *user_name, int state);
	void OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason );

// GLXPlayerObserver interface implement
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );
	void OnChatRoomConfigResult( const char * room, bool success );

// Other
	void AddChatRoom(GLXPlayerChatRoom *newRoom);
	bool IsInRoom(const char *room_jid);
	int  RoomCount();
private:

	void UpdateMobileHeight();
	const char *GetPosTalker(int posY);
	bool IsExist(const char *roomName);
	void CheckShowAvatar(const char *room_id, const char *newMember);

	struct MsgRecord
	{
		MsgRecord(const char *name, const char *msg, int col, bool isFrd) : from(name), content(msg), color(col), isFriend(isFrd)
		{
		}

		string from;
		string content;
		int    color;
		bool   isFriend;
	};

	typedef list<MsgRecord>            LIST_MSG;
	typedef list<MsgRecord>::iterator  IT_LIST_MSG;
	typedef map<string, LIST_MSG>      MAP_MSG_LIST;

	struct ChatRoomInfo
	{
		LIST_MSG	   m_msgList;
		ChatSubState   m_state;
		long           m_timer;
		bool           m_showAvatar;
		bool           m_isFriend;      //Valid only in 2 person
		CGLImage*      m_friendAvatar;

		ChatRoomInfo()
		{
			m_state = CHAT_CHAT;
			m_timer = -1;               //Not start
 			m_showAvatar = true;
			m_friendAvatar = NULL;
			m_isFriend = true;        
		}

		~ChatRoomInfo()
		{
			XP_API_DEL(m_friendAvatar);
		}
	};

	typedef map<string, ChatRoomInfo>   MAP_CHAT_ROOM_INFO;

	GLXPlayerChatRoom     * m_curRoom;
	GLXPlayerChatRoomList * m_chatRoomList;

	ChatSubState            m_sub_state;
	char                    m_inputMsg[256];

	MAP_CHAT_ROOM_INFO      m_roomInfoList;
	ChatRoomInfo          * m_curRoomInfo;
	bool                    m_curShowAvatar;


	int                     m_msgColor;
	int                     m_tempMsgColor;
	GLLiveMenu                  * m_menu;
	GLLiveMenu                  * m_setMenu;
	GLLiveMenu                  * m_confirmMenu;
	int                     m_pressFrames;

	bool                    m_gettingAvatar;               //Is downloading friend avatar from server
	ChatRoomInfo          * m_gettingAvatarRoom;           //The room whose avatar is downloading


	bool                    m_bShowingTab;
	int                     m_showedTab;

	//Scroll data
	int        m_offset;
	int        m_oldOffset;
	int        m_touchPos;         //Y-
	bool       m_moving;
	int        m_speed;
	int        m_mobileHeight;          //mobile height
	float      m_scrollBarHeight;

	bool       m_newChat;
	char      *m_newFriend;
	char       m_addTalkFriend[MAX_USERNAME];

	bool       m_friendUpdated;
	bool       m_friendUpdating;
	bool       m_isPressingInputArea;
	bool       m_isPressingTab;
	bool       m_isInviteFriend;
//#ifdef IPHONE
//	UITextField *m_inputPad;
//#endif
};

class GLLiveStateChatInvitation : public GLLiveState
{
public:
	GLLiveStateChatInvitation(GLLiveStateHandler * handler, GLLiveState *pLastState);
	virtual ~GLLiveStateChatInvitation();
	
	//Game state interface
	void Paint();
	bool HandleEvent( _TOUCH_EVENT event );
    void SwitchState();
	void UpdateAnimation();
	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_CHAT_INVITATION;}
	bool IsProcess(){return false;}
	void OnMenuItemClicked( int id );
	bool PushInvitation(const char *room, const char *invitor, const char *reason);
	void PopInvitation();

// GLXPlayerObserver interface implement
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestSuccess( int func_id, char * data, int data_len );
	void OnRequestFailure( int func_id, int error_code );
	void SetLastState(GLLiveState *pLastState);
	
private:
	GLLiveState *m_lastState;

	GLLiveMenu      *m_menu;

	struct InvitorInfo
	{
		InvitorInfo(const char *_room, const char *_invitor, const char *_reason)
		{
			m_room = XP_API_STRNEW(_room);
			m_invitor = XP_API_STRNEW(_invitor);
			m_reason = XP_API_STRNEW(_reason);
		}
		~InvitorInfo()
		{
			XP_API_DEL(m_room);
			XP_API_DEL(m_invitor);
			XP_API_DEL(m_reason);
		}
		char *m_room;
		char *m_invitor;
		char *m_reason;
	};

	//Invitation stack
	InvitorInfo *m_invStack[MAX_INVITATION_STACK];
	int m_top;

	bool   m_isLogout;
	bool   m_acceptFailed;
};
