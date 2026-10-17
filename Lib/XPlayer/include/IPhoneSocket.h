#ifndef __IPHONE_SOCKET_H__
#define __IPHONE_SOCKET_H__

#include "common/GLXPlayerSocket.h"
#include "Game_defines.h"

#ifdef OS_IPHONE

#include "Porting.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


typedef struct _processParam
{
	volatile int *flagAddress;
	char serverName[1024];
}PROCESSPARAM;

void threadGetHostByNameProcess( void *param );

/**
 * The concrete class to implement socket on iPhone.
 *
 *
 * @version 0.0.1
 *
 */
class CIPhoneSocket: public GLXPlayerSocket
	{
	public:
		CIPhoneSocket( char * server_name = NULL, int server_port = 0, 
					  GLXPlayerSocketObserver * observer = NULL );
		
		~CIPhoneSocket(void);
		
		bool GetLocalIP(char * szLocalIP);

		//bool GetServerIP(char * serverName, char *serverIP);

		static void Startup();

		static void Clearup();

		static void *GetHostIP( char *server_name );

		static void TransToIp(char * &name);
		
	protected:
		int RecvFromUnkownIP( char * buffer, int size, char ** ip, int * port );
		
		void Init();
		
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
		
		////////////////    
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
		
		bool CreateTcpSocket();
		
		bool CreateUdpSocket();
		
		char * GetSocketAddr();
		
		unsigned short GetSocketPort();
		
		void Listen( int count );
		
		bool ConnectTo( char * ip, int port );
		
		GLXPlayerSocket * Accept();

		void SetBroadcast();

		int SendBroadcast(const char * buffer, int size, int port);
		
		///////////////
		
	private:
		
		fd_set m_set;
		pthread_t m_getHostByNameThreadId;
		PROCESSPARAM m_getHostByNameThreadParam;
	};

#endif // OS_IPHONE

#endif // __IPHONE_SOCKET_H__
