#ifndef __GL_XPLAYER_CHAT_OBSERVER_H__
#define __GL_XPLAYER_CHAT_OBSERVER_H__

/**
 * This class is an observer to notify all the events on chat.
 * <p>
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerChatObserver
{

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
	virtual void OnChatDisconnect( int e, int reason ) = 0;

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

};

#endif