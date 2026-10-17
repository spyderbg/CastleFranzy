#include "../../../../include/Multiplayer/P2P/GLXPlayerMPbyP2P.h"
#include "upnp/PortMapper.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

GLXPlayerMPbyP2P::GLXPlayerMPbyP2P(GLXPlayerMPBaseObserver *observer): GLXPlayerMPBase( observer )
{
	m_upnp_enabled = false;

	m_socket_ping = GLXPlayerSocketFactory::GetSocket();
	m_socket_ping->CreateUdpSocket();

	m_upnp_tested = false;
	m_host = false;
	m_old_create_session_name = NULL;
	m_old_create_session_data = NULL;
	m_lBinaryDataList_Head = NULL;
	currentsessiondata = NULL;

	m_room_list = NULL;
	m_curr_room = NULL;

	isStartJoin = false;

	Init();	
	PortMapper::GetInstance()->RegistUpnuObserver(this);
	m_upnpSetState=SET_UPNP_NONE;
}

GLXPlayerMPbyP2P::~GLXPlayerMPbyP2P(void)
{
	XP_API_DEL( MP_SERVER_ADDRESS );

	if (wtcp != NULL)
	{
		mpDisconnect();
		XP_API_DEL( wtcp );
	}

	//clearInGameMessages();
	//clearListedItems();
	//clearCurrentSession();	
	XP_API_DEL(currentsessiondata);
	mpClearRoomData();
	currentsessiondata = NULL;

	if ( m_upnp_enabled )
	{
		PortMapper::GetInstance()->RemoveAllPortMapping();
	}

	PortMapper::ForceDestroy();
}

/**
* Function Init.
* Initialize all varialbe of GLXPlayerMPBase instance.
*/
void GLXPlayerMPbyP2P::Init()
{	
	wtcp = NULL;	

	m_type    = -1;
	m_subtype = -1;

	m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;	
	m_MPHasOpponentFinished = false;

	m_errMsg = NULL;

	m_ip_router = XP_API_NEW XIPAddress();
	m_ip_local = XP_API_NEW XIPAddress();
	m_upnp_try_times = 0;

	LoadConfig();
}

/**
* Function LoadConfig.
* Load the Multiplayer configuration: server address, server socket, ...
*/
bool GLXPlayerMPbyP2P::LoadConfig()
{
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

		if ( XP_API_STRCMP( name, XPLAYER_CONF_MPURL ) == 0 )
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
		if ( XP_API_STRCMP( name, XPLAYER_CONF_UPNP_PORT ) == 0 )
		{
			m_ip_local->setTcpPort( XP_API_ATOI( value ) );
			m_ip_local->setUdpPort( XP_API_ATOI( value ) );
			m_ip_router->setTcpPort( XP_API_ATOI( value ) );
			m_ip_router->setUdpPort( XP_API_ATOI( value ) );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_BROADCAST_PORT ) == 0 )
		{
			m_ip_local->setBroadcastPort( XP_API_ATOI( value ) );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_UPNP_PORT_TRY ) == 0 )
		{
			m_upnp_try_times = ( XP_API_ATOI( value ) );
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
}

// if this function returns true, the message has yet to be processed .. 
// if it returns false, the message has already been processed  and erased, except in game messages that are put
// in the in game message list
bool GLXPlayerMPbyP2P::mpProcessIncomingMessages()
{
	if ( mpIsConnected() )
	{
		wtcp->keepConnectionAlive();
	}

	XP_API_DEL( currentPacket );
	currentPacket = wtcp->getNextIncomingPacket();

	if ( currentPacket == NULL )
	{
		return false;
	}

	if (currentPacket->getByte(m_type) == false)
	{
		XP_DEBUG_OUT("mpProcessIncomingMessages: Msg error");

		XP_API_DEL( currentPacket );
		currentPacket = NULL;
		return false;
	}


	m_subtype = 0;

	switch (m_type)
	{
	case BASIC_MESSAGE_TYPE_CONNECT:
		{
			if (currentPacket->getByte(m_subtype) == false)
			{
				XP_DEBUG_OUT("mpProcessIncomingMessages: Connect msg error");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				return false;
			}

			switch (m_subtype)
			{
			case CONNECTION_MESSAGE_TYPE_SUCCESS:				
				{
					// state after succesfull connect is that we're not logged in
					m_MPConnectLevel	=	CONNECT_LEVEL_NOT_LOGGED_IN;
					m_lastErrorCode		=	ERROR_NONE;
					m_observer->OnMPEstablishConnectionSuccess();
					return true;
				}			            
			case CONNECTION_MESSAGE_TYPE_ERROR:
				{
					m_MPConnectLevel	=	CONNECT_LEVEL_NOT_CONNECTED;
					m_lastErrorCode		=	ERROR_CONNECTION;
					setErrMsg();
					m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
					XP_DEBUG_OUT("connection error");
					return true;
				}			
			case CONNECTION_MESSAGE_TYPE_DISCONECT:
				{
					mpDisconnect();
					m_canReceiveFromServer = false;
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					m_lastErrorCode = ERROR_NONE;
					m_observer->OnMPDisconnectSuccess();
					return false;
				}
			default:
				{
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					return false;
				}
			}
		}
		break;

	case BASIC_MESSAGE_TYPE_GAME:
		{
			if (currentPacket->getByte(m_subtype) == false)
			{
				XP_DEBUG_OUT("mpProcessIncomingMessages: Game msg error");
				XP_API_DEL( currentPacket );
				currentPacket = NULL;
				return false;
			}

			switch (m_subtype)
			{
			case GAME_MESSAGE_REQUEST:
				{
					BYTE result = 0;
					if (currentPacket->getByte(result) == false)
					{
						XP_DEBUG_OUT("Msg error\n");
						XP_API_DEL( currentPacket );
						currentPacket = NULL;
						m_lastErrorCode = ERROR_BAD_RESPONSE;
						m_observer->OnMPNetworkError(m_lastErrorCode);
						return false;
					}

					if (result == REQUEST_MESSAGE_CREATE_SESSION )   
					{
						if (currentPacket->getByte(result) == false)
						{
							XP_DEBUG_OUT("Msg error 2\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							// we're totally connected, also logged in
							m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION_MASTER;

							//clearListedItems();

							//noitems = 1;
							//m_lNameList = XP_API_NEW char*[noitems];
							//m_lByteDataList = XP_API_NEW BYTE[noitems];
							//m_lNameList[0] = XP_API_STRNEW( m_username );
							//m_lByteDataList[0] = 1;
							m_lastErrorCode = ERROR_NONE;
							m_curr_room->StartRoom();
							m_observer->OnMPCreateSessionSuccess();
						}
						else
						{
							setErrMsg();
							m_lastErrorCode = ERROR_REQUEST_FAILED;
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;					
					}
					else if(result == REQUEST_MESSAGE_LOGIN)
					{
						if ( !currentPacket->getByte(result) )
						{
							XP_DEBUG_OUT("Msg error 2\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							// we're totally connected, also logged in
							m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
							m_lastErrorCode = ERROR_NONE;
							m_observer->OnMPLoginSuccess();						
						}
						else
						{						
							m_lastErrorCode = ERROR_REGISTER_FAILED;
							setErrMsg();
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;
					}
					else if( result == REQUEST_MESSAGE_GET_QUICK_SESSION 
						|| result == REQUEST_MESSAGE_LIST_SESSIONS)
					{
						if (currentPacket->getByte(result) == false)
						{
							XP_DEBUG_OUT("Msg error 2\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							m_lastErrorCode = ERROR_NONE;

							mpClearRoomData();
							m_room_list = XP_API_NEW COnlineGameRoomList( currentPacket );

							m_lastErrorCode = ERROR_NONE;
							m_observer->OnMPListSessionSuccess();
						}
						else
						{
							setErrMsg();
							m_lastErrorCode = ERROR_REQUEST_FAILED;
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;
					}
					else if(result == REQUEST_MESSAGE_JOIN_SESSION)
					{
						return true;
					}
					else if(result == REQUEST_MESSAGE_LEAVE_SESSION)
					{
						if (currentPacket->getByte(result) == false)
						{
							XP_DEBUG_OUT("Msg error 2\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							//m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
							m_lastErrorCode = ERROR_NONE;
							m_curr_room->HostLeave();
							mpClearRoomData();
							OnHostLost();
							m_observer->OnMPLeaveSessionSuccess();
						}
						else
						{
							setErrMsg();
							m_lastErrorCode = ERROR_REQUEST_FAILED;
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;
					}				
					else if(result == REQUEST_MESSAGE_KICK_OUT_PLAYER)
					{
						return true;
					}
					else if(result == REQUEST_MESSAGE_START_GAME)
					{
						if (currentPacket->getByte(result) == false)
						{
							XP_DEBUG_OUT("Msg error\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							if ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION)
							{
								m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;
							}
							else if ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION_MASTER)
							{
								m_MPConnectLevel = CONNECT_LEVEL_IN_GAME_MASTER;
							}

							m_MPHasOpponentFinished = false;

							// refresh the player list
							//clearListedItems();
							// get the player names ..
							//currentPacket->getByte(noitems);

							//if (noitems > 0)
							//{
							//	m_lNameList = (char **)XP_API_NEW char*[noitems];
							//	m_lByteDataList = XP_API_NEW BYTE[noitems];

							//	BYTE len = 0;
							//	for (BYTE cnt = 0; cnt < noitems; cnt++)
							//	{
							//		currentPacket->getString(m_lNameList[cnt],len);
							//		m_lByteDataList[cnt] = (cnt + 1);
							//	}
							//}

							m_curr_room->StartGame();
							mpSendDisconnect();

							m_lastErrorCode = ERROR_NONE;
							m_observer->OnMPStartGameSuccess();
						}
						else
						{
							setErrMsg();
							m_lastErrorCode = ERROR_REQUEST_FAILED;
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;
					}
					else if (result == REQUEST_MESSAGE_FINISH_GAME)
					{
						if (currentPacket->getByte(result) == false)
						{
							XP_DEBUG_OUT("Msg error 2\n");
							XP_API_DEL( currentPacket );
							currentPacket = NULL;
							m_lastErrorCode = ERROR_BAD_RESPONSE;
							m_observer->OnMPNetworkError(m_lastErrorCode);
							return false;
						}

						if (result == RESPONSE_MESSAGE_SUCCESS)
						{
							m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
							m_lastErrorCode = ERROR_NONE;
							m_observer->OnMPFinishGameSuccess();
						}
						else
						{
							setErrMsg();
							m_lastErrorCode = ERROR_REQUEST_FAILED;
							m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
						}
						return true;
					}
					else
					{
						return true;
					}
				}                  
			case GAME_MESSAGE_PUSH:
				{
					XP_DEBUG_OUT("GAME_MESSAGE_PUSH\n");
					mpProcessPushMessages();
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					return false;
				}
			case GAME_MESSAGE_IN_GAME:
			case GAME_MESSAGE_IN_GAME_TOALL:
			case GAME_MESSAGE_IN_GAME_RELIABLE:
				{
					//if (incomingGameData == NULL)
					//{
					//   incomingGameData = currentPacket;
					//}
					//else
					//{
					//   incomingGameData->setNextPacket(currentPacket);
					//}
					//currentPacket = NULL;//mag
					return false;
				}               
			case GAME_MESSAGE_KEEP_ALIVE:
				{
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					return false;
				}

			default:
				{
					XP_API_DEL( currentPacket );
					currentPacket = NULL;
					return false;
				}
			}
		}
	}
	// current packet processed, erase it 
	XP_API_DEL( currentPacket );
	currentPacket = NULL;
	return false;
}


// IMPORTANT:
// The currentPacket MUST deleted after calling this function.
void GLXPlayerMPbyP2P::mpProcessPushMessages()
{
	if (!mpIsInSession())
	{
		XP_DEBUG_OUT("mpProcessPushMessages: not in session\n");
		return;
	}

	XP_DEBUG_OUT("GLXPlayerMPbyP2P::mpProcessPushMessages(){\n");

	if (currentPacket == NULL)
	{
		XP_DEBUG_OUT("mpProcessPushMessages: currentPacket is NULL\n");
		return;
	}

	BYTE result = 0;
	if (currentPacket->getByte(result) == false)
	{
		XP_DEBUG_OUT("mpProcessPushMessages: Msg error\n");
		return;
	}

	switch (result)
	{
	case PUSH_MESSAGE_KICK_OUT:
		{
			m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
		}
		break;

	case PUSH_MESSAGE_LEAVE_SESSION:
		{
			// rebuild the name lists
			//char *name = NULL;
			//BYTE nameLen = 0;
			//if (currentPacket->getString(name, nameLen) == false)
			//{
			//	XP_DEBUG_OUT("mpProcessPushMessages: Leave msg error\n");
			//	return;
			//}

			//int pos = 0;
			//while (pos < noitems)
			//{
			//	if (XP_API_STRCMP(name, m_lNameList[pos]) == 0)
			//	{
			//		break;
			//	}
			//	pos++;
			//}

			//XP_API_DEL(name);
			//name = NULL;

			//if (pos == noitems)
			//{
			//	return;
			//}

			//// have to remove the player ..  realloc the array

			//char** newnamelist = XP_API_NEW char*[noitems - 1];
			//BYTE* newbytedatalist = XP_API_NEW BYTE[noitems - 1];

			//if (pos > 0)
			//{// copy the players until the one 
			//	XP_API_MEMCPY(newnamelist,m_lNameList,pos*sizeof(char*));
			//	XP_API_MEMCPY(newbytedatalist,m_lByteDataList,pos);
			//}

			//if (pos < (noitems -1))
			//{// copy the players after
			//	XP_API_MEMCPY(newnamelist + pos,m_lNameList + pos + 1,(noitems - pos - 1)*sizeof(char*));
			//	XP_API_MEMCPY(newbytedatalist + pos,m_lByteDataList + pos + 1,noitems - pos - 1);
			//}

			//// free the removed player
			//XP_API_DEL(m_lNameList[pos]);
			//m_lNameList[pos] = NULL;

			//// switch the buffers
			//XP_API_DEL(m_lNameList);
			//m_lNameList = NULL;
			//m_lNameList = newnamelist;

			//XP_API_DEL(m_lByteDataList);
			//m_lByteDataList = NULL;
			//m_lByteDataList = newbytedatalist;

			//noitems--;
		}
		break;

	case PUSH_MESSAGE_JOIN_SESSION:
		{
			//BYTE index = 0;
			//if (currentPacket->getByte(index) == false)
			//{
			//	XP_DEBUG_OUT("mpProcessPushMessages: Join msg error\n");
			//	return;
			//}

			//char *name = NULL; BYTE nameLen = 0;
			//if (currentPacket->getString(name, nameLen) == false)
			//{
			//	XP_DEBUG_OUT("mpProcessPushMessages: Join msg error 2\n");
			//	return;
			//}

			//BYTE newlen = 0;
			//if (m_lNameList != NULL)
			//{
			//	newlen = noitems + 1;
			//}
			//else
			//{
			//	newlen = 1;
			//}

			//char** newnamelist = XP_API_NEW char*[newlen];
			//BYTE* newbytedatalist = XP_API_NEW BYTE[newlen];

			//XP_API_MEMCPY(newnamelist,m_lNameList,noitems*sizeof(char*));
			//XP_API_MEMCPY(newbytedatalist,m_lByteDataList,noitems);

			//newnamelist[noitems] = name;
			//newbytedatalist[noitems] = index;

			//// switch the buffersw 
			//XP_API_DEL(m_lNameList);
			//m_lNameList = NULL;
			//m_lNameList = newnamelist;

			//XP_API_DEL(m_lByteDataList);
			//m_lByteDataList = NULL;
			//m_lByteDataList = newbytedatalist;       

			//noitems = newlen;
		}
		break;

	case PUSH_MESSAGE_START_GAME:
		{
			// rebuild namelist
			if ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION_MASTER )
			{
				// the master must ignore this message, the start reply is on the way ..
				break;
			}

			// rebuild the name list .. 
			//clearListedItems();

			//if (currentPacket->getByte(noitems) == false)
			//{
			//	XP_DEBUG_OUT("mpProcessPushMessages: Start msg error\n");
			//	return;
			//}

			//m_lNameList = XP_API_NEW char*[noitems];
			//m_lByteDataList = XP_API_NEW BYTE[noitems];

			//BYTE len = 0;
			//for (int i = 0; i < noitems; i++)
			//{
			//	currentPacket->getString(m_lNameList[i],len);
			//	m_lByteDataList[i] = i + 1;
			//}
			//if ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION )
			//{
			//	m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;
			//	m_MPHasOpponentFinished = false;
			//}
			//else
			//{
			//	m_lastErrorCode = ERROR_WRONGFULL_RSTATE;
			//}
		}
		break;

	case PUSH_MESSAGE_FINISH_GAME:
		{
			m_MPHasOpponentFinished = true;
		}
		break;
	}

	XP_API_DEL( currentPacket );
	currentPacket = NULL;

	XP_DEBUG_OUT("}\n");
}

// mpSendGameData MUST be called afterwards or the DataPacket MUST be deleted
// to avoid memory leaks.
DataPacket *GLXPlayerMPbyP2P::mpPrepareGameData()
{   
	if (!mpIsInGame())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return NULL;
	}

	DataPacket* gamePacket = new DefaultDataPacket();
	gamePacket->addByte(BASIC_MESSAGE_TYPE_GAME);
	gamePacket->addByte(GAME_MESSAGE_IN_GAME);
	return gamePacket;
}

void GLXPlayerMPbyP2P::mpSendGameData(DataPacket* packet, bool bReliable)
{
	if ( !mpIsInGame() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	m_curr_room->SendGameData( packet, bReliable );
	m_lastErrorCode = ERROR_NONE;
}

void GLXPlayerMPbyP2P::mpDisconnect()
{
	wtcp->disconnect();

	// cleanup .. 
	if (currentPacket != NULL)
	{
		XP_API_DEL( currentPacket );
		currentPacket = NULL;
	}

	m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;   
}

void GLXPlayerMPbyP2P::mpSendListRoom( BYTE count, BYTE start, DataPacket * condition )
{
	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	if ( start < 0 )	// make sure we don't request negative indexess
	{
		start = 0;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendListSessionsPackage( count,start );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

void GLXPlayerMPbyP2P::mpSendListRoomByName( BYTE count, BYTE start, char * name )
{
	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	if ( start < 0 )	// make sure we don't request negative indexess
	{
		start = 0;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendListSessionsByNamePackage( count,start, name );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

void GLXPlayerMPbyP2P::mpSendListRoomByData( BYTE count, BYTE start, DataPacket * condition )
{
	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	if ( start < 0 )	// make sure we don't request negative indexess
	{
		start = 0;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendListSessionsByDataPackage( count,start, (char *)condition->getMessageBody(), condition->getMessageLen() );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

void GLXPlayerMPbyP2P::mpSendListAllRoom( BYTE count, BYTE start )
{
	if ( !mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	if ( count <= 0 )	// try get at least one session ..
	{
		count = 1;
	}

	if ( start < 0 )	// make sure we don't request negative indexess
	{
		start = 0;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendListSessionsAllPackage( count,start );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

void GLXPlayerMPbyP2P::mpSendCreateRoom( char * name, DataPacket * room_data, DataPacket * player_data,
										int min_player_count, int max_player_count, char * password )
{
	mpClearRoomData();

	// Create new room in local.
	m_curr_room = XP_API_NEW COnlineGameRoom();
	m_curr_room->SetName( name );
	m_curr_room->SetData( room_data );
	m_curr_room->SetMinPlayerCount( min_player_count );
	m_curr_room->SetMaxPlayerCount( max_player_count );
	m_curr_room->SetPassword( password );

	// Add host player.
	m_curr_room->AddPlayer( m_username, m_country, player_data, m_ip_router->getIP(),
		m_ip_local->getIP(), m_ip_router->getTcpPort(), m_ip_router->getUdpPort(),
		m_ip_local->getTcpPort(), m_ip_local->getUdpPort() );
	m_curr_room->SetHost( 0 );
	m_upnpSetState=SET_UPNP_BEGIN;

}

void GLXPlayerMPbyP2P::mpSendJoinRoom( char * name, DataPacket * player_data, char * password )
{
	if(m_connectState == CONNECT_STATE_START)
	{
		isStartJoin = true;
		// Search host index
		int index = -1;
		for( int i=0; i<m_room_list->Count(); i++ )
		{
			COnlineGameRoom * room = m_room_list->Get( i );
			if ( XP_API_STRCMP( name, room->GetName() ) == 0 )
			{
				index = i;
				break;
			}
		}

		if ( index < 0 )
		{
			XP_DEBUG_OUT( "Get session index failed!!!!!\n" );
			m_lastErrorCode = ERROR_CONNECTION;
			m_connectState = CONNECT_STATE_FAILED;
			return;
		}

		m_curr_room = m_room_list->Get( index );
		m_curr_room->SetPassword( password );

		if ( !m_curr_room->AddPlayer( m_username, m_country, player_data, m_ip_router->getIP(), 
			m_ip_local->getIP(), m_ip_router->getTcpPort(), m_ip_router->getUdpPort(),
			m_ip_local->getTcpPort(), m_ip_local->getUdpPort() ) )
		{
			m_connectState = CONNECT_STATE_FAILED;
			return;
		}

		m_connectState = CONNECT_STATE_WAITING;
	}
	else if(m_connectState == CONNECT_STATE_WAITING)
	{
		bool connected = m_curr_room->JoinRoom( m_username, m_country, player_data, m_ip_router->getIP(), 
			m_ip_local->getIP(), m_ip_router->getTcpPort(), m_ip_router->getUdpPort(),
			m_ip_local->getTcpPort(), m_ip_local->getUdpPort(), ONLINE_PLAYER_CLINET );

		if ( !connected )
		{
			if((m_curr_room->GetJoinRoomState()==COnlineGameRoom::JOINSTATE_WAITING) ||
				(m_curr_room->GetJoinRoomState()==COnlineGameRoom::JOINSTATE_PERPARE))
			{
				return;
			}
			else
			{
				XP_DEBUG_OUT( "Connect failed!!!!!\n" );
				m_lastErrorCode = ERROR_CONNECTION;
				m_connectState = CONNECT_STATE_FAILED;
				m_curr_room->SetJoinRoomState(COnlineGameRoom::JOINSTATE_START);
				return;
			}
		}
		else
		{
			m_connectState = CONNECT_STATE_SUCCESS;
			m_lastErrorCode = ERROR_PENDING;
			m_curr_room->SetJoinRoomState(COnlineGameRoom::JOINSTATE_START);
		}
	}
}

void GLXPlayerMPbyP2P::mpSendLeaveRoom()
{
	if ( !mpIsInRoom() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	if ( m_curr_room->AmITheHost() )
	{
		wtcp->sendLeaveSessionPackage();

#ifdef MP_ENABLE_TIMEOUT
		//Set timer
		wtcp->last_activity_time = XP_API_GET_TIME();
#endif
	}
	else
	{
		m_curr_room->LeaveRoom();
	}
}

void GLXPlayerMPbyP2P::mpSendKickOutPlayer(unsigned short* playerName)
{
	char tmp[256];

	ConvertUnicodeToUTF8_Old(tmp, playerName);
	mpSendKickOutPlayer(tmp);
}


void GLXPlayerMPbyP2P::mpSendKickOutPlayer(char* playerName)
{
	if ( !mpIsInRoom() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	if ( m_curr_room->AmITheHost() )
	{
		m_curr_room->KickoutPlayer( playerName );
	}
}

void GLXPlayerMPbyP2P::mpSendLeaveGame()
{
	if ( !mpIsInGame() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	if ( m_curr_room->AmITheHost() )
	{
		m_curr_room->HostLeave();
	}
	else
	{
		m_curr_room->LeaveRoom();
	}

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

void GLXPlayerMPbyP2P::mpSendStartGame()
{
	if ( !mpIsInRoom() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	if ( m_curr_room->CanStart() )
	{
		m_lastErrorCode = ERROR_PENDING;
		wtcp->sendStartGamePackage();

#ifdef MP_ENABLE_TIMEOUT
		//Set timer
		wtcp->last_activity_time = XP_API_GET_TIME();
#endif
	}
}

void GLXPlayerMPbyP2P::mpSendFinishGame()
{
	if ( !mpIsInGame() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		return;
	}

	m_lastErrorCode = ERROR_PENDING;
	m_curr_room->FinishGame();
}

bool GLXPlayerMPbyP2P::mpKeepConnectionToGameServer()
{
	if ( !wtcp )
	{
		return false;
	}

	if ( !wtcp->isConnected() )
	{
		// communication error occured, disconnect.
		return false;
	}

	wtcp->receiveDataLen();
	wtcp->keepConnectionAlive();
	return true;
}

long GLXPlayerMPbyP2P::mpRecvPing( char ** ip, int * port, char * data, int size )
{
	int iResult = m_socket_ping->Select( GLXPlayerSocket::XSOCKET_SELECT_READ );
	if ( iResult > 0 )
	{
		int iCount = m_socket_ping->RecvFromUnkownIP( data, size, ip, port );
		return iCount;
	}

	return -1;
}

void GLXPlayerMPbyP2P::mpHandlePing()
{
	if ( NULL == m_room_list )
	{
		return;
	}

	// Send udp ping package to every room host
	for ( int i=0; i<m_room_list->Count(); i++ )
	{
		COnlineGameRoom * room = m_room_list->Get( i );
		if ( 0 == room->GetPingStartTime() )
		{
			room->SendPing( m_socket_ping, m_ip_router );
		}
	}

	char * ip = NULL;
	int port = 0;
	char data[128] = { 0 };

	// Receive responsed ping package
	if ( mpRecvPing( &ip, &port, data, sizeof(data) ) > 0 )
	{		
		int index = 0;

		char name[128] = { 0 };
		XP_API_PARSE_DATA( data, name, 1, XPLAYER_DATA_SEPERATOR );

		for ( index=0; index<m_room_list->Count(); index++ )
		{
			COnlineGameRoom * room = m_room_list->Get( index );
			if ( XP_API_STRCMP( room->GetName(), name ) == 0 )
			{
				break;
			}
		}

		if ( index < m_room_list->Count() )
		{
			char buffer[128] = { 0 };
			XP_API_SPRINTF( buffer, "%c|%s|%d", BASIC_MESSAGE_TYPE_PING, m_room_list->Get( index )->GetName(), 
				m_room_list->Get( index )->GetPingStartTime() );

			// Check which ping package has returned
			if ( XP_API_STRCMP( buffer, data ) == 0 )
			{				
				long time_send = m_room_list->Get( index )->GetPingStartTime();
				long time_recv = XP_API_GET_TIME();
				long time_spend = time_recv - time_send;

				if ( time_spend > 30 )
				{
					time_spend -= 30;
				}
				m_room_list->Get( index )->SetPingEndTime( time_send + time_spend );
			}
		}

		XP_API_DEL( ip );		
	}
}

int GLXPlayerMPbyP2P::mpBuildSessionData( char ** buffer, COnlinePlayer * player, char * nationality_data, 
										 char * sessiondata, int sessiondatalen, bool password_flag )
{
	// ${internal_ip}|${external_ip}|${tcp_data_external_port}|${udp_data_external_port}|${ping_external_port}|
	// ${nationality_data}|${password_flag}|${session_data}
	int len_total = 0;
	char header[32] = { 0 };

	XP_DEBUG_OUT( "[[[[ Create Room = %s %d %d %s %d %d %s ]]]]\n", 
		player->GetInternalIP(), player->GetInternalTcpPort(), player->GetInternalUdpPort(),
		player->GetExternalIP(), player->GetExternalTcpPort(), player->GetExternalUdpPort(), nationality_data );

	int local_ip = XP_API_IP_STR2INT( player->GetInternalIP() );
	int router_ip = XP_API_IP_STR2INT( player->GetExternalIP() );

	DefaultDataPacket * packet = XP_API_NEW DefaultDataPacket();
	packet->addInt( local_ip );				// local ip
	packet->addShort( player->GetInternalTcpPort() );		// local tcp port
	packet->addShort( player->GetInternalUdpPort() );		// local udp port
	packet->addInt( router_ip );			// router ip
	packet->addShort( player->GetExternalTcpPort() );	// router tcp port
	packet->addShort( player->GetExternalUdpPort() );	// router udp port
	packet->addByteLenString( nationality_data, XP_API_STRLEN( nationality_data ) );	// nationality
	if ( password_flag )
	{
		packet->addByte( 0x1 );				// has password
	}
	else
	{
		packet->addByte( 0x0 );				// no password
	}
	packet->addByteLenString( sessiondata, sessiondatalen );							// room data
	packet->packMessage();

	len_total = packet->getMessageLen();
	*buffer = XP_API_NEW char[len_total];
	XP_API_MEMSET( *buffer, 0, len_total );
	XP_API_MEMCPY( *buffer, packet->getMessageBody(), len_total );

	XP_API_DEL( packet );

	return len_total;
}

bool GLXPlayerMPbyP2P::mpParseSessionData( int index, char * inter_ip, char * exter_ip, 
										  char * inter_ping, char * exter_ping, bool bUdp )
{
	char ip_internal[32] = { 0 };
	char ip_external[32] = { 0 };
	char port_data[8] = { 0 };
	char port_data_udp[8] = { 0 };
	char port_ping[8] = { 0 };
	int no = 0;

	XP_API_MEMSET( ip_internal, 0, 32 );
	XP_API_MEMSET( ip_external, 0, 32 );
	XP_API_MEMSET( port_data, 0, 8 );
	XP_API_MEMSET( port_data_udp, 0, 8 );
	XP_API_MEMSET( port_ping, 0, 8 );

	getValue( m_lBinaryDataList_Head[index], ip_internal, no++, '|' );
	getValue( m_lBinaryDataList_Head[index], ip_external, no++, '|' );
	getValue( m_lBinaryDataList_Head[index], port_data, no++, '|' );
	getValue( m_lBinaryDataList_Head[index], port_data_udp, no++, '|' );
	getValue( m_lBinaryDataList_Head[index], port_ping, no++, '|' );

	if ( NULL != inter_ip )
	{
		if ( bUdp )
		{
			XP_API_SPRINTF( inter_ip, "%s:%s", ip_internal, port_data_udp );
		}
		else
		{
			XP_API_SPRINTF( inter_ip, "%s:%s", ip_internal, port_data );
		}
	}

	if ( NULL != exter_ip )
	{
		if ( bUdp )
		{
			XP_API_SPRINTF( exter_ip, "%s:%s", ip_external, port_data_udp );
		}
		else
		{
			XP_API_SPRINTF( exter_ip, "%s:%s", ip_external, port_data );
		}
	}

	if ( NULL != inter_ping )
	{
		XP_API_SPRINTF( inter_ping, "%s:%s", ip_internal, port_ping );
	}

	if ( NULL != exter_ping )
	{
		XP_API_SPRINTF( exter_ping, "%s:%s", ip_external, port_ping );
	}

	return true;
}

void GLXPlayerMPbyP2P::mpClearRoomData()
{
	if ( NULL == m_room_list )
	{
		XP_API_DEL( m_curr_room );
		m_curr_room = NULL;
	}
	else
	{
		XP_API_DEL( m_room_list );
		m_room_list = NULL;
		m_curr_room = NULL;
	}
}

void GLXPlayerMPbyP2P::mpSendUpdateSession()
{
	if ( m_curr_room->AmITheHost() )
	{
		wtcp->sendUpdateSessionPackage( m_curr_room->GetPlayerCount() );
	}
}

void GLXPlayerMPbyP2P::mpSendUPnPPortMapping()
{
	m_upnp_enabled = false;
	m_upnp_tested = true;
	m_lastErrorCode = ERROR_PENDING;

	// Get local ip address
	char upnp_ip[128];
	XP_API_MEMSET(upnp_ip,0,128);
	PortMapper::GetInstance()->GetLocalIP( upnp_ip );
	m_ip_local->setIP( upnp_ip );

	if ( !m_ip_local->IsPrivateAddress() )
	{		
		m_ip_router->setIP( upnp_ip );
		m_ip_router->setTcpPort( m_ip_local->getTcpPort() );		
		m_ip_router->setUdpPort( m_ip_local->getUdpPort() );

		m_upnp_tested = true;
		m_upnp_enabled = false;
		m_lastErrorCode = ERROR_NONE;
		m_observer->OnMPuPnPTestSuccess();
		return;
	}

	// Check whether UPnP is enable or disable.

	PortMapper::GetInstance()->Init( m_ggi, m_username, m_upnp_try_times );
	//if( !PortMapper::GetInstance()->IsIGDFound() )
	//{
	//	m_lastErrorCode = ERROR_MP_UPNP_DISABLE;
	//	m_observer->OnMPFunctionError("uPnP disabled!", m_lastErrorCode);
	//	return;
	//}

	//// Get router ip address
	//XP_API_MEMSET(upnp_ip,0,128);
	//if( !PortMapper::GetInstance()->GetExternIP( upnp_ip ) )
	//{
	//	m_lastErrorCode = ERROR_MP_UPNP_GET_EXTERNAL_IP;
	//	m_observer->OnMPFunctionError("external ip error!", m_lastErrorCode);
	//	return;
	//}
	//m_ip_router->setIP( upnp_ip );

	//m_upnp_enabled = true;
	//m_lastErrorCode = ERROR_NONE;
	//m_observer->OnMPuPnPTestSuccess();
	return;
}

bool GLXPlayerMPbyP2P::mpCheckGameData( char * &name, DataPacket * &packet )
{
	if ( NULL == m_curr_room )
	{
		return false;
	}

	return m_curr_room->CheckGameData( name, packet );
}

void GLXPlayerMPbyP2P::mpUpdateRoom()
{
	int player_count = 0;

	if ( m_curr_room->AmITheHost() )
	{
		player_count = m_curr_room->GetPlayerCount();
	}

	m_curr_room->UpdateData();

	if ( m_curr_room->AmITheHost() && ( m_curr_room->GetPlayerCount() != player_count ) )
	{
		mpSendUpdateSession();
	}

	if ( !mpKeepConnectionToGameServer() )
	{
		m_lastErrorCode = ERROR_CONNECTION;
		mpDisconnect();
	}

	// The game had been started.
	if ( m_curr_room->IsInGame() )
	{
		if ( !m_curr_room->AmITheHost() )
		{
			mpSendDisconnect();
		}
		m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;
		m_observer->OnMPStartGameSuccess();
		return;
	}

	//leave room
	if(!m_curr_room->IsInRoom())
	{
		m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;
		mpClearRoomData();
		//m_observer->OnMPKickOutPlayerSuccess();
		m_observer->OnMPLeaveSessionSuccess();
		return;
	}
}

void GLXPlayerMPbyP2P::mpUpdateGame()
{
	//// Receive the response of disconnect request ( sent in starting game )
	//if ( wtcp->isConnected() )
	//{
	//	wtcp->receiveDataLen();
	//	if ( mpProcessIncomingMessages() )
	//	{
	//		if ( m_type == BASIC_MESSAGE_TYPE_CONNECT && m_subtype == CONNECTION_MESSAGE_TYPE_DISCONECT )
	//		{
	//			mpDisconnect();
	//		}
	//	}
	//	XP_API_DEL( currentPacket );
	//	currentPacket = NULL;
	//}

	m_curr_room->UpdateData();

	//game finished
	if ( m_curr_room->IsGameFinished() )
	{
		//mpClearRoomData();
		m_observer->OnMPFinishGameSuccess();
		return;
	}

	//kicked out or leave game
	if(!m_curr_room->IsInGame())
	{
		m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;
		mpClearRoomData();
		//m_observer->OnMPKickOutPlayerSuccess();
		m_observer->OnMPLeaveGameSuccess();
		return;
	}
}

void GLXPlayerMPbyP2P::mpUpdateGameFinish()
{
	m_curr_room->UpdateData();

	//start a new game
	if(m_curr_room->IsInGame())
	{
		m_observer->OnMPStartGameSuccess();
		return;
	}

	//leave game
	if ( !m_curr_room->IsGameFinished() )
	{
		mpClearRoomData();
		m_observer->OnMPLeaveGameSuccess();
		return;
	}
}

void GLXPlayerMPbyP2P::mpHandleRoomData()
{
	m_curr_room->UpdateData();

	if(m_curr_room->IsInRoom())
	{
		m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;
		m_observer->OnMPJoinSessionSuccess();
	}

	if ( m_curr_room->IsJobDone() )
	{
		if ( m_curr_room->IsJobSuccessed() )
		{
			m_lastErrorCode = ERROR_NONE;
		}
		else
		{
			m_lastErrorCode = m_curr_room->GetLastError();
		}

		XP_DEBUG_OUT( "mpP2PHandleRoomData(): m_lastErrorCode = %d\n", m_lastErrorCode );

		return;
	}

	// Send and receive keep alive package
	if ( !mpKeepConnectionToGameServer() )
	{
		m_lastErrorCode = ERROR_CONNECTION;
		mpDisconnect();
	}
}

void GLXPlayerMPbyP2P::mpUpdateServerInteraction()
{
	if(wtcp == NULL)
	{
		return;
	}

	// Make sure that we are not in a Signal Loss condition.
	if (wtcp->isConnected() == false)
	{
		if(m_connectState == CONNECT_STATE_WAITING)
		{
			if(!wtcp->connect())
			{
				if(wtcp->GetConnectState() == Connection::CONNECT_STATE_WAITING)
				{
					return;
				}
				else
				{
					m_connectState = CONNECT_STATE_FAILED;
					wtcp->SetConnceState(Connection::CONNECT_STATE_START);

					// communication error occured, disconnect.
					m_lastErrorCode = ERROR_CONNECTION;
					m_observer->OnMPNetworkError(m_lastErrorCode);
					mpDisconnect();
					return;					
				}
			}
			else
			{
#ifdef MP_ENABLE_TIMEOUT 
				wtcp->last_activity_time = (long) XP_API_GET_TIME();
#endif // MP_ENABLE_TIMEOUT
				m_connectState = CONNECT_STATE_SUCCESS;
				wtcp->SetConnceState(Connection::CONNECT_STATE_START);
				return;
			}
		}
		else
		{
			// communication error occured, disconnect.
			m_lastErrorCode = ERROR_CONNECTION;
			m_canReceiveFromServer = false;
			m_observer->OnMPNetworkError(m_lastErrorCode);
			mpDisconnect();
			return;
		}

	}

	wtcp->receiveDataLen();

#ifdef MP_ENABLE_TIMEOUT 
	if (XP_API_GET_TIME() - wtcp->last_activity_time > MP_CONN_TIMEOUT)
	{
		XP_DEBUG_OUT("Timeout expired!\n");
		m_lastErrorCode = ERROR_REQUEST_TIME_OUT;
		m_observer->OnMPNetworkError(m_lastErrorCode);
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

void GLXPlayerMPbyP2P::mpUpdateHostClientInteraction()
{
	m_lastErrorCode = ERROR_PENDING;

	if ( NULL == m_curr_room )
	{
		m_lastErrorCode = ERROR_BAD_RESPONSE;
		return;
	}	

	if ( m_curr_room->IsInRoom() )
	{
		mpUpdateRoom();
	}
	else if ( m_curr_room->IsInGame() )
	{
		mpUpdateGame();
	}
	else if(m_curr_room->IsGameFinished())
	{
		mpUpdateGameFinish();
	}
	else
	{
		if(isStartJoin)
		{
			if(m_connectState == GLXPlayerMPBase::CONNECT_STATE_WAITING)
			{
				mpSendJoinRoom( NULL, NULL);
			}

			if(m_connectState == GLXPlayerMPBase::CONNECT_STATE_SUCCESS)
			{
				m_connectState = GLXPlayerMPBase::CONNECT_STATE_START;
				isStartJoin = false;
			}
			else if(m_connectState == GLXPlayerMPBase::CONNECT_STATE_FAILED)
			{
				m_connectState = GLXPlayerMPBase::CONNECT_STATE_START;
				isStartJoin = false;
				m_observer->OnMPNetworkError(ERROR_CONNECTION);
			}
		}

		if(m_curr_room->GetMyself() != NULL)
			mpHandleRoomData();
	}
}

void GLXPlayerMPbyP2P::Update()
{
	if(m_canReceiveFromServer == true)
	{
		mpUpdateServerInteraction();
	}
	mpUpdateHostClientInteraction();

	UpdateUpnpSet();
	PortMapper::GetInstance()->update();
}

//=============upnp observer===============

void GLXPlayerMPbyP2P::OnNetworkError()
{

}

void GLXPlayerMPbyP2P::OnRequestTimeout()
{

}

void GLXPlayerMPbyP2P::OnUpnpRequestFailed(char *errMsg, int errCode)
{
	m_lastErrorCode = errCode;
	m_observer->OnMPFunctionError(errMsg, m_lastErrorCode);
	if(errCode==ERROR_MP_UPNP_MAPPING)
	{
		m_upnpSetState=SET_UPNP_NONE;
	}
}

void GLXPlayerMPbyP2P::OnUpnpInitSuccess(const char* externIp)
{
	m_ip_router->setIP( externIp );

	m_upnp_enabled = true;
	m_lastErrorCode = ERROR_NONE;
	m_observer->OnMPuPnPTestSuccess();
}

void GLXPlayerMPbyP2P::OnAddPortMappingSuccess(int type,int portMapped)
{
	m_curr_room->OnAddPortMappingSuccess(type,portMapped);
}

void GLXPlayerMPbyP2P::OnRemovePortMappingSuccess()
{

}

void GLXPlayerMPbyP2P::OnRemoveAllPortMappingSuccess()
{

}

//=============upnp observer===============

/**
*
*terminate the thread
*/
void GLXPlayerMPbyP2P::CloseUpnpThread()
{
	//isCloseUpnpThread=true;
	PortMapper::GetInstance()->Terminate();

}

void GLXPlayerMPbyP2P::UpdateUpnpSet()
{
	switch(m_upnpSetState)
	{
	case SET_UPNP_NONE:
		return;
	case SET_UPNP_BEGIN:
		m_upnpSetState=SET_UPNP_WAIT;
		m_curr_room->InitSetMyself();
	case SET_UPNP_WAIT:
		if(m_curr_room->SetMyself( 0, ONLINE_PLAYER_HOST ))
		m_upnpSetState=SET_UPNP_END;
		return;
	case SET_UPNP_END:
		{
			m_upnpSetState=SET_UPNP_NONE;
			// Add external ip, port, and nationality as the header of the session data
			// For example: 10.15.0.171|219.142.114.185|56800|56800|56810|CN|gamedata
			int len = mpBuildSessionData( &currentsessiondata, m_curr_room->GetMyself(), m_country, 
				(char *)(m_curr_room->GetData())->getMessageBody(), (m_curr_room->GetData())->getMessageLen(), m_curr_room->HasPasswordSet()  );

			// Create new game room on Game Server
			m_lastErrorCode = ERROR_PENDING;
			wtcp->sendCreateSessionPackage( m_curr_room->GetName(), currentsessiondata, len );

			XP_API_DEL(currentsessiondata);

#ifdef MP_ENABLE_TIMEOUT
			//Set timer
			wtcp->last_activity_time = XP_API_GET_TIME();
#endif
		}
		return;
	}
}