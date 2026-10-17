#ifndef __GL_XPLAYER_MP_BASE_H__
#define __GL_XPLAYER_MP_BASE_H__

#include "../../Porting.h"
#include "Connection.h"
#include "DataPacket.h"
#include "GLXPlayerMPBaseObserver.h"
#include "XIPAddress.h"

///////////////////////////////////////////////////////////////////////////////
//
// Multiple Player		
//
///////////////////////////////////////////////////////////////////////////////

// multiplayer connect level 
#define CONNECT_LEVEL_NOT_CONNECTED			(0)
#define CONNECT_LEVEL_NOT_LOGGED_IN			(1)
#define CONNECT_LEVEL_IN_CONNECTED			(2)
#define CONNECT_LEVEL_IN_SESSION			(3)
#define CONNECT_LEVEL_IN_SESSION_MASTER		(4)
#define CONNECT_LEVEL_IN_GAME				(5)
#define CONNECT_LEVEL_IN_GAME_MASTER		(6)

#define CONNECT_LEVEL_NONE					(10)

class GLXPlayerMPBase
{
public:
	/**
	 * Multiplayer Specific - Constructor and Destructor. 
	 */
	 
	GLXPlayerMPBase(GLXPlayerMPBaseObserver * observer);
	virtual ~GLXPlayerMPBase(void);
	
	/**
	 * Multiplayer Specific - Update function, call when handling network request.
	 * @see #Update()
	 */
	virtual void Update() = 0;
	
	/**
	 * Multiplayer Specific - Send an connection establish messages to the server. 
	 */
	virtual void mpSendEstablishConnection();	
	
	/**
	 * Multiplayer Specific - Prepare a game data package, used for in game messages.
	 * 
	 */
	virtual DataPacket* mpPrepareGameData(){ return NULL;}
	
	/**
	 * Multiplayer Specific - Send a game data package, used for in game messages.
	 * 
	 */
	//virtual void mpSendGameData(DataPacket* packet) = 0;

	//virtual void mpSendGameData(DataPacket* packet, bool bReliable = false) = 0;

	/**
	 * Multiplayer Specific - Send a disconnect request to the multiplayer server.
	 * 
	 */
	virtual void mpSendDisconnect();
	
	/**
	 * Multiplayer Specific - Finishes the connection with the multiplayer server.
	 */
	virtual void mpDisconnect() = 0;
	
	/**
	 * Starts a request for either registration or login. This function is
	 * asynchronous.
	 * <p>
	 * The normal usage is sending login at the game load, after Gameloft logo,
	 * but before game splash screen and handle the response using
	 * <code>handleLogin()</code>.
	 * </p><p>
	 * When the application is started, a login with empty user name and phone
	 * number must be performed. If the user is registered, the server will reply
	 * with the username. If not, a registration process is started. The username
	 * must not be saved on the device, but retrieved always from the server.
	 * </p><p>
	 * As this process involves a precise call order to ensure that every
	 * possible error is handled, a detailed description on how the login
	 * sequence should look is given in the <code>handleLogin()</code> function's
	 * definition.
	 * </p>
	 * @param playerData Is by default <code>null</code>. If the game has
	 *        specific data associated with a player (like an avatar) it should
	 *        be passed here.
	 * @see #handleLogin()
	 *
	 */
	void mpSendLogin(char * uid);

	/**
	 * Multiplayer Specific - Kick out a player.
	 * <p>
	 * The session master can kick out a player before the game is started. The kicked out player will be removed from
	 * the session an receive a push message informing him about it.
	 * If a player that is not master tries to kick out another player, this will result in an error.
	 * @param playerName The name of the player to be kicked out.
	 * 
	 */
	virtual void mpSendKickOutPlayer(char* playerName){}

	virtual void mpSendKickOutPlayer(unsigned short* playerName){}

	/**
	 * Multiplayer Specific - Leave the game.
	 */
	virtual void mpSendLeaveGame(){}

	/**
	 * Multiplayer Specific - Start the game.
	 * <p>
	 * The start signal can be sent only by the session master, after enough players have joined the session.
	 * If another player send the start signal or the master tries to start the game when there are not enough players yet,
	 * this will result in an error response.
	 * After the game is started, the players can send in game messages to each other in whatever format they choose.
	 * 
	 */
	virtual void mpSendStartGame(){}
	
	/**
	 * Multiplayer Specific - Finish the game.
	 * <p>
	 * This message is send by each player after he finished playing the multiplayer game.
	 * The player will be removed from the session. The difference between finish game and leave game is that
	 * the session is not terminated when the master finished the game, but waits for all the players to finish.
	 * 
	 */
	virtual void mpSendFinishGame(){}
	
	/**
	 * Multiplayer Specific - Test if the player is connected to the multiplayer server.
	 * <p>
	 * @return Returns true if connected, false otherwise.
	 */
	bool mpIsConnected()
	{
		return m_MPConnectLevel >= CONNECT_LEVEL_NOT_LOGGED_IN;
	}

	/**
	 * Multiplayer Specific - Test if the player is logged on the multiplayer server.
	 * <p>
	 * @return Returns true if logged in, false otherwise.
	 */
	bool mpIsLoggedIn()
	{
		return m_MPConnectLevel >= CONNECT_LEVEL_IN_CONNECTED;
	}

	/**
	 * Multiplayer Specific - Test if the player is in a game session on the multiplayer server.
	 * <p>
	 * @return Returns true if in session, false otherwise.
	 */
	virtual bool mpIsInSession() = 0;

	virtual bool mpIsInRoom() = 0;

	/**
	 * Multiplayer Specific - Test if the player is in a playing a game on the multiplayer server.
	 * <p>
	 * @return Returns true if in game, false otherwise.
	 */
	virtual bool mpIsInGame() = 0;

	/**
	 * Multiplayer Specific - Test if the player is in a master in a session on the multiplayer server.
	 * <p>
	 * @return Returns true if master, false otherwise.
	 */
	virtual bool mpIsMaster() = 0;

	/**
	 * Multiplayer Specific - Test if the the opponent has finished the game.
	 * <p>
	 * @return Returns true if finished, false otherwise.
	 */
	bool mpHasOpponnentFinished()
	{
		return m_MPHasOpponentFinished;
	}

	/**
	 * Get nationality information.
	 *
	 */
	//char * getNationality( int index );
	
	//char *GetSessionListData(int i);

	virtual void mpSendUPnPPortMapping(){}

	void SetUsername(char *name)
	{
		XP_API_STRCPY(m_username, name);
	}

	char *GetUsername()
	{
		return m_username;
	}

	void SetCountry(char *country)
	{
		XP_API_STRCPY(m_country, country);
	}

	int mpGetConnectState()
	{ return m_connectState;}

	void mpSetConnectState(int state)
	{ m_connectState = state;}
	
protected:
	void Init();
	bool LoadConfig();
	
	char * getErrMsg();
	void setErrMsg();
	/**
	 * Processes incoming queue messages;
	 * return true only if the message needs further processing
	 * @return boolean
	 */
	virtual bool mpProcessIncomingMessages() = 0;

	// process the push messages from the server
	virtual void mpProcessPushMessages(){}

public: // Public variables	
	//for nonblocking connect state
	enum NonBlockConnectState
	{
		CONNECT_STATE_START,
		CONNECT_STATE_WAITING,
		CONNECT_STATE_SUCCESS,
		CONNECT_STATE_FAILED
	};
	
	//true, no login
	//false, login
	bool GetMPConnectOrNot() { return m_MPConnectLevel < CONNECT_LEVEL_IN_CONNECTED; }
	BYTE GetMPConnectLevel() { return m_MPConnectLevel; }

protected:

	GLXPlayerMPBaseObserver* m_observer;
	
	// User profile
	char m_username[UTF8_XPLAYER_MAX_USERNAME_LENGTH + 1];	
	char m_country[16];	
	
	// GGI
	unsigned long m_ggi;	

	char* m_errMsg;
	int m_lastErrorCode;
	
	// TCP connection object
	Connection * wtcp;
	
	// Multiplayer Server
	char* MP_SERVER_ADDRESS;
	unsigned long	MP_TCP_PORT;

	// one of the CONNECT_LEVEL_ defines - used to store the connection state ..
	BYTE m_MPConnectLevel;
	BYTE m_MPConnectLevel_temp;

	// true if the opponent has finished playing the multiplayer game 
	bool m_MPHasOpponentFinished;

	// the basic type of the message received .. This can be either CONNECT message (used for establishing and terminating the connection)
	//  or GAME message ( all other messages )
	BYTE m_type;

	// the subtype of the message received ..
	BYTE m_subtype;

	// the packet that is currenly being processed
	DataPacket* currentPacket;

	//for nonblocking connect state
	int m_connectState;

	bool m_canReceiveFromServer;
};
#endif
