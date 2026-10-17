#ifndef __WIN32_SOCKET_H__
#define __WIN32_SOCKET_H__

#include "Online/Common/GLXPlayerSocket.h"
#include "System/GLLive_Memory.h"  //byhy for debug memory leak
#include <WinSock.h>

/**
 * The concrete class to implement socket on Windows.
 *
 *
 * @version 0.0.1
 *
 */
namespace gllive
{
	class CWin32Socket : public GLXPlayerSocket
	{
	public:
		CWin32Socket( char * server_name = NULL, int server_port = NULL, 
			GLXPlayerSocketObserver * observer = NULL );

		~CWin32Socket(void);
		
		bool GetLocalIP(char * szLocalIP);

		bool GetServerIP(char * serverName, char *serverIP);

		static void Startup();

		static void Clearup();

	protected:

		bool CreateSocket();

		int GetLastError();

		void * GetHostByName( char *server_name );

		void * GetHostName( char ** name );

		bool SetNonBlocking();

		bool Connect();

		bool CloseSocket();

		int Select( XSocketSelectFlag flag );

		int Send( char * data, int size );

		int Recv( char * data, int size );

		bool ConnectByName( char *server_name );

		bool IsReadyForRW();

		/**
		 *
		 *
		 */
		
		int Bind( char * addr = 0, unsigned short port = 0 );

		/**
		 * Send data to (ip:port) with udp protocol.
		 * <p>
		 * @param buffer: the data to send
		 *        size:   the size of the data
		 * @return:       the size to send
		 *
		 */
		int SendTo( const char * buffer, int size, const char * ip, int port );

		/**
		 * Receive data from (ip:port) with upd protocol.
		 * <p>
		 * @param
		 *     buffer: the buffer to receive data
		 *     size:   the size of the buffer
		 *     ip:     the remote address with the format "a.b.c.d"
		 *     port:   the remote port
		 * @return:       
		 *     the size of received data
		 *
		 */
		int RecvFrom( char * buffer, int size, const char * ip, int port );

		int RecvFromUnkownIP( char * buffer, int size, char ** ip, int * port );

		bool CreateTcpSocket();

		bool CreateUdpSocket();

		char * GetSocketAddr();

		unsigned short GetSocketPort();

		void Listen( int count );

		bool ConnectTo( char * ip, int port );

		GLXPlayerSocket * Accept();

		void SetBroadcast();

		int SendBroadcast(const char * buffer, int size, int port);

	private:

		fd_set m_set;

	};
}

#endif // __WIN32_SOCKET_H__