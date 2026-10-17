#ifndef __GL_XPLAYER_CHAT_ROOM_OBSERVER_H__
#define __GL_XPLAYER_CHAT_ROOM_OBSERVER_H__

namespace gllive
{
	enum GLXPlayerParticipantState
	{
		GL_XPLAYER_PARTICIPANT_ONLINE = 1,
		GL_XPLAYER_PARTICIPANT_CHAT = 2,
		GL_XPLAYER_PARTICIPANT_AWAY = 3,
		GL_XPLAYER_PARTICIPANT_BUSY = 4,
		GL_XPLAYER_PARTICIPANT_OFFLINE = 5
	};
	
	/**
	 * This class is an observer to notify all the events on conference.
	 * <p>
	 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
	 *
	 */
	class GLXPlayerChatRoomObserver1
		{
			
		public:
			
			/**
			 * This method will be invoked after a invitation was arrived to ask you join a conference.
			 *
			 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
			 * @param invitor the jid of the sender, such as "username@gameloft.com"
			 * @param reason the appended information with the invitation, such as "come on"
			 * @param password the password will be appended if the chat room has been encrypted.
			 *
			 */
			virtual void OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
		const char * password,const char *realInvitor ) = 0;
			
			/**
			 * This method will be invoked after a conference message was arrived.
			 *
			 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
			 * @param nickname the nickname of the sender
			 * @param message the content of the message
			 *
			 */
			virtual void OnChatRoomMessage( const char * room, const char * nickname, const char * message ) = 0;
			
			/**
			 * This method will be invoked after a conference has been found.
			 *
			 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
			 * @param nickname the nickname of the chat room
			 *
			 */
			virtual void OnChatRoomFound( const char * room, const char * nickname ) = 0;
			
			/**
			 * This method will be invoked after a participant enter or leave the conference.
			 *
			 * @param room_jid the jid of the chat room, such as "chatroomname@conference.gameloft.com"
			 * @param user_name the nickname of the participant, such as "username"
			 * @param state online state
			 *
			 */
			virtual void OnChatRoomPresence( const char * room_jid, const char * user_name, int state ) = 0;
			
			/**
			 * If you had sent a request to invite some guy join your confercence, 
			 * this method will be invoked after the guy accept or reject your invitation.
			 *
			 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
			 * @param invitor the jid of the sender, such as "username@gameloft.com"
			 * @param reason the appended information with the invitation, such as "come on"
			 *
			 */
			virtual void OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason ) = 0;
			
			virtual void OnChatRoomConfigResult( const char * room, bool success ) = 0;
		};
}

#endif