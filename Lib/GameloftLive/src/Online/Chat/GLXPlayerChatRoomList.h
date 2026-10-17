#ifndef __GL_XPLAYER_CHAT_ROOM_LIST_H__
#define __GL_XPLAYER_CHAT_ROOM_LIST_H__

#include "GLXPlayerChatRoom.h"
#include <vector>

namespace gllive
{
	class GLXPlayerChatRoomList
		{
			
		public:
			
			GLXPlayerChatRoomList(void);
			
			~GLXPlayerChatRoomList(void);
			
			/**
			 * 
			 *
			 */
			GLXPlayerChatRoom * Find( const char * jid );
			
			bool Add( const char * jid, GLXPlayerChatRoom * room );
			
			int Count();
			
			GLXPlayerChatRoom * Get( int index );
			
			void Clear();
			
			GLXPlayerChatRoom* Remove(const char *jid);
			
		private:
			
			typedef std::map< std::string, GLXPlayerChatRoom * > MAP_CHAT_ROOM;
			typedef std::map< std::string, GLXPlayerChatRoom * >::iterator IT_MAP_CHAT_ROOM;
			
			MAP_CHAT_ROOM m_list;
			
		};
}

#endif