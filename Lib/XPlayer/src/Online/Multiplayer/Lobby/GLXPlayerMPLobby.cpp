#include "../../../../include/Multiplayer/Lobby/GLXPlayerMPLobby.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

/**
* GLXPlayerMPLobby Constructor. 
*/
GLXPlayerMPLobby::GLXPlayerMPLobby(GLXPlayerMPBaseObserver * observer, char* serverAddress) : GLXPlayerMPBase(observer)
{
	m_sLobbyServerAddress = NULL;
	if(serverAddress != NULL)
	{
		int len = XP_API_STRLEN(serverAddress);
		m_sLobbyServerAddress = XP_API_NEW char[len+1];
		m_sLobbyServerAddress[len] = '\0';
		XP_API_STRCPY(m_sLobbyServerAddress, serverAddress);
	}

	Init();	

	//m_iLobbyId = -1;
}

/**
* GLXPlayerMPLobby Destructor. 
*/
GLXPlayerMPLobby::~GLXPlayerMPLobby(void)
{
	XP_API_DEL(m_sLobbyServerAddress);
	XP_API_DEL( MP_SERVER_ADDRESS );

	if (wtcpLobby != NULL)
	{
		mpDisconnect();
		XP_API_DEL( wtcpLobby );
	}
}

void GLXPlayerMPLobby::Init()
{	
	wtcpLobby = NULL;	
	
	m_type    = -1;
	m_subtype = -1;

	m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;	
	m_MPConnectLevel_temp = CONNECT_LEVEL_NONE;

	m_MPHasOpponentFinished = false;
	
	m_errMsg = NULL;
	
	m_bSecondConnecting = false;
	m_lSecConStartTime = -1;//-1 for first connect, if failed, directly change the status to error
	
	m_bAutoMatchStatus = false;

	m_bCheckTimeOut = false;

	SetLobbyStatus(LOBBY_STATUS_COUNT);

	LoadConfig();
}

/**
 * Function LoadConfig.
 * Load the Multiplayer configuration: server address, server socket, ...
 */
bool GLXPlayerMPLobby::LoadConfig()
{
#ifdef LOBBY_THREAD_TEST
	m_ggi = 18154;

	//char value[] = /*"socket://alpha01.gameloft.com:7300";*/"socket://10.15.1.207:7500";
	char value[] = "socket://beta02.gameloft.com:7000";
	char temp[128] = { 0 };
	char host[128] = { 0 };

	XP_API_MEMSET( temp, 0, 128 );
	XP_API_MEMSET( host, 0, 128 );
	XP_API_PARSE_DATA( value, temp, 2, '/' );
	XP_API_PARSE_DATA( temp, host, 0, ':' );

	int len_total = XP_API_STRLEN( temp );
	int len_addrs = XP_API_STRLEN( host );

	MP_SERVER_ADDRESS = XP_API_NEW char[len_addrs+1];
	XP_API_MEMSET( MP_SERVER_ADDRESS, 0, len_addrs+1 );
	XP_API_MEMCPY( MP_SERVER_ADDRESS, host, len_addrs );

	char * mp_port = XP_API_NEW char[len_total-len_addrs];
	XP_API_MEMSET( mp_port, 0, len_total-len_addrs );
	XP_API_MEMCPY( mp_port, temp+len_addrs+1, len_total-len_addrs-1 );
	MP_TCP_PORT = XP_API_ATOI( mp_port );
	XP_API_DEL( mp_port );
	return true;	
#else //LOBBY_THREAD_TEST
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
    char tmp[FILENAME_MAX];
	XP_API_MEMSET(tmp,0,FILENAME_MAX);
	GetConfigFilePath(tmp, XPLAYER_FILE_CONF);
	pFile = XP_API_FILE_OPEN( tmp, "r" );
#else
	pFile = XP_API_FILE_OPEN( XPLAYER_FILE_CONF, "r" );
#endif

	if( !pFile )
	{
		XP_DEBUG_OUT("[GLXPlayerMPBase] load config - %s not found \n", XPLAYER_FILE_CONF );
		return false;
	}

	int length = XP_API_FILE_GET_LENGTH( pFile );
	char * buffer = XP_API_NEW char[length+1];
	
	XP_API_MEMSET( buffer, 0, length+1 );
	XP_API_FILE_READ( buffer, length, 1, pFile );

	int line = 0;
	char * data = XP_API_NEW char[length+1];

	XP_API_MEMSET( data, 0, length+1 );
	XP_API_PARSE_DATA( buffer, data, line++, '\n' );
	if (data[XP_API_STRLEN(data)-1] == '\r')
	{
		data[XP_API_STRLEN(data)-1] = 0;
	}
	int len = XP_API_STRLEN( data );
	
	char* value = m_sLobbyServerAddress;
	if(value != NULL)
	{
		char temp[128] = { 0 };
		char host[128] = { 0 };

		XP_API_MEMSET( temp, 0, 128 );
		XP_API_MEMSET( host, 0, 128 );
		XP_API_PARSE_DATA( value, temp, 2, '/' );
		XP_API_PARSE_DATA( temp, host, 0, ':' );

		int len_total = XP_API_STRLEN( temp );
		int len_addrs = XP_API_STRLEN( host );

		MP_SERVER_ADDRESS = XP_API_NEW char[len_addrs+1];
		XP_API_MEMSET( MP_SERVER_ADDRESS, 0, len_addrs+1 );
		XP_API_MEMCPY( MP_SERVER_ADDRESS, host, len_addrs );

		char * mp_port = XP_API_NEW char[len_total-len_addrs];
		XP_API_MEMSET( mp_port, 0, len_total-len_addrs );
		XP_API_MEMCPY( mp_port, temp+len_addrs+1, len_total-len_addrs-1 );
		MP_TCP_PORT = XP_API_ATOI( mp_port );
		XP_API_DEL( mp_port );
	}

	while( len > 0 )
	{
		char name[32] = { 0 };
		char value[256] = { 0 };
		int pos = 0;

		pos = XP_API_PARSE_DATA( data, name, 0, ':' );
		pos = XP_API_PARSE_DATA( data, value, 1, ':' );
		XP_API_MEMSET( value, 0, 256 );
		XP_API_MEMCPY( value, data+pos, XP_API_STRLEN(data)-pos );
		
		XP_API_STRTRIM( (char *) name );
		XP_API_STRTRIM( (char *) value );

		if ( XP_API_STRCMP( name, XPLAYER_CONF_GGI ) == 0 )
		{
			m_ggi = XP_API_ATOI( value );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_MPURL ) == 0 && m_sLobbyServerAddress == NULL )
		{
			// socket://beta-gs.gameloft.com:5676
			char temp[128] = { 0 };
			char host[128] = { 0 };

			XP_API_MEMSET( temp, 0, 128 );
			XP_API_MEMSET( host, 0, 128 );
			XP_API_PARSE_DATA( value, temp, 2, '/' );
			XP_API_PARSE_DATA( temp, host, 0, ':' );
			
			int len_total = XP_API_STRLEN( temp );
			int len_addrs = XP_API_STRLEN( host );
			
			MP_SERVER_ADDRESS = XP_API_NEW char[len_addrs+1];
			XP_API_MEMSET( MP_SERVER_ADDRESS, 0, len_addrs+1 );
			XP_API_MEMCPY( MP_SERVER_ADDRESS, host, len_addrs );

			char * mp_port = XP_API_NEW char[len_total-len_addrs];
			XP_API_MEMSET( mp_port, 0, len_total-len_addrs );
			XP_API_MEMCPY( mp_port, temp+len_addrs+1, len_total-len_addrs-1 );
			MP_TCP_PORT = XP_API_ATOI( mp_port );
			XP_API_DEL( mp_port );
		}

		XP_API_MEMSET( data, 0, length+1 );
		XP_API_PARSE_DATA( buffer, data, line++, '\n' );
		if (data[XP_API_STRLEN(data)-1] == '\r')
		{
			data[XP_API_STRLEN(data)-1] = 0;
		}		
		len = XP_API_STRLEN( data );
	}

	XP_API_DEL( data );
	XP_API_DEL( buffer );
	XP_API_FILE_CLOSE( pFile );
	return true;	
#endif //LOBBY_THREAD_TEST
}

void GLXPlayerMPLobby::mpSendDisconnect()
{
	if(wtcpLobby == NULL)
		return;

	if ( !wtcpLobby->isConnected() )
	{
		// already disconnected or loss of signal, ensure cleanup is done.
		mpDisconnect();
		m_lastErrorCode = ERROR_NONE;
		m_observer->OnMPDisconnectSuccess();
		return;
	}
   
	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendFinishConnectionPackage();

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_DISCONNECT);
}

void GLXPlayerMPLobby::mpSendEstablishConnection()
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendEstablishConnection(){\n");

	m_bSendRejoinAfterGameOver = false;// init send rejoin process
	if( !m_bSecondConnecting )
		m_MPConnectLevel_temp = CONNECT_LEVEL_NONE;
	//if(m_connectState == CONNECT_STATE_START)
	{
		if ( mpIsConnected() )
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPEstablishConnectionSuccess();
			return;
		}

		if ( NULL != wtcpLobby )
		{
			// Disconnect and cleanup.
			mpDisconnect();
			delete wtcpLobby;
			wtcpLobby = NULL;
		}

		m_lastErrorCode = ERROR_PENDING;

		wtcpLobby = XP_API_NEW ConnectionLobby( MP_SERVER_ADDRESS, MP_TCP_PORT );
		wtcpLobby->setName( m_username );

		m_connectState = CONNECT_STATE_WAITING;
	}

	XP_DEBUG_OUT("}\n");
}

// if this function returns true, the message has yet to be processed .. 
// if it returns false, the message has already been processed  and erased, except in game messages that are put
// in the in game message list
/**
 * Function mpProcessIncomingMessages.
 * Processes incoming queue messages;
 * return true only if the message needs further processing
 * @return True / False
 */
bool GLXPlayerMPLobby::mpProcessIncomingMessages()
{
	if ( mpIsLoggedIn()/*mpIsConnected()*/ )
    {
       bool hassent = wtcpLobby->keepConnectionAlive();
	}

	XP_API_DEL( currentPacket );
	currentPacket = wtcpLobby->getNextIncomingPacket();

	if ( currentPacket == NULL )
	{
		return false;
	}

	BYTE version;
	BYTE encoding;

	currentPacket->getByte( version );
	currentPacket->getByte( encoding );

	int type = currentPacket->GetMessageType();

	switch( type )
	{
	case SMSG_CONNECT:
		{
			//if( m_MPConnectLevel < CONNECT_LEVEL_IN_SESSION ) //add lqg for connecting the second time no reset the connect level
				m_MPConnectLevel	=	CONNECT_LEVEL_NOT_LOGGED_IN;

			m_lastErrorCode		=	ERROR_NONE;
			m_bCheckTimeOut = false;
			m_observer->OnMPEstablishConnectionSuccess();
			return true;
		}
		break;
	case SMSG_DISCONNECT:
		if(GetLobbyStatus() == LOBBY_STATUS_DISCONNECT)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			mpDisconnect();
			XP_API_DEL( currentPacket );
			currentPacket = NULL;
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPDisconnectSuccess();
			return false;
		}
		break;
     case SMSG_KEEP_ALIVE:
		{
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}
			
			if( result == ERROR_NONE)
			{	
			}
			else if( result == ERROR_KEEP_ALIVE_NOTINLOBBY || result == ERROR_KEEP_ALIVE_NOTEXIST_LOBBY )
			{
			}
		}
		break;

	case SMSG_LOGIN:
		if(GetLobbyStatus() == LOBBY_STATUS_LOGIN)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			if( m_MPConnectLevel_temp != CONNECT_LEVEL_NONE ) //add lqg for connecting the second time no reset the connect level
			{
				m_MPConnectLevel = m_MPConnectLevel_temp;
				m_MPConnectLevel_temp = CONNECT_LEVEL_NONE;
			}
			else
				m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
			
			m_lastErrorCode = ERROR_NONE;

			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}
			
			if( result == ERROR_NONE)
			{	
				m_observer->OnMPLoginSuccess( currentPacket );
			}
			else 
			//if( result == ERROR_LOGIN_INVALID_UID || result == ERROR_LOGIN_COMMUNICATE_WITH_GLLIVE )
			//ERROR_KEEP_ALIVE_NOTEXIST_LOBBY				= 0x002A,
			//ERROR_KEEP_ALIVE_NOTINLOBBY					= 0x002B
			{
				setErrMsg();
				m_lastErrorCode = result;

				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case SMSG_CREATE_LOBBY:
		if(GetLobbyStatus() == LOBBY_STATUS_CREATE)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;
			m_master = true;
			
			m_lastErrorCode = ERROR_NONE;

			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_observer->OnMPCreateSessionSuccess( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case SMSG_LEAVE_LOBBY:
		if(GetLobbyStatus() == LOBBY_STATUS_LEAVE)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPLeaveSessionSuccess();
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
			return true;
		}
		break;
	case PMSG_LEAVE_LOBBY:
		{
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_LEAVE_LOBBY);
			break;
		}
	case SMSG_QUERY_LOBBY:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_CONDITION)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPListSessionSuccess( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
			return true;			
		}
		break;

	case SMSG_JOIN_LOBBY:
		if(GetLobbyStatus() == LOBBY_STATUS_JOIN)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;

				m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;
				m_master = false;

				m_observer->OnMPJoinSessionSuccess( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case PMSG_JOIN_LOBBY_UNLAUNCHED:
		{
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_JOIN_LOBBY_UNLAUNCHED);
			break;
		}
	case PMSG_JOIN_LOBBY_LAUNCHED:
		{
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_JOIN_LOBBY_LAUNCHED);
			break;
		}
	case SMSG_SET_USER_STATUS:
		if(GetLobbyStatus() == LOBBY_STATUS_SET_STATUS)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPSetParamSuccess( SMSG_SET_USER_STATUS );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}

			break;
		}
	case PMSG_SET_USER_STATUS:
		{
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_SET_USER_STATUS);
			break;
		}
	case SMSG_START_GAME:
		if(GetLobbyStatus() == LOBBY_STATUS_LAUNCH_GAME)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			if(mpIsInSession())
			{
				m_bCheckTimeOut = false;
				int result = 0;

				if (currentPacket->getInt(result) == false)
				{
					XP_DEBUG_OUT("Msg error\n");
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					m_lastErrorCode = ERROR_BAD_RESPONSE;
					m_observer->OnMPNetworkError(m_lastErrorCode);
					return false;
				}

				if (result == ERROR_NONE)
				{
					m_lastErrorCode = ERROR_NONE;

					m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;

					m_lLaunchGameTime = XP_API_GET_TIME();

					m_observer->OnMPStartGameSuccess(currentPacket);
				}
				else if( ERROR_START_GAME_NOT_ALLPLAYER_READY == result )
				{
					m_lastErrorCode = ERROR_NONE;

					m_observer->OnMPStartGameFailed( result );
				}
				else
				{
					setErrMsg();
					m_lastErrorCode = result;
					m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
				}
			}
			break;
		}
	case PMSG_START_GAME:
		{
			if(mpIsInSession())
			{
				m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;

				m_lLaunchGameTime = XP_API_GET_TIME();

				m_observer->OnMPProcessPushMessages(currentPacket, PMSG_START_GAME);
			}
			break;
		}
	case PMSG_GAMESERVER_ADDR:
		{
			if(mpIsInSession())
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPProcessPushMessages(currentPacket, PMSG_GAMESERVER_ADDR);
			}
		}
		break;

	case SMSG_REJOIN_GAME:
		if(GetLobbyStatus() == LOBBY_STATUS_REJOIN)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if (currentPacket->getInt(result) == false)
			{
				XP_DEBUG_OUT("Msg error\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if (result == ERROR_NONE)
			{
				m_lastErrorCode = ERROR_NONE;

				if( m_bSendRejoinAfterGameOver )
				{		
					XP_DEBUG_OUT("SMSG_REJOIN_GAME: m_bSendRejoinAfterGameOver == true\n");
					m_bSendRejoinAfterGameOver = false;
					m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;
					m_observer->OnMPRejoinLobbySuccess(currentPacket, true);
				}
				else
				{
					if( CONNECT_LEVEL_IN_GAME == m_MPConnectLevel )
					{
						XP_DEBUG_OUT("SMSG_REJOIN_GAME: get rejoin in game, no change status\n");
						m_observer->OnMPRejoinLobbySuccess(currentPacket, false);
					}
					else
					{
						XP_DEBUG_OUT("SMSG_REJOIN_GAME: get rejoin in session, change status\n");
						m_observer->OnMPRejoinLobbySuccess(currentPacket, true);
					}
				}
			}
			else if( ERROR_START_GAME_NOT_ALLPLAYER_READY == result )
			{
				m_lastErrorCode = ERROR_NONE;

				m_bSendRejoinAfterGameOver = false;

				m_observer->OnMPStartGameFailed( result );
			}
			else
			{
				m_bSendRejoinAfterGameOver = false;

				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case PMSG_REJOIN_GAME:
		{
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_REJOIN_GAME);
			break;
		}

	case SMSG_SET_GAME_PARAMETER:
		if(GetLobbyStatus() == LOBBY_STATUS_SET_GAME_PARAM)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPSetParamSuccess( SMSG_SET_GAME_PARAMETER );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case PMSG_SET_GAME_PARAMETER:
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_SET_GAME_PARAMETER);
		}
		break;

	case SMSG_SET_USER_PARAMETER:
		if(GetLobbyStatus() == LOBBY_STATUS_SET_USER_PARAM)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPSetParamSuccess( SMSG_SET_USER_PARAMETER );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case PMSG_SET_USER_PARAMETER:
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_SET_USER_PARAMETER);
		}
		break;

	case SMSG_GET_LOBBY_INFOR:
		if(GetLobbyStatus() == LOBBY_STATUS_GET_MORE_INFO)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPGetLobbyInfo( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_QUERY_LOBBY_BY_FRIENDID:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_NAME)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPGetLobbyForName( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_QUERY_LOBBY_OF_FRIENDS:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_FRIENDID)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPGetLobbyForFriend( currentPacket );
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_AUTOMATCH_USER: //this means error occured
		if(GetAutoMatchStatus())
		{
			int err = 0;
			currentPacket->getInt(err);

			if(err == ERROR_NONE || err == ERROR_AUTOMATCH_WAITING)
			{
				m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;
				m_observer->OnMPAutoMatchRequeset(err);
			}
			else 
			{
				//if(err == ERROR_INVALID_INPUT)
				setErrMsg();
				m_lastErrorCode = err;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_AUTOMATCH_USER_CANCLE:
		if(GetLobbyStatus() == LOBBY_STATUS_AUTOMATCH_CANCLE)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPAutoMatchRequesetCancel();
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;
	case PMSG_AUTOMATCH_USER:
		if(GetAutoMatchStatus())
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_AUTOMATCH_USER);
		}
		break;

	case SMSG_KICK_OUT_PLAYER:
		if(GetLobbyStatus() == LOBBY_STATUS_KICK_OUT)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPKickOutPlayerSuccess();
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case PMSG_KICK_OUT_PLAYER:
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_KICK_OUT_PLAYER);
		}
		break;

	case PMSG_SAMEACCOUNT_LOGIN:
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPProcessPushMessages(currentPacket, PMSG_SAMEACCOUNT_LOGIN);
		}
		break;

	case SMSG_QUERY_LOBBY_GAMEPAMAM:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_CONDITION_GAMEPAMAM)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPListSessionSuccessWithGameParam(currentPacket);
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_QUERY_LOBBY_BY_FRIENDID_GAMEPARAM:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_NAME_GAMEPAMAM)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPListSessionSuccessWithGameParam(currentPacket);
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	case SMSG_QUERY_LOBBY_OF_FRIENDS_GAMEPARAM:
		if(GetLobbyStatus() == LOBBY_STATUS_FILTER_FOLLOW_FRIENDID_GAMEPAMAM)
		{
			SetLobbyStatus(LOBBY_STATUS_COUNT);
			m_bCheckTimeOut = false;
			int result = 0;

			if ( currentPacket->getInt( result ) == false )
			{
				XP_DEBUG_OUT("Msg error 2\n");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				m_lastErrorCode = ERROR_BAD_RESPONSE;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return false;
			}

			if ( result == ERROR_NONE )
			{
				m_lastErrorCode = ERROR_NONE;
				m_observer->OnMPListSessionSuccessWithGameParam(currentPacket);
			}
			else
			{
				setErrMsg();
				m_lastErrorCode = result;
				m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
			}
		}
		break;

	default:
		break;
	}

	// current packet processed, erase it 
	XP_API_DEL( currentPacket );
	currentPacket = NULL;
	return false;
}

void GLXPlayerMPLobby::mpDisconnect()
{
   wtcpLobby->disconnect();

   // cleanup .. 
   if (currentPacket != NULL)
   {
      XP_API_DEL( currentPacket );
      currentPacket = NULL;
   }
   
   if( m_MPConnectLevel >= CONNECT_LEVEL_IN_SESSION && m_bSecondConnecting ) //add lqg for connecting the second time no reset the connect level
   {
      m_MPConnectLevel_temp = m_MPConnectLevel;
   }

   m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;   
}

/**
* Function Update.
* Called when handling network request.
* @see #Update()
*/
void GLXPlayerMPLobby::Update()
{
	// Make sure that we are not in a Signal Loss condition.
	if (wtcpLobby->isConnected() == false)
	{
		if(wtcpLobby->GetSocket() != NULL
			&& m_connectState == CONNECT_STATE_WAITING)
		{
			if(!wtcpLobby->connect())
			{
				if(wtcpLobby->GetConnectState() == Connection::CONNECT_STATE_WAITING)
				{
					return;
				}
				else
				{
					m_connectState = CONNECT_STATE_FAILED;
					wtcpLobby->SetConnceState(Connection::CONNECT_STATE_START);

					//>>for second connecting
#ifndef LOBBY_THREAD_TEST
					if( m_lSecConStartTime != 0 && !m_bSecondConnecting )//for check if the second connect success.if failed,notice user that there is an error
					{
						mpDisconnect();
						m_lastErrorCode = ERROR_CONNECTION;
						m_observer->OnMPNetworkError(m_lastErrorCode);
						return;
					} 

					if( !m_bSecondConnecting )
					{
						m_bSecondConnecting = true;
						m_lSecConStartTime = XP_API_GET_TIME();
					}

					long time = 0;
					if( CONNECT_LEVEL_IN_GAME == m_MPConnectLevel )
						time = SECOND_CONNECTING_TIME_IN_GAME;
					else
						time = SECOND_CONNECTING_TIME_IN_LOBBY;
					if( XP_API_GET_TIME() - m_lSecConStartTime > time )
					{
						m_bSecondConnecting = false;
					}
#endif
					//<<end

					// communication error occured, disconnect.
					mpDisconnect();
					m_lastErrorCode = ERROR_CONNECTION;
					m_observer->OnMPNetworkError(m_lastErrorCode);
					return;					
				}
			}
			else
			{
#ifdef MP_ENABLE_TIMEOUT 
				wtcpLobby->last_activity_time = (long) XP_API_GET_TIME();
				m_bCheckTimeOut = true;
#endif // MP_ENABLE_TIMEOUT
				m_connectState = CONNECT_STATE_SUCCESS;
				wtcpLobby->SetConnceState(Connection::CONNECT_STATE_START);
				return;
			}
		}
		else
		{
			//>>for second connecting
#ifndef LOBBY_THREAD_TEST
			if( m_lSecConStartTime != 0 && !m_bSecondConnecting )//for check if the second connect success.if failed,notice user that there is an error
			{
				mpDisconnect();
				m_lastErrorCode = ERROR_CONNECTION;
				m_observer->OnMPNetworkError(m_lastErrorCode);
				return;
			} 

			if( !m_bSecondConnecting && m_lSecConStartTime == 0 )
			{
				m_bSecondConnecting = true;
				m_lSecConStartTime = XP_API_GET_TIME();
			}

			long time = 0;
			if( CONNECT_LEVEL_IN_GAME == m_MPConnectLevel )
				time = SECOND_CONNECTING_TIME_IN_GAME;
			else
				time = SECOND_CONNECTING_TIME_IN_LOBBY;
			if( XP_API_GET_TIME() - m_lSecConStartTime > time )
			{
				m_bSecondConnecting = false;
			}
#endif
			//<<end

			// communication error occured, disconnect.
			mpDisconnect();
			m_lastErrorCode = ERROR_CONNECTION;
			m_observer->OnMPNetworkError(m_lastErrorCode);
			return;
		}
	}

	wtcpLobby->receiveDataLen();

#ifdef MP_ENABLE_TIMEOUT 
	if (m_bCheckTimeOut && XP_API_GET_TIME() - wtcpLobby->last_activity_time > MP_CONN_TIMEOUT)
	{
		m_bCheckTimeOut = false;
		XP_DEBUG_OUT("Timeout expired!\n");

		if(mpIsLoggedIn())
			m_lastErrorCode = ERROR_LOBBY_TIME_OUT;
		else
			m_lastErrorCode = ERROR_REQUEST_TIME_OUT;

		m_observer->OnMPNetworkError(m_lastErrorCode);
		SetLobbyStatus(LOBBY_STATUS_COUNT);
		mpDisconnect();		
		return;
	}
#endif // MP_ENABLE_TIMEOUT

	if (mpProcessIncomingMessages() == true)
	{
		// Unexpected msg, clear it.
		XP_API_DEL( currentPacket );
		currentPacket = NULL;
	}
}


//lobby client

void GLXPlayerMPLobby::mpSendLobbyLogin(long ggi, char * uid, int len, BYTE bKeepLobbyStatus)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendLobbyLogin\n");

	m_lastErrorCode = ERROR_PENDING;
	
	wtcpLobby->sendLobbyLoginPackage( ggi, uid, len, bKeepLobbyStatus );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_LOGIN);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendCreateLobby(char* name, BYTE bVisibility, BYTE bLock, 
										 int maxCount, char*  gameParam, char* playerParam, CLobbyParameterAndQuery* clobbyParamAndQuery )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendCreateLobby(){\n");

	if ( !mpIsLoggedIn() /*|| mpIsInSession()*/ )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}	

	m_lastErrorCode = ERROR_PENDING;
	
	wtcpLobby->sendCreateLobbyPackage( m_ggi, name, bVisibility, bLock, maxCount, gameParam, playerParam, clobbyParamAndQuery);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_CREATE);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendJoinLobby(int lobby_id, BYTE user_status, char* playerParameter)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendJoinLobby(){\n");

	if ( !mpIsLoggedIn()/* || mpIsInSession() */)
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	wtcpLobby->sendJoinLobbyPackage( lobby_id, user_status, playerParameter );

	m_lastErrorCode = ERROR_PENDING;

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_JOIN);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyList(int userNumber, int start_num, BYTE count, 
										  CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyList(){\n");

	if (!mpIsLoggedIn())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	if ( start_num < 0 )	// make sure we don't request negative indexess
	{
		start_num = 0;
	}
	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	m_lastErrorCode = ERROR_PENDING;	
	wtcpLobby->sendGetLobbyListPackage( m_ggi, userNumber, start_num, count, clobbyParamAndQuery, bCheckJoinable );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_CONDITION);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendLeaveLobby(/*int lobby_id*/)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendLeaveLobby(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	wtcpLobby->sendLeaveLobbyPackage(/*lobby_id*/);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_LEAVE);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendLunchLobbyGame()
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendLunchLobbyGame(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	wtcpLobby->sendLunchLobbyGamePackage();

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_LAUNCH_GAME);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendSetPlayerStatus(BYTE status)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendSetPlayerStatus(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	wtcpLobby->sendSetPlayerStatusPackage(status);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_SET_STATUS);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendRejoinLobby( int iNormal )
{
	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendRejoinLobbyPackage( iNormal );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_REJOIN);
}

void GLXPlayerMPLobby::mpSendSetGameParameter( char* gameParameter, int lenParam, CLobbyParameterAndQuery* clobbyParamAndQuery )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendSetGameParameter(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendSetGameParameter( gameParameter, lenParam, clobbyParamAndQuery );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_SET_GAME_PARAM);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendSetUserParameter( char* userParameter, int lenParam )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendSetUserParameter(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendSetUserParameter( userParameter, lenParam );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_SET_USER_PARAM);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyInfo( int lobbyID )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyInfo(){\n");

	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendGetLobbyInfo( lobbyID );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_GET_MORE_INFO);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyForName( char* nameQuery )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyForName(){\n");

	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendGetLobbyForName( nameQuery );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_NAME);

	XP_DEBUG_OUT("}\n");

}

void GLXPlayerMPLobby::mpSendGetLobbyForFriend( int userNumber, BYTE bVisibility, GLXPlayerUserFriend* userFriend )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyForName(){\n");

	m_lastErrorCode = ERROR_PENDING;

	int countFriend = userFriend->GetFriendsCount();

	//set the max friend count 
	if( countFriend > MAX_FRIEND_COUNT_SEND_TO_SERVER )
		countFriend = MAX_FRIEND_COUNT_SEND_TO_SERVER;
	
	char* nameArray = NULL;
	int len_nameArray = 0;
	int count = countFriend - 1;
	while( count >= 0 )
	{
		char name[128] = { 0 };
		if( userFriend->GetName( count ) == NULL )
		{
			count--;
			continue;
		}
		XP_API_STRCPY( name, userFriend->GetName( count ) );

		int len = len_nameArray;
		//if( nameArray != NULL )
		//{
		//	len = XP_API_STRLEN( nameArray );
		//}
		int lenName = XP_API_STRLEN(name);
		char* catName = XP_API_NEW char[ len + lenName + 2 + 1 ];
		catName[len + lenName + 2] = '\0';
		if( nameArray != NULL )
		{
			for(int i = 0; i < len_nameArray; i++)
				catName[i] = nameArray[i];
		}
		catName[len++] = (BYTE)((lenName & 0x0000FF00)>>8);
		catName[len++] = (BYTE)((lenName & 0x000000FF));

		XP_API_STRCPY( &catName[len], name );
		XP_API_DEL( nameArray );
		nameArray = catName;
		len_nameArray += lenName + 2;
		count--;
	}

	wtcpLobby->sendGetLobbyForFriend( userNumber, bVisibility, (BYTE)((countFriend & 0x000000FF)), nameArray, len_nameArray );

	XP_API_DEL(nameArray);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_FRIENDID);

	XP_DEBUG_OUT("}\n");

}

void GLXPlayerMPLobby::mpSendAutoMatchRequest(char* gameParam, char* playerParam, short playerLevel, short levelDifference)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendAutoMatchRequest(){\n");

	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}	

	m_lastErrorCode = ERROR_PENDING;
	
	SetAutoMatchStatus(true);

	wtcpLobby->sendAutoMatchPackage(gameParam, playerParam, playerLevel, levelDifference);

	//no need time out
//#ifdef MP_ENABLE_TIMEOUT
//	//Set timer
//	wtcpLobby->last_activity_time = XP_API_GET_TIME();
//	m_bCheckTimeOut = true;
//#endif
	m_bCheckTimeOut = false;

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendCancelAutoMatchRequest()
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendCancelAutoMatchRequest(){\n");

	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}	

	m_lastErrorCode = ERROR_PENDING;
	
	SetAutoMatchStatus(false);
	m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;

	wtcpLobby->sendCancelAutoMatchPackage();

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_AUTOMATCH_CANCLE);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendKickOut(char* name)
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendKickOut(){\n");

	if ( !mpIsLoggedIn() /*|| mpIsInSession()*/ )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}	

	m_lastErrorCode = ERROR_PENDING;
	
	wtcpLobby->sendKickOutPackage(name);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_KICK_OUT);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyListWithGameParam( int userNumber, int start_num, BYTE count, 
									 CLobbyParameterAndQuery* clobbyParamAndQuery, BYTE bCheckJoinable )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyListWithGameParam(){\n");

	if (!mpIsLoggedIn())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	if ( start_num < 0 )	// make sure we don't request negative indexess
	{
		start_num = 0;
	}
	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	m_lastErrorCode = ERROR_PENDING;	
	wtcpLobby->sendGetLobbyListPackageWithGameParam( m_ggi, userNumber, start_num, count, clobbyParamAndQuery, bCheckJoinable );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_CONDITION_GAMEPAMAM);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyForNameWithGameParam( char* nameQuery )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyForName(){\n");

	m_lastErrorCode = ERROR_PENDING;
	wtcpLobby->sendGetLobbyForNameWithGameParam( nameQuery );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_NAME_GAMEPAMAM);

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPLobby::mpSendGetLobbyForFriendWithGameParam( int userNumber, BYTE bVisibility, GLXPlayerUserFriend* userFriend )
{
	XP_DEBUG_OUT("GLXPlayerMPLobby::mpSendGetLobbyForName(){\n");

	m_lastErrorCode = ERROR_PENDING;

	int countFriend = userFriend->GetFriendsCount();

	//set the max friend count 
	if( countFriend > MAX_FRIEND_COUNT_SEND_TO_SERVER )
		countFriend = MAX_FRIEND_COUNT_SEND_TO_SERVER;
	
	char* nameArray = NULL;
	int len_nameArray = 0;
	int count = countFriend - 1;
	while( count >= 0 )
	{
		char name[128] = { 0 };
		if( userFriend->GetName( count ) == NULL )
		{
			count--;
			continue;
		}
		XP_API_STRCPY( name, userFriend->GetName( count ) );

		int len = len_nameArray;
		//if( nameArray != NULL )
		//{
		//	len = XP_API_STRLEN( nameArray );
		//}
		int lenName = XP_API_STRLEN(name);
		char* catName = XP_API_NEW char[ len + lenName + 2 + 1 ];
		catName[len + lenName + 2] = '\0';
		if( nameArray != NULL )
		{
			for(int i = 0; i < len_nameArray; i++)
				catName[i] = nameArray[i];
		}
		catName[len++] = (BYTE)((lenName & 0x0000FF00)>>8);
		catName[len++] = (BYTE)((lenName & 0x000000FF));

		XP_API_STRCPY( &catName[len], name );
		XP_API_DEL( nameArray );
		nameArray = catName;
		len_nameArray += lenName + 2;
		count--;
	}

	wtcpLobby->sendGetLobbyForFriendWithGameParam( userNumber, bVisibility, (BYTE)((countFriend & 0x000000FF)), nameArray, len_nameArray );

	XP_API_DEL(nameArray);

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcpLobby->last_activity_time = XP_API_GET_TIME();
	m_bCheckTimeOut = true;
#endif

	SetLobbyStatus(LOBBY_STATUS_FILTER_FOLLOW_FRIENDID_GAMEPAMAM);

	XP_DEBUG_OUT("}\n");
}
