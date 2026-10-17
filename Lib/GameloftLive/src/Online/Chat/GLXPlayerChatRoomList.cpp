#include "GLXPlayerChatRoomList.h"

GLXPlayerChatRoomList::GLXPlayerChatRoomList(void)
{

}

GLXPlayerChatRoomList::~GLXPlayerChatRoomList(void)
{
	Clear();
}

void GLXPlayerChatRoomList::Clear()
{
	for ( IT_MAP_CHAT_ROOM it = m_list.begin(); it != m_list.end(); it++ )
	{
		XP_API_DEL( (*it).second );
	}

	m_list.clear();
}

GLXPlayerChatRoom * GLXPlayerChatRoomList::Find( const char * jid )
{
	std::string room_jid = jid;
	IT_MAP_CHAT_ROOM it = m_list.find( room_jid );
	if ( it == m_list.end() )
	{
		return NULL;
	}
	else
	{
		return m_list[ room_jid ];
	}
}

bool GLXPlayerChatRoomList::Add( const char * jid, GLXPlayerChatRoom * room )
{
	std::string room_jid = jid;
	m_list[ room_jid ] = room;
	return true;
}

int GLXPlayerChatRoomList::Count()
{
	return m_list.size();
}

GLXPlayerChatRoom * GLXPlayerChatRoomList::Get( int index )
{
	int count = 0;
	for ( IT_MAP_CHAT_ROOM it = m_list.begin(); it != m_list.end(); it++ )
	{
		if ( index == count )
		{
			return (*it).second;
		}
		count++;
	}

	return NULL;
}

GLXPlayerChatRoom* GLXPlayerChatRoomList::Remove(const char *jid)
{
	GLXPlayerChatRoom* nextRoom = NULL;
	
	std::string room_jid = jid;
	IT_MAP_CHAT_ROOM it = m_list.find( room_jid );
	IT_MAP_CHAT_ROOM it_tmp = it;
	if ( it == m_list.end() )
	{
		return NULL;
	}

	XP_API_DEL(it->second);
	IT_MAP_CHAT_ROOM nextIt = ++it;
	m_list.erase(it_tmp);

	if(nextIt == m_list.end())
	{
		if(m_list.size() > 0)
		{
			nextIt = m_list.begin();
		}
		else
		{
			return NULL;
		}
	}

	return nextIt->second;
}