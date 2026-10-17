#include "GLXPlayerChatRoomHandler.h"
#include "gloox/DataForm.h"
#include "memory.h"

GLXPlayerChatRoomHandler::GLXPlayerChatRoomHandler( GLXPlayerChat * pClient )
	: m_pClient( pClient )
{

}

GLXPlayerChatRoomHandler::~GLXPlayerChatRoomHandler(void)
{

}

/////////////////////////////
// MUCInvitationHandler
/////////////////////////////
void GLXPlayerChatRoomHandler::handleMUCInvitation( 
	const JID& room, const JID& invitee, const std::string& reason,
	const std::string& body, const std::string& password,
    bool cont,const std::string& realInvitor)
{
	XP_DEBUG_OUT( "[GLXPlayerChatMUCHandler] handleMUCInvitation\n" );
	m_pClient->OnChatMUCInvitation(room,invitee,reason,body,password,cont,realInvitor);
}

/////////////////////////////
// MUCRoomHandler
/////////////////////////////
void GLXPlayerChatRoomHandler::handleMUCParticipantPresence( MUCRoom * room, 
	const MUCRoomParticipant participant, Presence presence )
{
	if( presence == PresenceAvailable )
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCParticipantPresence ! %s is in the room, too\n", 
			participant.nick->resource().c_str() );
	}
	else if( presence == PresenceUnavailable )
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCParticipantPresence ! %s left the room\n", 
			participant.nick->resource().c_str() );
	}
	else
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCParticipantPresence Presence is %d of %s\n", 
			presence, participant.nick->resource().c_str() );
	}

	m_pClient->OnChatMUCParticipantPresence( room, participant, presence );
}

void GLXPlayerChatRoomHandler::handleMUCMessage( MUCRoom* room, const std::string& nick, 
	const std::string& message,	bool history, const std::string& when, bool priv )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCMessage %s said: '%s' (history: %s, private: %s)\n", nick.c_str(), message.c_str(),
        history ? "yes" : "no", priv ? "yes" : "no" );

	m_pClient->OnChatMUCMessage( room, nick, message, history, when, priv );
}

void GLXPlayerChatRoomHandler::handleMUCSubject( MUCRoom * /*room*/, const std::string& nick, 
	const std::string& subject )
{
	if( nick.empty() )
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCSubject Subject: %s\n", subject.c_str() );
	}
	else
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCSubject %s has set the subject to: '%s'\n", nick.c_str(), subject.c_str() );
	}
}

void GLXPlayerChatRoomHandler::handleMUCError( MUCRoom * /*room*/, StanzaError error )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCError !!!!!!!!got an error: %d", error );
}

void GLXPlayerChatRoomHandler::handleMUCInfo( MUCRoom * /*room*/, int features, const std::string& name,
	const DataForm *infoForm )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCInfo features: %d, name: %s, form xml: %s\n", 
		features, name.c_str(), infoForm->tag()->xml().c_str() );
}

void GLXPlayerChatRoomHandler::handleMUCItems( MUCRoom * /*room*/, const StringMap& items )
{
	StringMap::const_iterator it = items.begin();
	for( ; it != items.end(); ++it )
	{
		XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCItems %s -- %s is an item here\n", 
			(*it).first.c_str(), (*it).second.c_str() );
	}
}

void GLXPlayerChatRoomHandler::handleMUCInviteDecline( MUCRoom * room, const JID& invitee, 
	const std::string& reason )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCInviteDecline Invitee %s declined invitation. reason given: %s\n", 
		invitee.full().c_str(), reason.c_str() );
	m_pClient->OnChatMUCInviteDecline( room, invitee, reason );
}

bool GLXPlayerChatRoomHandler::handleMUCRoomCreation( MUCRoom *room )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCRoomCreation %s didn't exist, beeing created.\n", 
		room->name().c_str() );
	return true;
}

/////////////////////////////
// MUCRoomConfigHandler
/////////////////////////////
void GLXPlayerChatRoomHandler::handleMUCConfigList( MUCRoom *room, const MUCListItemList& items, MUCOperation operation )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCConfigList\n" );
}

void GLXPlayerChatRoomHandler::handleMUCConfigForm( MUCRoom *room, const DataForm& form )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCConfigForm\n" );
}

void GLXPlayerChatRoomHandler::handleMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCConfigResult\n" );
	m_pClient->OnChatMUCConfigResult( room, success, operation );
}

void GLXPlayerChatRoomHandler::handleMUCRequest( MUCRoom *room, const DataForm& form )
{
	XP_DEBUG_OUT( "[GLXPlayerChatRoomHandler] handleMUCRequest\n" );
}
