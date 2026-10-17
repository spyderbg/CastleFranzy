#include "../../../include/chat/GLXPlayerChatRoom.h"
//#include "memory.h"

GLXPlayerChatRoom::GLXPlayerChatRoom( GLXPlayerChat * pClient, std::string &room_jid, 
	std::string &room_name, std::string &user_name )
	: m_pClient( pClient )
{
	m_jid = XP_API_STRNEW( (char *)room_jid.c_str() );
	m_name = XP_API_STRNEW( (char *)room_name.c_str() );
	JID nick( room_jid + "/" + user_name );
	m_room = XP_API_NEW MUCRoom( m_pClient->GetClient(), nick,
		(MUCRoomHandler *)m_pClient->GetRoomHandler(), 
		(MUCRoomConfigHandler *)m_pClient->GetRoomHandler() );
}

GLXPlayerChatRoom::~GLXPlayerChatRoom(void)
{
	XP_API_DEL( m_name );
	XP_API_DEL( m_jid );
	XP_API_DEL( m_room );
	for ( _it = m_user_list.begin(); _it != m_user_list.end(); _it++ )
	{
		GLXPlayerChatRoomParticipant * participant = (*_it);
		XP_API_DEL( participant->name );
		XP_API_DEL( participant );
	}
	m_user_list.empty();
}

void GLXPlayerChatRoom::SetName( char * name )
{
	XP_API_DEL( m_name );
	m_name = XP_API_STRNEW( name );
}

GLXPlayerChatRoomParticipant * GLXPlayerChatRoom::FindParticipant( const char * name )
{
	for ( _it = m_user_list.begin(); _it != m_user_list.end(); _it++ )
	{
		GLXPlayerChatRoomParticipant * participant = (*_it);
		if ( XP_API_STRCMP( name, participant->name ) == 0 )
		{
			return participant;
		}
	}

	return NULL;
}

bool GLXPlayerChatRoom::RemoveParticipant( const char * name )
{
	for ( _it = m_user_list.begin(); _it != m_user_list.end(); _it++ )
	{
		GLXPlayerChatRoomParticipant * participant = (*_it);
		if ( XP_API_STRCMP( name, participant->name ) == 0 )
		{
			XP_API_DEL( participant->name );
			XP_API_DEL( participant );			
			m_user_list.erase( _it );
			return true;
		}
	}

	return false;
}

int GLXPlayerChatRoom::GetParticipantCount()
{
	return m_user_list.size();
}

GLXPlayerChatRoomParticipant * GLXPlayerChatRoom::GetParticipant( int index )
{
	int count = 0;
	for ( _it = m_user_list.begin(); _it != m_user_list.end(); _it++ )
	{
		GLXPlayerChatRoomParticipant * participant = (*_it);
		if ( index == count )
		{
			return participant;
		}
		count++;
	}

	return NULL;
}

void GLXPlayerChatRoom::OnParticipantPresence( const char * name, int state )
{
	switch( state )
	{
	case GL_XPLAYER_PARTICIPANT_ONLINE:
		{
			GLXPlayerChatRoomParticipant * participant = FindParticipant( name );
			if ( NULL == participant )
			{
				participant = XP_API_NEW GLXPlayerChatRoomParticipant();
				participant->name = XP_API_STRNEW( (char *)name );
				participant->state = state;
				m_user_list.push_back( participant );
			}
			else
			{
				participant->state = state;
			}
		}
		break;
	case GL_XPLAYER_PARTICIPANT_CHAT:
	case GL_XPLAYER_PARTICIPANT_AWAY:
	case GL_XPLAYER_PARTICIPANT_BUSY:
		{
			GLXPlayerChatRoomParticipant * participant = FindParticipant( name );
			if ( NULL != participant )
			{
				participant->state = state;
			}
		}
		break;
	case GL_XPLAYER_PARTICIPANT_OFFLINE:
		{
			RemoveParticipant( name );
		}
		break;
	default:
		break;
	}
}

void GLXPlayerChatRoom::Join()
{
	m_room->join();
//	m_room->getRoomInfo();
//	m_room->getRoomItems();
}

void GLXPlayerChatRoom::Leave()
{
	if(m_pClient->IsConnected())
	{
		m_room->leave();
	}
	else
	{
		m_room->disconnectLeave();
	}
}

void GLXPlayerChatRoom::Invite( char * invitee, char * reason )
{
	char jid_buffer[128] = { 0 };
	XP_API_MEMSET( jid_buffer, 0, 128 );
	XP_API_SPRINTF( jid_buffer, "%s@%s", invitee, m_pClient->GetDomain() );

	JID jid( jid_buffer );
	std::string str_reason = reason;
	m_room->invite( jid, str_reason, false );
}

void GLXPlayerChatRoom::Decline( char * room, char * invitor, char * invitee )
{
	std::string rooom_str = room;
	std::string resource_str = invitee;
	std::string invitor_str = invitor;

	JID room_jid( rooom_str );
	JID sender_jid( invitor_str );

	room_jid.setResource( resource_str );
	Stanza * response = m_room->declineInvitation( room_jid, sender_jid );
	m_pClient->GetClient()->send( response );
}

bool GLXPlayerChatRoom::SendChatMessage( char * message )
{
	std::string msg = message;
	m_room->send( msg );
	return true;
}