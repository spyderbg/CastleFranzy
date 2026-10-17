#include "../../../include/chat/GLXPlayerChatStateHandler.h"

GLXPlayerChatStateHandler::GLXPlayerChatStateHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{

}

GLXPlayerChatStateHandler::~GLXPlayerChatStateHandler(void)
{

}

void GLXPlayerChatStateHandler::handleChatState( const JID& from, ChatStateType state )
{
	XP_DEBUG_OUT( "[GLXPlayerChatStateHandler] handleChatState %d from %s\n", 
		state, (char *)from.full().c_str() );	
}