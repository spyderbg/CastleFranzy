#ifndef __GL_XPLAYER_CHAT_ROSTER_H__
#define __GL_XPLAYER_CHAT_ROSTER_H__

#include "GLXPlayerChatRosterItem.h"
#include <map>
#include <string>
#include <vector>

/**
 * This class is designed to represent the roster list.
 *
 * There is a roster item list maintained in it.
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
namespace gllive
{
	class GLXPlayerChatRoster
		{
			
		public:
			
			GLXPlayerChatRoster(void);
			
			~GLXPlayerChatRoster(void);
			
			/**
			 * The count of the contact in the roster.
			 *
			 */
			int Count();
			
			/**
			 * Add new contact.
			 *
			 */
			GLXPlayerChatRosterItem * Add( char * jid, char * name, char * group );
			
			/**
			 * Search the contact by order.
			 *
			 */
			GLXPlayerChatRosterItem * Get( int index );
			
			/**
			 * Search the contact by jid.
			 *
			 */
			GLXPlayerChatRosterItem * Find( char * jid );
			
			/**
			 * Delete a contact from the roster.
			 *
			 */
			bool Remove( char * jid );
			
			int GetGroupCount();
			
			char * GetGroupName( int index );
			
			char * GetNickname( char * jid );
			
			int GetUserCountInGroup( char * group_name );
			
			GLXPlayerChatRosterItem * GetUserInGroup( char * group_name, int index );
			
			bool AddGroup( char * group );
			
			bool DeleteGroup( char * group );
			
		private:
			
			bool IsGroupExisted( char * group );
			
			typedef std::map< std::string, GLXPlayerChatRosterItem * > MAP_ROSTER;
			typedef std::map< std::string, GLXPlayerChatRosterItem * >::iterator IT_MAP_ROSTER;
			
			MAP_ROSTER m_list;
			
			std::vector < std::string > m_group_list;
			std::vector < std::string >::iterator _it_group;
		};
}

#endif