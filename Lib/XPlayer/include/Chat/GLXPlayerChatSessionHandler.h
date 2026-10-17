#ifndef __GL_XPLAYER_CHAT_SESSION_HANDLER_H__
#define __GL_XPLAYER_CHAT_SESSION_HANDLER_H__

#include "gloox/messagesessionhandler.h"
#include "GLXPlayerChat.h"
#include "GLXPlayerChatSession.h"
#include <map>
#include <string>

class GLXPlayerChat;
class GLXPlayerChatSession;

using namespace gloox;

class GLXPlayerChatSessionHandler :
	public gloox::MessageSessionHandler
{

public:

	GLXPlayerChatSessionHandler( GLXPlayerChat * pClient );

	~GLXPlayerChatSessionHandler(void);

	void handleMessageSession( MessageSession *session );

	void SendChatMessage( char * jid, char * msg );

private:

	GLXPlayerChat * m_pClient;

	typedef std::map< std::string, GLXPlayerChatSession > MAP_SESSION;
	typedef std::map< std::string, GLXPlayerChatSession >::iterator IT_MAP_SESSION;
	
	MAP_SESSION m_mapSession;

};

#endif