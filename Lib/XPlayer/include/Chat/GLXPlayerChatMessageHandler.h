#ifndef __GL_XPLAYER_CHAT_MESSAGE_HANDLER_H__
#define __GL_XPLAYER_CHAT_MESSAGE_HANDLER_H__

#include "gloox/MessageHandler.h"
#include "GLXPlayerChat.h"

class GLXPlayerChat;

using namespace gloox;

/**
 * This class is designed to handle message.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerChatMessageHandler :
	public MessageHandler
{

public:

	GLXPlayerChatMessageHandler( GLXPlayerChat * pClient );

	~GLXPlayerChatMessageHandler(void);

	void handleMessage( Stanza *stanza, MessageSession *session = 0 );

private:
	
	GLXPlayerChat * m_pClient;

};

#endif