#include "../../../include/chat/GLXPlayerChatMessageHandler.h"
#include "../../../include/Porting.h"

GLXPlayerChatMessageHandler::GLXPlayerChatMessageHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{
}

GLXPlayerChatMessageHandler::~GLXPlayerChatMessageHandler(void)
{
}

void GLXPlayerChatMessageHandler::handleMessage( Stanza *stanza, MessageSession *session )
{
	XP_DEBUG_OUT( "[GLXPlayerChatMessageHandler] handleMessage type=%d subject=%s message=%s\n", 
		stanza->subtype(), stanza->subject().c_str(), stanza->body().c_str() );

	std::string jid = "";
	jid += stanza->from().username();
	jid += "@";
	jid += stanza->from().server();

	m_pClient->OnChatMessageArrived( (char *)jid.c_str(), (char *)stanza->body().c_str() );
}