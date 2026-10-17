#ifndef __GL_XPLAYER_MP_BY_WIFI_H__
#define __GL_XPLAYER_MP_BY_WIFI_H__

#include "../Porting.h"
#include "../Common/Connection.h"
#include "DataPacket.h"

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


class GLXPlayerMPbyWiFi
{
public: // Public functions]

	/**
	 * Multiplayer Specific - Constructor and Destructor. 
	 */
	 
	GLXPlayerMPbyWiFi(GLXPlayerMPBaseObserver * observer);
	virtual ~GLXPlayerMPbyWiFi(void);
	
	/**
	 * Multiplayer Specific - Update function, call when handling network request.
	 * @see #Update()
	 */
	void Update();
	
	/**
	 * Multiplayer Specific - Send an connection establish messages to the server. 
	 */
	void mpSendEstablishConnection();	
	
	/**
	 * Multiplayer Specific - Prepare a game data package, used for in game messages.
	 * 
	 */
	DataPacket* mpPrepareGameData();
	
	/**
	 * Multiplayer Specific - Send a game data package, used for in game messages.
	 * 
	 */
	void mpSendGameData(DataPacket* packet);
	
	/**
	 * Multiplayer Specific - get the next in game message received, or NULL if none 
	 *
	 */
	DataPacket* getNextInGameMessage();

	/**
	 * Multiplayer Specific - Send a disconnect request to the multiplayer server.
	 * 
	 */
	void mpSendDisconnect();
	
	/**
	 * Multiplayer Specific - Finishes the connection with the multiplayer server.
	 */
	void mpDisconnect();
	
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
	 * Multiplayer Specific - Send a quick game request to the server.
 	 * <p>
	 * The quick game request is used to get an available game from the server. The server responds either with a session
	 * list containing only one session, in which case a join request must be performed, either with no session, in which case
	 * a create session must be performed ..
	 * 
	 */
	void mpSendQuickGame();

	/**
	 * Multiplayer Specific - Send a list available games request to the server.
	 * <p>
	 * This is how it works : a request is made to the server, containing the number of sessions requested and the index of
	 * the first session. The server replies with the list, and the data is stored in internal buffers and can be accessed
	 * using the corresponding functions.
	 * The sessions are retrieved by page, using the parameters.
	 * For each session are returned the index of the session, the name, one data byte with the number of players currently in
	 * session and some binary data containing game specific session data (map, track, difficulty, etc.).
	 * <p> After the request is completed, the session list can be accessed with <code>getNameList()</code>,<code>getDataList()</code>,
	 * <code>getByteDataList()</code>,<code>getNameList()</code>, <code>getNumberOfSessions()</code> and <code>getFirstSessionIndex()</code>.
	 * @param numberOfSessions The number of session requested.
	 * @param firstSessionIndex The index of the first session requested.
	 * 
	 */
	void mpSendListSession(BYTE numberOfSessions, BYTE firstSessionIndex);
	
	/**
	 * Multiplayer Specific - Create a new game on the multiplayer server.
	 * <p>
	 * The creator of the session is called master. He can kick out a player that joins the session or start the game after
	 * enough player have joined.
	 * The created game can be seen by all the players who perform a list session request.
	 * The joining of XP_API_NEW players is signaled by a push message from the server.
	 * While being is session, the players list is available.
	 * @param sessionname The name of the session. This is identical with the player nickname.
	 * @param sessiondata Specific game data for the game session. This can include map, track, difficulty, etc.
	 * 
	 */	
	
	///@ Support UTF-8
	void mpSendCreateSession(unsigned short* sessionname, char* sessiondata);
	
	///@ Traditional function
	void mpSendCreateSession(char* sessionname, char* sessiondata);	

	/**
	 * Multiplayer Specific - Join a game session on the multiplayer server.
	 * <p>
	 * After the game is joined, the player will wait for the session master to start the game.
	 * The players list is available while being in session.
	 * @param sessionname The name of the session to be joined.
	 * @param mysessiondata The client info send to host when a client join
	 * 
	 */
	void mpSendJoinSession(char* sessionname);
	
	void mpSendJoinSession(unsigned short* sessionname);
	
	/**
	 * Multiplayer Specific - Leave a game session.
	 * <p>
	 * Every player can leave a session at any time. If the player is the master of the session, the session is terminated
	 * and all the other players will receive a kick out push message.
	 * 
	 */
	void mpSendLeaveSession();	

	/**
	 * Multiplayer Specific - Kick out a player.
	 * <p>
	 * The session master can kick out a player before the game is started. The kicked out player will be removed from
	 * the session an receive a push message informing him about it.
	 * If a player that is not master tries to kick out another player, this will result in an error.
	 * @param playerName The name of the player to be kicked out.
	 * 
	 */
	void mpSendKickOutPlayer(char* playerName);

	void mpSendKickOutPlayer(unsigned short* playerName);

	/**
	 * Multiplayer Specific - Start the game.
	 * <p>
	 * The start signal can be sent only by the session master, after enough players have joined the session.
	 * If another player send the start signal or the master tries to start the game when there are not enough players yet,
	 * this will result in an error response.
	 * After the game is started, the players can send in game messages to each other in whatever format they choose.
	 * 
	 */
	void mpSendStartGame();
	
	/**
	 * Multiplayer Specific - Finish the game.
	 * <p>
	 * This message is send by each player after he finished playing the multiplayer game.
	 * The player will be removed from the session. The difference between finish game and leave game is that
	 * the session is not terminated when the master finished the game, but waits for all the players to finish.
	 * 
	 */
	void mpSendFinishGame();
	
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
	bool mpIsInSession()
	{
		return ( m_MPConnectLevel >= CONNECT_LEVEL_IN_SESSION );
	}

	/**
	 * Multiplayer Specific - Test if the player is in a playing a game on the multiplayer server.
	 * <p>
	 * @return Returns true if in game, false otherwise.
	 */
	bool mpIsInGame()
	{
		return ( m_MPConnectLevel >= CONNECT_LEVEL_IN_GAME );
	}

	/**
	 * Multiplayer Specific - Test if the player is in a master in a session on the multiplayer server.
	 * <p>
	 * @return Returns true if master, false otherwise.
	 */
	bool mpIsMaster()
	{
		return ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION_MASTER) ||
			( m_MPConnectLevel == CONNECT_LEVEL_IN_GAME_MASTER);
	}

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
	 * Multiplayer Specific - Get the names list of the items hold in the internal buffers in certain multiplayer states.
	 * <p>
	 * The items can be either sessions, if a list session request was performed, or players, if the current player is inside a game session.
	 * @return The list of names.
	 */
	char** getNameList()
	{
		return m_lNameList;
	}

	/**
	 * Multiplayer Specific - Get the list of specific binary data for the items hold in the internal buffers in certain multiplayer states.
	 * <p>
	 * The items can be either sessions, if a list session request was performed, or players, if the current player is inside a game session.
	 * @return The list of data corresponding to each player.
	 */
	char** getDataList()
	{
		return m_lBinaryDataList;
	}

	/**
	 * Multiplayer Specific - Get the list of specific byte data for the items hold in the internal buffers in certain multiplayer states.
	 * <p>
	 * The items can be either sessions, if a list session request was performed, or players, if the current player is inside a game session.
	 * If the item is a session, this data is the number of players currently in session, if the item is a player, it is the index
	 * of the player in session.
	 * @return The list of data corresponding to each player.
	 */
	BYTE* getByteDataList()
	{
		return m_lByteDataList;
	}

	/**
	 * Get the number of players in a session.
	 *
	 */
	int getNumberOfPlayers( int index )
	{
		return m_lByteDataList[index] & 0xFF;
	}

	/**
	 * Get nationality information.
	 *
	 */
	char * getNationality( int index );

	/**
	 * Multiplayer Specific - Get the name of the session the player is currently in.
	 * <p>
	 * @return The name of the session.
	 */
	char* getSessionName()
	{
		return currentsessionname;
	}

	/**
	 * Multiplayer Specific - Get the speficic game data for the session the player is currently in.
	 * <p>
	 * @return The session data.
	 */
	char* getSessionData()
	{
		return currentsessiondata;
	}

	/**
	 * Multiplayer Specific - Get the number of sessions retrieved by the list session command.
	 * or players in session.
	 * <p>
	 * @return The number of sessions.
	 */
	BYTE getNumberOfItems()
	{
		return noitems;
	}

	/**
	 * Multiplayer Specific - Get the index of the first session on the multiplayer server,retrieved by the list session command.
	 * <p>
	 * @return The index of the first session.
	 */
	BYTE getFirstSessionIndex()
	{
		return nosessionsbase;
	}
	
	char *GetSessionListData(int i);
	
private: // Private functions	
	
	void Init();
	bool LoadConfig();
	
	char * getErrMsg();
	void setErrMsg();
	/**
	 * Processes incoming queue messages;
	 * return true only if the message needs further processing
	 * @return boolean
	 */
	bool mpProcessIncomingMessages();

	// process the push messages from the server
	void mpProcessPushMessages();

	void clearListedItems();

	//clears the internal buffer for storing in game messages
	void clearInGameMessages();

	// clears the data for the current session
	void clearCurrentSession();

private: // Private variables	

	GLXPlayerMultiPlayerObserver* m_observer;
	
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

	// true if the opponent has finished playing the multiplayer game 
	bool m_MPHasOpponentFinished;

	// the listed items ( session in session list or players when in session) have a name,
	// some data stored as string (custom session or player data) and
	// some data stored as byte (player index or the number of players in session)

	// hold the session names in list session  or player names in session
	char**   m_lNameList;
	
	//hold the session data which the client sents to host when join
	//char **m_sessionDataList;
	
	// hold the session data in list sessions or player data in session
	char**  m_lBinaryDataList; 

	// hold the session number of players in list sessions or the player index in session
	BYTE*   m_lByteDataList; 

	// the number of items currently hold in the internal buffers ..
	BYTE noitems;

	// the index of the first session retrieved by a list session request
	BYTE nosessionsbase;

	// the basic type of the message received .. This can be either CONNECT message (used for establishing and terminating the connection)
	//  or GAME message ( all other messages )
	BYTE m_type;

	// the subtype of the message received ..
	BYTE m_subtype;
   
	// the packet that is currenly being processed
	DataPacket* currentPacket;

	// the list of incoming in game messages
	DataPacket* incomingGameData;


	// the name of the session the player is currently in ( if any )
	char* currentsessionname;

	// the specific game data of the session the player is currently in (if any)
	char* currentsessiondata;

};
#endif