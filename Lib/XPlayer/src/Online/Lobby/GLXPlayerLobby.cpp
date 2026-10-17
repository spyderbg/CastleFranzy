#include "../../../include/Lobby/GLXPlayerLobby.h"
//#include "memory.h"

/**
*  Constructor.
* Create an instance of GLXPlayerLobby.
*/
GLXPlayerLobby::GLXPlayerLobby( ): GLXPlayerWebComponent()
{	
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );	
	
	m_lobbyServerAdd = NULL;
	m_lobbyServerPort = 0;
}

/**
* GLXPlayerLobby destructor.
* Destroy an instance of GLXPlayerLobby.
*/
GLXPlayerLobby::~GLXPlayerLobby(void)
{		
	XP_API_DEL(m_lobbyServerAdd);
	m_lobbyServerPort = 0;
}

/**
* Private function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerLobby::OnUpdateSuccess( int func_id )
{	
	switch(func_id)
	{			
		case FUNCTION_GET_LOBBY_SERVER:		
		case FUNCTION_GAME_INVITATION_GET_LOBBY:
			ProcessServerAddress(m_pResponsePtr);
			break;		
		default:
			break;
	}
	return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
}

/**
* Function sendGetLobbyServer.
* Get a lobby server 
* 
* @return success: |r|s|address:port
* @return fail: |r|e|error code
*/
bool GLXPlayerLobby::sendGetLobbyServer()
{	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_LOBBY_SERVER, m_ggi, m_uid);
	XP_DEBUG_OUT("GLXPlayerLobby::sendGetLobbyServer -> buffer = %s\n", buffer);
	return SendByGet(buffer);
}

/**
* Function sendGameInvitationGetLobby.
* Get a lobby server address for game invitation
* 
* @param inviteName A username who invite me
* @return success: |r|s|address:port
* @return fail: |r|e|error code
*/
bool GLXPlayerLobby::sendGameInvitationGetLobby(char* inviteName)
{	
	if(inviteName == NULL || XP_API_STRLEN(inviteName) <= 0)
	{
		m_observer->OnRequestFailure(FUNCTION_GAME_INVITATION_GET_LOBBY, ERROR_INIT );
		return false;
	}
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|cn|%s|", FUNCTION_GAME_INVITATION_GET_LOBBY, m_ggi, m_uid, inviteName);
	XP_DEBUG_OUT("GLXPlayerLobby::sendGameInvitationGetLobby -> buffer = %s\n", buffer);
	return SendByGet(buffer);
}

/**
* Private function ProcessServerAddress.
* Get server address and server port when receive from PHP server.
*
* @param data Data returned from the server.
*/
void GLXPlayerLobby::ProcessServerAddress(char* data)
{
	XP_API_DEL(m_lobbyServerAdd);
	m_lobbyServerPort = 0;
	
	if(data == NULL || XP_API_STRLEN(data) <= 0)
	{
		return;
	}
	
	char tmp[128];
	
	XP_API_MEMSET(tmp, 0, 128);
	getValue(data, tmp, 0, ':');	
	m_lobbyServerAdd = XP_API_STRNEW(tmp);
	
	XP_API_MEMSET(tmp, 0, 128);
	getValue(data, tmp, 1, ':');	
	m_lobbyServerPort = XP_API_ATOI(tmp);	
}