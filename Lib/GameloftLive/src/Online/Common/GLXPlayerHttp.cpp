#include "GLXPlayerHttp.h"

#ifdef ENABLE_ONLINE

#include "Porting.h"
#include "Memory.h"

/**
 * GLXPlayerHttp constructor.
 * @param server_address Server address, for example: "beta.gameloft.com".
 * @param php_version Used for identifing different functions.
 * @param php_address The web request address, for example: "ope/GenericXPlayer_v1.php".
 *  
 */
GLXPlayerHttp::GLXPlayerHttp(char * server_address, char * m_php_script_version, char * m_php_address) :
	m_server_address(server_address),
	m_php_script_version(m_php_script_version),
	m_php_address(m_php_address)
{
	m_socket = GLXPlayerSocketFactory::GetSocket( server_address , 80, this );
	m_post_send_buffer = NULL;
	m_sResponse = NULL;
}

/**
* GLXPlayerHttp destructor. 
*/
GLXPlayerHttp::~GLXPlayerHttp()
{
	Cancel();
	XP_API_DELETE_ARRAY( m_post_send_buffer );
	XP_API_DELETE_ARRAY( m_sResponse );
	delete( m_socket );
}

/**
* Function GetRequestData.
* Get the request string.	
* @return Request string.
*/
char * GLXPlayerHttp::GetRequestData()
{
	if ( m_send_by_post )
	{
		return m_post_send_buffer;
	}
	else
	{
		return m_abBufferIn;
	}
}

/**
* Function GetRequestLength.
* Get the length of request string.	
* @return Request string length.
*/
int GLXPlayerHttp::GetRequestLength()
{
	if ( m_send_by_post )
	{
		return XP_API_STRLEN( m_post_send_buffer );
	}
	else
	{
		return XP_API_STRLEN( m_abBufferIn );		
	}
}

/**
* Function SetResponseData.
* Set the response data as the data returned from the server.
* @param data Response data from server.	
* @param size Size of response data.		
*/
void GLXPlayerHttp::SetResponseData( char * data, int size )
{
	XP_API_DELETE_ARRAY( m_sResponse );
	m_sResponse = XP_API_NEW char[size+1];
	XP_API_MEMSET( m_sResponse, 0, size+1 );
	XP_API_MEMCPY( m_sResponse, data, size );
}


/**
* Function sendByGet.
* Starts a HTTP request by GET method but has no ver.	
* @param sUrl URL of the server
* @param sQuery The query to be sent to the server
*/
void GLXPlayerHttp::sendByGetWithNoVer( char *sUrl, char *sQuery )
{
	XP_DEBUG_OUT( "GLXPlayerHttp::sendByGet()\n" );

	if( NULL == sUrl || NULL == sQuery )
	{
		XP_DEBUG_OUT( "GLXPlayerHttp::sendByGet() invalid parameters\n" );
		return;
	}

	char temp[256] = { 0 };
	char host[256] = { 0 };

	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMSET( host, 0, 256 );

	int pos = XP_API_PARSE_DATA( sUrl, temp, 2, '/' );
	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMCPY( temp, sUrl+pos, XP_API_STRLEN(sUrl)-pos );

	pos = XP_API_PARSE_DATA( temp, host, 0, '/' );

	int len_total = XP_API_STRLEN( temp );
	int len_addrs = XP_API_STRLEN( host );

	char * server_address = XP_API_NEW char[len_addrs+1];
	XP_API_MEMSET( server_address, 0, len_addrs+1 );
	XP_API_MEMCPY( server_address, host, len_addrs );

	XP_API_MEMSET( m_abBufferIn, 0, SEND_BUFFER_SIZE );

	XP_API_DEL( m_sResponse );

	XP_API_STRCPY( m_abBufferIn, "GET " );
	XP_API_STRCAT( m_abBufferIn, sUrl );
	XP_API_STRCAT( m_abBufferIn, "?");
	XP_API_STRCAT( m_abBufferIn, sQuery );

	XP_API_STRCAT( m_abBufferIn, " HTTP/1.1\r\n" );
	XP_API_STRCAT( m_abBufferIn, "Host: " );
	XP_API_STRCAT( m_abBufferIn, server_address );
	XP_API_STRCAT( m_abBufferIn, "\r\n\r\n" );

	XP_API_DEL( server_address );
	server_address = NULL;

	m_send_by_post = false;

	m_socket->Start();

	m_sResponse = NULL;
}


/**
* Function sendByGet.
* Starts a HTTP request by GET method.	
* @param sUrl URL of the server.
* @param sQuery The query to be sent to the server.
*/
void GLXPlayerHttp::sendByGet( char *sUrl, char *sQuery )
{
	XP_DEBUG_OUT( "GLXPlayerHttp::sendByGet()\n" );

	if( NULL == sUrl || NULL == sQuery )
	{
		XP_DEBUG_OUT( "GLXPlayerHttp::sendByGet() invalid parameters\n" );
		return;
	}

	char temp[256] = { 0 };
	char host[256] = { 0 };

	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMSET( host, 0, 256 );
	
	int pos = XP_API_PARSE_DATA( sUrl, temp, 2, '/' );
	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMCPY( temp, sUrl+pos, XP_API_STRLEN(sUrl)-pos );

	pos = XP_API_PARSE_DATA( temp, host, 0, '/' );
	
	int len_total = XP_API_STRLEN( temp );
	int len_addrs = XP_API_STRLEN( host );
	
	char * server_address = XP_API_NEW char[len_addrs+1];
	XP_API_MEMSET( server_address, 0, len_addrs+1 );
	XP_API_MEMCPY( server_address, host, len_addrs );

	XP_API_MEMSET( m_abBufferIn, 0, SEND_BUFFER_SIZE );

	XP_API_DELETE_ARRAY( m_sResponse );

	XP_API_STRCPY( m_abBufferIn, "GET " );
	XP_API_STRCAT( m_abBufferIn, sUrl );
	XP_API_STRCAT( m_abBufferIn, "?");
	XP_API_STRCAT( m_abBufferIn, sQuery );
	XP_API_STRCAT( m_abBufferIn, "&v=" );
	XP_API_STRCAT( m_abBufferIn, m_php_script_version );
	XP_API_STRCAT( m_abBufferIn, " HTTP/1.1\r\n" );
	XP_API_STRCAT( m_abBufferIn, "Host: " );
	XP_API_STRCAT( m_abBufferIn, server_address );
	XP_API_STRCAT( m_abBufferIn, "\r\n\r\n" );

	XP_API_DELETE_ARRAY( server_address );
	server_address = NULL;

	m_send_by_post = false;

	m_socket->Start();

	m_sResponse = NULL;
}

/**
* Function sendByPost.
* Starts a HTTP request by POST method.	
* @param sUrl URL of the server.
* @param sQuery The query to be sent to the server.
*/
void GLXPlayerHttp::sendByPost( char *sUrl, char *sQuery )
{
	XP_DEBUG_OUT( "GLXPlayerHttp::sendByPost()\n" );

	if( NULL == sUrl || NULL == sQuery )
	{
		XP_DEBUG_OUT( "GLXPlayerHttp::sendByPost() invalid parameters\n" );
		return;
	}

	char temp[256] = { 0 };
	char host[256] = { 0 };

	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMSET( host, 0, 256 );
	
	int pos = XP_API_PARSE_DATA( sUrl, temp, 2, '/' );
	XP_API_MEMSET( temp, 0, 256 );
	XP_API_MEMCPY( temp, sUrl+pos, XP_API_STRLEN(sUrl)-pos );

	pos = XP_API_PARSE_DATA( temp, host, 0, '/' );
	
	int len_total = XP_API_STRLEN( temp );
	int len_addrs = XP_API_STRLEN( host );
	
	char * server_address = XP_API_NEW char[len_addrs+1];
	XP_API_MEMSET( server_address, 0, len_addrs+1 );
	XP_API_MEMCPY( server_address, host, len_addrs );

	XP_API_MEMSET( m_abBufferIn, 0, SEND_BUFFER_SIZE );

	XP_API_DELETE_ARRAY( m_sResponse );
	
	XP_API_STRCAT(sQuery, "&v=");
	XP_API_STRCAT(sQuery, m_php_script_version);	
	
	XP_API_SPRINTF(m_abBufferIn, "POST %s HTTP/1.1\r\n"
		 "Host: %s\r\n"		 
		 "Content-type: application/x-www-form-urlencoded\r\n"
		 "Content-length: %d\r\n\r\n", sUrl, server_address, XP_API_STRLEN(sQuery) );

	XP_API_DELETE_ARRAY( m_post_send_buffer );
	m_post_send_buffer = XP_API_NEW char[ XP_API_STRLEN(m_abBufferIn) + XP_API_STRLEN(sQuery) + 1 ];
	XP_API_MEMSET( m_post_send_buffer, 0, XP_API_STRLEN(m_abBufferIn) + XP_API_STRLEN(sQuery) + 1 );
	XP_API_SPRINTF( m_post_send_buffer, "%s%s", m_abBufferIn, sQuery );


	XP_API_DELETE_ARRAY( server_address );
	server_address = NULL;

	m_send_by_post = true;

	m_socket->Start();

	m_sResponse = NULL;
}

/**
* Function UpdateRequest.
* Update function to send the request to the server.
*/
void GLXPlayerHttp::UpdateRequest()
{
	m_socket->Run();
}

/**
* Function IsInProgress.
* Check if the connection is in process or not.
* @return True / False
*/
bool GLXPlayerHttp::IsInProgress()
{
	return m_socket->IsInProgress();
}

/**
* Function IsErrorOccurred.
* Check if error occur or not.
* @return True / False
*/
bool GLXPlayerHttp::IsErrorOccurred()
{
	return m_socket->IsErrorOccurred();
}

/**
* Function GetResponse.
* Get the response from HTTP server.
* @return Response string.
*/
char* GLXPlayerHttp::GetResponse()
{
	return m_sResponse;
}

/**
* Function Cancel.
* Cancel current request.
*/
void GLXPlayerHttp::Cancel()
{
	m_socket->Cancel();
}

#endif // ENABLE_ONLINE