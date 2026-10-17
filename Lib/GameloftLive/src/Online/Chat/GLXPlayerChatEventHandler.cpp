#include "GLXPlayerChatEventHandler.h"

GLXPlayerChatEventHandler::GLXPlayerChatEventHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{
}

GLXPlayerChatEventHandler::~GLXPlayerChatEventHandler(void)
{
}

void GLXPlayerChatEventHandler::handleMessageEvent( const JID& from, MessageEventType event_type )
{
	XP_DEBUG_OUT( "[GLXPlayerChatEventHandler] handleMessageEvent %d from %s\n", 
		event_type, (char *)from.full().c_str() );
}