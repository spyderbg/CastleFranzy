#include "GLXPlayerChatRoster.h"
#include "memory.h"

GLXPlayerChatRoster::GLXPlayerChatRoster(void)
{
}

GLXPlayerChatRoster::~GLXPlayerChatRoster(void)
{
	for ( IT_MAP_ROSTER it = m_list.begin(); it != m_list.end(); it++ )
	{
		GLXPlayerChatRosterItem * item = (*it).second;
		XP_API_DEL( item );
	}

	m_list.clear();
	m_group_list.empty();
}

GLXPlayerChatRosterItem * GLXPlayerChatRoster::Add( char * jid, char * name, char * group )
{
	if ( !IsGroupExisted( group ) )
	{
		std::string group_name = group;
		m_group_list.push_back( group_name );
	}
	
	GLXPlayerChatRosterItem * item = NULL;
	item = Find( jid );
	if ( NULL == item )
	{
		std::string id = jid;
		m_list[ id ] = XP_API_NEW GLXPlayerChatRosterItem( jid, name, group );
		return m_list[ id ];
	}
	else
	{
		item->AddGroup( group );
		return item;
	}
}

int GLXPlayerChatRoster::GetGroupCount()
{
	return m_group_list.size();
}

char * GLXPlayerChatRoster::GetGroupName( int index )
{
	int count = 0;
	for ( _it_group = m_group_list.begin(); _it_group != m_group_list.end(); _it_group++ )
	{
		if ( count == index )
		{
			return (char *)(*_it_group).c_str();
		}
		count++;
	}

	return NULL;
}

int GLXPlayerChatRoster::Count()
{
	return m_list.size();
}

char * GLXPlayerChatRoster::GetNickname( char * jid )
{
	GLXPlayerChatRosterItem * item = Find( jid );
	if ( NULL == item )
	{
		return jid;
	}

	return item->GetName();
}

GLXPlayerChatRosterItem * GLXPlayerChatRoster::Get( int index )
{	
	int count = 0;
	IT_MAP_ROSTER it;

	for ( it = m_list.begin(); it != m_list.end(); it++ )
	{
		if ( count == index)
		{
			break;
		}
		count ++;
	}

	if ( it == m_list.end() )
	{
		return NULL;
	}

	return (*it).second;
}

int GLXPlayerChatRoster::GetUserCountInGroup( char * group_name )
{
	int count = 0;

	for ( IT_MAP_ROSTER it = m_list.begin(); it != m_list.end(); it++ )
	{
		GLXPlayerChatRosterItem * item = (*it).second;
		if ( item->IsInGroup( group_name ) )
		{
			count++;
		}
	}

	return count;
}

GLXPlayerChatRosterItem * GLXPlayerChatRoster::GetUserInGroup( char * group_name, int index )
{
	int count = 0;

	for ( IT_MAP_ROSTER it = m_list.begin(); it != m_list.end(); it++ )
	{
		GLXPlayerChatRosterItem * item = (*it).second;
		if ( item->IsInGroup( group_name ) )
		{
			if ( count == index )
			{
				return item;
			}
			else
			{
				count++;
			}
		}
	}

	return NULL;
}

GLXPlayerChatRosterItem * GLXPlayerChatRoster::Find( char * jid )
{
	std::string id = jid;
	IT_MAP_ROSTER it = m_list.find( jid );
	if ( it == m_list.end() )
	{
		return NULL;
	}

	return (*it).second;
}

bool GLXPlayerChatRoster::Remove( char * jid )
{
	std::string id = jid;
	IT_MAP_ROSTER it = m_list.find( jid );
	if ( it == m_list.end() )
	{
		return false;
	}
	GLXPlayerChatRosterItem * item = (*it).second;
	XP_API_DEL( item );
	m_list.erase( it );
	return true;
}

bool GLXPlayerChatRoster::AddGroup( char * group )
{
	if ( IsGroupExisted( group ) )
	{
		return false;
	}

	std::string group_name = group;
	m_group_list.push_back( group_name );
	return true;
}

bool GLXPlayerChatRoster::DeleteGroup( char * group )
{
	if ( !IsGroupExisted( group ) )
	{
		return false;
	}

	for ( _it_group = m_group_list.begin(); _it_group != m_group_list.end(); _it_group++ )
	{
		if ( XP_API_STRCMP( (*_it_group).c_str(), group ) == 0 )
		{
			break;
		}
	}

	for ( IT_MAP_ROSTER it = m_list.begin(); it != m_list.end(); it++ )
	{
		GLXPlayerChatRosterItem * item = (*it).second;
		item->DeleteGroup( group );
	}	

	m_group_list.erase( _it_group );
	return true;
}

/////////////////////////////
// Private functions
/////////////////////////////
bool GLXPlayerChatRoster::IsGroupExisted( char * group )
{
	bool bGroupExisted = false;
	_it_group = m_group_list.begin();
	while( _it_group != m_group_list.end() )
	{
		if ( XP_API_STRCMP( (*_it_group).c_str(), group ) == 0 )
		{
			bGroupExisted = true;
			break;
		}
		_it_group++;
	}

	return bGroupExisted;
}