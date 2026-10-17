#ifndef __GL_XPLAYER_CHAT_EVENT_HANDLER_H__
#define __GL_XPLAYER_CHAT_EVENT_HANDLER_H__

#include "gloox/MessageEventHandler.h"
#include "GLXPlayerChat.h"

class GLXPlayerChat;

using namespace gloox;

/**
 * This class is designed to handle message event.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerChatEventHandler :
	public MessageEventHandler
{

public:

	GLXPlayerChatEventHandler( GLXPlayerChat * pClient );

	~GLXPlayerChatEventHandler(void);

	void handleMessageEvent( const JID& from, MessageEventType event );

private:
	
	GLXPlayerChat * m_pClient;

};

#endif