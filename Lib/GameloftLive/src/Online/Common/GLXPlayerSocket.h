#ifndef __GL_XPLAYER_SOCKET_H__
#define __GL_XPLAYER_SOCKET_H__

#include "GLXPlayerSocketObserver.h"
#include <string>

#define SEND_BUFFER_SIZE	(1024)
#define RECV_BUFFER_SIZE	(65535)
#define RECV_ONCE_BUFFER_SIZE	(2048)

/**
 * The basic class of XPlayer socket on all kinds of platform.
 *
 *
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 *
 */
namespace gllive {
	
	class GLXPlayerSocket
		{
		public:
			
			GLXPlayerSocket( char * server_name, int server_port, GLXPlayerSocketObserver * observer );
			
			virtual ~GLXPlayerSocket();
			
			enum XSocketState
			{
				XSOCKET_STATE_NONE = 0,
				XSOCKET_STATE_START_CONNECT,
				XSOCKET_STATE_WAIT_CONNECTING,
				XSOCKET_STATE_SEND_REQUEST,
				XSOCKET_STATE_WAIT_RESPONSE,
				XSOCKET_STATE_CLOSE_CONNECTION,
				XSOCKET_STATE_REQUEST_COMPLETE,
				XSOCKET_STATE_ERROR,
				XSOCKET_STATE_CANCELLED
			};
			
			enum XSocketSelectFlag
			{
				XSOCKET_SELECT_READ,
				XSOCKET_SELECT_WRITE
			};
			
			enum XSocketType
			{
				XSOCKET_TYPE_NONE = 0,
				XSOCKET_TYPE_TCP = 1,
				XSOCKET_TYPE_UDP = 2
			};	
			
			virtual bool GetLocalIP(char * szLocalIP) = 0;
			
			//virtual bool GetServerIP(char * serverName, char *serverIP) = 0;
			
			virtual void Start();
			
			virtual void Run();
			
			virtual void Cancel();
			
			virtual bool IsInProgress();
			
			virtual bool IsErrorOccurred();
			
			virtual void * GetHostByName( char *server_name ) = 0;
			
			virtual void * GetHostName( char ** name ) = 0;
			
			virtual bool CreateSocket() = 0;
			
			virtual int GetLastError() = 0;
			
			virtual bool SetNonBlocking() = 0;
			
			virtual bool Connect() = 0;
			
			virtual bool ConnectByName(char *server_name) = 0;
			
			virtual bool CloseSocket() = 0;
			
			virtual int Select( XSocketSelectFlag flag ) = 0;
			
			virtual int Send( char * data, int size ) = 0;
			
			virtual int Recv( char * data, int size ) = 0;
			
			virtual bool IsReadyForRW( ) = 0;
			
			virtual GLXPlayerSocket * Accept() = 0;
			
			/**
			 *
			 *
			 */
			virtual int Bind( char * addr = 0, unsigned short port = 0 ) = 0;
			
			/**
			 * Send data to (ip:port) with udp protocol.
			 * <p>
			 * @param 
			 *     buffer: the data to send
			 *     size:   the size of the data
			 *     ip:     the remote address with the format "a.b.c.d"
			 *     port:   the remote port
			 * @return:       
			 *     the size to send
			 *
			 */
			virtual int SendTo( const char * buffer, int size, const char * ip, int port ) = 0;
			
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
			virtual int RecvFrom( char * buffer, int size, const char * ip, int port ) = 0;
			
			virtual int RecvFromUnkownIP( char * buffer, int size, char ** ip, int * port ) = 0;
			
			/**
			 * Listen to the TCP connect request.
			 *
			 *
			 */
			virtual void Listen( int count ) = 0;
			
			virtual bool ConnectTo( char * ip, int port ) = 0;
			
			virtual bool CreateTcpSocket() = 0;
			
			virtual bool CreateUdpSocket() = 0;
			
			virtual char * GetSocketAddr() = 0;
			
			virtual unsigned short GetSocketPort() = 0;
			
			char * GetAcceptIP();
			
			int GetAcceptPort();
			
			void SetAcceptIP( char * ip );
			
			void SetAcceptPort( int port );
			
			void SetSocket( int soc );
			
			int GetSocketType();
			
			void SetSocketType( int type );
			
			virtual void SetBroadcast() = 0;
			
			virtual int SendBroadcast(const char * buffer, int size, int port) = 0;
			
			int GetSocket()
			{
				return m_socket;
			}
			
		protected:
			
			XSocketState m_state;
			
			int m_socket;
			
			void * m_server_addr;
			
			int    m_server_port;
			
			char * m_server_name;
			
			GLXPlayerSocketObserver * m_observer;
			
			char m_abBuffer[RECV_ONCE_BUFFER_SIZE];
			
			int m_nReceived;
			
			XSocketType m_type;
			
			char * m_accept_ip;
			
			int m_accept_port;
			
			//char * m_buffer;
			std::string m_buffer;
			
			bool m_buffer_used;
			
			bool m_bReceiveHttpHeader;
			
			int m_total_length;
			
			//-----------------change the connect function to nonblock, add the connect state //by han yuan 2009.3.31
		protected:
			int m_nonBlockConnectState;
			volatile int m_gethostbynameState;
			long m_connectTimeStart;
			long m_gethostbynametimeStart;
			
		public:
			enum NonBlockConnectState
			{
				NONBLOCK_CONNECT_STATE_START,
				NONBLOCK_CONNECT_STATE_WAITING,
				NONBLOCK_CONNECT_STATE_SUCCESS,
				NONBLOCK_CONNECT_STATE_FAILED
			};
			
			enum GetHsotByNameState
			{
				GETHOSTBYNAME_STATE_INIT,
				GETHOSTBYNAME_STATE_WAITING
			};
			
			int GetNonBlockConnectState()
			{ 
				return m_nonBlockConnectState;
			}
			
			void SetNonBlockConnectState(int state)
			{ 
				m_nonBlockConnectState = state;
			}
			
			//int GetGetHostByNameState()
			//{
			//	return m_gethostbynameState;
			//}
			//------------------------------------------
			
		private:
			
			int CalculateTotalLength();
			void RemoveHttpHeader();
		};
}

#endif // __XSOCKET__H__