#include "../../../include/Download/GLXPlayerDownload.h"
//#include "memory.h"


#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

GLXPlayerDownload::GLXPlayerDownload() : GLXPlayerWebComponent()
{
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );	
}

GLXPlayerDownload::~GLXPlayerDownload()
{
}

/**
* Function LoadConfig.
* Load all configuration.
* @return True / False.
*/
bool GLXPlayerDownload::LoadConfig()
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
		
		if ( XP_API_STRCMP( name, XPLAYER_CONF_DWURL ) == 0 )
		{
			// http://beta-dw.gameloft.com/dwXPlayer_v2.php
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

	XP_API_DEL( data );
	XP_API_DEL( buffer );

	XP_API_FILE_CLOSE( pFile );
	return true;	
}


/**
* Function OnUpdateParse.
* The default process to handle response.
* The subclass can replace it with its own implementation.
*
*/
bool GLXPlayerDownload::OnUpdateParse()
{
	StartResponseParser( m_http_engine->GetResponse(), m_http_engine->GetResponseLen() );

	if(NULL==m_pResponsePtr || 0==m_pResponseLen)
	{
		m_observer->OnNetworkError();
		return false;
	}

#ifdef ENABLE_UNIT_TEST
	XP_FILE_HANDLE pFile;
	#ifdef OS_IPHONE
		char buffer[FILENAME_MAX];
		GetSaveFilePath(buffer, UNIT_TEST_OUTPUT_NAME);
		pFile = XP_API_FILE_OPEN( buffer, "at+" );	
	#else
		pFile = XP_API_FILE_OPEN( UNIT_TEST_OUTPUT_NAME, "at" );
	#endif
		if(pFile != NULL)
		{
			signed   int nWritten;
			int size = XP_API_STRLEN(m_pResponsePtr) + 16;
			char *tmp = XP_API_NEW char[size];
			XP_API_MEMSET(tmp, 0, size);
			XP_API_SPRINTF(tmp, "Response: %s\n", m_pResponsePtr);			
			nWritten = XP_API_FILE_WRITE((void*)tmp, 1, XP_API_STRLEN(tmp), pFile);				

			XP_API_FILE_CLOSE(pFile);
			XP_API_DEL(tmp);
		}		
#endif //ENABLE_UNIT_TEST

	char token[ONLINE_MAX_PACKET_SIZE];	

	XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
	GetNextResponseToken( token );

	if (XP_API_STRCMP(token, "s") == 0)
	{
		return OnUpdateSuccess( FUNCTION_DOWNLOAD_ITEM );
	}
	else if(XP_API_STRCMP( token, "e" ) == 0)
	{
		return OnUpdateFailure( FUNCTION_DOWNLOAD_ITEM );
	}
	return true;
}

/**
* Protected function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerDownload::OnUpdateSuccess( int func_id )
{
	if(func_id == FUNCTION_DOWNLOAD_ITEM)
	{
		m_observer->OnRequestSuccess( func_id, m_pResponsePtr, m_pResponseLen-2 ); //dwXPlayer_v2.php, length is not include "s|"
		return true;
	}
	return false;
}


bool GLXPlayerDownload::SendDownloadFile(int ggi, char *uid, char *filename)
{
	char buffer[ONLINE_MAX_PACKET_SIZE ];

	if(ggi<=0 || uid==NULL || filename==NULL)
	{
		m_observer->OnRequestFailure(FUNCTION_DOWNLOAD_ITEM, ERROR_INIT);
		return false;
	}

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|", FUNCTION_DOWNLOAD_ITEM, ggi, uid, filename);
	XP_DEBUG_OUT("[SendDownloadFile] before String2Blob -> buffer = %s\n", buffer);

	return SendByGet( buffer );
}