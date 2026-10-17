#ifndef __GL_XPLAYER_CHAT_STATE_HANDLER_H__
#define __GL_XPLAYER_CHAT_STATE_HANDLER_H__

#include "gloox/ChatStateHandler.h"
#include "GLXPlayerChat.h"

namespace gllive
{
	class GLXPlayerChat;
	class GLXPlayerChatStateHandler :
		public ChatStateHandler
		{
			
		public:
			
			GLXPlayerChatStateHandler( GLXPlayerChat * pClient );
			
			~GLXPlayerChatStateHandler(void);
			
			void handleChatState( const JID& from, ChatStateType state );
			
		private:
			
			GLXPlayerChat * m_pClient;
			
		};
}

#endif