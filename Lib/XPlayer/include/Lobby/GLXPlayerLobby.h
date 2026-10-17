#ifndef __GL_XPLAYER_LOBBY_H__
#define __GL_XPLAYER_LOBBY_H__

#include "../Porting.h"
#include "../Common/GLXPlayerWebComponent.h"

/**
* Class GLXPlayerLobby. 
* Class GLXPlayerLobby is designed for Youtube functions. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
 * @version 1.0.0 
 *  
 */
class GLXPlayerLobby : public GLXPlayerWebComponent
{

public:
	/**
	* GLXPlayerLobby constructor.
	* Create an instance of GLXPlayerLobby.
	*/
	GLXPlayerLobby();
	
	/**
	* GLXPlayerLobby destructor.
	* Destroy an instance of GLXPlayerLobby.
	*/
	virtual ~GLXPlayerLobby(void);		
	
	/**
	* Function sendGetLobbyServer.
	* Get a lobby server 
	* 
	* @return success: f|107|r|s|address:port
	* @return fail: f|107|r|e|error code
	*/
	bool sendGetLobbyServer();	
	
	/**
	* Function sendGameInvitationGetLobby.
	* Get a lobby server address for game invitation
	* 
	* @param inviteName A username who invite me
	* @return success: |r|s|address:port
	* @return fail: |r|e|error code
	*/
	bool sendGameInvitationGetLobby(char* inviteName);
	
	/**
		Get lobby server address.
	*/
	char* getLobbyServerAddress(){return m_lobbyServerAdd;}
	
	/**
		Get lobby server port.
	*/
	int getLobbyServerPort(){return m_lobbyServerPort;}
	
protected:	
	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool OnUpdateSuccess( int func_id );	
	
	/**
	* Private function ProcessServerAddress.
	* Get server address and server port when receive from PHP server.
	*
	* @param data Data returned from the server.
	*/
	void ProcessServerAddress(char* data);
	
private:	
	
	/**
		Lobby server address.
	*/
	char* m_lobbyServerAdd;
	
	/**
		Lobby server port.
	*/
	int m_lobbyServerPort;
};
#endif
