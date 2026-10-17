#include "GLXPlayerChatRosterItem.h"
#include "memory.h"

GLXPlayerChatRosterItem::GLXPlayerChatRosterItem( const char * jid, const char * nickname, const char * group_name )
{
	m_jid = XP_API_STRNEW( (char *)jid );
	m_name = XP_API_STRNEW( (char *)nickname );
	m_state = GL_XPLAYER_CHAT_STATE_OFFLINE;
	std::string group = group_name;
	m_group.push_back( group );
}

GLXPlayerChatRosterItem::~GLXPlayerChatRosterItem(void)
{
	XP_API_DELETE_ARRAY( m_jid );
	XP_API_DELETE_ARRAY( m_name );
	m_group.empty();
}

void GLXPlayerChatRosterItem::PresenceStateChanged( int state )
{
	switch( state )
	{
	case PresenceUnknown:
		m_state = GL_XPLAYER_CHAT_STATE_OFFLINE;
		break;
	case PresenceAvailable:
		m_state = GL_XPLAYER_CHAT_STATE_ONLINE;
		break;
	case PresenceChat:
		m_state = GL_XPLAYER_CHAT_STATE_CHAT;
		break;
	case PresenceAway:
		m_state = GL_XPLAYER_CHAT_STATE_AWAY;
		break;
	case PresenceDnd:
		m_state = GL_XPLAYER_CHAT_STATE_BUSY;
		break;
	case PresenceXa:
		m_state = GL_XPLAYER_CHAT_STATE_AWAY;
		break;
	case PresenceUnavailable:
		m_state = GL_XPLAYER_CHAT_STATE_OFFLINE;
		break;
	default:
		m_state = GL_XPLAYER_CHAT_STATE_OFFLINE;
		break;
	}
}

bool GLXPlayerChatRosterItem::AddGroup( char * group_name )
{
	bool bExisted = false;
	_it = m_group.begin();
	while( _it != m_group.end() )
	{
		if ( XP_API_STRCMP( group_name, (*_it).c_str() ) == 0 )
		{
			bExisted = true;
			break;
		}
		_it++;
	}

	if ( bExisted )
	{
		return false;
	}

	std::string group = group_name;
	m_group.push_back( group );
	return true;
}

bool GLXPlayerChatRosterItem::IsInGroup( char * group_name )
{
	bool bExisted = false;
	_it = m_group.begin();
	while( _it != m_group.end() )
	{
		if ( XP_API_STRCMP( group_name, (*_it).c_str() ) == 0 )
		{
			bExisted = true;
			break;
		}
		_it++;
	}

	return bExisted;
}

bool GLXPlayerChatRosterItem::DeleteGroup( char * group_name )
{
	if ( !IsInGroup( group_name ) )
	{
		return false;
	}

	if ( IsInGroup( " " ) )
	{
		return true;
	}

	for ( _it = m_group.begin(); _it != m_group.end(); _it++ )
	{
		if ( XP_API_STRCMP( group_name, (*_it).c_str() ) == 0 )
		{
			break;
		}
	}

	m_group.erase( _it );
	std::string group = " ";
	m_group.push_back( group );

	return true;
}