#include "GLXPlayerLogin.h"
#include "memory.h"

namespace gllive
{
	/**
	* GLXPlayerLogin constructor.
	* Create an instance of GLXPlayerLogin.
	*/
	GLXPlayerLogin::GLXPlayerLogin() : GLXPlayerWebComponent()
	{
		m_isLoggedIn = false;
		m_keepAliveReturns = 0;
		m_lastSendKeepAliveTime = 0;
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
			m_isLoggedIn = true;
			m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
		}
		else if ( func_id == FUNCTION_LOGOUT )
		{
			m_isLoggedIn = false;
			m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
		}
		//else if (func_id == FUNCTION_GET_COUNTRY)
		//{
		//	GetNextResponseToken(token); // skip uid
		//	XP_API_STRCPY(m_country, GetNextResponseToken(token));
		//	m_observer->OnRequestSuccess( func_id, m_pResponsePtr, XP_API_STRLEN( m_pResponsePtr ) );
		//}
		else if ( func_id == FUNCTION_KEEP_ALIVE )
		{
			GetNextResponseToken( token ); //skip 't'
			m_keepAliveReturns = XP_API_ATOI( GetNextResponseToken( token ) );
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
	bool GLXPlayerLogin::SendLogin( char * name, char * password, int timestamp, int type, bool isReturnName, char* deviceToken, char* lang, char* UDID)
	{
		if ( NULL == name || NULL == password )
		{
			m_observer->OnRequestFailure( FUNCTION_LOGIN, ERROR_INIT );
			return false;
		}

		XP_API_STRCPY(m_username, name);

		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|p|%s",FUNCTION_LOGIN, m_ggi, name, password);
		
		char subMsg[128] = {0};
		if ( type > 0 )
		{
			XP_API_SPRINTF(subMsg, "|o|%d", type); 	
			XP_API_STRCAT(buffer, subMsg);		
		}
		
		if(deviceToken != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|d|%s", deviceToken); 	
			XP_API_STRCAT(buffer, subMsg);
		}
		if(UDID)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|udid|%s", UDID);
			XP_API_STRCAT(buffer, subMsg);
		}

		//Return user number
		XP_API_STRCAT(buffer, "|un|1");
		
		XP_API_MEMSET(subMsg, 0, 128);
		XP_API_SPRINTF(subMsg, "|n|%d|l|%s", isReturnName, lang); 
		XP_API_STRCAT(buffer, subMsg);

		//Add timestamp
		XP_API_SPRINTF(subMsg, "|t|%d", timestamp); 	
		XP_API_STRCAT(buffer, subMsg);	
		
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
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%d|", FUNCTION_LOGOUT, m_ggi, m_uid, m_keepAliveReturns);
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
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%d|", FUNCTION_KEEP_ALIVE, m_ggi, m_uid, m_keepAliveReturns );
		//XP_API_SPRINTF(buffer, "f|%d|u|%s", FUNCTION_KEEP_ALIVE, m_uid );

		m_lastSendKeepAliveTime = XP_API_GET_TIME();
		XP_DEBUG_OUT("SendKeepAlive\n");	

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
	bool GLXPlayerLogin::SendGetAPNSInfo()
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE  );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER, m_ggi, m_uid);

		XP_DEBUG_OUT("GLXPlayerLogin::SendGetAPNSInfo\n");	

		return SendByGet( buffer );
	}

	/**
	* Function GetTimeStamp
	* Return the time stamp of this login
	*/
	long GLXPlayerLogin::GetTimeStamp() const
	{
		return m_keepAliveReturns;
	}
}