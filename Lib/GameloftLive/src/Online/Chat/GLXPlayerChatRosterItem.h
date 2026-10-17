#ifndef __GL_XPLAYER_CHAT_ROSTER_ITEM_H__
#define __GL_XPLAYER_CHAT_ROSTER_ITEM_H__

#include "gloox/gloox.h"
#include "../Porting.h"

namespace gllive
{
	enum GLXPlayerChatRosterItemState
	{
		GL_XPLAYER_CHAT_STATE_OFFLINE = 1,
		GL_XPLAYER_CHAT_STATE_ONLINE = 2,
		GL_XPLAYER_CHAT_STATE_CHAT = 3,
		GL_XPLAYER_CHAT_STATE_AWAY = 4,
		GL_XPLAYER_CHAT_STATE_BUSY = 5
	};
	
	/**
	 * This class is designed to represent an item in the roster.
	 *
	 * There are some information in an item:
	 *	jid:	the unique identifier
	 *	name:	the nickname to show
	 *	group:	the group name
	 *	state:	online, offline, away, busy and so on
	 *
	 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
	 *
	 */
	class GLXPlayerChatRosterItem
		{
			
		public:
			
			GLXPlayerChatRosterItem( const char * jid, const char * name, const char * group );
			
			~GLXPlayerChatRosterItem(void);
			
			char * GetJid()
			{
				return m_jid;
			}
			
			char * GetName()
			{
				return m_name;
			}
			
			bool AddGroup( char * group_name );
			
			GLXPlayerChatRosterItemState GetState()
			{
				return m_state;
			}
			
			void PresenceStateChanged( int state );
			
			void SetState( int state )
			{
				m_state = (GLXPlayerChatRosterItemState)state;
			}
			
			bool IsInGroup( char * group_name );
			
			bool DeleteGroup( char * group_name );
			
		private:
			
			GLXPlayerChatRosterItemState m_state;
			
			char * m_jid;
			
			char * m_name;
			
			StringList m_group;
			StringList::iterator _it;
		};
}

#endif