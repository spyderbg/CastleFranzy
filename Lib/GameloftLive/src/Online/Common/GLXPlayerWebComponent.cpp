#include "GLXPlayerWebComponent.h"
#include "GLXPlayerSereverConfig.h"
#include "memory.h"
#include "GLLiveDEF.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GLLGetConfigFilePath(char* filePath, const char* filename);
#endif

/**
* GLXPlayerWebComponent constructor. 
*/
GLXPlayerWebComponent::GLXPlayerWebComponent()
{
	m_http_server_address = NULL;
	m_http_php_address = NULL;
	m_http_php_version = NULL;
	m_http_url = NULL;
	m_pResponsePtr = NULL;
	m_enabled = false;

	LoadConfig();

	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, 
		m_http_php_version, m_http_php_address );
	
#ifdef ENABLE_TIMEOUT	
	m_callStartTime = 0;
	m_callFunctionID = -1;
#endif //ENABLE_TIMEOUT
}

/**
* GLXPlayerWebComponent destructor. 
*/
GLXPlayerWebComponent::~GLXPlayerWebComponent(void)
{	
	m_pResponsePtr = NULL;
	XP_API_DELETE_ARRAY( m_http_server_address );
	XP_API_DELETE_ARRAY( m_http_php_address );
	XP_API_DELETE_ARRAY( m_http_php_version );
	XP_API_DELETE_ARRAY( m_http_url );
	XP_API_DEL( m_http_engine );
	
#ifdef ENABLE_TIMEOUT	
	m_callStartTime = 0;
	m_callFunctionID = -1;
#endif //ENABLE_TIMEOUT
}

/**
 * Function Update.
 * The default process to handle data.
 * The subclass can replace it with its own implementation.
 *
 */
void GLXPlayerWebComponent::Update()
{
	if ( !m_enabled )
	{
		return;
	}

	m_http_engine->UpdateRequest();

	if ( m_http_engine->IsInProgress() )
	{
	
	#ifdef ENABLE_TIMEOUT	
		if (XP_API_GET_TIME() - m_callStartTime >  CONN_TIMEOUT)
		{
			///@ Clean up connections .	
			int callFunctionID = m_callFunctionID;
			Cancel();
			m_observer->OnRequestTimeout(callFunctionID);
		}		
	#endif //ENABLE_TIMEOUT
	
		return;
	}

	m_enabled = false;
	XP_DEBUG_OUT("GLXPlayerWebComponent::Update(): m_enable is changed to false!!\n");

	if ( m_http_engine->IsErrorOccurred() )
	{
		m_observer->OnNetworkError();
		return;
	}

	OnUpdateParse();
}

/**
 * Function Cancel.
 * Cancel current transaction.
 *
 */
void GLXPlayerWebComponent::Cancel()
{
	XP_DEBUG_OUT("GLXPlayerWebComponent::Update(): Cancel!!\n");
	m_enabled = false;
	m_http_engine->Cancel();
	
#ifdef ENABLE_TIMEOUT	
	m_callStartTime = 0;
	m_callFunctionID = -1;
#endif //ENABLE_TIMEOUT
}

/**
 * Function OnUpdateParse.
 * The default process to handle response.
 * The subclass can replace it with its own implementation.
 *
 */
bool GLXPlayerWebComponent::OnUpdateParse()
{
	StartResponseParser( m_http_engine->GetResponse() );

	char token[ONLINE_MAX_PACKET_SIZE];
	char func_type = 0;

	XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
	GetNextResponseToken( token );

	if ( XP_API_STRCMP( token, "f" ) == 0 )
	{
		func_type = 'f';
	}
	else if ( XP_API_STRCMP( token, "g" ) == 0 )
	{
		func_type = 'g';
	}
	else
	{
		m_observer->OnNetworkError();
		return true;
	}	
	
	XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
	GetNextResponseToken( token );
	int func_id = XP_API_ATOI( token );
	func_id = GetFunctionID( func_type, func_id );

	if( !IsNextResponseStringToken("r") )
	{
		m_observer->OnNetworkError();
		return true;
	}

	XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
	GetNextResponseToken( token );

	if( XP_API_STRCMP( token, "s" ) == 0 )
	{		
		return OnUpdateSuccess( func_id );
	}

	if( XP_API_STRCMP( token, "e" ) == 0 )
	{
		return OnUpdateFailure( func_id );
	}

	return true;
}

/**
 * Function OnUpdateSuccess.
 * The default process to handle successful response.
 * The subclass can replace it with its own implementation.
 * @param func_id Function ID.
 */
bool GLXPlayerWebComponent::OnUpdateSuccess( int func_id )
{
	m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
	return true;
}

/**
 * Function OnUpdateFailure.
 * The default process to handle failed response.
 * The subclass can replace it with its own implementation.
 * @param func_id Function ID.
 */
bool GLXPlayerWebComponent::OnUpdateFailure( int func_id )
{
	int err_code = GetNextResponseIntToken();
	m_observer->OnRequestFailure( func_id, err_code );
	return true;
}

/**
 * Function SendByGet.
 * Send request to HTTP server by GET method.
 * @param url URL of the server and the request string (  after ?b=xxxx&v=4) 
 */
bool GLXPlayerWebComponent::SendByGet( char * url )
{
	if ( url && strlen(url) > 0 && url[strlen(url) - 1] != '|' )
	{
		XP_API_STRCAT(url, "|");
	}
	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	char * blob = NULL;	
	m_enabled = true;

	blob = SSEncDec_String2Blob( url );
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "b=%s", blob );
	XP_API_DELETE_ARRAY( blob );
	
#ifdef ENABLE_TIMEOUT	
	char tmp[8] = {0};
	getValue(url, tmp, 1, '|');
	m_callFunctionID = XP_API_ATOI(tmp);
	m_callStartTime = XP_API_GET_TIME();
#endif //ENABLE_TIMEOUT

	XP_DEBUG_OUT("SendByGet %s\n", buffer );
	m_http_engine->sendByGet( m_http_url, buffer );
	return true;	
}

/**
 * Function SendByPost.
 * Send request to HTTP server by POST method.
 * @param url URL of the server and the request string (  after b=xxxx&v=4) 
 */
bool GLXPlayerWebComponent::SendByPost( char * url )
{
	char * blob = NULL;
	m_enabled = true;
	blob = SSEncDec_String2Blob( url );
	
	int len = XP_API_STRLEN(blob) + 32;
	char* buffer = XP_API_NEW char[len];
	XP_API_MEMSET( buffer, 0, len );
	XP_API_SPRINTF( buffer, "b=%s", blob );
	XP_API_DELETE_ARRAY( blob );
	
#ifdef ENABLE_TIMEOUT	
	char tmp[8] = {0};
	getValue(url, tmp, 1, '|');
	m_callFunctionID = XP_API_ATOI(tmp);
	m_callStartTime = XP_API_GET_TIME();
#endif //ENABLE_TIMEOUT

	XP_DEBUG_OUT("SendByPost buffer length %d\n", XP_API_STRLEN(buffer));
	m_http_engine->sendByPost( m_http_url, buffer );
	XP_API_DELETE_ARRAY(buffer);
	return true;
}

/**
 * Function StartResponseParser.
 * The default process to parse the response string from the server.
 * @param pResponse Response string.
 */
void GLXPlayerWebComponent::StartResponseParser( char * pResponse )
{
	m_pResponsePtr = pResponse;
}

/**
 * Function GetNextResponseToken.
 * Get the next token of the response string by XPLAYER_DATA_SEPERATOR, default is '|'.
 * @param pToken Return token.
 * @return token.
 */
char * GLXPlayerWebComponent::GetNextResponseToken( char * pToken )
{
	char * startPtr = m_pResponsePtr;
	int len = 0;

	while( *m_pResponsePtr )
	{
		if( *m_pResponsePtr == XPLAYER_DATA_SEPERATOR )
		{
			m_pResponsePtr++;
			break;
		}

		m_pResponsePtr++;
		len++;
	}

	if( len == 0 )
	{
		pToken[0] = 0;
	}
	else
	{
		XP_API_STRNCPY( pToken, startPtr, len );
		pToken[len] = 0;
	}

	return pToken;
}

/**
 * Function GetNextResponseIntToken.
 * Get the next integer value of the response string by XPLAYER_DATA_SEPERATOR, default is '|'.	 
 * @return An integer value.
 */
int GLXPlayerWebComponent::GetNextResponseIntToken()
{
	char pToken[256] = { 0 };

	XP_API_MEMSET( pToken, 0, 256 );
	GetNextResponseToken( pToken );

	return XP_API_ATOI( pToken );
}

/**
 * Function IsNextResponseStringToken.
 * Check if next response string token is same as pStr.
 * @param pStr Value to compare.
 * @return True / False.
 */
bool GLXPlayerWebComponent::IsNextResponseStringToken( const char * pStr )
{
	char pToken[256] = { 0 };

	XP_API_MEMSET( pToken, 0, 256 );
	GetNextResponseToken( pToken );

	return ( XP_API_STRCMP( pStr, pToken ) == 0 );
}

/**
 * Function IsNextResponseIntToken.
 * Check if next integer value of the response string is equal value.
 * @param value Value to compare.
 * @return True / False.
 */
bool GLXPlayerWebComponent::IsNextResponseIntToken( int value )
{
	char pToken[256] = { 0 };

	XP_API_MEMSET( pToken, 0, 256 );
	GetNextResponseToken( pToken );
	int tokenValue = XP_API_ATOI( pToken );
	return ( value == tokenValue );
}

/**
 * Function GetFunctionID.
 * Get the function Id returned from the server
 * @param type Function type ('f' / 'g').
 * @param id Function ID.
 * @return Function ID or -1.
 */
int GLXPlayerWebComponent::GetFunctionID( char type, int id )
{
	if ( type == 'f' )
	{
		return id;
	}
	else if ( type == 'g' )
	{
		return id + GUNCTION_BASE;
	}
	else
	{
		return -1;
	}
}

/**
 * Function LoadConfig.
 * Load all configuration.
 * @return True / False.
 */
bool GLXPlayerWebComponent::LoadConfig()
{
#ifdef GET_CONFIG_FROM_SERVER
	{
		int pos = 0;

		char temp[256] = { 0 };
		char host[256] = { 0 };

		m_http_url = XP_API_STRNEW( GLXPlayerSereverConfig::m_webConfig_XPlayerURL );

		XP_API_MEMSET( temp, 0, 256 );
		XP_API_MEMSET( host, 0, 256 );
		
		pos = XP_API_PARSE_DATA( m_http_url, temp, 2, '/' );
		XP_API_MEMSET( temp, 0, 256 );
		XP_API_MEMCPY( temp, m_http_url+pos, XP_API_STRLEN(m_http_url)-pos );

		pos = XP_API_PARSE_DATA( temp, host, 0, '/' );
		
		int len_total = XP_API_STRLEN( temp );
		int len_addrs = XP_API_STRLEN( host );
		
		m_http_server_address = XP_API_NEW char[len_addrs+1];
		XP_API_MEMSET( m_http_server_address, 0, len_addrs+1 );
		XP_API_MEMCPY( m_http_server_address, host, len_addrs );

		m_http_php_address = XP_API_NEW char[len_total-len_addrs+1];
		XP_API_MEMSET( m_http_php_address, 0, len_total-len_addrs+1 );
		XP_API_MEMCPY( m_http_php_address, temp+len_addrs, len_total-len_addrs );
	}
	{
		m_http_php_version = XP_API_STRNEW( GLXPlayerSereverConfig::m_webConfig_PhpVer );
	}

	{
		m_ggi=GLXPlayerSereverConfig::m_webConfig_ggi;
	}
#else
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
    char tmp[FILENAME_MAX];
	XP_API_MEMSET(tmp,0,FILENAME_MAX);
	GLLGetConfigFilePath(tmp, XPLAYER_FILE_CONF);
	pFile = XP_API_FILE_OPEN( tmp, "r" );
#else
	pFile = XP_API_FILE_OPEN( XPLAYER_FILE_CONF, "r" );
#endif

	if( !pFile )
	{
		XP_DEBUG_OUT("[GLXPlayerUser] load config - %s not found \n", XPLAYER_FILE_CONF );
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
      
		if ( XP_API_STRCMP( name, XPLAYER_CONF_URL ) == 0 )
		{
			// http://beta.gameloft.com/ope/GenericXPlayer_v1.php
			char temp[256] = { 0 };
			char host[256] = { 0 };

			m_http_url = XP_API_STRNEW( value );

			XP_API_MEMSET( temp, 0, 256 );
			XP_API_MEMSET( host, 0, 256 );
			
			pos = XP_API_PARSE_DATA( value, temp, 2, '/' );
			XP_API_MEMSET( temp, 0, 256 );
			XP_API_MEMCPY( temp, value+pos, XP_API_STRLEN(value)-pos );

			pos = XP_API_PARSE_DATA( temp, host, 0, '/' );
			
			int len_total = XP_API_STRLEN( temp );
			int len_addrs = XP_API_STRLEN( host );
			
			m_http_server_address = XP_API_NEW char[len_addrs+1];
			XP_API_MEMSET( m_http_server_address, 0, len_addrs+1 );
			XP_API_MEMCPY( m_http_server_address, host, len_addrs );

			m_http_php_address = XP_API_NEW char[len_total-len_addrs+1];
			XP_API_MEMSET( m_http_php_address, 0, len_total-len_addrs+1 );
			XP_API_MEMCPY( m_http_php_address, temp+len_addrs, len_total-len_addrs );
		}

		if ( XP_API_STRCMP( name, XPLAYER_CONF_PHPVER ) == 0 )
		{
			m_http_php_version = XP_API_STRNEW( value );
		}

		XP_API_MEMSET( data, 0, length+1 );
		XP_API_PARSE_DATA( buffer, data, line++, '\n' );
		if (data[XP_API_STRLEN(data)-1] == '\r')
		{
			data[XP_API_STRLEN(data)-1] = 0;
		}		
		len = XP_API_STRLEN( data );
	}

	XP_API_DELETE_ARRAY( data );
	XP_API_DELETE_ARRAY( buffer );

	XP_API_FILE_CLOSE( pFile );
#endif
	return true;	
}