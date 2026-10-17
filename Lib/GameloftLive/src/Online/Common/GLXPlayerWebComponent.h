#ifndef __GL_XPLAYER_WEB_COMPONENT_H__
#define __GL_XPLAYER_WEB_COMPONENT_H__

#include "GLXPlayerComponent.h"
#include "GLXPlayerHttp.h"

/**
 * Class GLXPlayerWebComponent
 * It is the basic class of all the XPlayer Web Components 
 * which need to connect to web server.
 * GLXPlayerHttp is used to send data to and receive data from web server.
 * Methods SendByGet() and SendByPost() have been implemented in it,
 * so the subclass need not care about it.
 *
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 * @author Tran Thai Phi <<phi.tranthai@gameloft.com>> 
 * @version 1.0.0
 */
namespace gllive {
	
	class GLXPlayerWebComponent : public GLXPlayerComponent
		{
			
		public:
			
			/**
			 * GLXPlayerWebComponent constructor. 
			 */
			GLXPlayerWebComponent(void);
			
			/**
			 * GLXPlayerWebComponent destructor. 
			 */
			virtual ~GLXPlayerWebComponent(void);	
			
			/**
			 * Function SendByGet.
			 * Send request to HTTP server by GET method.
			 * @param url URL of the server and the request string (  after ?b=xxxx&v=4) 
			 */
			virtual bool SendByGet( char * url );
			
			/**
			 * Function SendByPost.
			 * Send request to HTTP server by POST method.
			 * @param url URL of the server and the request string (  after b=xxxx&v=4) 
			 */
			virtual bool SendByPost( char * url );
			
			/**
			 * Function Update.
			 * The default process to handle data.
			 * The subclass can replace it with its own implementation.
			 *
			 */
			virtual void Update();
			
			/**
			 * Function Cancel.
			 * Cancel current transaction.
			 *
			 */
			virtual void Cancel();
			
			/**
			 * Function OnUpdateParse.
			 * The default process to handle response.
			 * The subclass can replace it with its own implementation.
			 *
			 */
			virtual bool OnUpdateParse();
			
			/**
			 * Function OnUpdateSuccess.
			 * The default process to handle successful response.
			 * The subclass can replace it with its own implementation.
			 * @param func_id Function ID.
			 */
			virtual bool OnUpdateSuccess( int func_id );
			
			/**
			 * Function OnUpdateFailure.
			 * The default process to handle failed response.
			 * The subclass can replace it with its own implementation.
			 * @param func_id Function ID.
			 */
			virtual bool OnUpdateFailure( int func_id );
			
		protected:
			
			/**
			 * Function StartResponseParser.
			 * The default process to parse the response string from the server.
			 * @param pResponse Response string.
			 */
			void StartResponseParser( char * pResponse );
			
			/**
			 * Function GetNextResponseToken.
			 * Get the next token of the response string by XPLAYER_DATA_SEPERATOR, default is '|'.
			 * @param pToken Return token.
			 * @return token.
			 */
			char * GetNextResponseToken( char * pToken );
			
			/**
			 * Function GetNextResponseIntToken.
			 * Get the next integer value of the response string by XPLAYER_DATA_SEPERATOR, default is '|'.	 
			 * @return An integer value.
			 */
			int GetNextResponseIntToken();
			
			/**
			 * Function IsNextResponseStringToken.
			 * Check if next response string token is same as pStr.
			 * @param pStr Value to compare.
			 * @return True / False.
			 */
			bool IsNextResponseStringToken( const char * pStr );
			
			/**
			 * Function IsNextResponseIntToken.
			 * Check if next integer value of the response string is equal value.
			 * @param value Value to compare.
			 * @return True / False.
			 */
			bool IsNextResponseIntToken( int value );
			
			/**
			 * Function GetFunctionID.
			 * Get the function Id returned from the server
			 * @param type Function type ('f' / 'g').
			 * @param id Function ID.
			 * @return Function ID or -1.
			 */
			int GetFunctionID( char type, int id );
			
			/**
			 * Function LoadConfig.
			 * Load all configuration.
			 * @return True / False.
			 */
			bool LoadConfig();
			
		protected:
			
			/**
			 * Variable m_http_server_address.
			 * Web server address: "beta.gameloft.com", "gold.gameloft.com", ...
			 */
			char * m_http_server_address;
			
			/**
			 * Variable m_http_php_address.
			 * PHP address: "/ope/GenericXPlayer_v1.php", ...
			 */	
			
			char * m_http_php_address;
			
			/**
			 * Variable m_http_php_version.
			 * PHP version: "&v=4", ...
			 */
			char * m_http_php_version;
			
			/**
			 * Variable m_http_url.
			 * URL address. e.g. http://beta.gameloft.com/ope/GenericXPlayer_v1.php
			 */
			char * m_http_url;
			
			/**
			 * Variable m_http_engine.
			 * Http Engine
			 */
			GLXPlayerHttp * m_http_engine;
			
			/**
			 * Variable m_pResponsePtr.
			 * Response string from the HTTP server.
			 */
			char * m_pResponsePtr;
			
			/**
			 * Variable m_enabled.
			 * Enable to handle Update() function or not.
			 * @see Update()
			 */
			bool m_enabled;
			
#ifdef ENABLE_TIMEOUT
			
			/**
			 * Variable m_callStartTime.
			 * The time when the current request started - used for timeout implementation.
			 * @see Update()
			 */
			long m_callStartTime;
			
			/**
			 * Variable m_callFunctionID.
			 * Function ID called when start request - used for timeout implementation.
			 * @see Update()
			 */
			int m_callFunctionID;
#endif //ENABLE_TIMEOUT
			
		};
}

#endif
