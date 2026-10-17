#ifndef __GL_XPLAYER_SOCKET_FACTORY_H__
#define __GL_XPLAYER_SOCKET_FACTORY_H__

#include "GLXPlayerSocket.h"

/**
 * The class factory providing GLXPlayerSocket instance.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
class GLXPlayerSocketFactory
{
public:
	
	static GLXPlayerSocket * GetSocket( char * server_address = 0, int server_port = 0, 
		GLXPlayerSocketObserver * observer = 0 );

};

#endif // __GL_XPLAYER_SOCKET_FACTORY_H__