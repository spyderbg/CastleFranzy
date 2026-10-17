#ifndef __GL_XPLAYER_OBSERVER_H__
#define __GL_XPLAYER_OBSERVER_H__

/**
 * Abstract Class GLXPlayerObserver. 
 * Class GLXPlayerObserver is the observer of XPlayer component.
 * This class designed as an abstract class. So client developer has to re-implement all virtual functions.
 *
 * @author  World Online Team <<online-adm@gameloft.com>>
 * @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
 * @version 1.0.0 
 *  
 */
namespace gllive {
	
	class GLXPlayerObserver
		{
		public:
			
			virtual void OnMenuItemClicked( int id ) = 0;
			
			
			
		public:
			
			/**
			 * Virtual Function OnNetworkError.
			 * Indicate that network is broken or can not connect to the server.	
			 * <p>
			 */
			virtual void OnNetworkError() = 0;
			
			/**
			 * Virtual Function OnRequestTimeout.
			 * Connection time out or request timeout.
			 * <p>
			 * @param func_id Function ID.	
			 */
			virtual void OnRequestTimeout( int func_id ) = 0;
			
			/**
			 * Virtual Function OnRequestSuccess.
			 * The correct response.
			 * <p>
			 * @param func_id Function ID.
			 * @param data Returned data.
			 * @param data_len Returned data length.
			 */
			virtual void OnRequestSuccess( int func_id, char * data, int data_len ) = 0;
			
			/**
			 * Virtual Function OnRequestFailure.
			 * The wrong response or the operation is failed on server side.
			 * <p>
			 * @param func_id Function ID.
			 * @param error_code Error code.
			 */
			virtual void OnRequestFailure( int func_id, int error_code ) = 0;
		public:
			
			/**
			 * The method will be invoked after the connection to the chat server was established successfully.
			 *
			 */
			virtual void OnChatConnected() = 0;
			
			/**
			 * The method will be invoked if the connection to the chat server has been broken.
			 *
			 */
			virtual void OnChatDisconnect( int e ) = 0;
			
			/**
			 * After the login.
			 *
			 */
			virtual void OnChatRoster( const char * jid, const char * name, const char * group, int state ) = 0;
			
			virtual void OnChatRosterPresence( const char * jid, const char * name, const char * group, int state ) = 0;
			
			virtual void OnChatSession( char * from ) = 0;
			
			/**
			 * The method will be invoked after a message was arrived.
			 * 
			 * @param from the jid of the sender, such as "username@gameloft.com"
			 * @param message the content of the message, such as "hello"
			 *
			 */
			virtual void OnChatMessage( char * from, char * message ) = 0;
			
			virtual void OnChatAddBuddy( const char * jid ) = 0;
			
			virtual void OnChatRemoveBuddy( const char * jid ) = 0;
			
			/**
			 * This method will be invoked after a user sent a buddy request to you.
			 *
			 * @param jid the jid of the applicant, such as "username@gameloft.com"
			 * @param message the appended information, such as "May I have a talk to you?"
			 *
			 */
			virtual void OnChatBuddyRequest( const char * jid, const char * message ) = 0;
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
											  const char * password, const char *realInvitor ) = 0;
			
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