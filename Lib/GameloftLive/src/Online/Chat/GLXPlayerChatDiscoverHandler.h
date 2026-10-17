#ifndef __GL_XPLAYER_CHAT_DISCOVER_HANDLER_H__
#define __GL_XPLAYER_CHAT_DISCOVER_HANDLER_H__

#include "gloox/DiscoHandler.h"
#include "GLXPlayerChat.h"

namespace gllive
{
	class GLXPlayerChat;

	class GLXPlayerChatDiscoverHandler :
		public DiscoHandler
		{
			
		public:
			
			GLXPlayerChatDiscoverHandler( GLXPlayerChat * pClient );
			
			~GLXPlayerChatDiscoverHandler(void);
			
			// DiscoHandler
			void handleDiscoInfoResult( Stanza *stanza, int context );
			
			void handleDiscoItemsResult( Stanza *stanza, int context );
			
			void handleDiscoError( Stanza *stanza, int context );
			
		private:
			
			GLXPlayerChat * m_pClient;
			
		};
}

#endif