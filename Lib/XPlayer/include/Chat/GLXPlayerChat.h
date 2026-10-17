#ifndef __GL_XPLAYER_CHAT_H__
#define __GL_XPLAYER_CHAT_H__

#include "../Common/GLXPlayerWebComponent.h"

#include "../Porting.h"
#include "GLXPlayerChatObserver.h"
#include "GLXPlayerChatRoomObserver.h"
#include "gloox/client.h"
#include "gloox/connectionlistener.h"
#include "gloox/messagesessionhandler.h"
#include "gloox/rostermanager.h"
#include "gloox/disco.h"
#include "gloox/messageeventfilter.h"
#include "gloox/chatstatefilter.h"
#include "gloox/messagehandler.h"
#include "gloox/messageeventhandler.h"
#include "gloox/chatstatehandler.h"
#include "GLXPlayerChatSessionHandler.h"
#include "GLXPlayerChatRoomHandler.h"
#include "GLXPlayerChatRoom.h"
#include "GLXPlayerChatDiscoverHandler.h"
#include <vector>

class GLXPlayerChatSessionHandler;
class GLXPlayerChatRoomHandler;
class GLXPlayerChatRoom;
class GLXPlayerChatDiscoverHandler;

using namespace gloox;

typedef struct _GLXPlayerBuddyRequest
{
	std::string jid;
	std::string nickname;
	std::string groupname;
}GLXPlayerBuddyRequest;

/**
 * This class is a Jabber Client wrapper.
 * <p>
 * We use gloox to implement the chat functions.
 * Gloox is a rock-solid, full-featured Jabber/XMPP client library, written in C++. 
 * It makes writing spec-compliant clients easy and allows for hassle-free 
 * integration of Jabber/XMPP functionality into existing applications. 
 * Gloox is released under the GNU GPL. Commercial licensing and support are available.
 *
 * Please implement your oberver class XXX inherit from GLXPlayerChatObserver.
 * All the events on chat will trigger the predefined method of this oberver class.
 * <p>
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerChat : public GLXPlayerWebComponent, public ConnectionListener, RosterListener
{

public:
	/**
	* Constructor
	*/
	GLXPlayerChat(void);
	
	/**
	* Destructor
	*/
	~GLXPlayerChat(void);
	
	/**
	* Function Update()
	* Update the request, should be called every frame.
	*/
	void Update();

	/**
	* Function RegisterChatObserver
	* Register chat observer.
	* @param observer The receiver of the chat event.
	*/
	bool RegisterChatObserver( GLXPlayerChatObserver * observer )
	{
		m_chat_observer = observer;
		return true;
	}

	/**
	* Function RegisterChatObserver
	* Register conference observer.
	* @param observer The receiver of the conference event.
	*/
	bool RegisterChatRoomObserver( GLXPlayerChatRoomObserver * observer )
	{
		m_chat_room_observer = observer;
		return true;
	}

	/**
	* Login
	* Login to chat server.
	* @param username User name (xxx@gameloft.com)
	* @param password Password, the same as pasword to login to Gameloft Live.
	*/
	bool Login( char * username, char * password );

	/**
	* Logout
	* Logout of chat server.
	*/
	void Logout();

	/**
	* Logout
	* Logout of chat server.
	*/
	void LogoutInMMO();
	
	/**
	* Function SendGetChatWarning().
	* Get warning in chat room.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool SendGetChatWarning();
	
	/**
	* Function SendChatMessage
	* Send message to a contact.
	* @param jid username@gameloft.com
	* @param message message information
	*/
	bool SendChatMessage( char * jid, char * message );
	
	/**
	* Function SendKeepAliveMessage
	* Keep connection to Chat server alive.	
	*/
	void SendKeepAliveMessage()
	{		
		m_client->send(" ");
		m_lastSendTime = XP_API_GET_TIME();
	}

	/**
	* Function ShouldSendKeepAlive
	* Check if should call eep SendKeepAliveMessage.	
	*/
	bool ShouldSendKeepAlive()
	{
		long current_time = XP_API_GET_TIME();

		if ( ( current_time - m_lastSendTime ) > KEEP_ALIVE_WITH_CHATSERVER_INTERVAL )
		{
			return true;
		}
		return false;
	}

	/**
	* Function SetDomain
	* Set the domain, such as ''gameloft.com''.
	* @param domain Domain, such as "gameloft.com"
	*/
	bool SetDomain( char * domain );

	/**
	* Function GetDomain
	* Return the domain, such as ''gameloft.com''.
	*/
	char * GetDomain()
	{
		return m_domain;
	}

	/**
	* Function SetConferenceDomain
	* Set the domain of conference. It is generally add ''conference'' into the head of the domain, e.g. ''conference.gameloft.com''.
	* @param domain Domain, such as ''conference.gameloft.com''.
	*/
	bool SetConferenceDomain( char * domain );

	/**
	* Function SetServer
	* Set the chat server address to be connected. 
	* @param server The IP address of chat server, such as ''10.15.0.65''. If the domain has public IP address,  please fill it with domain, such as ''gameloft.com''.
	*/
	bool SetServer( char * server );

	/**
	 * Add a contact into buddy list.
	 * @param jid username@gameloft.com
	 * @param name lutest ( the nick name to show )
	 * @param group gameloft ( the group name )
	 * @param message the appended message
	 */
	bool AddBuddy( char * jid, char * name, char * group, char * message );

	/**
	 * Remove a contact from buddy list.
	 * @param jid username@gameloft.com
	 */
	bool RemoveBuddy( char * jid );

	/**
	 * Accept the request to add you into the buddy list.
	 * @param jid the jid of the applicant, such as "username@gameloft.com"
	 * @param nickname the nickname in local
	 * @param groupname the group name in local
	 */
	bool AcceptBuddyRequest( char * jid, char * nickname, char * groupname );

	/**
	 * Reject the request to add you into the buddy list.
	 * @param jid username@gameloft.com
	 */
	bool RejectBuddyRequest( char * jid );

	/**
	 * Accept the request to add you into the chat room.
	 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 * @param invitee the user to be invited, normally is current user, such as "nickname"
	 * @return A new chat room will be created automatically, you should delete it after the chat room is closed.
	 */
	GLXPlayerChatRoom * AcceptRoomInvitation( char * room, char * invitee );

	/**
	 * Reject the request to add you into the chat room.
	 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
	 * @param invitor the jid of the user who sent the invitation request, such as "username@gameloft.com"
	 * @param invitee the name of the user who received the invitation request, such as "gameloft2009"
	 */
	bool RejectRoomInvitation( char * room, char * invitor, char * invitee );

	/**
	 * Create a new conference.
	 * @param room_name the name of the conference, such as "roomname" ( "@conference.gameloft.com" will be appended after it )
	 * @param user_name the name of the user, such as "username" ( the final format is "roomname@conference.gameloft.com/username")
	 */
	GLXPlayerChatRoom * CreateChatRoom( char * room_name, char * user_name );

	/**
	* Function ListChatRoom
	* Query all the existed conferences.
	*/
	bool ListChatRoom();

	/**
	* Funcation AddGroup
	* Create new group.
	* @param	jid JID
	* @param	group_name Gruop Name
	*/
	bool AddGroup( char * jid, char * group_name );


	/**
	* Function GetClient
	* @return Client IP.
	*/
	Client * GetClient()
	{
		return m_client;
	}

	/**
	* Function GLXPlayerChatRoomHandler
	* @return Room Handler
	*/
	GLXPlayerChatRoomHandler * GetRoomHandler()
	{
		return m_muc_handler;
	}

	/**
	* Function GetClient
	* Check if the request get error.
	*/
	bool IsError();
	
	/**
	* Function IsConnected
	* Check if the connection is established
	*/	
	bool IsConnected()
	{
		return m_enabled;
	}

	/**
	* Connection Listener
	*/	
	void onConnect();
	void onDisconnect( ConnectionError e );
	bool onTLSConnect( const CertInfo& info );

	/**
	* Roster Listener
	*/
    void handleItemAdded( const JID& jid );	
	void handleItemSubscribed( const JID& jid );
	void handleItemRemoved( const JID& jid );
    void handleItemUpdated( const JID& jid );
    void handleItemUnsubscribed( const JID& jid );
    void handleRoster( const Roster& roster );
    void handleRosterPresence( const RosterItem& item, const std::string& resource, Presence presence, const std::string& /*msg*/ );
    void handleSelfPresence( const RosterItem& item, const std::string& resource, Presence presence, const std::string& /*msg*/ );
    bool handleSubscriptionRequest( const JID& jid, const std::string& /*msg*/ );
    bool handleUnsubscriptionRequest( const JID& jid, const std::string& /*msg*/ );
    void handleNonrosterPresence( Stanza* stanza );
    void handleRosterError( Stanza * /*stanza*/ );

	/**
	* For handler callback
	*/
	void OnChatSessionCreated( char * jid );
	void OnChatMessageArrived( char * jid, char * message );
	void OnChatMUCInvitation( const JID& room, const JID& invitee, const std::string& reason, const std::string& body, const std::string& password, bool cont );
	void OnChatMUCMessage( MUCRoom* room, const std::string& nick, const std::string& message, bool history, const std::string& when, bool priv );
	void OnChatMUCDiscover( const std::string& jid, const std::string& name );
	void OnChatMUCParticipantPresence( MUCRoom * room, const MUCRoomParticipant participant, Presence presence );
	void OnChatMUCInviteDecline( MUCRoom * room, const JID& invitee, const std::string& reason );
	void OnChatMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation );

private:
	/**
	 * Function LoadConfig.
	 * Load all chat configuration.
	 */
	bool LoadConfig();

	/**
	 * Variable m_chat_observer.
	 * An instance of GLXPlayerChatObserver.
	 */
	GLXPlayerChatObserver * m_chat_observer;

	/**
	 * Variable m_chat_room_observer.
	 * An instance of GLXPlayerChatRoomObserver.
	 */
	GLXPlayerChatRoomObserver * m_chat_room_observer;

	/**
	 * Variable m_domain.
	 * Chat the domain, such as ''gameloft.com''.
	 */
	char * m_domain;

	/**
	 * Variable m_conference_domain.
	 * Chat conference the domain, such as ''conference.gameloft.com''.
	 */
	char * m_conference_domain;

	/**
	 * Variable m_server_address.
	 * Chat server address.
	 */
	char * m_server_address;

	/**
	 * Variable m_client.
	 * Chat client.
	 */
	Client * m_client;

	/**
	 * Variable m_error.
	 * Error code
	 */
	int m_error;

	/**
	 * A session is a conversation.	 
	 */
	GLXPlayerChatSessionHandler * m_session_handler;

	/**
	 * Handle multiple user chat.
	 */
	GLXPlayerChatRoomHandler * m_muc_handler;

	/**
	 * Handle server discover.
	 */
	GLXPlayerChatDiscoverHandler * m_disco_handler;

	/**
	 * Store the jid of the contact had been asked to add into buddy list.	
	 * If the GLXPlayerChat received the add buddy request,
	 * and the jid found in history, it will accept it automatically.
	 */
	std::vector < GLXPlayerBuddyRequest > m_buddy_request_history;
	std::vector < GLXPlayerBuddyRequest >::iterator _it_buddy_history;

	/**
	 * Variable m_enabled.
	 * Enable update.
	 */
	bool m_enabled;

	/**
	 * Variable m_lastSendTime.
	 * Last sent time.
	 */
	long m_lastSendTime;
};

#endif
