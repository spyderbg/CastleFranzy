#include "GLXPlayerSereverConfig.h"


#ifdef OS_IPHONE
#import <stdio.h>
extern void GLLGetConfigFilePath(char* filePath, const char* filename);
extern void GLLGetSaveFilePath(char* filePath, const char* filename);
#endif

namespace gllive {
	bool GLXPlayerSereverConfig::s_isLoadConfig=false;

	int GLXPlayerSereverConfig::m_webConfig_ggi=0;
	char GLXPlayerSereverConfig::m_webConfig_XPlayerURL[256];
	char GLXPlayerSereverConfig::m_webConfig_XPlayerDWURL[256];
	char GLXPlayerSereverConfig::m_webConfig_PhpVer[2];
	char GLXPlayerSereverConfig::m_webURLType[2];
	char GLXPlayerSereverConfig::m_webConfig_ChatServer[64];
	char GLXPlayerSereverConfig::m_webConfig_ChatDomain[64];
	char GLXPlayerSereverConfig::m_webConfig_ChatRoomDomain[96];




	GLXPlayerSereverConfig::GLXPlayerSereverConfig()
	{
		m_gameVersion=NULL;
		m_configGameVersion=NULL;
		m_lastTime=XP_API_NEW GameDate;
		m_curTime=XP_API_NEW GameDate;
		GetCurTimeDate(m_curTime);
		
		XP_API_DELETE_ARRAY( m_http_server_address );
		XP_API_DELETE_ARRAY( m_http_php_address );
		XP_API_DELETE_ARRAY( m_http_php_version );
		XP_API_DELETE_ARRAY( m_http_url );
		LoadConfig();
		
		XP_API_DEL( m_http_engine );
		m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );
	}
	GLXPlayerSereverConfig::~GLXPlayerSereverConfig()
	{
		XP_API_DEL(m_gameVersion);
		XP_API_DEL(m_configGameVersion);
		XP_API_DEL(m_lastTime);
		XP_API_DEL(m_curTime);

	}
	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	//m_webConfig_url;
	//m_webConfig_XPlayerURL;
	//m_webConfig_ChatServer;
	//m_webConfig_ChatDomain;
	//m_webConfig_ChatRoomDomain;
	//m_webConfig_PhpVer;
	bool GLXPlayerSereverConfig::OnUpdateSuccess( int func_id )
	{
		char token[ONLINE_MAX_PACKET_SIZE];


		if(func_id == FUNCTION_GET_WEBCONFIG)
		{



			while(XP_API_STRLEN(GetNextResponseToken( token ))!=0)
			{

				if(XP_API_STRCMP(token,XPLAYER_SER_CONF_CONFIG_URL_TYPE)==0)
				{
					//char* state=XP_API_STRNEW(GetNextResponseToken( token ));

					XP_API_STRCPY(m_webURLType,GetNextResponseToken( token ));


				}
				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_URL)==0)
				{

					XP_API_STRCPY(m_webConfig_XPlayerURL,GetNextResponseToken( token ));
					//m_webConfig_XPlayerURL = XP_API_STRNEW(GetNextResponseToken( token ));


				}
				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_PHPVER)==0)
				{

					XP_API_STRCPY(m_webConfig_PhpVer,GetNextResponseToken( token ));
					//m_webConfig_PhpVer=XP_API_STRNEW(GetNextResponseToken( token ));

				}

				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_DWURL)==0)
				{

					XP_API_STRCPY(m_webConfig_XPlayerDWURL,GetNextResponseToken( token ));

					//m_webConfig_PhpVer=XP_API_STRNEW(GetNextResponseToken( token ));

				}

				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_CHAT_SERVER)==0)
				{

					XP_API_STRCPY(m_webConfig_ChatServer,GetNextResponseToken( token ));
					//m_webConfig_ChatServer=XP_API_STRNEW(GetNextResponseToken( token ));

				}
				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_CHAT_DOMAIN)==0)
				{

					XP_API_STRCPY(m_webConfig_ChatDomain,GetNextResponseToken( token ));
					//m_webConfig_ChatDomain=XP_API_STRNEW(GetNextResponseToken( token ));

				}
				if(XP_API_STRCMP( token,XPLAYER_SER_CONF_CHAT_ROOM_DOMIAN)==0)
				{

					XP_API_STRCPY(m_webConfig_ChatRoomDomain,GetNextResponseToken( token ));
					//m_webConfig_ChatRoomDomain=XP_API_STRNEW(GetNextResponseToken( token ));

				}

			}




			if(XP_API_STRLEN(m_webURLType)==0||
				XP_API_STRLEN(m_webConfig_XPlayerURL)==0||
				XP_API_STRLEN(m_webConfig_PhpVer)==0||
				XP_API_STRLEN(m_webConfig_ChatServer)==0||
				XP_API_STRLEN(m_webConfig_ChatDomain)==0||
				//XP_API_STRLEN(m_webConfig_XPlayerDWURL)==0||//not necessary
				XP_API_STRLEN(m_webConfig_ChatRoomDomain)==0)
			{
				m_observer->OnRequestFailure( FUNCTION_GET_WEBCONFIG, ERROR_BAD_RESPONSE );
			}else

			{
				s_isLoadConfig=true;
				//	s_runCount=0;
				SaveServerConfig();
				m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );

			}
		}

		else
		{
			return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
		}
		return true;
	}

	/**
	*Function SendGetSreverConfig.
	*getAllServerURL from web
	*
	*f|1|i|ggi|v|0.01|
	*/
	bool GLXPlayerSereverConfig::SendGetSreverConfig(int& retCode)
	{
		if(m_ggi==NULL||m_gameVersion==NULL)
		{
			//m_observer->OnRequestFailure( FUNCTION_LOGIN, ERROR_INIT );
			retCode=-1;
			return false;
		}
		if(s_isLoadConfig==true)
		{
			//m_observer->OnRequestSuccess( FUNCTION_GET_WEBCONFIG, NULL, 0 );
			retCode=1;
			return true;
		}
		if(loadServerConfig())
		{
			s_isLoadConfig=true;
			//SaveServerConfig();
			retCode=1;
			//m_observer->OnRequestSuccess( FUNCTION_GET_WEBCONFIG, NULL, 0 );
			return true;
		}
		//init
		XP_API_MEMSET(m_webURLType,0,sizeof(m_webURLType));
		XP_API_MEMSET(m_webConfig_XPlayerURL,0,sizeof(m_webConfig_XPlayerURL));
		XP_API_MEMSET(m_webConfig_PhpVer,0,sizeof(m_webConfig_PhpVer));
		XP_API_MEMSET(m_webConfig_XPlayerDWURL,0,sizeof(m_webConfig_XPlayerDWURL));
		XP_API_MEMSET(m_webConfig_ChatServer,0,sizeof(m_webConfig_ChatServer));
		XP_API_MEMSET(m_webConfig_ChatDomain,0,sizeof(m_webConfig_ChatDomain));
		XP_API_MEMSET(m_webConfig_ChatRoomDomain,0,sizeof(m_webConfig_ChatRoomDomain));




		char url[ONLINE_MAX_PACKET_SIZE ];



		XP_API_MEMSET( url, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(url, "f|%d|i|%d|v|%s|", FUNCTION_GET_WEBCONFIG, m_ggi, m_gameVersion);
		XP_DEBUG_OUT("[GetWebConfig] before String2Blob -> buffer = %s\n", url);

		char buffer[ONLINE_MAX_PACKET_SIZE];
		char * blob = NULL;	
		m_enabled = true;

		blob = SSEncDec_String2Blob( url );
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF( buffer, "b=%s", blob );
		XP_API_DEL( blob );

	#ifdef ENABLE_TIMEOUT	
		char tmp[8] = {0};
		getValue(url, tmp, 1, '|');
		m_callFunctionID = XP_API_ATOI(tmp);
		m_callStartTime = XP_API_GET_TIME();
	#endif //ENABLE_TIMEOUT

		XP_DEBUG_OUT("SendByGet %s\n", buffer );

		m_http_engine->sendByGetWithNoVer( m_http_url, buffer );
		retCode=0;
		return true;

	}

	/**
	* Function LoadConfig.
	* Load all configuration.
	* @return True / False.
	*/
	bool GLXPlayerSereverConfig::LoadConfig()
	{

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
			XP_DEBUG_OUT("[GLXPlayerSereverConfig] load config - %s not found \n", XPLAYER_FILE_CONF );
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


		{
			int pos=0;
			char temp[256] = { 0 };
			char host[256] = { 0 };

			m_http_url = XP_API_STRNEW( SERVER_CONFIG_URL );

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
				m_webConfig_ggi=m_ggi;
			}




			if(XP_API_STRCMP(name,XPLAYER_CONF_GAME_VER)==0)
			{
				m_gameVersion=XP_API_STRNEW( value );


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

	bool GLXPlayerSereverConfig::loadServerConfig()
	{
		XP_FILE_HANDLE pFile;

	#ifdef OS_IPHONE
		char tmp[FILENAME_MAX];
		XP_API_MEMSET(tmp,0,FILENAME_MAX);
		GLLGetSaveFilePath(tmp, PROFILE_FILE_SERVER_CONFIG);
		pFile = XP_API_FILE_OPEN( tmp, "r" );
	#else
		pFile = XP_API_FILE_OPEN( PROFILE_FILE_SERVER_CONFIG, "r" );
	#endif

		if( !pFile )
		{
			XP_DEBUG_OUT("[GLXPlayerSereverConfig] loadServerConfig - %s not found \n", PROFILE_FILE_SERVER_CONFIG );

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


		//s_runCount=0;
		XP_API_MEMSET(m_webURLType,0,sizeof(m_webURLType));
		XP_API_MEMSET(m_webConfig_XPlayerURL,0,sizeof(m_webConfig_XPlayerURL));
		XP_API_MEMSET(m_webConfig_PhpVer,0,sizeof(m_webConfig_PhpVer));
		XP_API_MEMSET(m_webConfig_XPlayerDWURL,0,sizeof(m_webConfig_XPlayerDWURL));
		XP_API_MEMSET(m_webConfig_ChatServer,0,sizeof(m_webConfig_ChatServer));
		XP_API_MEMSET(m_webConfig_ChatDomain,0,sizeof(m_webConfig_ChatDomain));
		XP_API_MEMSET(m_webConfig_ChatRoomDomain,0,sizeof(m_webConfig_ChatRoomDomain));
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




			if ( XP_API_STRCMP( name, XPLAYER_SER_CONF_CONFIG_URL_TYPE ) == 0 )
			{
				//m_webConfig_XPlayerURL=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webURLType,value);
			}

			if ( XP_API_STRCMP( name, XPLAYER_SER_CONF_URL ) == 0 )
			{
				//m_webConfig_XPlayerURL=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_XPlayerURL,value);
			}


			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_PHPVER)==0)
			{
				//m_webConfig_PhpVer=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_PhpVer,value);
			}
			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_PHPVER)==0)
			{
				//m_webConfig_PhpVer=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_PhpVer,value);
			}
			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_DWURL)==0)
			{
				//m_webConfig_ChatServer=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_XPlayerDWURL,value);
			}

			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_CHAT_SERVER)==0)
			{
				//m_webConfig_ChatServer=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_ChatServer,value);
			}

			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_CHAT_DOMAIN)==0)
			{
				//m_webConfig_ChatDomain=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_ChatDomain,value);
			}

			if(XP_API_STRCMP(name,XPLAYER_SER_CONF_CHAT_ROOM_DOMIAN)==0)
			{
				//m_webConfig_ChatRoomDomain=XP_API_STRNEW( value );

				XP_API_STRCPY(m_webConfig_ChatRoomDomain,value);
			}
			/*if ( XP_API_STRCMP( name, XPLAYER_SER_CONF_CONFIG_RUN_TIME) == 0 )
			{

			s_runCount= XP_API_ATOI(value);
			}*/
			if ( XP_API_STRCMP( name, XPLAYER_CONF_GAME_VER) == 0 )
			{
				m_configGameVersion=XP_API_STRNEW(value);

			}
			if ( XP_API_STRCMP( name, XPLAYER_SER_CONF_CONFIG_RUN_DATE) == 0 )
			{
				char date[8];
				memset(date,0,8);
				int tmpPos=0;
				tmpPos=XP_API_PARSE_DATA(value,date,0,'-');
				m_lastTime->year=XP_API_ATOI(date);
				memset(date,0,8);
				tmpPos=XP_API_PARSE_DATA(value,date,1,'-');
				m_lastTime->yday=XP_API_ATOI(date);

				;
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
		/*if(m_webConfig_XPlayerURL==NULL||
		m_webConfig_PhpVer==NULL||
		m_webConfig_ChatServer==NULL||
		m_webConfig_ChatDomain==NULL||
		m_webConfig_ChatRoomDomain==NULL)
		{
		return false;
		}*/
		if(XP_API_STRLEN(m_webURLType)==0||
			XP_API_STRLEN(m_webConfig_XPlayerURL)==0||
			XP_API_STRLEN(m_webConfig_PhpVer)==0||
			XP_API_STRLEN(m_webConfig_ChatServer)==0||
			XP_API_STRLEN(m_webConfig_ChatDomain)==0||
			//XP_API_STRLEN(m_webConfig_XPlayerDWURL)==0||//not necessary
			XP_API_STRLEN(m_webConfig_ChatRoomDomain)==0
			//||(s_runCount%RUN_TIME_COUNT)==0
			||isOutOfDate(XPLAYER_SER_CONF_CONFIG_OUT_DATE)
			||m_configGameVersion==NULL)
		{
			return false;
		}
		if(XP_API_STRCMP(m_configGameVersion,m_gameVersion)!=0)
		{
			return false;
		}
		return true;	

	}


	bool GLXPlayerSereverConfig::SaveServerConfig()
	{
		XP_FILE_HANDLE pFile;

	#ifdef OS_IPHONE
		char buffer[FILENAME_MAX];
		GLLGetSaveFilePath(buffer, PROFILE_FILE_SERVER_CONFIG);
		pFile = XP_API_FILE_OPEN( buffer, "wb+" );	
	#else
		pFile = XP_API_FILE_OPEN( PROFILE_FILE_SERVER_CONFIG, "wb" );
	#endif

		if(!pFile)
		{
			XP_DEBUG_OUT("SaveProfile(), Error creating file %s \n", PROFILE_FILE_SERVER_CONFIG);
			return false;
		}
		//s_runCount++;
		signed   int nWritten;
		char tmp[1024];

		XP_API_MEMSET( tmp, 0, 1024 );
		XP_API_SPRINTF( tmp,
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %s\r\n"\
			"%s: %d-%d\r\n",
			XPLAYER_CONF_GAME_VER,
			m_gameVersion,
			XPLAYER_SER_CONF_CONFIG_URL_TYPE,
			m_webURLType,
			XPLAYER_SER_CONF_URL,
			m_webConfig_XPlayerURL,
			XPLAYER_SER_CONF_PHPVER,
			m_webConfig_PhpVer,
			XPLAYER_SER_CONF_DWURL,
			m_webConfig_XPlayerDWURL,
			XPLAYER_SER_CONF_CHAT_SERVER,
			m_webConfig_ChatServer,
			XPLAYER_SER_CONF_CHAT_DOMAIN,
			m_webConfig_ChatDomain,	
			XPLAYER_SER_CONF_CHAT_ROOM_DOMIAN, 
			m_webConfig_ChatRoomDomain,
			XPLAYER_SER_CONF_CONFIG_RUN_DATE, 
			m_curTime->year,
			m_curTime->yday
			);


		nWritten = XP_API_FILE_WRITE((void*)tmp,	1, XP_API_STRLEN(tmp),		pFile);
		XP_API_FILE_CLOSE(pFile);
		return true;
	}
	bool GLXPlayerSereverConfig::isOutOfDate(int days)
	{

		if(m_lastTime->year<m_curTime->year)
		{
			return true;

		}else if(m_lastTime->year==m_curTime->year)
		{
			if(m_curTime->yday>m_lastTime->yday+days)
				return true;
			else
				return false;


		}else
		{
			return false;
		}

		return false;
	}
	void GLXPlayerSereverConfig::GetCurTimeDate(GameDate* date)
	{

		time_t sysTime;
		time(&sysTime);

		struct tm t;
	#ifdef OS_WIN32
		localtime_s(&t,&sysTime);
	#endif
	#ifdef OS_IPHONE
		localtime_r(&sysTime,&t);
	#endif
		date->year=t.tm_year;
		date->yday=t.tm_yday;

	}
}