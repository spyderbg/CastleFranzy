#ifndef __GL_XPLAYER_CHAT_ROOM_H__
#define __GL_XPLAYER_CHAT_ROOM_H__

#include "GLXPlayerChat.h"
#include "gloox/MUCRoom.h"
#include <vector>

class GLXPlayerChat;

using namespace gloox;

/**
 * This structure is designed to represent a participant in a conference.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
typedef struct _GLXPlayerChatRoomParticipant
{
	/**
	 * The nickname of the participant.
	 *
	 */
	char * name;

	/**
	 * The network state of the participant,such as offline, on-line, and so on.
	 *
	 */
	int state;

}GLXPlayerChatRoomParticipant;

/**
 * This class is designed to represent a conference.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerChatRoom
{

public:

	/**
	 * Constructor.
	 *
	 * @param
	 *	GLXPlayerChat * pClient:	the jabber client
	 *	char * room_jid:			the jid of the conference, such as "chatroomname@conference.gameloft.com"
	 *	char * room_name:			the name of the conference, such as "chatroomname"
	 *	char * user_name:			the name of the user, such as "username"
	 *
	 */
	GLXPlayerChatRoom( GLXPlayerChat * pClient, std::string &room_jid, std::string &room_name, std::string &user_name );

	~GLXPlayerChatRoom(void);

	/**
	 * Join a conference.
	 *
	 */
	void Join();

	/**
	 * Leave a conference.
	 *
	 */
	void Leave();

	/**
	 * Invite the others to join the conference.
	 *
	 */
	void Invite( char * invitee, char * reason );

	/**
	 * Reject the request to add you into the conference.
	 *
	 * @param
	 *	char * room:	the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 *	char * invitor:	the jid of the user who sent the invitation request, such as "username@gameloft.com"
	 *	char * invitee:	the name of the user who received the invitation request, such as "lu1"
	 *
	 */
	void Decline( char * room, char * invitor, char * invitee );

	/**
	 * Send message to all the users in the conference.
	 *
	 * @param
	 *	char * message:	the content of the message
	 *
	 */
	bool SendChatMessage( char * message );

	char * GetName()
	{
		return m_name;
	}

	void SetName( char * name );

	char * GetJid()
	{
		return m_jid;
	}

	void OnParticipantPresence( const char * name, int state );

	int GetParticipantCount();

	GLXPlayerChatRoomParticipant * GetParticipant( int index );

private:

	GLXPlayerChatRoomParticipant * FindParticipant( const char * name );

	bool RemoveParticipant( const char * name );

	GLXPlayerChat * m_pClient;

	MUCRoom * m_room;

	char * m_name;

	char * m_jid;

	std::vector < GLXPlayerChatRoomParticipant * > m_user_list;
	std::vector < GLXPlayerChatRoomParticipant * >::iterator _it;

};

#endif
