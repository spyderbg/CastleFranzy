#include "../../../include/YouTube/GLXPlayerYoutube.h"
#include "../3rdParty/zlib/zlib.h"

/**
*  constructor.
* Create an instance of GLXPlayerYoutube.
*/
GLXPlayerYoutube::GLXPlayerYoutube( ): GLXPlayerWebComponent()
{	
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );
		
	m_videoID = NULL;
}

/**
* GLXPlayerYoutube destructor.
* Destroy an instance of GLXPlayerYoutube.
*/
GLXPlayerYoutube::~GLXPlayerYoutube(void)
{	
	XP_API_DEL(m_videoID);
}


/**
 * Function SendVideoByPost.
 * Send request to HTTP server by POST method.
 * @param pData: video data.
 * @param size: video's length.
 * @param videoID: video id
 * @return success:
 * @return fail:
**/
bool GLXPlayerYoutube::SendVideoByPost(char *url, char *pData, int &size, char* videoID)
{
	char * blob = NULL;
	m_enabled = true;

#ifdef ENABLE_UNIT_TEST
	XP_FILE_HANDLE pFile;
	#ifdef OS_IPHONE
		char buff[FILENAME_MAX];
		GetSaveFilePath(buff, UNIT_TEST_OUTPUT_NAME);
		pFile = XP_API_FILE_OPEN( buff, "at+" );	
	#else
		pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
	#endif
		if(pFile != NULL)
		{
			signed   int nWritten;
			int size = XP_API_STRLEN(url) + 16;
			char *tmp = XP_API_NEW char[size];
			XP_API_MEMSET(tmp, 0, size);
			XP_API_SPRINTF(tmp, "Request: %s\n", url);			
			nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);				

			XP_API_FILE_CLOSE(pFile);
			XP_API_DEL(tmp);
		}
#endif //ENABLE_UNIT_TEST

	blob = SSEncDec_String2Blob( url );
	
	int len = XP_API_STRLEN(blob) + 32;
	char* buffer = XP_API_NEW char[len];
	XP_API_MEMSET( buffer, 0, len );
	XP_API_SPRINTF( buffer, "b=%s", blob );
	XP_API_DEL( blob );

	XP_DEBUG_OUT("SendByPost buffer length %d\n", XP_API_STRLEN(buffer));
	m_http_engine->sendVideoByPost(m_http_url, buffer, pData, size, videoID);
	XP_API_DEL(buffer);
	return true;
}

/**
* Function sendCheckReplayVideoAvailable.
* Check for replay video available or not.
* @return success: |r|s|t|value (0/1)
* @return fail: |e|error code
*/
bool GLXPlayerYoutube::sendCheckReplayVideoAvailable()
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_CHECK_REPLAYVIDEO_AVAILABLE, m_ggi, m_uid);	
	XP_DEBUG_OUT("GLXPlayerUser::sendAddVideoInfo -> buffer = %s\n", buffer);	
	return SendByGet( buffer );
}

/**
* Function sendAddVideoInfo.
* Add video's informations which contains title comments and ggi.
* 
* @param title video's title.
* @param comment video's comments.
* @return success: |r|s|vi|video_ id
* @return fail: |e|error code; ERROR_YOUTUBE_ACCOUNT_NOT_EXIT = 72
*/
bool GLXPlayerYoutube::sendAddVideoInfo(char* account, char* password,char* title, char* comment, bool isPrivate)
{
	if(NULL == account || NULL == password || NULL == title || NULL == comment)
	{
		m_observer->OnRequestFailure(FUNCTION_ADD_VIDEO_INFO, ERROR_INIT );
		return false;
	}
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|gu|%s|gp|%s|l|%s|vc|%s|p|%d|", FUNCTION_ADD_VIDEO_INFO, m_ggi, m_uid, account, password, title, comment, isPrivate?1:0);	
	XP_DEBUG_OUT("GLXPlayerUser::sendAddVideoInfo -> buffer = %s\n", buffer);	
	return SendByGet( buffer );
}

/**
* Function sendAddVideoData.
* Add video data.
* 
* @param pData: video data.
* @param size: video's length.
* @param videoID: video's ID
* @return success:
* @return fail:
*/
bool GLXPlayerYoutube::sendAddVideoData(char *pData, int &size)
{
	if(NULL == pData || 0 == size ||  NULL == m_videoID)
	{
		m_observer->OnRequestFailure(FUNCTION_UPLOAD_VIDEO_DATA, ERROR_INIT );
		return false;
	}

    uLongf destLen = compressBound (size);
	char *tmpDest = XP_API_NEW char[destLen+4];

	if(tmpDest == NULL)
	{
		m_observer->OnRequestFailure(FUNCTION_UPLOAD_VIDEO_DATA, ERROR_COMPRESS_MEM );
		return false;
	}

	XP_API_MEMSET(tmpDest, 0, destLen+4);
	Bytef *dest = (Bytef *)tmpDest;
    const Bytef *source = (Bytef *)pData;
    uLong sourceLen = size;

	*(dest) = (BYTE)((size & 0xFF000000)>>24);
    *(dest+1) = (BYTE)((size & 0x00FF0000)>>16);
    *(dest+2) = (BYTE)((size & 0x0000FF00)>>8);
    *(dest+3) = (BYTE)((size & 0x000000FF));

	int comressRet = compress (dest+4, &destLen, source, sourceLen);
	destLen += 4;

	if(comressRet != Z_OK)
	{
		switch(comressRet)
		{
		case Z_MEM_ERROR:
			m_observer->OnRequestFailure(FUNCTION_UPLOAD_VIDEO_DATA, ERROR_COMPRESS_MEM );
			break;
		default:
			m_observer->OnRequestFailure(FUNCTION_UPLOAD_VIDEO_DATA, ERROR_COMPRESS_UNKNOWN );
			break;
		}
		
		return false;
	}

	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|vid|%s|", FUNCTION_UPLOAD_VIDEO_DATA, m_ggi, m_uid, m_videoID);
	XP_DEBUG_OUT("GLXPlayerUser::sendAddVideoData -> buffer = %s\n", buffer);

	//bool retValue = SendVideoByPost(buffer, pData, size, m_videoID);
	bool retValue = SendVideoByPost(buffer, (char *)dest, (int &)destLen, m_videoID);
	XP_API_DELETE_ARRAY(tmpDest);

	return retValue;
}


/**
* Private function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerYoutube::OnUpdateSuccess( int func_id )
{	
	switch(func_id)
	{			
		case FUNCTION_ADD_VIDEO_INFO:
			processAddVideoInfo(m_pResponsePtr);
			break;		
		
		default:
			break;
	}
	return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
}


/**
* Private function processAddVideoInfo.
* Get video information after call sendAddVideoInfo().
* @see sendAddVideoInfo()
* @param data Data returned from the server.
*/
void GLXPlayerYoutube::processAddVideoInfo(char* data)
{
	XP_API_DEL(m_videoID);
	if(data == NULL || XP_API_STRLEN(data) <= 0)
	{
		return;
	}
	int index = 0;
	char tmp[32];
	
	XP_API_MEMSET(tmp, 0, 32);	
	getValue(data, tmp, index++, '|');
	
	if( XP_API_STRCMP( tmp, "vid" ) == 0)
	{
		XP_API_MEMSET(tmp, 0, 32);	
		getValue(data, tmp, index++, '|');
		m_videoID = XP_API_STRNEW(tmp);
	}
}
