#include "../../../../include/Multiplayer/ForwardbyServer/GLXPlayerMPbyServer.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif


/**
* GLXPlayerMPbyServer Constructor. 
*/
GLXPlayerMPbyServer::GLXPlayerMPbyServer(GLXPlayerMPBaseObserver * observer): GLXPlayerMPBase(observer)
{
	Init();	
}

/**
* GLXPlayerMPbyServer Destructor. 
*/
GLXPlayerMPbyServer::~GLXPlayerMPbyServer(void)
{
	XP_API_DEL( MP_SERVER_ADDRESS );

	if (wtcp != NULL)
	{
		mpDisconnect();
		XP_API_DEL( wtcp );
	}

	clearInGameMessages();
	clearListedItems();
	clearCurrentSession();	
}

void GLXPlayerMPbyServer::Init()
{	
	wtcp = NULL;	
	
	//init the multiplayer members 
	//currentPacket     = NULL;
	incomingGameData  = NULL;
	m_lNameList       = NULL;
	m_lBinaryDataList = NULL;
	m_lByteDataList   = NULL;

	noitems        = 0;
	nosessionsbase = 0;

	currentsessionname = NULL;
	currentsessiondata = NULL;

	m_type    = -1;
	m_subtype = -1;

	m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;	
	m_MPHasOpponentFinished = false;
	
	m_errMsg = NULL;
	
	LoadConfig();
}

/**
 * Function LoadConfig.
 * Load the Multiplayer configuration: server address, server socket, ...
 */
bool GLXPlayerMPbyServer::LoadConfig()
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

/**
 * Function clearListedItems.
 * Clear all items list.	 
 */
void GLXPlayerMPbyServer::clearListedItems()
{
	XP_API_DEL_ARRAY(m_lNameList,noitems);
	m_lNameList = NULL;	
	
	XP_API_DEL_ARRAY(m_lBinaryDataList,noitems);
	m_lBinaryDataList = NULL;
	
	XP_API_DEL(m_lByteDataList);
	m_lByteDataList = NULL;

	noitems = 0;
	nosessionsbase = 0;
}

/**
 * Function clearInGameMessages.
 * Clears the internal buffer for storing in game messages
 */
void GLXPlayerMPbyServer::clearInGameMessages()
{
   if (incomingGameData == NULL)
   {
      return;
   }
   // Delete all incoming packets.
   // This logic ensures that we don't try to delete a NULL, and that we don't
   // miss any packets.
   DataPacket* temp = NULL;
   while(incomingGameData != NULL)
   {
      temp = incomingGameData;
      // move to next
      incomingGameData = incomingGameData->getNextPacket();
      // delete current packet
      delete temp;
   }
   incomingGameData = NULL;
}

/**
 * Function clearCurrentSession.
 * Clears the data for the current session
 */
void GLXPlayerMPbyServer::clearCurrentSession()
{
   XP_API_DEL(currentsessionname);
   currentsessionname = NULL;

   XP_API_DEL(currentsessiondata);
   currentsessiondata = NULL;
}

/**
* Function getNextInGameMessage.
* Get the next in game message received, or NULL if none.
* Notice: The caller MUST delete the DataPacket
*/
DataPacket* GLXPlayerMPbyServer::getNextInGameMessage()
{
	if ( !mpIsInGame() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return NULL;
	}

	if ( incomingGameData == NULL )
	{
		return NULL;
	}

	DataPacket* packet = incomingGameData;
	incomingGameData = packet->getNextPacket();

	return packet;
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
bool GLXPlayerMPbyServer::mpProcessIncomingMessages()
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

						clearListedItems();

						noitems = 1;
						m_lNameList = XP_API_NEW char*[noitems];
						m_lByteDataList = XP_API_NEW BYTE[noitems];
						m_lNameList[0] = XP_API_STRNEW( m_username );
						m_lByteDataList[0] = 1;
						m_lastErrorCode = ERROR_NONE;
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

						clearListedItems();

						currentPacket->getByte(noitems);
						int tmp = 0;
						currentPacket->getInt(tmp);
						nosessionsbase = tmp;

						if (noitems > 0)
						{
							m_lNameList = XP_API_NEW char*[noitems];
							m_lByteDataList = XP_API_NEW BYTE[noitems];
							m_lBinaryDataList = XP_API_NEW char*[noitems];							

							// get the session names
							BYTE len = 0;
							int cnt;

							for (cnt = 0; cnt < noitems; cnt++)
							{
								currentPacket->getByteLenString(m_lNameList[cnt], len);
								currentPacket->getByte(m_lByteDataList[cnt]);
							}

							// get the session data ..
							for (cnt = 0; cnt < noitems; cnt++)
							{
								currentPacket->getByteLenString(m_lBinaryDataList[cnt], len);
							}
						}

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
					if ( !currentPacket->getByte( result ) )
					{
						XP_DEBUG_OUT("Msg error 2\n");
						XP_API_DEL( currentPacket );
						currentPacket = NULL;
						m_lastErrorCode = ERROR_BAD_RESPONSE;
						m_observer->OnMPNetworkError(m_lastErrorCode);
						return false;
					}

					if ( result == RESPONSE_MESSAGE_SUCCESS )
					{
						m_MPConnectLevel = CONNECT_LEVEL_IN_SESSION;

						clearListedItems();
						// get the player names ..
						currentPacket->getByte( noitems );

						if ( noitems > 0 )
						{
							m_lNameList = XP_API_NEW char*[noitems];
							m_lByteDataList = XP_API_NEW BYTE[noitems];							

							BYTE len = 0;
							for (BYTE cnt = 0; cnt < noitems; cnt++)
							{
								currentPacket->getByteLenString(m_lNameList[cnt],len);
								m_lByteDataList[cnt] = (cnt + 1);
							}

							// get sessiondata
							currentPacket->getByteLenString( currentsessiondata, len );

							BYTE started = 0;
							currentPacket->getByte( started );
						}

						m_lastErrorCode = ERROR_NONE;
						m_observer->OnMPJoinSessionSuccess();
					}
					else
					{
						setErrMsg();
						m_lastErrorCode = ERROR_REQUEST_FAILED;
						m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
					}
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
						m_MPConnectLevel = CONNECT_LEVEL_IN_CONNECTED;
						m_lastErrorCode = ERROR_NONE;
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
						// remove the player from the list
						int pos = -1;

						char*			name = NULL;
						unsigned char	name_len;

						currentPacket->getByteLenString(name, name_len);

						for (int cnt = 0 ; cnt < noitems; cnt ++)
						{
							if (XP_API_STRCMP(name, m_lNameList[cnt]) == 0)
							{
								pos = cnt;
								break;
							}
						}

						XP_API_DEL(name);
						name = NULL;

						if (pos >= 0)
						{
							// remove that player from the list .. 
							// realloc the list 

							char**			newnamelist		=	XP_API_NEW char*[noitems - 1];
							unsigned char*	newbytedatalist	=	XP_API_NEW unsigned char[noitems - 1];

							if (pos > 0)	// copy the players until the one 
							{
								XP_API_MEMCPY(newnamelist,m_lNameList,pos*sizeof(char*));
								XP_API_MEMCPY(newbytedatalist,m_lByteDataList,pos);
							}

							if (pos < (noitems -1))	// copy the players after
							{
								XP_API_MEMCPY(newnamelist + pos,m_lNameList + pos + 1,(noitems - pos - 1)*sizeof(char*));
								XP_API_MEMCPY(newbytedatalist + pos,m_lByteDataList + pos + 1,noitems - pos - 1);
							}

							// free the removed player
							XP_API_DEL(m_lNameList[pos]);
							m_lNameList[pos] = NULL;

							// switch the buffers
							XP_API_DEL(m_lNameList);
							m_lNameList = NULL;
							m_lNameList = newnamelist;

							XP_API_DEL( m_lByteDataList );
							m_lByteDataList = NULL;
							m_lByteDataList = newbytedatalist;			

							noitems--;
						}

						m_lastErrorCode = ERROR_NONE;
						m_observer->OnMPKickOutPlayerSuccess();
					}
					else
					{
						setErrMsg();
						m_lastErrorCode = ERROR_REQUEST_FAILED;
						m_observer->OnMPFunctionError(m_errMsg, m_lastErrorCode);
					}
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
						clearListedItems();
						// get the player names ..
						currentPacket->getByte(noitems);

						if (noitems > 0)
						{
							m_lNameList = (char **)XP_API_NEW char*[noitems];
							m_lByteDataList = XP_API_NEW BYTE[noitems];

							BYTE len = 0;
							for (BYTE cnt = 0; cnt < noitems; cnt++)
							{
								currentPacket->getByteLenString(m_lNameList[cnt],len);
								m_lByteDataList[cnt] = (cnt + 1);
							}
						}

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
               if (incomingGameData == NULL)
               {
                  incomingGameData = currentPacket;
               }
               else
               {
                  incomingGameData->setNextPacket(currentPacket);
               }
               currentPacket = NULL;//mag
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


/**
 * Function mpProcessPushMessages.
 * Process the push messages received from the server	
 * IMPORTANT: The currentPacket MUST deleted after calling this function.
 */
void GLXPlayerMPbyServer::mpProcessPushMessages()
{
	if (!mpIsInSession())
	{
		XP_DEBUG_OUT("mpProcessPushMessages: not in session\n");
		return;
	}

	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpProcessPushMessages(){\n");

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
			m_observer->OnMPLeaveSessionSuccess();
		}
		break;

		case PUSH_MESSAGE_LEAVE_SESSION:
		{
			// rebuild the name lists
			char *name = NULL;
			BYTE nameLen = 0;
			if (currentPacket->getByteLenString(name, nameLen) == false)
			{
				XP_DEBUG_OUT("mpProcessPushMessages: Leave msg error\n");
				return;
			}

			int pos = 0;
			while (pos < noitems)
			{
				if (XP_API_STRCMP(name, m_lNameList[pos]) == 0)
				{
					break;
				}
				pos++;
			}

			XP_API_DEL(name);
			name = NULL;

			if (pos == noitems)
			{
				return;
			}

			// have to remove the player ..  realloc the array

			char** newnamelist = XP_API_NEW char*[noitems - 1];
			BYTE* newbytedatalist = XP_API_NEW BYTE[noitems - 1];

			if (pos > 0)
			{// copy the players until the one 
				XP_API_MEMCPY(newnamelist,m_lNameList,pos*sizeof(char*));
				XP_API_MEMCPY(newbytedatalist,m_lByteDataList,pos);
			}

			if (pos < (noitems -1))
			{// copy the players after
				XP_API_MEMCPY(newnamelist + pos,m_lNameList + pos + 1,(noitems - pos - 1)*sizeof(char*));
				XP_API_MEMCPY(newbytedatalist + pos,m_lByteDataList + pos + 1,noitems - pos - 1);
			}

			// free the removed player
			XP_API_DEL(m_lNameList[pos]);
			m_lNameList[pos] = NULL;

			// switch the buffers
			XP_API_DEL(m_lNameList);
			m_lNameList = NULL;
			m_lNameList = newnamelist;

			XP_API_DEL(m_lByteDataList);
			m_lByteDataList = NULL;
			m_lByteDataList = newbytedatalist;

			noitems--;
		}
		break;

		case PUSH_MESSAGE_JOIN_SESSION:
		{
			BYTE index = 0;
			if (currentPacket->getByte(index) == false)
			{
				XP_DEBUG_OUT("mpProcessPushMessages: Join msg error\n");
				return;
			}

			char *name = NULL; BYTE nameLen = 0;
			if (currentPacket->getByteLenString(name, nameLen) == false)
			{
				XP_DEBUG_OUT("mpProcessPushMessages: Join msg error 2\n");
				return;
			}

			BYTE newlen = 0;
			if (m_lNameList != NULL)
			{
				newlen = noitems + 1;
			}
			else
			{
				newlen = 1;
			}

			char** newnamelist = XP_API_NEW char*[newlen];
			BYTE* newbytedatalist = XP_API_NEW BYTE[newlen];

			XP_API_MEMCPY(newnamelist,m_lNameList,noitems*sizeof(char*));
			XP_API_MEMCPY(newbytedatalist,m_lByteDataList,noitems);

			newnamelist[noitems] = name;
			newbytedatalist[noitems] = index;

			// switch the buffersw 
			XP_API_DEL(m_lNameList);
			m_lNameList = NULL;
			m_lNameList = newnamelist;

			XP_API_DEL(m_lByteDataList);
			m_lByteDataList = NULL;
			m_lByteDataList = newbytedatalist;       

			noitems = newlen;
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
			clearListedItems();

			if (currentPacket->getByte(noitems) == false)
			{
				XP_DEBUG_OUT("mpProcessPushMessages: Start msg error\n");
				return;
			}

			m_lNameList = XP_API_NEW char*[noitems];
			m_lByteDataList = XP_API_NEW BYTE[noitems];

			BYTE len = 0;
			for (int i = 0; i < noitems; i++)
			{
				currentPacket->getByteLenString(m_lNameList[i],len);
				m_lByteDataList[i] = i + 1;
			}
			if ( m_MPConnectLevel == CONNECT_LEVEL_IN_SESSION )
			{
				m_MPConnectLevel = CONNECT_LEVEL_IN_GAME;
				m_MPHasOpponentFinished = false;
			}
			else
			{
				m_lastErrorCode = ERROR_WRONGFULL_RSTATE;
			}
			
			m_observer->OnMPStartGameSuccess();
		}
		break;

		case PUSH_MESSAGE_FINISH_GAME:
		{
			m_MPHasOpponentFinished = true;
			m_observer->OnMPFinishGameSuccess();
		}
		break;
	}

	XP_API_DEL( currentPacket );
	currentPacket = NULL;

	XP_DEBUG_OUT("}\n");
}

// mpSendGameData MUST be called afterwards or the DataPacket MUST be deleted
// to avoid memory leaks.
DataPacket *GLXPlayerMPbyServer::mpPrepareGameData()
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

/**
* Function mpSendGameData.
* Send a game data package, used for in game messages.
* @param packet Data packet.
*/
void GLXPlayerMPbyServer::mpSendGameData(DataPacket* packet)
{
   if (!mpIsInGame())
   {
      m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
	  m_observer->OnMPNetworkError(m_lastErrorCode);
      if (packet != NULL)
      {
         delete packet;
      }
      return;
   }

   m_lastErrorCode = ERROR_NONE;

   // close the package
   packet->packMessage();
   wtcp->addOutgoingPacket(packet);

   #ifdef MP_ENABLE_TIMEOUT 
   //Set timer
   wtcp->last_activity_time = XP_API_GET_TIME();
   #endif
}

void GLXPlayerMPbyServer::mpDisconnect()
{
   wtcp->disconnect();

   // cleanup .. 
   if (currentPacket != NULL)
   {
      XP_API_DEL( currentPacket );
      currentPacket = NULL;
   }
   clearInGameMessages();
   clearListedItems();
   clearCurrentSession();

   m_MPConnectLevel = CONNECT_LEVEL_NOT_CONNECTED;   
}

void GLXPlayerMPbyServer::mpSendQuickGame()
{
	XP_DEBUG_OUT("GLXPlayerMPbyP2P::mpSendQuickGame(){\n");

	if (!mpIsLoggedIn())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	// clear the data .. 
	clearListedItems();

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendQuickGamePackage();

   //TODO: Might have to add in timeout here if a handle function is implemented
}

/**
 * Function mpSendListSession.
 * Send a list available games request to the server.
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
void GLXPlayerMPbyServer::mpSendListSession(BYTE numberOfSessions, BYTE firstSessionIndex)
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendListSession(){\n");

	if (!mpIsLoggedIn())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	if ( numberOfSessions <= 0 )	// try get at least one session ..
	{
		numberOfSessions = 1;
	}

	if ( firstSessionIndex < 0 )	// make sure we don't request negative indexess
	{
		firstSessionIndex = 0;
	}

	m_lastErrorCode = ERROR_PENDING;	
	wtcp->sendListSessionsPackage( numberOfSessions,firstSessionIndex );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
	XP_DEBUG_OUT("}\n");
}

/**
 * Function mpSendCreateSession Support UTF-8.
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
void GLXPlayerMPbyServer::mpSendCreateSession(unsigned short* sessionname, char* sessiondata)
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendCreateSession() unicode\n");

	char tmp[256];

	ConvertUnicodeToUTF8_Old(tmp, sessionname);
	mpSendCreateSession(tmp, sessiondata);
}

/**
 * Function mpSendCreateSession.
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
void GLXPlayerMPbyServer::mpSendCreateSession( char* sessionname, char* sessiondata)
{
	if ( !mpIsLoggedIn() || mpIsInSession() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}	
	
	XP_API_MEMSET( m_username, 0, UTF8_XPLAYER_MAX_USERNAME_LENGTH + 1 );	
	XP_API_SPRINTF(m_username, sessionname);
	
	// Clear current session name and session data
	clearCurrentSession();

	// Clear player list of the room
	clearListedItems();

	currentsessionname = XP_API_STRNEW( sessionname );

	m_lastErrorCode = ERROR_PENDING;
	
	wtcp->sendCreateSessionPackage( sessionname,sessiondata,XP_API_STRLEN(sessiondata) );

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

}

/**
 * Function mpSendJoinSession Support UTF-8.
 * Join a game session on the multiplayer server.
 * <p>
 * After the game is joined, the player will wait for the session master to start the game.
 * The players list is available while being in session.
 * @param sessionname The name of the session to be joined.
 * @param sessiondata The client info send to host when a client join
 * 
 */
void GLXPlayerMPbyServer::mpSendJoinSession(unsigned short* sessionname)
{
	char tmp[256];

	ConvertUnicodeToUTF8_Old(tmp, sessionname);
	mpSendJoinSession(tmp);
}

/**
 * Function mpSendJoinSession.
 * Join a game session on the multiplayer server.
 * <p>
 * After the game is joined, the player will wait for the session master to start the game.
 * The players list is available while being in session.
 * @param sessionname The name of the session to be joined.
 * @param sessiondata The client info send to host when a client join
 * 
 */
void GLXPlayerMPbyServer::mpSendJoinSession( char* sessionname)
{
	if ( !mpIsLoggedIn() || mpIsInSession() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	// Search host index
	int index = -1;
	for( int i=0; i<noitems; i++ )
	{
		if ( XP_API_STRCMP( sessionname, m_lNameList[i] ) == 0 )
		{
			index = i;
			break;
		}
	}

	if ( index < 0 )
	{
		XP_DEBUG_OUT( "Get session index failed!!!!!\n" );
		m_lastErrorCode = ERROR_CONNECTION;
		return;
	}

	// Clear the old session data
	clearCurrentSession();
	currentsessionname = XP_API_STRNEW( sessionname );
	wtcp->sendJoinSessionPackage( sessionname );
	m_lastErrorCode = ERROR_PENDING;

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

}

/**
 * Function mpSendLeaveSession.
 * Leave a game session.
 * <p>
 * Every player can leave a session at any time. If the player is the master of the session, the session is terminated
 * and all the other players will receive a kick out push message.
 * 
 */
void GLXPlayerMPbyServer::mpSendLeaveSession()
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendLeaveSession(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	clearCurrentSession();
	clearListedItems();

	m_lastErrorCode = ERROR_PENDING;

	wtcp->sendLeaveSessionPackage();
#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

	XP_DEBUG_OUT("}\n");
}

/**
 * Function mpSendKickOutPlayer Support UTF-8.
 * Kick out a player.
 * <p>
 * The session master can kick out a player before the game is started. The kicked out player will be removed from
 * the session an receive a push message informing him about it.
 * If a player that is not master tries to kick out another player, this will result in an error.
 * @param playerName The name of the player to be kicked out.
 * 
 */
void GLXPlayerMPbyServer::mpSendKickOutPlayer(unsigned short* playerName)
{
	char tmp[256];

	ConvertUnicodeToUTF8_Old(tmp, playerName);
	mpSendKickOutPlayer(tmp);
}

/**
 * Function mpSendKickOutPlayer.
 * Kick out a player.
 * <p>
 * The session master can kick out a player before the game is started. The kicked out player will be removed from
 * the session an receive a push message informing him about it.
 * If a player that is not master tries to kick out another player, this will result in an error.
 * @param playerName The name of the player to be kicked out.
 * 
 */
void GLXPlayerMPbyServer::mpSendKickOutPlayer(char* playerName)
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendKickOutPlayer(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;
	
	wtcp->sendKickOutPlayerPackage(playerName);

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

	XP_DEBUG_OUT("}\n");
}

/**
 * Function mpSendStartGame.
 * Start the game.
 * <p>
 * The start signal can be sent only by the session master, after enough players have joined the session.
 * If another player send the start signal or the master tries to start the game when there are not enough players yet,
 * this will result in an error response.
 * After the game is started, the players can send in game messages to each other in whatever format they choose.
 * 
 */
void GLXPlayerMPbyServer::mpSendStartGame()
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendStartGame(){\n");

	if (!mpIsInSession())
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendStartGamePackage();

#ifdef MP_ENABLE_TIMEOUT
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

	XP_DEBUG_OUT("}\n");
}

/**
 * Function mpSendFinishGame.
 * Finish the game.
 * <p>
 * This message is send by each player after he finished playing the multiplayer game.
 * The player will be removed from the session. The difference between finish game and leave game is that
 * the session is not terminated when the master finished the game, but waits for all the players to finish.
 * 
 */
void GLXPlayerMPbyServer::mpSendFinishGame()
{
	XP_DEBUG_OUT("GLXPlayerMPbyServer::mpSendFinishGame(){\n");

	if ( !mpIsInGame() )
	{
		m_lastErrorCode = ERROR_WRONGFULL_QSTATE;
		m_observer->OnMPNetworkError(m_lastErrorCode);
		return;
	}

	m_lastErrorCode = ERROR_PENDING;

	wtcp->sendFinishGamePackage();

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif

	XP_DEBUG_OUT("}\n");
}

void GLXPlayerMPbyServer::mpSendUPnPPortMapping()
{
	m_observer->OnMPuPnPTestSuccess();
	return;
}

/**
* Function Update.
* Called when handling network request.
* @see #Update()
*/
void GLXPlayerMPbyServer::Update()
{
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