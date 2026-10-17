#ifndef __GL_XPLAYER_OBSERVER_H__
#define __GL_XPLAYER_OBSERVER_H__

/**
* Abstract Class GLXPlayerObserver. 
* Class GLXPlayerObserver is the observer of XPlayer component.
* This class designed as an abstract class. So client developer has to re-implement all virtual functions.
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
* @version 1.0.0 
*  
*/
class GLXPlayerObserver
{

public:

	/**
	* Virtual Function OnNetworkError.
	* Indicate that network is broken or can not connect to the server.	
	* <p>
	*/
	virtual void OnNetworkError() = 0;

	/**
	* Virtual Function OnRequestTimeout.
	* Connection time out or request timeout.
	* <p>
	* @param func_id Function ID.	
	*/
	virtual void OnRequestTimeout( int func_id ) = 0;

	/**
	* Virtual Function OnRequestSuccess.
	* The correct response.
	* <p>
	* @param func_id Function ID.
	* @param data Returned data.
	* @param data_len Returned data length.
	*/
	virtual void OnRequestSuccess( int func_id, char * data, int data_len ) = 0;

	/**
	* Virtual Function OnRequestFailure.
	* The wrong response or the operation is failed on server side.
	* <p>
	* @param func_id Function ID.
	* @param error_code Error code.
	*/
	virtual void OnRequestFailure( int func_id, int error_code ) = 0;

};

#endif