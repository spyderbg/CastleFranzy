#include "GLXPlayerChatDiscoverHandler.h"

GLXPlayerChatDiscoverHandler::GLXPlayerChatDiscoverHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{
}

GLXPlayerChatDiscoverHandler::~GLXPlayerChatDiscoverHandler(void)
{
}

void GLXPlayerChatDiscoverHandler::handleDiscoInfoResult( Stanza *stanza, int context )
{
	XP_DEBUG_OUT( "[GLXPlayerChatDiscoverHandler] handleDiscoInfoResult\n" );
}

void GLXPlayerChatDiscoverHandler::handleDiscoItemsResult( Stanza *stanza, int context )
{
	XP_DEBUG_OUT( "[GLXPlayerChatDiscoverHandler] handleDiscoItemsResult\n" );
	
	Tag::TagList quli = stanza->children();
	Tag *queli = *(quli.begin());
	Tag::TagList tli = queli->children();
	Tag::TagList::iterator it = tli.begin();
	for ( ; it!=tli.end(); it++ )
	{
		Tag *tag = *it;        
		std::string jid = tag->findAttribute("jid");
		std::string name = tag->findAttribute("name");
		m_pClient->OnChatMUCDiscover( jid, name );
	}
}

void GLXPlayerChatDiscoverHandler::handleDiscoError( Stanza *stanza, int context )
{
	XP_DEBUG_OUT( "[GLXPlayerChatDiscoverHandler] handleDiscoError\n" );
}