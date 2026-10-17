#include "../../../../include/Multiplayer/Common/GLXPlayerMPBase.h"

#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

/**
* GLXPlayerMPBase Constructor. 
*/
GLXPlayerMPBase::GLXPlayerMPBase(GLXPlayerMPBaseObserver * observer): m_observer( observer )
{
	Init();	
}

/**
* GLXPlayerMPBase Destructor. 
*/
GLXPlayerMPBase::~GLXPlayerMPBase(void)
{
	XP_API_DEL( currentPacket );
}	

/**
 * Function Init.
 * Initialize all varialbe of GLXPlayerMPBase instance.
 */
void GLXPlayerMPBase::Init()
{	
	//init the multiplayer members 
	currentPacket = NULL;
	m_connectState = CONNECT_STATE_START;
	m_canReceiveFromServer = false;
	
	LoadConfig();
}

/**
 * Function LoadConfig.
 * Load the Multiplayer configuration: server address, server socket, ...
 */
bool GLXPlayerMPBase::LoadConfig()
{
	MP_SERVER_ADDRESS = NULL;
	return true;	
}

/**
* Function mpSendDisconnect.
* Send a disconnect request to the multiplayer server.	
*/
void GLXPlayerMPBase::mpSendDisconnect()
{
	if(wtcp == NULL)
		return;

	if ( !wtcp->isConnected() )
	{
		// already disconnected or loss of signal, ensure cleanup is done.
		mpDisconnect();
		m_lastErrorCode = ERROR_NONE;
		m_observer->OnMPDisconnectSuccess();
		return;
	}
   
	m_lastErrorCode = ERROR_PENDING;
	wtcp->sendFinishConnectionPackage();

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
}

/**
* Function mpSendEstablishConnection.
* Send an connection establish messages to the server. 
*/
void GLXPlayerMPBase::mpSendEstablishConnection()
{
	//if(m_connectState == CONNECT_STATE_START)
	{
		if ( mpIsConnected() )
		{
			m_lastErrorCode = ERROR_NONE;
			m_observer->OnMPEstablishConnectionSuccess();
			return;
		}

		if ( NULL != wtcp )
		{
			// Disconnect and cleanup.
			mpDisconnect();
			delete wtcp;
			wtcp = NULL;
		}

		m_lastErrorCode = ERROR_PENDING;

		wtcp = XP_API_NEW Connection( MP_SERVER_ADDRESS, MP_TCP_PORT );
		wtcp->setName( m_username );

		m_connectState = CONNECT_STATE_WAITING;
		m_canReceiveFromServer = true;
	}
}

/**
 * Function mpSendLogin.
 *Starts a request for login. 	 
 * @param uid User ID.
 */
void GLXPlayerMPBase::mpSendLogin(char* uid)
{
	XP_DEBUG_OUT("GLXPlayerMPbyP2P::mpSendLogin(){\n");	
	
	if ( !mpIsConnected() || mpIsLoggedIn() )
	{
		m_lastErrorCode = ERROR_NONE;
		m_observer->OnMPLoginSuccess();
		return;
	}

	// the login nickname for the multiplayer server is GGI-UID
	char mpLoginNickname[(ONLINE_MAX_PACKET_SIZE + 1)];

	XP_API_MEMSET(mpLoginNickname,0,ONLINE_MAX_PACKET_SIZE + 1);
	XP_API_SPRINTF( mpLoginNickname,"%d-%s", m_ggi ,uid );
	
	XP_DEBUG_OUT("GLXPlayerMPbyP2P::mpLoginNickname %s", mpLoginNickname);
	
	m_lastErrorCode = ERROR_PENDING;	
	wtcp->sendLoginPackage( mpLoginNickname );

#ifdef MP_ENABLE_TIMEOUT 
	//Set timer
	wtcp->last_activity_time = XP_API_GET_TIME();
#endif
	XP_DEBUG_OUT("}\n");
}

/**
 * Function getErrMsg.
 * Get current error message.
 * @return Current error message.
 */
char * GLXPlayerMPBase::getErrMsg()
{
	return m_errMsg;
}

/**
 * Function setErrMsg.
 * Set the error message receivced from the server.	
 */
void GLXPlayerMPBase::setErrMsg()
{
	if( NULL != m_errMsg )
	{
		delete( m_errMsg );
		m_errMsg = NULL;
	}

	if ( NULL != currentPacket )
	{
		int size = currentPacket->getMessageLen();
		char* msg = (char *)currentPacket->getMessageBody();

		m_errMsg = XP_API_NEW char[size+1];
		XP_API_MEMSET( m_errMsg, 0, size+1 );
		XP_API_MEMCPY( m_errMsg, msg, size );			
	}
}