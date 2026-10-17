#include "../../../include/chat/GLXPlayerChat.h"
//#include "memory.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

GLXPlayerChat::GLXPlayerChat()
{
	m_enabled = false;
	m_domain = NULL;
	m_conference_domain = NULL;
	m_server_address = NULL;
	m_client = NULL;	

	m_session_handler = NULL;
	m_muc_handler = NULL;
	m_disco_handler = NULL;

	m_lastSendTime = 0;

	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );	
}

GLXPlayerChat::~GLXPlayerChat(void)
{
	XP_API_DEL( m_domain );
	XP_API_DEL( m_conference_domain );
	XP_API_DEL( m_server_address );
	XP_API_DEL( m_session_handler );
	XP_API_DEL( m_muc_handler );
	XP_API_DEL( m_disco_handler );
	XP_API_DEL( m_client );
}

bool GLXPlayerChat::LoadConfig()
{
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
    char tmp[FILENAME_MAX];
	XP_API_MEMSET(tmp,0,FILENAME_MAX);
	GetConfigFilePath(tmp, XPLAYER_FILE_CONF);
	pFile = XP_API_FILE_OPEN( tmp, "r" );
#else
	pFile = XP_API_FILE_OPEN( XPLAYER_FILE_CONF, "r" );
#endif

	if( !pFile )
	{
		XP_DEBUG_OUT("[GLXPlayerChat] load config - %s not found \n", XPLAYER_FILE_CONF );
		return false;
	}

	int length = XP_API_FILE_GET_LENGTH( pFile );
	char * buffer = XP_API_NEW char[length+1];
	
	XP_API_MEMSET( buffer, 0, length+1 );
	XP_API_FILE_READ( buffer, length, 1, pFile );

	int line = 0;
	char * data = XP_API_NEW char[length+1];

	XP_API_MEMSET( data, 0, length+1 );
	XP_API_PARSE_DATA( buffer, data, line++, '\n' );
	if (data[XP_API_STRLEN(data)-1] == '\r')
	{
		data[XP_API_STRLEN(data)-1] = 0;
	}
	int len = XP_API_STRLEN( data );
	
	while( len > 0 )
	{
		char name[32] = { 0 };
		char value[256] = { 0 };
		int pos = 0;

		pos = XP_API_PARSE_DATA( data, name, 0, ':' );
		pos = XP_API_PARSE_DATA( data, value, 1, ':' );
		XP_API_MEMSET( value, 0, 256 );
		XP_API_MEMCPY( value, data+pos, XP_API_STRLEN(data)-pos );
		
		XP_API_STRTRIM( (char *) name );
		XP_API_STRTRIM( (char *) value );
		
		if ( XP_API_STRCMP( name, XPLAYER_CONF_GGI ) == 0 )
		{
			m_ggi = XP_API_ATOI( value );
		}
      
		if ( XP_API_STRCMP( name, XPLAYER_CONF_URL ) == 0 )
		{
			// http://beta.gameloft.com/ope/GenericXPlayer_v1.php
			char temp[256] = { 0 };
			char host[256] = { 0 };

			m_http_url = XP_API_STRNEW( value );

			XP_API_MEMSET( temp, 0, 256 );
			XP_API_MEMSET( host, 0, 256 );
			
			pos = XP_API_PARSE_DATA( value, temp, 2, '/' );
			XP_API_MEMSET( temp, 0, 256 );
			XP_API_MEMCPY( temp, value+pos, XP_API_STRLEN(value)-pos );

			pos = XP_API_PARSE_DATA( temp, host, 0, '/' );
			
			int len_total = XP_API_STRLEN( temp );
			int len_addrs = XP_API_STRLEN( host );
			
			m_http_server_address = XP_API_NEW char[len_addrs+1];
			XP_API_MEMSET( m_http_server_address, 0, len_addrs+1 );
			XP_API_MEMCPY( m_http_server_address, host, len_addrs );

			m_http_php_address = XP_API_NEW char[len_total-len_addrs+1];
			XP_API_MEMSET( m_http_php_address, 0, len_total-len_addrs+1 );
			XP_API_MEMCPY( m_http_php_address, temp+len_addrs, len_total-len_addrs );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_PHPVER ) == 0 )
		{
			m_http_php_version = XP_API_STRNEW( value );
		}
		
		if ( XP_API_STRCMP( name, XPLAYER_CONF_CHAT_SERVER ) == 0 )
		{
			SetServer( value );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_CHAT_DOMAIN ) == 0 )
		{
			SetDomain( value );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_CHAT_ROOM_DOMIAN ) == 0 )
		{
			SetConferenceDomain( value );
		}

		XP_API_MEMSET( data, 0, length+1 );
		XP_API_PARSE_DATA( buffer, data, line++, '\n' );
		if (data[XP_API_STRLEN(data)-1] == '\r')
		{
			data[XP_API_STRLEN(data)-1] = 0;
		}		
		len = XP_API_STRLEN( data );
	}

	XP_API_DEL( data );
	XP_API_DEL( buffer );

	XP_API_FILE_CLOSE( pFile );

	return true;	
}

bool GLXPlayerChat::SetDomain( char * domain )
{
	XP_API_DEL( m_domain );
	m_domain = XP_API_STRNEW( domain );
	return true;
}

bool GLXPlayerChat::SetConferenceDomain( char * domain )
{
	XP_API_DEL( m_conference_domain );
	m_conference_domain = XP_API_STRNEW( domain );
	return true;
}

bool GLXPlayerChat::SetServer( char * server )
{
	XP_API_DEL( m_server_address );
	if ( NULL != server && XP_API_STRLEN( server ) > 0 )
	{
		m_server_address = XP_API_STRNEW( server );
	}	
	return true;	
}

bool GLXPlayerChat::Login( char * username, char * password )
{
	char jid_buffer[128] = { 0 };

	m_enabled = true;
	m_lastSendTime = XP_API_GET_TIME();

	m_session_handler = XP_API_NEW GLXPlayerChatSessionHandler( this );
	m_muc_handler = XP_API_NEW GLXPlayerChatRoomHandler( this );
	m_disco_handler = XP_API_NEW GLXPlayerChatDiscoverHandler( this );

	//Add by Cai Zhao Dong 2009-06-16
	char lowerName[64];
	XP_API_TOLOWER(username, lowerName);

	XP_API_MEMSET( jid_buffer, 0, 128 );
	XP_API_SPRINTF( jid_buffer, "%s@%s/GLXPlayerChat", lowerName, m_domain );
	JID jid( jid_buffer );
	XP_DEBUG_OUT("Login Jid is: %s\n", jid_buffer);
	m_client = XP_API_NEW Client( jid, password );
	m_client->setPresence( PresenceAvailable, 5 );
	//m_client->setServer( "10.209.248.70" );
	//m_client->setServer( "219.142.114.190" );
	if ( NULL != m_server_address )
	{
		m_client->setServer( m_server_address );
	}
	m_client->registerConnectionListener( this );
	m_client->registerMessageSessionHandler( m_session_handler, StanzaMessageChat | StanzaMessageGroupchat );
	m_client->rosterManager()->registerRosterListener( this, false );
	m_client->registerMUCInvitationHandler( m_muc_handler );
	m_client->disco()->setVersion( "Chat", GLOOX_VERSION, "Win32" );
	m_client->disco()->setIdentity( "client", "xplayer" );
	m_client->disco()->registerDiscoHandler( m_disco_handler );
	StringList ca;
	ca.push_back( "/path/to/cacert.crt" );
	m_client->setCACerts( ca );
	return m_client->connect( false );
}

void GLXPlayerChat::Logout()
{

	XP_API_DEL( m_session_handler );
	XP_API_DEL( m_muc_handler );
	XP_API_DEL( m_disco_handler );
	XP_API_DEL( m_client );
	m_enabled = false;
}

void GLXPlayerChat::LogoutInMMO()
{

	m_client->disconnect();
	XP_API_DEL( m_session_handler );
	XP_API_DEL( m_muc_handler );
	XP_API_DEL( m_disco_handler );
	XP_API_DEL( m_client );
	m_enabled = false;
}

bool GLXPlayerChat::SendChatMessage( char * username, char * message )
{
	char jid_buffer[128] = { 0 };

	m_enabled = true;

	XP_API_MEMSET( jid_buffer, 0, 128 );
	XP_API_SPRINTF( jid_buffer, "%s@%s", username, m_domain );
	JID jid( jid_buffer );

	XP_DEBUG_OUT( "[GLXPlayerChat] Send [%s] to [%s]\n", message, jid.full().c_str() );
	m_session_handler->SendChatMessage( (char *)jid.full().c_str(), message );
	m_lastSendTime = XP_API_GET_TIME();
	return true;
}

bool GLXPlayerChat::AddBuddy( char * jid, char * name, char * group, char * message )
{
	std::string id = jid;
	std::string nickname = name;
	std::string groupname = group;
	std::string msg = message;

	bool bInHistory = false;
	_it_buddy_history = m_buddy_request_history.begin();
	for( ; _it_buddy_history != m_buddy_request_history.end(); _it_buddy_history++ )
	{
		if ( (*_it_buddy_history).jid == id )
		{
			bInHistory = true;
			break;
		}
	}

	if ( bInHistory )
	{
		(*_it_buddy_history).nickname = name;
		(*_it_buddy_history).groupname = group;
	}
	else
	{
		GLXPlayerBuddyRequest request;
		request.jid = id;
		request.nickname = nickname;
		request.groupname = groupname;
		m_buddy_request_history.push_back( request );
	}

	StringList list;
	list.push_back( groupname );
	m_client->rosterManager()->subscribe( id, nickname, list, msg );
	return true;
}

bool GLXPlayerChat::RemoveBuddy( char * jid )
{
	std::string id = jid;
	m_client->rosterManager()->remove( id );
	return true;
}

bool GLXPlayerChat::AcceptBuddyRequest( char * jid, char * nickname, char * groupname )
{
	std::string id = jid;
	std::string name = nickname;
	std::string group = groupname;
	m_client->rosterManager()->ackSubscriptionRequest( id, true );
    StringList groups;
	groups.push_back( group );
    JID jidd( jid );
    m_client->rosterManager()->subscribe( jidd, name, groups, "" );
	return true;
}

bool GLXPlayerChat::RejectBuddyRequest( char * jid )
{
	std::string id = jid;
	m_client->rosterManager()->ackSubscriptionRequest( id, false );
	return true;
}

GLXPlayerChatRoom * GLXPlayerChat::AcceptRoomInvitation( char * room, char * invitee )
{
	char lroom[64];
	char linvitee[64];
	
	XP_API_TOLOWER(room, lroom);
	XP_API_TOLOWER(invitee, linvitee);

	std::string jid = lroom;
	std::string user = linvitee;
	int pos = jid.find( "@" );
	std::string name = jid.substr( 0, pos );
	GLXPlayerChatRoom * char_room = XP_API_NEW GLXPlayerChatRoom( this, jid, name, user );
	char_room->Join();
	return char_room;
}

bool GLXPlayerChat::RejectRoomInvitation( char * room, char * invitor, char * invitee )
{
	char lroom[64];
	char linvitee[64];
	char linvitor[64];
	
	XP_API_TOLOWER(room, lroom);
	XP_API_TOLOWER(invitee, linvitee);
	XP_API_TOLOWER(invitor, linvitor);

	std::string jid = lroom;
	std::string user = linvitee;
	int pos = jid.find( "@" );
	std::string name = jid.substr( 0, pos );
	GLXPlayerChatRoom * char_room = XP_API_NEW GLXPlayerChatRoom( this, jid, name, user );
	char_room->Decline( lroom, linvitor, linvitee );
	XP_API_DEL( char_room );
	return true;
}

GLXPlayerChatRoom * GLXPlayerChat::CreateChatRoom( char * room_name, char * user_name )
{
	char roomLower[64];
	char userLower[64];
	
	XP_API_TOLOWER(room_name, roomLower);
	XP_API_TOLOWER(user_name, userLower);
	
	
	std::string room = roomLower;
	std::string user = userLower;
	std::string domain = m_conference_domain;
	std::string jid = room + "@" + domain;
	GLXPlayerChatRoom * chat_room = XP_API_NEW GLXPlayerChatRoom( this, jid, room, user );
	chat_room->Join();
	return chat_room;
}

bool GLXPlayerChat::AddGroup( char * jid, char * group_name )
{
	std::string group = group_name;
    StringList groups;
	groups.push_back( group );
	JID id("");
	m_client->rosterManager()->add( id, "", groups );
	return true;
}

bool GLXPlayerChat::ListChatRoom()
{
	Disco * discovery = m_client->disco();
	JID jid( m_conference_domain );
	discovery->getDiscoItems( jid, "", m_disco_handler, 0, "" );
	return true;
}

bool GLXPlayerChat::IsError()
{
	return ( m_error != 0 );
}

void GLXPlayerChat::Update()
{
	if ( m_enabled )
	{
		m_error = m_client->recv( 10 );

		if(ShouldSendKeepAlive())
		{
			SendKeepAliveMessage();
		}
	}
	GLXPlayerWebComponent::Update();
}

//////////////////////////////
// ConnectionListener
//////////////////////////////
void GLXPlayerChat::onConnect()
{
	XP_DEBUG_OUT( "[GLXPlayerChat] Connected!!!\n" );	
	m_chat_observer->OnChatConnected();
}

void GLXPlayerChat::onDisconnect( ConnectionError e )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] Disconnected: %d\n", e );
	m_enabled = false;

	//if( e == ConnAuthenticationFailed )
	//{
	//	XP_DEBUG_OUT( "[GLXPlayerChat] auth failed. reason: %d\n", m_client->authError() );
	//}

	if(e == ConnStreamError)
	{
		StreamError streamReason = m_client->streamError();
		m_chat_observer->OnChatDisconnect( e, streamReason );
	}
	else if(e == ConnAuthenticationFailed)
	{
		AuthenticationError authReason = m_client->authError();
		m_chat_observer->OnChatDisconnect( e, authReason );
	}
	else
	{
		m_chat_observer->OnChatDisconnect( e, -1 );
	}
}

bool GLXPlayerChat::onTLSConnect( const CertInfo& info )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
		info.status, info.issuer.c_str(), info.server.c_str(),
		info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
		info.compression.c_str() );
	return true;
}

//////////////////////////////
// RosterListener
//////////////////////////////
void GLXPlayerChat::handleItemAdded( const JID& jid )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] added %s\n", jid.bare().c_str() );
}
	
void GLXPlayerChat::handleItemSubscribed( const JID& jid )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] subscribed %s\n", jid.bare().c_str() );
	m_chat_observer->OnChatAddBuddy( jid.full().c_str() );
}

void GLXPlayerChat::handleItemRemoved( const JID& jid )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] removed %s\n", jid.bare().c_str() );
	m_chat_observer->OnChatRemoveBuddy( jid.full().c_str() );
}

void GLXPlayerChat::handleItemUpdated( const JID& jid )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] updated %s\n", jid.bare().c_str() );
}

void GLXPlayerChat::handleItemUnsubscribed( const JID& jid )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] unsubscribed %s\n", jid.bare().c_str() );
}

void GLXPlayerChat::handleRoster( const Roster& roster )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] roster arriving\nitems:\n" );
	Roster::const_iterator it = roster.begin();
	for( ; it != roster.end(); ++it )
	{
		XP_DEBUG_OUT( "jid: %s, name: %s, subscription: %d\n",
			(*it).second->jid().c_str(), (*it).second->name().c_str(),
			(*it).second->subscription() );

		StringList g = (*it).second->groups();
		StringList::const_iterator it_g = g.begin();
		for( ; it_g != g.end(); ++it_g )
		{
			XP_DEBUG_OUT( "\tgroup: %s\n", (*it_g).c_str() );
		}

		RosterItem::ResourceMap::const_iterator rit = (*it).second->resources().begin();
		for( ; rit != (*it).second->resources().end(); ++rit )
		{
			XP_DEBUG_OUT( "resource: %s\n", (*rit).first.c_str() );
		}
		
		// User and contact are subscribed to each other
//		if ( (*it).second->subscription() == RosterItem::S10nBoth )
		{
			std::string name = (*it).second->name();
			char * pName = NULL;
			if ( name.length() > 0 )
			{
				pName = (char *)(*it).second->name().c_str();
			}
			else
			{
				pName = (char *)(*it).second->jid().c_str();
			}

			char * pGroup = NULL;
//			for ( StringList::const_iterator it_group=g.begin(); it_group != g.end(); it_group++ )
//			{
//				pGroup = (char *)(*it_group).c_str();
//				m_chat_observer->OnChatRoster( (*it).second->jid().c_str(), pName, pGroup, (*it).second->subscription() );
				m_chat_observer->OnChatRoster( (*it).second->jid().c_str(), pName, "123", (*it).second->subscription() );
//			}
		}
	}
}

void GLXPlayerChat::handleRosterPresence( const RosterItem& item, const std::string& resource,
                                   Presence presence, const std::string& /*msg*/ )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] presence received: %s/%s -- %d\n", item.jid().c_str(), resource.c_str(), presence );
	
	StringList g = item.groups();
	StringList::const_iterator it_g = g.begin();

	for( ; it_g != g.end(); ++it_g )
	{
		m_chat_observer->OnChatRosterPresence( item.jid().c_str(), item.name().c_str(), (*it_g).c_str(), presence );	
	}	
}

void GLXPlayerChat::handleSelfPresence( const RosterItem& item, const std::string& resource,
                                   Presence presence, const std::string& /*msg*/ )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] self presence received: %s/%s -- %d\n", item.jid().c_str(), resource.c_str(), presence );	
}

bool GLXPlayerChat::handleSubscriptionRequest( const JID& jid, const std::string& msg )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] subscription: %s\n", jid.bare().c_str() );
	bool bInHistory = false;
	std::string id = jid.bare();
	_it_buddy_history = m_buddy_request_history.begin();
	for( ; _it_buddy_history != m_buddy_request_history.end(); _it_buddy_history++ )
	{
		if ( (*_it_buddy_history).jid == id )
		{
			bInHistory = true;
			break;
		}
	}

	if ( bInHistory )
	{		
		AcceptBuddyRequest( (char *)(*_it_buddy_history).jid.c_str(), (char *)(*_it_buddy_history).nickname.c_str(), 
			(char *)(*_it_buddy_history).groupname.c_str() );
		m_buddy_request_history.erase( _it_buddy_history );
	}
	else
	{
		m_chat_observer->OnChatBuddyRequest( jid.bare().c_str(), msg.c_str() );
	}
	return true;
}

bool GLXPlayerChat::handleUnsubscriptionRequest( const JID& jid, const std::string& /*msg*/ )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] unsubscription: %s\n", jid.bare().c_str() );
	return true;
}

void GLXPlayerChat::handleNonrosterPresence( Stanza* stanza )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] received presence from entity not in the roster: %s\n", stanza->from().full().c_str() );
}

void GLXPlayerChat::handleRosterError( Stanza * /*stanza*/ )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] an roster-related error occured\n" );
}

//////////////////////////////
// For handler callback
//////////////////////////////
void GLXPlayerChat::OnChatSessionCreated( char * jid )
{
	m_chat_observer->OnChatSession( jid );
}

void GLXPlayerChat::OnChatMessageArrived( char * jid, char * message )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] received [%s] from [%s]\n", message, jid );
	m_chat_observer->OnChatMessage( jid, message );
}

void GLXPlayerChat::OnChatMUCInvitation( const JID& room, const JID& invitee, const std::string& reason,
	const std::string& body, const std::string& password, bool cont )
{
	XP_DEBUG_OUT( "[GLXPlayerChat] invitation room=%s invitee=%s reason=%s body=%s password=%s\n", 
		room.full().c_str(), invitee.full().c_str(), reason.c_str(), body.c_str(), password.c_str() );

	char tempInviteeName[128];
	XP_API_MEMSET(tempInviteeName, 0, 128);
	XP_API_PARSE_DATA( invitee.full().c_str(), tempInviteeName, 0, '@' );

	m_chat_room_observer->OnChatRoomInvitation( room.full().c_str(), invitee.full().c_str(), reason.c_str(),
		password.c_str() );
}

void GLXPlayerChat::OnChatMUCMessage( MUCRoom* room, const std::string& nick, const std::string& message,
	bool history, const std::string& when, bool priv )
{
	char jid[256] = { 0 };

	XP_API_MEMSET( jid, 0, 256 );
	XP_API_SPRINTF( jid, "%s@%s", room->name().c_str(), room->service().c_str() );

	m_chat_room_observer->OnChatRoomMessage( jid, nick.c_str(), message.c_str() );
}

void GLXPlayerChat::OnChatMUCDiscover( const std::string& jid, const std::string& name )
{
	m_chat_room_observer->OnChatRoomFound( jid.c_str(), name.c_str() );
}

void GLXPlayerChat::OnChatMUCParticipantPresence( MUCRoom * room, const MUCRoomParticipant participant,
	Presence presence )
{	
	std::string room_str = room->name() + "@" + room->service();
	const char * room_jid = room_str.c_str();
	const char * user_name = participant.nick->resource().c_str();

	switch ( presence )
	{
	case PresenceAvailable:
		m_chat_room_observer->OnChatRoomPresence( room_jid, user_name, GL_XPLAYER_PARTICIPANT_ONLINE );
		break;
	case PresenceChat:
		m_chat_room_observer->OnChatRoomPresence( room_jid, user_name, GL_XPLAYER_PARTICIPANT_CHAT );
		break;
	case PresenceAway:
	case PresenceXa:
		m_chat_room_observer->OnChatRoomPresence( room_jid, user_name, GL_XPLAYER_PARTICIPANT_AWAY );
		break;
	case PresenceDnd:
		m_chat_room_observer->OnChatRoomPresence( room_jid, user_name, GL_XPLAYER_PARTICIPANT_BUSY );
		break;
	case PresenceUnavailable:
		m_chat_room_observer->OnChatRoomPresence( room_jid, user_name, GL_XPLAYER_PARTICIPANT_OFFLINE );
		break;
	default:
		break;
	}	
}

void GLXPlayerChat::OnChatMUCInviteDecline( MUCRoom * room, const JID& invitee, const std::string& reason )
{
	std::string room_str = room->name() + "@" + room->service();
	const char * room_jid = room_str.c_str();

	m_chat_room_observer->OnChatRoomInvitationDecline( room_jid, invitee.full().c_str(), reason.c_str() );
}

void GLXPlayerChat::OnChatMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation )
{
	std::string room_str = room->name() + "@" + room->service();
	const char * room_jid = room_str.c_str();

	m_chat_room_observer->OnChatRoomConfigResult( room_jid, success );
}

/**
* Function SendGetChatWarning().
* Get warning in chat room.	
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerChat::SendGetChatWarning()
{
	char buffer[ONLINE_MAX_PACKET_SIZE ];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_CHAT_WARNING, m_ggi, m_uid);
	XP_DEBUG_OUT("[GLXPlayerChat::SendGetChatWarning] -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}

//////////////////////////////
// Private functions
//////////////////////////////
