#include "../../../include/Common/GLXPlayerHttp.h"

#ifdef ENABLE_ONLINE

#include "../../../include/Porting.h"
//#include "Memory.h"



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
	m_post_send_buffer_len = 0;
	m_sResponse = NULL;
	m_sResponseLen = 0;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	m_requestQueue.clear();
	m_numOfUnResolvedRequest = 0;
#endif
}

/**
* GLXPlayerHttp destructor. 
*/
GLXPlayerHttp::~GLXPlayerHttp()
{
	Cancel();
	XP_API_DEL( m_post_send_buffer );
	XP_API_DEL( m_sResponse );
	delete( m_socket );
}

#ifdef ENABLE_HTTP_REQUEST_QUEUE
bool GLXPlayerHttp::GetRequest()
{
	if(!m_requestQueue.empty())
	{
		std::deque<RequestQueue>::iterator it_queue = m_requestQueue.begin();

		m_send_by_post = it_queue->sendbyPost;
		if(m_send_by_post)
		{
			m_post_send_buffer_len = it_queue->request.length();
			XP_API_DELETE_ARRAY(m_post_send_buffer);
			m_post_send_buffer = XP_API_NEW char[m_post_send_buffer_len];
			XP_API_MEMSET(m_post_send_buffer, 0, m_post_send_buffer_len);
			XP_API_MEMCPY(m_post_send_buffer, it_queue->request.c_str(), m_post_send_buffer_len);
		}
		else
		{
			int len = it_queue->request.length();
			XP_API_MEMSET(m_abBufferIn, 0, SEND_BUFFER_SIZE);
			XP_API_MEMCPY(m_abBufferIn, it_queue->request.c_str(), len);
		}

		m_requestQueue.pop_front();

		return true;
	}
	else
	{
		m_send_by_post = false;
		XP_API_MEMSET(m_abBufferIn, 0, SEND_BUFFER_SIZE);
		XP_API_DEL( m_post_send_buffer );
		m_post_send_buffer_len = 0;

		return false;
	}
}
#endif

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
		//return XP_API_STRLEN( m_post_send_buffer );
		return m_post_send_buffer_len;
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
	XP_API_DEL( m_sResponse );
	m_sResponse = XP_API_NEW char[size+1];
	XP_API_MEMSET( m_sResponse, 0, size+1 );
	XP_API_MEMCPY( m_sResponse, data, size );
	m_sResponseLen = size;
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

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	if(m_numOfUnResolvedRequest == 0)
	{
		Start();
	}

	RequestQueue request;
	request.sendbyPost = false;
	request.request = m_abBufferIn;
	m_requestQueue.push_back(request);
	XP_API_MEMSET(m_abBufferIn, 0, SEND_BUFFER_SIZE);
	++m_numOfUnResolvedRequest;
#else
	Start();
#endif

	XP_API_DEL(m_sResponse);
	m_sResponseLen = 0;
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

	XP_API_DEL( m_sResponse );

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

	XP_API_DEL( server_address );
	server_address = NULL;

	m_send_by_post = false;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	if(m_numOfUnResolvedRequest == 0)
	{
		Start();
	}

	RequestQueue request;
	request.sendbyPost = false;
	request.request = m_abBufferIn;
	m_requestQueue.push_back(request);
	XP_API_MEMSET(m_abBufferIn, 0, SEND_BUFFER_SIZE);
	++m_numOfUnResolvedRequest;
#else
	Start();
#endif

	XP_API_DEL(m_sResponse);
	m_sResponseLen = 0;
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

	XP_API_DEL( m_sResponse );

	XP_API_STRCAT(sQuery, "&v=");
	XP_API_STRCAT(sQuery, m_php_script_version);	

	XP_API_SPRINTF(m_abBufferIn, "POST %s HTTP/1.1\r\n"
		"Host: %s\r\n"		 
		"Content-type: application/x-www-form-urlencoded\r\n"
		"Content-length: %d\r\n\r\n", sUrl, server_address, XP_API_STRLEN(sQuery) );

	XP_API_DEL( m_post_send_buffer );
	int length = XP_API_STRLEN(m_abBufferIn) + XP_API_STRLEN(sQuery);
	m_post_send_buffer = XP_API_NEW char[ length + 1 ];
	XP_API_MEMSET( m_post_send_buffer, 0, length + 1 );
	XP_API_SPRINTF( m_post_send_buffer, "%s%s", m_abBufferIn, sQuery );
	m_post_send_buffer_len = length;


	XP_API_DEL( server_address );
	server_address = NULL;

	m_send_by_post = true;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	if(m_numOfUnResolvedRequest == 0)
	{
		Start();
	}

	RequestQueue request;
	request.sendbyPost = true;
	request.request.append(m_post_send_buffer, m_post_send_buffer_len);
	m_requestQueue.push_back(request);
	XP_API_DELETE_ARRAY(m_post_send_buffer);
	m_post_send_buffer_len = 0;
	++m_numOfUnResolvedRequest;
#else
	Start();
#endif

	XP_API_DEL(m_sResponse);
	m_sResponseLen = 0;
}

/**
* Function sendVideoByPost.
* Starts a HTTP request by POST method.	
* @param pData: video data.
* @param size: video's length.
* @param videoID: the video id
* @return success: f|106|r|s|
* @return fail: f|106|e|error code|
*/
void GLXPlayerHttp::sendVideoByPost(char *sUrl, char *sQuery, char *pData, int &size, char* videoID)
{
	char *postHeader =	"POST %s?%s HTTP/1.1\r\n" \
		"Host: %s\r\n" \
		"Accept: */*\r\n" \
		"Content-Type: multipart/form-data; boundary=----GL_HTTP_POST_AVI\r\n" \
		"Content-length: %d\r\n" \
		"Proxy-Connection: Keep-Alive\r\n" \
		"Pragma: no-cache\r\n\r\n";	


	char *postBody =	"------GL_HTTP_POST_AVI\r\n" \
		"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n" \
		"Content-Type: video/avi\r\n\r\n" ;


	char *postTail = "\r\n------GL_HTTP_POST_AVI--\r\n" ;

	//--------------------------------------------------------------
	char tempUrl[256] = { 0 };
	char host[256] = { 0 };

	XP_API_MEMSET( tempUrl, 0, 256 );
	XP_API_MEMSET( host, 0, 256 );

	int pos = XP_API_PARSE_DATA( sUrl, tempUrl, 2, '/' );
	XP_API_MEMSET( tempUrl, 0, 256 );
	XP_API_MEMCPY( tempUrl, sUrl+pos, XP_API_STRLEN(sUrl)-pos );

	pos = XP_API_PARSE_DATA( tempUrl, host, 0, '/' );

	int len_total = XP_API_STRLEN( tempUrl );
	int len_addrs = XP_API_STRLEN( host );

	char * server_address = XP_API_NEW char[len_addrs+1];
	XP_API_MEMSET( server_address, 0, len_addrs+1 );
	XP_API_MEMCPY( server_address, host, len_addrs );
	//--------------------------------------------------------------------

	XP_API_DELETE_ARRAY( m_post_send_buffer );
	m_post_send_buffer = XP_API_NEW char[size + 1024];
	XP_API_MEMSET(m_post_send_buffer, 0, size + 1024) ;

	char *temp = XP_API_NEW char[size + 1024] ;
	XP_API_MEMSET(temp, 0, size + 1024) ;

	XP_API_STRCAT(sQuery, "&v=");
	XP_API_STRCAT(sQuery, m_php_script_version);

	//int tempSize = sprintf_s(temp, size+1024, postBody, videoID) ;
	int tempSize = sprintf(temp, postBody, videoID) ;
	XP_API_MEMCPY(temp + tempSize, pData, size) ;

	XP_API_MEMCPY(temp + tempSize + size, postTail, strlen(postTail)) ;
	tempSize += size + (int)XP_API_STRLEN(postTail) ;

	//size = sprintf_s(m_post_send_buffer, size+1024, postHeader, sUrl, sQuery, server_address, tempSize) ;
	size = sprintf(m_post_send_buffer, postHeader, sUrl, sQuery, server_address, tempSize) ;

	XP_API_MEMCPY(m_post_send_buffer + size, temp, tempSize) ;
	size += tempSize ;
	m_post_send_buffer_len = size;

	XP_API_DELETE_ARRAY(temp);
	XP_API_DEL( server_address );

	m_send_by_post = true;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	if(m_numOfUnResolvedRequest == 0)
	{
		Start();
	}

	RequestQueue request;
	request.sendbyPost = true;
	request.request.append(m_post_send_buffer, m_post_send_buffer_len);
	m_requestQueue.push_back(request);
	XP_API_DELETE_ARRAY(m_post_send_buffer);
	m_post_send_buffer_len = 0;
	++m_numOfUnResolvedRequest;
#else
	Start();
#endif


	XP_API_DEL(m_sResponse);
	m_sResponseLen = 0;
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
* Function GetResponseLen.
* Get the length of response buffer.
* @return length of Response buffer.
*/
int GLXPlayerHttp::GetResponseLen()
{
	return m_sResponseLen;
}

/**
* Function SetResponseLen.
* Set the length of response buffer.
*/
void GLXPlayerHttp::SetResponseLen(int len)
{
	m_sResponseLen = len;
}

/**
* Function Cancel.
* Cancel current request.
*/
void GLXPlayerHttp::Cancel()
{
	m_socket->Cancel();
	XP_API_DEL( m_post_send_buffer );
	XP_API_DEL( m_sResponse );
	m_sResponseLen = 0;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	m_requestQueue.clear();
	m_numOfUnResolvedRequest = 0;
#endif
}

#endif // ENABLE_ONLINE
