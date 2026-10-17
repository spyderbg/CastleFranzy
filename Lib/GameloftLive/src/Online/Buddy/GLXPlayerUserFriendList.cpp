#include "GLXPlayerUserFriendList.h"

GLXPlayerUserFriendList::GLXPlayerUserFriendList(void)
{

}

GLXPlayerUserFriendList::~GLXPlayerUserFriendList(void)
{
	Clear();
}

void GLXPlayerUserFriendList::Clear()
{
	for ( IT_MAP_FRIEND it = m_list.begin(); it != m_list.end(); it++ )
	{
		XP_API_DEL( (*it).second );
	}

	m_list.clear();
}

GLXPlayerUserFriend * GLXPlayerUserFriendList::Find( char * name )
{
	std::string friend_name = name;
	IT_MAP_FRIEND it = m_list.find( friend_name );
	if ( it == m_list.end() )
	{
		return NULL;
	}
	else
	{
		return m_list[ friend_name ];
	}
}

bool GLXPlayerUserFriendList::Add( char * name, GLXPlayerUserFriend * user_friend )
{
	std::string friend_name = name;
	m_list[ friend_name ] = user_friend;
	return true;
}

int GLXPlayerUserFriendList::Count()
{
	return m_list.size();
}

GLXPlayerUserFriend * GLXPlayerUserFriendList::Get( int index )
{
	int count = 0;
	for ( IT_MAP_FRIEND it = m_list.begin(); it != m_list.end(); it++ )
	{
		if ( index == count )
		{
			return (*it).second;
		}
		count++;
	}

	return NULL;
}