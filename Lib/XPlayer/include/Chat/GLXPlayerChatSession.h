#ifndef __GL_XPLAYER_CHAT_SESSION_H__
#define __GL_XPLAYER_CHAT_SESSION_H__

#include "GLXPlayerChatEventHandler.h"
#include "GLXPlayerChatStateHandler.h"
#include "GLXPlayerChatMessageHandler.h"
#include "gloox/MessageEventFilter.h"
#include "gloox/ChatStateFilter.h"

class GLXPlayerChatEventHandler;
class GLXPlayerChatStateHandler;
class GLXPlayerChatMessageHandler;

using namespace gloox;

class GLXPlayerChatSession
{

public:

	MessageSession		* pMessageSession;
	MessageEventFilter	* pMessageEventFilter;
	ChatStateFilter		* pChatStateFilter;

	GLXPlayerChatEventHandler	* m_pEventHandler;
	GLXPlayerChatStateHandler	* m_pStateHandler;
	GLXPlayerChatMessageHandler	* m_pMessageHandler;

};

#endif