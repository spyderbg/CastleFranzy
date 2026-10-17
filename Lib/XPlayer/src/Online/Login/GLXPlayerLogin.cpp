#include "../../../include/Login/GLXPlayerLogin.h"
//#include "memory.h"


/**
* GLXPlayerLogin constructor.
* Create an instance of GLXPlayerLogin.
*/
GLXPlayerLogin::GLXPlayerLogin() : GLXPlayerWebComponent()
{
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );
	
	m_isLoggedIn = false;
	m_keepAliveReturns = 0;
	m_lastSendKeepAliveTime = 0;
	
	m_APNS_IP = NULL;
	m_APNS_Port = 0;	
	m_APNS_FeedBack_IP = NULL;
	m_APNS_FeedBack_Port = 0;
	m_publicPemPath = NULL;	
	m_privateKeyPemPath = NULL;	
	m_privateKeyPassword = NULL;
	
	m_productListGGI = NULL;
	m_productListID = NULL;
	m_productListName = NULL;	
	m_productListCount = 0;
	
	m_userNumber = NULL;
	m_userName = NULL;
	m_userLanguage = NULL;
	m_generalCondition = NULL;
	m_lastUpdateTime = NULL;
}

/**
* GLXPlayerLogin destructor.
* Destroy an instance of GLXPlayerLogin.
*/
GLXPlayerLogin::~GLXPlayerLogin(void)
{
	m_isLoggedIn = false;
	m_keepAliveReturns = 0;
	m_lastSendKeepAliveTime = 0;	
	
	XP_API_DEL(m_APNS_IP);	
	XP_API_DEL(m_APNS_FeedBack_IP);	
	XP_API_DEL(m_publicPemPath);	
	XP_API_DEL(m_privateKeyPemPath);	
	XP_API_DEL(m_privateKeyPassword);	
	
	XP_API_DEL(m_userNumber);	
	XP_API_DEL(m_userName);	
	XP_API_DEL(m_userLanguage);	
	XP_API_DEL(m_generalCondition);	
	XP_API_DEL(m_lastUpdateTime);	
	
	m_APNS_Port = 0;
	m_APNS_FeedBack_Port = 0;	
	clearProductInfo();
}

/**
* Function GLXPlayerLogin clearProductInfo().
* Clear the list of product's information.
*/
void GLXPlayerLogin::clearProductInfo()
{
	XP_API_DEL(m_productListGGI);
	XP_API_DEL(m_productListID);
	XP_API_DEL_ARRAY(m_productListName, m_productListCount);
	m_productListCount = 0;
}

/**
* Function Update.
* Called each frame to Update state.	
*/
void GLXPlayerLogin::Update()
{
#ifndef ENABLE_UNIT_TEST
	#ifndef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		KeepAlive();
	#endif
#endif //ENABLE_UNIT_TEST
	GLXPlayerWebComponent::Update();
}

/**
* Function KeepAlive.
* Keep connection to web server alive.	
*/
void GLXPlayerLogin::KeepAlive()
{
	if ( ShouldSendKeepAlive() )
	{
		SendKeepAlive();
	}	
}

/**
* Private function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerLogin::OnUpdateSuccess( int func_id )
{
	char token[ONLINE_MAX_PACKET_SIZE];

	if( func_id == FUNCTION_LOGIN )
	{
		if( !IsNextResponseStringToken( "u" ) )
		{
			m_observer->OnRequestFailure( func_id, ERROR_BAD_RESPONSE );
			return true;
		}

		XP_API_MEMSET( token, 0, 256 );
		XP_API_DEL( m_uid );
		m_uid = XP_API_STRNEW( GetNextResponseToken( token ) );

#ifndef MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER
		if( !IsNextResponseStringToken( "t" ) )
		{
			m_observer->OnRequestFailure( func_id, ERROR_BAD_RESPONSE );
			return true;
		}

		XP_API_MEMSET( token, 0, 256 );
		m_keepAliveReturns = XP_API_ATOI( GetNextResponseToken( token ) );
#endif
		if( IsNextResponseStringToken( "un" ) )
		{
			m_userNumber = XP_API_STRNEW(GetNextResponseToken( token ));
		}
		
		if( IsNextResponseStringToken( "n" ) )
		{
			m_userName = XP_API_STRNEW(GetNextResponseToken( token ));
		}
		
		if( IsNextResponseStringToken( "l" ) )
		{
			m_userLanguage = XP_API_STRNEW(GetNextResponseToken( token ));
		}
		
		if( IsNextResponseStringToken( "gct" ) )
		{
			m_generalCondition = XP_API_STRNEW(GetNextResponseToken( token ));
		}
		
		m_isLoggedIn = true;
		m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
	}
	else if ( func_id == FUNCTION_LOGOUT )
	{
		m_isLoggedIn = false;
		m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
	}	
	else if ( func_id == FUNCTION_KEEP_ALIVE )
	{
		GetNextResponseToken( token ); //skip 't'
		m_keepAliveReturns = XP_API_ATOI( GetNextResponseToken( token ) );
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}
	else if ( func_id == FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER )
	{		
		processApplePushServer(m_pResponsePtr);
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}
	else if ( func_id == FUNCTION_GET_PRODUCT )
	{
		processGetProductInfo(m_pResponsePtr);
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}
	else if ( func_id == FUNCTION_GET_GENERAL_CONDITIONS )
	{	
		char* tempToken = XP_API_NEW char[XP_API_STRLEN(m_pResponsePtr)];
		XP_API_DEL(m_generalCondition);
		XP_API_DEL(m_lastUpdateTime);		
		
		if( IsNextResponseStringToken( "d" ) )
		{
			m_generalCondition = XP_API_STRNEW(GetNextResponseToken( tempToken ));
		}
		
		if( IsNextResponseStringToken( "t" ) )
		{
			m_lastUpdateTime = XP_API_STRNEW(GetNextResponseToken( token ));
		}

		XP_API_DEL(tempToken);
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}
	else
	{
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}
	return true;
}

/**
* Function SendLogin.
* Login web server.
* @param name User name.
* @param password User password.
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerLogin::SendLogin( char * name, char * password, int type, char* deviceToken, bool isReturnUserNumber, bool isReturnName, char* language,  bool bSendLastTimeStamp)
{
	if ( NULL == name || NULL == password )
	{
		m_observer->OnRequestFailure( FUNCTION_LOGIN, ERROR_INIT );
		return false;
	}
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|p|%s|",FUNCTION_LOGIN, m_ggi, name, password);
	
	char subMsg[128] = {0};
	if ( type > 0 )
	{
		XP_API_SPRINTF(subMsg, "o|%d|", type); 	
		XP_API_STRCAT(buffer, subMsg);		
	}
	
	if(deviceToken != NULL)
	{
		XP_API_MEMSET(subMsg, 0, 128);
		XP_API_SPRINTF(subMsg, "d|%s|", deviceToken); 	
		XP_API_STRCAT(buffer, subMsg);
	}
	
	XP_API_MEMSET(subMsg, 0, 128);
	XP_API_SPRINTF(subMsg, "un|%d|", isReturnUserNumber); 	
	XP_API_STRCAT(buffer, subMsg);
	
	XP_API_MEMSET(subMsg, 0, 128);
	XP_API_SPRINTF(subMsg, "n|%d|", isReturnName); 	
	XP_API_STRCAT(buffer, subMsg);
		
	if(language != NULL)
	{
		XP_API_MEMSET(subMsg, 0, 128);
		XP_API_SPRINTF(subMsg, "l|%s|", language); 	
		XP_API_STRCAT(buffer, subMsg);
	}
	
	if(bSendLastTimeStamp && m_keepAliveReturns > 0)
	{
		XP_API_MEMSET(subMsg, 0, 128);
		XP_API_SPRINTF(subMsg, "t|%d|", m_keepAliveReturns); 	
		XP_API_STRCAT(buffer, subMsg);
	}
	
	XP_DEBUG_OUT("[Login] %s\n", buffer);
	return SendByGet( buffer );
}

/**
* Function SendLogout.
* Logout web server.	
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerLogin::SendLogout()
{
	char buffer[ONLINE_MAX_PACKET_SIZE ];

	m_isLoggedIn = false;

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_LOGOUT, m_ggi, m_uid);
	XP_DEBUG_OUT("[Logout] before String2Blob -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Protected function SendKeepAlive.
* Keep connection to web server alive.	
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerLogin::SendKeepAlive()
{
	char buffer[ONLINE_MAX_PACKET_SIZE];

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE  );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_KEEP_ALIVE, m_ggi, m_uid);

	m_lastSendKeepAliveTime = XP_API_GET_TIME();
	XP_DEBUG_OUT("SendKeepAlive -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Protected function ShouldSendKeepAlive.
* Calculate should call send keep alive or not	
* @return True / False
*/
bool GLXPlayerLogin::ShouldSendKeepAlive()
{
	if( !m_isLoggedIn )
	{
		return false;
	}
		
	long current_time = XP_API_GET_TIME();

	if ( ( current_time - m_lastSendKeepAliveTime ) > KEEP_ALIVE_WITH_WEBSERVER_INTERVAL )
	{
		return true;
	}

	return false;
}

/**
* Function SendGetAPNSInfo.
* Send request to get apple push notification server informations.
* @return Success: |r|s|server ip|server port|publicCertPemPath|privateKeyPemPath|privateKeyPassword 
* @return Fail:  |r|e|error code  ERROR_NO_RESPONSE   = 27;	ERROR_INVALID_GGI    = 28
*/
bool GLXPlayerLogin::SendGetAPNSInfo(int tag)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE  );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%d|", FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER, m_ggi, m_uid, tag);

	XP_DEBUG_OUT("GLXPlayerLogin::SendGetAPNSInfo -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}



/**
* Function SendGetProductInfo.
* Get products' information by GGIs
* @return Success:f|95|r|s|product information list
* @return Product information include product's GGI,product id,product name
* @return Fail:  f|95|r|e|error code
*/
bool GLXPlayerLogin::SendGetProductInfo(char * ggiList)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE  );
	if(ggiList != NULL || XP_API_STRLEN(ggiList) <= 0)
	{
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|l|%s|", FUNCTION_GET_PRODUCT, m_ggi, m_uid, ggiList);
	}	
	else
	{
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_PRODUCT, m_ggi, m_uid);
	}
	XP_DEBUG_OUT("GLXPlayerLogin::SendGetProductInfo -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}


/**
* Function SendGetGeneralConditions.
* Get general conditions from server 
* @param lang Language e.g. EN, DE, FR ....(iso 639-1 ), default is EN
* @return Success: |r|s
* @return Fail:  |r|e|error code
*/
bool GLXPlayerLogin::SendGetGeneralConditions(char * lang)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE  );
	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|l|%s|", FUNCTION_GET_GENERAL_CONDITIONS, m_ggi, m_uid, lang);
	
	XP_DEBUG_OUT("GLXPlayerLogin::SendGetGeneralConditions -> buffer = %s\n", buffer);
	return SendByPost( buffer );
}


/**
* Private function processApplePushServer.
* Process product information from function SendGetAPNSInfo().
* @see SendGetAPNSInfo()	
* @param msgString Data returned from the server.
*/
void GLXPlayerLogin::processApplePushServer(char* msgString)
{
	if(msgString == NULL || XP_API_STRLEN(msgString) <= 0)
	{
		return;
	}
	
	XP_API_DEL(m_APNS_IP);	
	XP_API_DEL(m_APNS_FeedBack_IP);
	XP_API_DEL(m_publicPemPath);	
	XP_API_DEL(m_privateKeyPemPath);	
	XP_API_DEL(m_privateKeyPassword);	
	
	m_APNS_IP = XP_API_NEW char[32];	
	m_APNS_FeedBack_IP = XP_API_NEW char[32];	
	m_publicPemPath = XP_API_NEW char[256];	
	m_privateKeyPemPath = XP_API_NEW char[256];
	m_privateKeyPassword = XP_API_NEW char[256];
	
	XP_API_MEMSET(m_APNS_IP, 0, 32);
	XP_API_MEMSET(m_publicPemPath, 0, 256);
	XP_API_MEMSET(m_privateKeyPemPath, 0, 256);
	XP_API_MEMSET(m_privateKeyPassword, 0, 256);
	
	int index = 0;
	char tmp[16] = {0};
	XP_API_MEMSET(tmp, 0, 16);
	
	getValue(msgString, m_APNS_IP, index++, '|');
	getValue(msgString, tmp, index++, '|');
	m_APNS_Port = XP_API_ATOI(tmp);
	
	getValue(msgString, m_APNS_FeedBack_IP, index++, '|');
	XP_API_MEMSET(tmp, 0, 16);		
	getValue(msgString, tmp, index++, '|');
	m_APNS_FeedBack_Port = XP_API_ATOI(tmp);
	
	getValue(msgString, m_publicPemPath, index++, '|');
	getValue(msgString, m_privateKeyPemPath, index++, '|');
	getValue(msgString, m_privateKeyPassword, index++, '|');
}

/**
* Private function processGetProductInfo.
* Process product information from function SendGetProductInfo().
* @see SendGetProductInfo()	
* @param msgString Data returned from the server.
*/
void GLXPlayerLogin::processGetProductInfo(char* msgString)
{	
	if(msgString == NULL || XP_API_STRLEN(msgString) <= 0)
	{
		return;
	}	
	clearProductInfo();

	m_productListCount++; //the first product has no '|' at the beginning.
	for(int i = 0; i < XP_API_STRLEN(msgString); i++)
	{
		if(msgString[i] == '|')
		{
			m_productListCount++;
		}
	}
	//|1st GGI^1st product^1st product name|2nd GGI^2nd prduct^2nd product name Split with “^?and “|?	m_productListGGI = XP_API_NEW int[m_productListCount];
	m_productListID = XP_API_NEW int[m_productListCount];
	m_productListName = XP_API_NEW char*[m_productListCount];
	
	char tmpStr[512];	
	char tmp[16];
	int index = 0;
	
	for(int i = 0; i < m_productListCount; i++)
	{
		XP_API_MEMSET(tmpStr, 0, 512);
		getValue(msgString, tmpStr, i, '|');
		
		XP_API_MEMSET(tmp, 0, 16);
		getValue(tmpStr, tmp, index++, '^');
		m_productListGGI[i] = XP_API_ATOI(tmp);
		
		XP_API_MEMSET(tmp, 0, 16);
		getValue(tmpStr, tmp, index++, '^');
		m_productListID[i] = XP_API_ATOI(tmp);
		
		m_productListName[i] = XP_API_NEW char[256];
		XP_API_MEMSET(m_productListName[i], 0, 256);
		getValue(tmpStr, m_productListName[i], index++, '^');
		index = 0;
	}
	
}

/**
* Function getAPNSIP.
* @see processApplePushServer()	
* @return m_APNS_IP.
*/
char* GLXPlayerLogin::getAPNSIP()
{	
	return m_APNS_IP;
}

/**
* Function getAPNSPort.
* @see processApplePushServer()	
* @return m_APNS_Port.
*/
int GLXPlayerLogin::getAPNSPort()
{	
	return m_APNS_Port;
}

/**
* Function getAPNSFeedBackIP.
* @see processApplePushServer()	
* @return m_APNS_FeedBack_IP.
*/
char* GLXPlayerLogin::getAPNSFeedBackIP()
{	
	return m_APNS_FeedBack_IP;
}

/**
* Function getAPNSFeedBackPort.
* @see processApplePushServer()	
* @return m_APNS_FeedBack_Port.
*/
int GLXPlayerLogin::getAPNSFeedBackPort()
{	
	return m_APNS_FeedBack_Port;
}

/**
* Function getAPNSPublicPemPath.
* @see processApplePushServer()	
* @return m_publicPemPath.
*/
char* GLXPlayerLogin::getAPNSPublicPemPath()
{	
	return m_publicPemPath;
}

/**
* Function getAPNSPrivateKeyPemPath.
* @see processApplePushServer()	
* @return m_privateKeyPemPath.
*/
char* GLXPlayerLogin::getAPNSPrivateKeyPemPath()
{	
	return m_privateKeyPemPath;
}

/**
* Function getAPNSPrivateKeyPemPath.
* @see processApplePushServer()	
* @return m_privateKeyPassword.
*/
char* GLXPlayerLogin::getAPNSPrivateKeyPassword()
{	
	return m_privateKeyPassword;
}


/**
* Function getProductListCount.
* @see processGetProductInfo()	
* @return m_productListCount.
*/
int GLXPlayerLogin::getProductListCount()
{
	return m_productListCount;
}

/**
* Function getProductListCount.
* @see processGetProductInfo()	
* @param index index of product.	
* @return m_productListGGI[index].
*/
int GLXPlayerLogin::getProductGGI(int index)
{
	if(m_productListGGI != NULL && index >= 0)
	{
		return m_productListGGI[index];
	}
	else
	{
		return -1;
	}
}

/**
* Function getProductID.
* @see processGetProductInfo()	
* @param index index of product.	
* @return m_productListID[index].
*/
int GLXPlayerLogin::getProductID(int index)
{
	if(m_productListID != NULL && index >= 0)
	{
		return m_productListID[index];
	}
	else
	{
		return -1;
	}
}

/**
* Function getProductName.
* @see processGetProductInfo()	
* @param index index of product.	
* @return m_productListName[index].
*/
char* GLXPlayerLogin::getProductName(int index)
{
	if(m_productListName != NULL && index >= 0)
	{
		return m_productListName[index];
	}
	else
	{
		return NULL;
	}
}

//>>lqguo: game invitation and get lobby server ip and port
bool GLXPlayerLogin::SendGetLobbyServer()
{
	char buffer[ONLINE_MAX_PACKET_SIZE] = { 0 };

	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_LOBBY_SERVER, m_ggi, m_uid);

	m_lastSendKeepAliveTime = XP_API_GET_TIME();
	XP_DEBUG_OUT("SendGetLobbyServer -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}

bool GLXPlayerLogin::SendGameInvitationGetLobbyServer(char* name)
{
	char buffer[ONLINE_MAX_PACKET_SIZE] = { 0 };

	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|cn|%s", FUNCTION_GAME_INVITATION_GET_LOBBY, m_ggi, m_uid, name);

	m_lastSendKeepAliveTime = XP_API_GET_TIME();
	XP_DEBUG_OUT("SendGameInvitationGetLobbyServer -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}
//<<end