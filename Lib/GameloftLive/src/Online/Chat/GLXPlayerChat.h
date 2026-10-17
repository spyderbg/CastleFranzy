#ifndef __GL_XPLAYER_CHAT_H__
#define __GL_XPLAYER_CHAT_H__

#include "../Porting.h"
#include "GLXPlayerChatObserver.h"
#include "GLXPlayerChatRoomObserver.h"
#include "gloox/glliveclient.h"
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
#include "GLXPlayerObserver.h"

namespace gllive
{
	class GLXPlayerChatSessionHandler;
	class GLXPlayerChatRoomHandler;
	class GLXPlayerChatRoom;
	class GLXPlayerChatDiscoverHandler;
	
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
	class GLXPlayerChat : public ConnectionListener, RosterListener
	{
		
	public:
		
		GLXPlayerChat(void);
		
		~GLXPlayerChat(void);
		
		static void ToLower(const char* src, char *dest);
		
		void Update();
		
		bool RegisterChatObserver( GLXPlayerObserver * observer )
		{
			m_chat_observer = observer;
			return true;
		}
		
		bool RegisterChatRoomObserver( GLXPlayerObserver * observer )
		{
			m_chat_room_observer = observer;
			return true;
		}
		
		/**
		 * Login
		 *
		 */
		bool Login( char * username, char * password );
		
		/**
		 * Logout
		 *
		 */
		void Logout();
		
		/**
		 * Send message to a contact.
		 *
		 * @param jid username@gameloft.com
		 * @param message message information
		 *
		 */
		bool SendChatMessage( char * jid, char * message );
		void SendKeepAliveMessage()
		{
			//Tag *m = new Tag( "IQ" );
			//m->addAttribute( "type", "get" );
			//new Tag( m, "body", "keep alive" );
			
			////m->addAttribute( "from", m_parent->jid().full() );
			////m->addAttribute( "to", m_target.full() );
			//m->addAttribute( "id", m_client->getID() );
			
			//m_client->send(m);
			m_client->send(" ");
			m_lastSendTime = XP_API_GET_TIME();
		}
		
		bool ShouldSendKeepAlive()
		{
			long current_time = XP_API_GET_TIME();
			
			if ( ( current_time - m_lastSendTime ) > KEEP_ALIVE_WITH_CHATSERVER_INTERVAL )
			{
				return true;
			}
			
			return false;
		}
		
		bool SetDomain( char * domain );
		
		char * GetDomain()
		{
			return m_domain;
		}
		
		bool SetConferenceDomain( char * domain );
		
		bool SetServer( char * server );
		
		/**
		 * Add a contact into buddy list.
		 * @param jid username@gameloft.com
		 * @param name lutest ( the nick name to show )
		 * @param group gameloft ( the group name )
		 * @param message the appended message
		 *
		 */
		bool AddBuddy( char * jid, char * name, char * group, char * message );
		
		/**
		 * Remove a contact from buddy list.
		 *
		 * @param jid username@gameloft.com
		 *
		 */
		bool RemoveBuddy( char * jid );
		
		/**
		 * Accept the request to add you into the buddy list.
		 *
		 * @param jid the jid of the applicant, such as "username@gameloft.com"
		 * @param nickname the nickname in local
		 * @param groupname the group name in local
		 *
		 */
		bool AcceptBuddyRequest( char * jid, char * nickname, char * groupname );
		
		/**
		 * Reject the request to add you into the buddy list.
		 *
		 * @param jid username@gameloft.com
		 *
		 */
		bool RejectBuddyRequest( char * jid );
		
		/**
		 * Accept the request to add you into the chat room.
		 *
		 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
		 * @param invitee the user to be invited, normally is current user, such as "nickname"
		 *
		 * @return A new chat room will be created automatically, you should delete it after the chat room is closed.
		 *
		 */
		GLXPlayerChatRoom * AcceptRoomInvitation( char * room, char * invitee );
		
		/**
		 * Reject the request to add you into the chat room.
		 *
		 * @param room the jid of the chat room, such as "chatroomname@conference.gameloft.com"
		 * @param invitor the jid of the user who sent the invitation request, such as "username@gameloft.com"
		 * @param invitee the name of the user who received the invitation request, such as "lu1"
		 *
		 */
		bool RejectRoomInvitation( char * room, char * invitor, char * invitee );
		
		/**
		 * Create a new conference.
		 *
		 * @param room_name the name of the conference, such as "roomname" ( "@conference.gameloft.com" will be appended after it )
		 * @param user_name the name of the user, such as "username" ( the final format is "roomname@conference.gameloft.com/username" )
		 *
		 */
		GLXPlayerChatRoom * CreateChatRoom( char * room_name, char * user_name );
		
		bool ListChatRoom();
		
		bool AddGroup( char * jid, char * group_name );
		
		
		GLLiveClient * GetClient()
		{
			return m_client;
		}
		
		GLXPlayerChatRoomHandler * GetRoomHandler()
		{
			return m_muc_handler;
		}
		
		bool IsError();
		bool IsConnected()
		{
			return m_enabled;
		}
		
		// ConnectionListener
		
		void onConnect();
		
		void onDisconnect( ConnectionError e );
		
		bool onTLSConnect( const CertInfo& info );
		
		// RosterListener
		
		void handleItemAdded( const JID& jid );
		
		void handleItemSubscribed( const JID& jid );
		
		void handleItemRemoved( const JID& jid );
		
		void handleItemUpdated( const JID& jid );
		
		void handleItemUnsubscribed( const JID& jid );
		
		void handleRoster( const Roster& roster );
		
		void handleRosterPresence( const RosterItem& item, const std::string& resource,
								  Presence presence, const std::string& /*msg*/ );
		
		void handleSelfPresence( const RosterItem& item, const std::string& resource,
								Presence presence, const std::string& /*msg*/ );
		
		bool handleSubscriptionRequest( const JID& jid, const std::string& /*msg*/ );
		
		bool handleUnsubscriptionRequest( const JID& jid, const std::string& /*msg*/ );
		
		void handleNonrosterPresence( Stanza* stanza );
		
		void handleRosterError( Stanza * /*stanza*/ );
		
		// For handler callback
		
		void OnChatSessionCreated( char * jid );
		void OnChatMessageArrived( char * jid, char * message );
	void OnChatMUCInvitation( const JID& room, const JID& invitee, const std::string& reason, const std::string& body, const std::string& password, bool cont ,const std::string& realInvitor);
	void OnChatMUCMessage( MUCRoom* room, const std::string& nick, const std::string& message, bool history, const std::string& when, bool priv );
		void OnChatMUCDiscover( const std::string& jid, const std::string& name );
		void OnChatMUCParticipantPresence( MUCRoom * room, const MUCRoomParticipant participant,
										  Presence presence );
		void OnChatMUCInviteDecline( MUCRoom * room, const JID& invitee, const std::string& reason );
		void OnChatMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation );
	private:
		
		bool LoadConfig();
		
		GLXPlayerObserver * m_chat_observer;
		
		GLXPlayerObserver * m_chat_room_observer;
		
		char * m_domain;
		
		char * m_conference_domain;
		
		char * m_server_address;
		
		GLLiveClient * m_client;
		
		int m_error;
		
		/**
		 * A session is a conversation.
		 *
		 */
		GLXPlayerChatSessionHandler * m_session_handler;
		
		/**
		 * Handle multiple user chat.
		 *
		 */
		GLXPlayerChatRoomHandler * m_muc_handler;
		
		/**
		 * Handle server discover.
		 *
		 */
		GLXPlayerChatDiscoverHandler * m_disco_handler;
		
		/**
		 * Store the jid of the contact had been asked to add into buddy list.
		 * <p>
		 * If the GLXPlayerChat received the add buddy request,
		 * and the jid found in history, it will accept it automatically.
		 *
		 */
		std::vector < GLXPlayerBuddyRequest > m_buddy_request_history;
		std::vector < GLXPlayerBuddyRequest >::iterator _it_buddy_history;
		
		bool m_enabled;
		
		long m_lastSendTime;
	};
}

#endif
