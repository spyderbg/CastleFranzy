#ifndef __GL_XPLAYER_SOCKET_OBSERVER_H__
#define __GL_XPLAYER_SOCKET_OBSERVER_H__

/**
 * The class providing data to send and receiving data.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
namespace gllive {
	
	class GLXPlayerSocketObserver
		{
			
		public:
			
			virtual char * GetRequestData() = 0;
			virtual int    GetRequestLength() = 0;
			virtual void   SetResponseData( char * data, int size ) = 0;
		};
}

#endif // __GL_XPLAYER_SOCKET_OBSERVER_H__
