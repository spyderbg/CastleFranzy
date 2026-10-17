#ifndef __GL_XPLAYER_LOGIN_H__
#define __GL_XPLAYER_LOGIN_H__

#include "../Common/GLXPlayerWebComponent.h"

namespace gllive
{
	/**
	* Class GLXPlayerLogin. 
	* Class GLXPlayerLogin is designed for login, logout and keep the connection to web server. 
	*
	* @author  World Online Team <<online-adm@gameloft.com>>
	* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
	 * @version 1.0.0 
	 *  
	 */
	class GLXPlayerLogin : public GLXPlayerWebComponent
	{

	public:

		enum XPlayerLoginType
		{
			XPlayer_Login = 0,
			XPlayer_Login_Live = 1,
			XPlayer_Login_Game = 2
		};

		/**
		* GLXPlayerLogin constructor.
		* Create an instance of GLXPlayerLogin.
		*/
		GLXPlayerLogin(void);		
		
		/**
		* GLXPlayerLogin destructor.
		* Destroy an instance of GLXPlayerLogin.
		*/
		virtual ~GLXPlayerLogin(void);
		
		/**
		* Function Update.
		* Called each frame to Update state.	
		*/
		virtual void Update();	
		
		/**
		* Function SendLogin.
		* Login web server.
		* @param name User name.
		* @param password User password.
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendLogin( char * name, char * password, int timestamp, int type = 1, bool isReturnName = false, char* deviceToken = NULL, char* lang = "EN", char* UDID = 0);
		
		
		/**
		* Function SendLogout.
		* Logout web server.	
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendLogout();	
		
		/**
		* Function SendGetAPNSInfo.
		* Send request to get apple push notification server informations.
		* @return Success: |r|s|server ip|server port|publicCertPemPath|privateKeyPemPath|privateKeyPassword 
		* @return Fail:  |r|e|error code  ERROR_NO_RESPONSE   = 27;	ERROR_INVALID_GGI    = 28
		*/
		bool SendGetAPNSInfo();		
		
		/**
		* Function IsLoggedIn.
		* Check if logged in or not.
		* @return True / False.
		*/
		bool IsLoggedIn(){return m_isLoggedIn;}

		/**
		* Function IsLoggedIn.
		* Set logged in or not.
		*/
		void SetLoggedIn(bool login){m_isLoggedIn = login;};
		
		/**
		* Function KeepAlive.
		* Keep connection to web server alive.	
		*/
		void KeepAlive();

		/**
		* Function GetTimeStamp
		* Return the time stamp of this login
		*/
		long GetTimeStamp() const;
		
	protected:
		/**
		* Protected function OnUpdateSuccess.
		* Handle success message returned from the Web server.	
		* @param func_id Function ID.
		* @return True / False.
		*/
		virtual bool OnUpdateSuccess( int func_id );
		
		/**
		* Protected function SendKeepAlive.
		* Keep connection to web server alive.	
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendKeepAlive();
		
		/**
		* Protected function ShouldSendKeepAlive.
		* Calculate should call send keep alive or not	
		* @return True / False
		*/
		bool ShouldSendKeepAlive();		
		
		/**
		* Variable m_keepAliveReturns.
		* Time value response from server form last called sendKeepAlive()	
		*/
		long m_keepAliveReturns;
		
		/**
		* Variable m_isLoggedIn.
		* Is logged in?
		*/
		bool m_isLoggedIn;
		
		/**
		* Variable m_lastSendKeepAliveTime.
		* The time of last called sendKeepAlive().
		*/
		long m_lastSendKeepAliveTime;

		char m_username[16];	
	};
}
#endif // __GL_XPLAYER_MESSAGE_H__
