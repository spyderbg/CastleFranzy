#include "../../../include/chat/GLXPlayerChatSessionHandler.h"
//#include "memory.h"

GLXPlayerChatSessionHandler::GLXPlayerChatSessionHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{

}

GLXPlayerChatSessionHandler::~GLXPlayerChatSessionHandler(void)
{
	for ( IT_MAP_SESSION it = m_mapSession.begin(); it != m_mapSession.end(); it++ )
	{
		m_pClient->GetClient()->disposeMessageSession( (*it).second.pMessageSession );
		XP_API_DEL( (*it).second.m_pEventHandler );
		XP_API_DEL( (*it).second.m_pStateHandler );
		XP_API_DEL( (*it).second.m_pMessageHandler );
	}

	m_mapSession.clear();
}

void GLXPlayerChatSessionHandler::handleMessageSession( MessageSession *session )
{
	std::string jid = "";
	jid += session->target().username();
	jid += "@";
	jid += session->target().server();

	XP_DEBUG_OUT( "[GLXPlayerChatSessionHandler] handleMessageSession %s\n", (char *)jid.c_str() );

	// When and new message session opened
	// Create and new chat filter and handler
	// save the all information to the list...
	// m_pMessageSession = session;
	IT_MAP_SESSION it = m_mapSession.find( jid );
	if ( it == m_mapSession.end() )
	{
		m_mapSession[ jid ].pMessageSession = session;

		m_mapSession[ jid ].m_pMessageHandler = XP_API_NEW GLXPlayerChatMessageHandler( m_pClient );
		m_mapSession[ jid ].pMessageSession->registerMessageHandler( m_mapSession[ jid ].m_pMessageHandler ); 

		m_mapSession[ jid ].pMessageEventFilter = XP_API_NEW MessageEventFilter( session );
		m_mapSession[ jid ].m_pEventHandler  = XP_API_NEW GLXPlayerChatEventHandler( m_pClient );
		m_mapSession[ jid ].pMessageEventFilter->registerMessageEventHandler( m_mapSession[ jid ].m_pEventHandler );

		m_mapSession[ jid ].pChatStateFilter = XP_API_NEW ChatStateFilter( session );
		m_mapSession[ jid ].m_pStateHandler = XP_API_NEW GLXPlayerChatStateHandler( m_pClient );
		m_mapSession[ jid ].pChatStateFilter->registerChatStateHandler( m_mapSession[ jid ].m_pStateHandler );
		m_pClient->OnChatSessionCreated( (char *)jid.c_str() );
	}
	else
	{
		m_pClient->GetClient()->disposeMessageSession( (*it).second.pMessageSession );
		XP_API_DEL( (*it).second.m_pEventHandler );
		XP_API_DEL( (*it).second.m_pStateHandler );
		XP_API_DEL( (*it).second.m_pMessageHandler );

		(*it).second.pMessageSession = session;

		(*it).second.m_pMessageHandler = XP_API_NEW GLXPlayerChatMessageHandler( m_pClient );
		(*it).second.pMessageSession->registerMessageHandler( (*it).second.m_pMessageHandler ); 

		(*it).second.pMessageEventFilter = XP_API_NEW MessageEventFilter( session );
		(*it).second.m_pEventHandler  = XP_API_NEW GLXPlayerChatEventHandler( m_pClient );
		(*it).second.pMessageEventFilter->registerMessageEventHandler( (*it).second.m_pEventHandler );

		(*it).second.pChatStateFilter = XP_API_NEW ChatStateFilter( session );
		(*it).second.m_pStateHandler = XP_API_NEW GLXPlayerChatStateHandler( m_pClient );
		(*it).second.pChatStateFilter->registerChatStateHandler( (*it).second.m_pStateHandler );
	}	
}

void GLXPlayerChatSessionHandler::SendChatMessage( char * jid_str, char * msg )
{
	std::string jid = jid_str;
	IT_MAP_SESSION it = m_mapSession.find( jid );
	if ( it == m_mapSession.end() )
	{
		MessageSession * session = NULL;
		JID j( jid );
		j.setResource( "spark" );

		session = XP_API_NEW MessageSession( m_pClient->GetClient(), j );

		m_mapSession[ jid ].pMessageSession = session;

		m_mapSession[ jid ].m_pMessageHandler = XP_API_NEW GLXPlayerChatMessageHandler( m_pClient );
		m_mapSession[ jid ].pMessageSession->registerMessageHandler( m_mapSession[ jid ].m_pMessageHandler ); 

		m_mapSession[ jid ].pMessageEventFilter = XP_API_NEW MessageEventFilter( session );
		m_mapSession[ jid ].m_pEventHandler  = XP_API_NEW GLXPlayerChatEventHandler( m_pClient );
		m_mapSession[ jid ].pMessageEventFilter->registerMessageEventHandler( m_mapSession[ jid ].m_pEventHandler );

		m_mapSession[ jid ].pChatStateFilter = XP_API_NEW ChatStateFilter( session );
		m_mapSession[ jid ].m_pStateHandler = XP_API_NEW GLXPlayerChatStateHandler( m_pClient );
		m_mapSession[ jid ].pChatStateFilter->registerChatStateHandler(  m_mapSession[ jid ].m_pStateHandler );
	}

	if ( m_mapSession[ jid ].pMessageSession != NULL )
	{
		m_mapSession[ jid ].pMessageEventFilter->raiseMessageEvent( MessageEventDisplayed );
		m_mapSession[ jid ].pMessageEventFilter->raiseMessageEvent( MessageEventComposing );
		m_mapSession[ jid ].pChatStateFilter->setChatState( ChatStateComposing );
		m_mapSession[ jid ].pMessageSession->send( msg, "GLXPlayer" );
	}
}