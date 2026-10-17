#ifndef __GL_XPLAYER_SOCKET_OBSERVER_H__
#define __GL_XPLAYER_SOCKET_OBSERVER_H__


#include "../XPlayer_defines.h"
/**
 * The class providing data to send and receiving data.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerSocketObserver
{

public:

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	virtual bool   GetRequest() = 0;
	virtual void   PopRequest() = 0;
#endif
	virtual char  *GetRequestData() = 0;
	virtual int    GetRequestLength() = 0;

	virtual void   SetResponseData( char * data, int size ) = 0;
	virtual void   SetResponseLen(int len) = 0;
	
	
};

#endif // __GL_XPLAYER_SOCKET_OBSERVER_H__
