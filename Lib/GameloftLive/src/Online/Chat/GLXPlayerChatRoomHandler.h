#ifndef __GL_XPLAYER_CHAT_MUC_HANDLER_H__
#define __GL_XPLAYER_CHAT_MUC_HANDLER_H__

#include "gloox/MUCInvitationHandler.h"
#include "gloox/MUCRoomHandler.h"
#include "gloox/MUCRoomConfigHandler.h"

#include "GLXPlayerChat.h"

namespace gllive
{
	class GLXPlayerChat;
	
	class GLXPlayerChatRoomHandler : public MUCInvitationHandler, MUCRoomHandler, MUCRoomConfigHandler
	{
		
	public:
		
		GLXPlayerChatRoomHandler( GLXPlayerChat * pClient );
		
		~GLXPlayerChatRoomHandler(void);
		
		// MUCInvitationHandler
		void handleMUCInvitation( const JID& room, const JID& invitee, const std::string& reason,
								 const std::string& body, const std::string& password,
        bool cont,const std::string& realInvitor);
		
		// MUCRoomHandler
		void handleMUCParticipantPresence( MUCRoom * /*room*/, const MUCRoomParticipant participant,
										  Presence presence );
		
		void handleMUCMessage( MUCRoom* room, const std::string& nick, const std::string& message,
							  bool history, const std::string& when, bool priv );
		
		void handleMUCSubject( MUCRoom * /*room*/, const std::string& nick, const std::string& subject );
		
		void handleMUCError( MUCRoom * /*room*/, StanzaError error );
		
		void handleMUCInfo( MUCRoom * /*room*/, int features, const std::string& name, const DataForm *infoForm );
		
		void handleMUCItems( MUCRoom * /*room*/, const StringMap& items );
		
		void handleMUCInviteDecline( MUCRoom * /*room*/, const JID& invitee, const std::string& reason );
		
		bool handleMUCRoomCreation( MUCRoom *room );
		
		// MUCRoomConfigHandler
		void handleMUCConfigList( MUCRoom *room, const MUCListItemList& items, MUCOperation operation );
		
		void handleMUCConfigForm( MUCRoom *room, const DataForm& form );
		
		void handleMUCConfigResult( MUCRoom *room, bool success, MUCOperation operation );
		
		void handleMUCRequest( MUCRoom *room, const DataForm& form );
		
	private:
		
		GLXPlayerChat * m_pClient;
		
	};
}

#endif