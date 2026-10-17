#pragma once

#include "../../../include/YouTube/GLXPlayerYoutube.h"
#include "../../../include/common/GLXPlayerObserver.h"

class GLReplayObserver : public GLXPlayerObserver
{
public:
	GLReplayObserver(void);
	~GLReplayObserver(void);


	void OnNetworkError() ;

	/**
	* Virtual Function OnRequestTimeout.
	* Connection time out or request timeout.
	* <p>
	* @param func_id Function ID.	
	*/
	void OnRequestTimeout( int func_id ) ;

	/**
	* Virtual Function OnRequestSuccess.
	* The correct response.
	* <p>
	* @param func_id Function ID.
	* @param data Returned data.
	* @param data_len Returned data length.
	*/
	void OnRequestSuccess( int func_id, char * data, int data_len );

	/**
	* Virtual Function OnRequestFailure.
	* The wrong response or the operation is failed on server side.
	* <p>
	* @param func_id Function ID.
	* @param error_code Error code.
	*/
	void OnRequestFailure( int func_id, int error_code );


	void Update() ;
	void Send() ;

private:
	GLXPlayerYoutube *m_GLXPlayerYoutube ;
	bool bSend  ;
	bool bCheckReplayVideoAvailable ;
};
