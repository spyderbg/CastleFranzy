#include "IPhoneSocket.h"
#include "Porting.h"

#ifdef OS_IPHONE
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

namespace gllive {
	
	const int HOST_BUF_LEN = 4;
	
	struct hostent * l_host[HOST_BUF_LEN];
	int l_count = 0;
	
	
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif	/* MAX */
	
	CIPhoneSocket::CIPhoneSocket( char * server_name, int server_port, GLXPlayerSocketObserver * observer )
	: GLXPlayerSocket( server_name, server_port, observer )
	{
		
	}
	
	CIPhoneSocket::~CIPhoneSocket(void)
	{
		if(m_gethostbynameState != GETHOSTBYNAME_STATE_INIT)
		{
			pthread_cancel(m_getHostByNameThreadId);
		}
		CloseSocket();
	}
	
	void CIPhoneSocket::Startup()
	{
		for ( int i=0; i<HOST_BUF_LEN; i++ )
		{
			l_host[i] = NULL;
		}
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &sa, 0);
	}
	
	void CIPhoneSocket::Clearup()
	{
		for ( int i=0; i<l_count; i++ )
		{
			XP_API_DEL( l_host[i]->h_name );		
			for (int j=0; j<1; j++ )
			{
				XP_API_DEL( l_host[i]->h_addr_list[j] );
				l_host[i]->h_addr_list[j] = NULL;
			}
			XP_API_DEL( l_host[i]->h_addr_list );
			XP_API_DEL( l_host[i] );
			l_host[i] = NULL;
		}
		l_count = 0;
	}
	
	void CIPhoneSocket::Init()
	{
		
	}
	
#define IFCONF_BUFFERSIZE 4000
	
	bool CIPhoneSocket::GetLocalIP(char * szLocalIP)
	{
		char buffer[IFCONF_BUFFERSIZE];
		char *ptr = buffer;
		struct ifconf ifc = { IFCONF_BUFFERSIZE, { buffer } };
		
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1)
			return false;
		
		if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
			return false;
		
		while (ptr < buffer + ifc.ifc_len)
		{
			struct ifreq *ifr = (struct ifreq *)ptr;
			struct sockaddr_in *sin = (struct sockaddr_in *)&ifr->ifr_addr;
			char *ipaddr = inet_ntoa(sin->sin_addr);
			char *cptr;
			
			// for next one in buffer
			ptr += sizeof(ifr->ifr_name) + MAX(sizeof(struct sockaddr), ifr->ifr_addr.sa_len);
			
			if (ifr->ifr_addr.sa_family != AF_INET) 
			{
				continue; // ignore if not desired address family
			}
			
			if ((cptr = (char *)XP_API_STRCHR(ifr->ifr_name, ':',1)) != NULL) 
			{
				*cptr = 0; // replace colon with null
			}
			
			ioctl(sockfd, SIOCGIFFLAGS, ifr);
			if ((ifr->ifr_flags & IFF_UP) == 0) 
			{
				continue; // ignore if interface not up
			}
			
			if(XP_API_STRCMP(ipaddr, "127.0.0.1") == 0)
			{
				continue;
			}
			
			XP_API_STRCPY(szLocalIP, ipaddr);
		}
		
		close(sockfd);
		return true;
	}
	
	//bool CIPhoneSocket::GetServerIP(char * serverName, char *serverIP)
	//{
	//	if(serverName == NULL || serverIP == NULL)
	//		return false;
	//
	//	struct hostent *host_entry = (struct hostent *)GetHostByName( serverName );
	//	
	//	char* tmpIP = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
	//	
	//	XP_API_STRCPY(serverIP, tmpIP);
	//	return true;	
	//}
	
	void *CIPhoneSocket::GetHostIP( char *server_name )
	{
		for( int i=0; i<l_count; i++ )
		{
			if ( XP_API_STRCMP( server_name, l_host[i]->h_name ) == 0 )
			{
				XP_DEBUG_OUT( "GetHostByName() Existed %s \n", server_name );
				return l_host[i];
			}
		}
		
		struct hostent *server;
		server = gethostbyname( server_name );
		
		if(server == NULL)
		{
			return NULL;
		}
		
		if ( l_count < HOST_BUF_LEN )
		{
			l_host[l_count] = XP_API_NEW struct hostent;
			XP_API_MEMSET( l_host[l_count], 0, sizeof( struct hostent ) );
			l_host[l_count]->h_length = server->h_length;
			l_host[l_count]->h_name = XP_API_STRNEW( server_name );
			l_host[l_count]->h_addr_list = XP_API_NEW char * [1];
			for (int i=0; i<1; i++ )
			{
				l_host[l_count]->h_addr_list[i] = 	XP_API_STRNEW( server->h_addr_list[i] );
			}
			l_count++;
			XP_DEBUG_OUT( "GetHostByName() - Add %s \n", server_name );
		}
		
		return server;	
	}
	
	void CIPhoneSocket::TransToIp(char * &name)
	{
		struct hostent *host = (struct hostent *)GetHostIP( name );
		XP_API_DEL( name );
		if (host == NULL)
		{
			// TODO: error
		}
		// use the first address here
		else
		{
			name = inet_ntoa (*(struct in_addr *)*host->h_addr_list);
		}
	}
	
	bool CIPhoneSocket::CreateSocket()
	{
		//initialize socket
		m_socket = socket( AF_INET, SOCK_STREAM, 0 );
		
		if ( m_socket < 0 ) 
		{
			XP_DEBUG_OUT( "CWin32Socket::CreateSocket() error [%d] \n", GetLastError() );
			m_state = XSOCKET_STATE_ERROR;
			return false;
		}	
		
		return true;
	}
	
	void * CIPhoneSocket::GetHostName( char ** name )
	{
		char szHostName[255];
		gethostname(szHostName, 255);
		
		*name = XP_API_STRNEW( szHostName );
		return NULL;
	}
	
	//void * CIPhoneSocket::GetHostByName( char *server_name )
	//{
	//	//struct hostent *server;
	//	//server = gethostbyname( server_name );
	//	//return server;
	//
	//
	//	for( int i=0; i<l_count; i++ )
	//	{
	//		if ( XP_API_STRCMP( server_name, l_host[i]->h_name ) == 0 )
	//		{
	//			XP_DEBUG_OUT( "GetHostByName() Existed %s \n", server_name );
	//			return l_host[i];
	//		}
	//	}
	//
	//	struct hostent *server;
	//	server = gethostbyname( server_name );
	//
	//	if(server == NULL)
	//	{
	//		return NULL;
	//	}
	//
	//	if ( l_count < HOST_BUF_LEN )
	//	{
	//		l_host[l_count] = XP_API_NEW struct hostent;
	//		XP_API_MEMSET( l_host[l_count], 0, sizeof( struct hostent ) );
	//		l_host[l_count]->h_length = server->h_length;
	//		l_host[l_count]->h_name = XP_API_STRNEW( server_name );
	//		l_host[l_count]->h_addr_list = XP_API_NEW char * [1];
	//		for (int i=0; i<1; i++ )
	//		{
	//			l_host[l_count]->h_addr_list[i] = 	XP_API_STRNEW( server->h_addr_list[i] );
	//		}
	//		l_count++;
	//		XP_DEBUG_OUT( "GetHostByName() - Add %s \n", server_name );
	//	}
	//
	//	return server;	
	//}
	
	void * CIPhoneSocket::GetHostByName( char *server_name )
	{
		if(!server_name || XP_API_STRLEN(server_name) > 1024)//avoid the server_name too long, maybe is invalid
			return NULL;
		for( int i=0; i<l_count; i++ )
		{
			//avoid the pointer is NULL
			if ( !l_host[i] || !l_host[i]->h_name )
			{
				continue;
			}
			if ( XP_API_STRCMP( server_name, l_host[i]->h_name ) == 0 )
			{
				XP_DEBUG_OUT( "GetHostByName() Existed %s \n", server_name );
				return l_host[i];
			}
		}
		
		if(m_gethostbynameState == GETHOSTBYNAME_STATE_INIT)
		{
			//start a new thread to get the host ip
			m_gethostbynametimeStart = XP_API_GET_TIME();
			
			int ret;
			
			m_getHostByNameThreadParam.flagAddress = &m_gethostbynameState;
			strlcpy(m_getHostByNameThreadParam.serverName, server_name, sizeof(m_getHostByNameThreadParam.serverName));//crash point
			
			ret = pthread_create(&m_getHostByNameThreadId, NULL, (void* (*)(void *))threadGetHostByNameProcess, (void *)&m_getHostByNameThreadParam);
			
			if ( !ret )
			{
				m_gethostbynameState = GETHOSTBYNAME_STATE_WAITING;
			}
		}
		
		return NULL;
		
	}
	
	void threadGetHostByNameProcess( void *param )
	{
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		
		PROCESSPARAM *p = (PROCESSPARAM*)param;
		*(p->flagAddress) = GLXPlayerSocket::GETHOSTBYNAME_STATE_WAITING;
		
		struct hostent *server;
		pthread_testcancel();
		server = gethostbyname( p->serverName );
		pthread_testcancel();
		
		if(server == NULL)
		{
			//*(p->flagAddress) = GLXPlayerSocket::GETHOSTBYNAME_STATE_INIT;
			return;
		}
		
		if ( l_count < HOST_BUF_LEN )
		{
			l_host[l_count] = XP_API_NEW struct hostent;
			XP_API_MEMSET( l_host[l_count], 0, sizeof( struct hostent ) );
			l_host[l_count]->h_length = server->h_length;
			l_host[l_count]->h_name = XP_API_STRNEW( p->serverName );
			l_host[l_count]->h_addr_list = XP_API_NEW char * [1];
			for (int i=0; i<1; i++ )
			{
				l_host[l_count]->h_addr_list[i] = 	XP_API_STRNEW( server->h_addr_list[i] );
			}
			l_count++;
			XP_DEBUG_OUT( "GetHostByName() - Add %s \n", p->serverName );
		}
		
		*(p->flagAddress) = GLXPlayerSocket::GETHOSTBYNAME_STATE_INIT;
		
		return;	
	}
	
	int CIPhoneSocket::GetLastError()
	{
		return errno;
	}
	
	bool CIPhoneSocket::SetNonBlocking()
	{
		long arg = fcntl( m_socket, F_GETFL, NULL);
		if( arg < 0 )
		{
			XP_DEBUG_OUT("CIPhoneSocket::SetNonBlocking() err = %d \n", GetLastError() );
			CloseSocket();
			m_state = XSOCKET_STATE_ERROR;
			return false;
		}
		
		arg |= O_NONBLOCK;
		
		arg = fcntl( m_socket, F_SETFL, arg );
		if(arg < 0)
		{
			XP_DEBUG_OUT( "CIPhoneSocket::SetNonBlocking() err = %d \n", GetLastError() );
			CloseSocket();
			m_state = XSOCKET_STATE_ERROR;
			return false;
		}
		
		return true;
	}
	
	bool CIPhoneSocket::ConnectByName( char *server_name )
	{
		if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_START)
		{
			m_server_addr = GetHostByName( server_name );
			if ( NULL == m_server_addr )
			{
				if(XP_API_GET_TIME() - m_gethostbynametimeStart < GETHOSTBYNAME_TIMEOUT)
				{
					return false;
				}
				else
				{
					XP_DEBUG_OUT( "CIPhoneSocket::ConnectByName(): GetHostByName() err = %d \n", GetLastError() );
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			
			//create a sockaddr_in structure for the connect
			struct sockaddr_in serv_addr;
			
			XP_API_MEMSET((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			XP_API_MEMCPY((char *)&serv_addr.sin_addr.s_addr,
						  (char *)((struct hostent *)m_server_addr)->h_addr, 
						  ((struct hostent *)m_server_addr)->h_length);
			serv_addr.sin_port = htons(m_server_port);
			
			if(!SetNonBlocking())
			{
				XP_DEBUG_OUT( "CIPhoneSocket::ConnectByName(): SetNonBlocking() err : can not set unblock!!!\n" );
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			
			//attempt connection to server
			int iResult = connect( m_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );
			
			if(iResult < 0)
			{
				// connection error
				if( GetLastError() != EINPROGRESS )
				{
					XP_DEBUG_OUT( "CIPhoneSocket::ConnectByName: connect() err = %d \n", GetLastError() );
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			
			m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_WAITING;
			m_connectTimeStart = XP_API_GET_TIME();
			return false;
		}
		else if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_WAITING)
		{
			int iResult = Select(XSOCKET_SELECT_WRITE);
			
			if(iResult < 0)
			{
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			else if(iResult == 0)
			{
				if(XP_API_GET_TIME() - m_connectTimeStart > NONBLOCKING_CONNECT_TIMEOUT)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			else if(iResult > 0) // ready for write -> connect is finished
			{
				int valopt; 
				socklen_t lon; 
				
				// check pending errors
				lon = sizeof(int);
				iResult = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
				
				if(iResult < 0)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				if(valopt)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				// remove non blocking flag
				{
					long arg = fcntl(m_socket, F_GETFL, NULL);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
					
					arg &= ~O_NONBLOCK;
					
					arg = fcntl(m_socket, F_SETFL, arg);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
				}
				
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_SUCCESS;
				return true;
			}
		}
		
		return false;
	}
	
	bool CIPhoneSocket::Connect()
	{
		if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_START)
		{
			//create a sockaddr_in structure for the connect
			struct sockaddr_in serv_addr;
			
			XP_API_MEMSET((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			XP_API_MEMCPY((char *)&serv_addr.sin_addr.s_addr,
						  (char *)((struct hostent *)m_server_addr)->h_addr, 
						  ((struct hostent *)m_server_addr)->h_length);
			serv_addr.sin_port = htons(m_server_port);
			
			if(!SetNonBlocking())
			{
				XP_DEBUG_OUT( "CIPhoneSocket::Connect() err : can not set unblock!!!\n" );
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			
			//attempt connection to server
			int iResult = connect( m_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );
			
			if(iResult < 0)
			{
				// connection error
				if( GetLastError() != EINPROGRESS )
				{
					XP_DEBUG_OUT( "CIPhoneSocket::Connect() err = %d \n", GetLastError() );
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			
			m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_WAITING;
			m_connectTimeStart = XP_API_GET_TIME();
			return false;
		}
		else if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_WAITING)
		{
			int iResult = Select(XSOCKET_SELECT_WRITE);
			
			if(iResult < 0)
			{
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			else if(iResult == 0)
			{
				if(XP_API_GET_TIME() - m_connectTimeStart > NONBLOCKING_CONNECT_TIMEOUT)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			else if(iResult > 0) // ready for write -> connect is finished
			{
				int valopt; 
				socklen_t lon; 
				
				// check pending errors
				lon = sizeof(int);
				iResult = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
				
				if(iResult < 0)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				if(valopt)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				// remove non blocking flag
				{
					long arg = fcntl(m_socket, F_GETFL, NULL);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
					
					arg &= ~O_NONBLOCK;
					
					arg = fcntl(m_socket, F_SETFL, arg);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
				}
				
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_SUCCESS;
				return true;
			}
		}
		
		return false;
	}
	bool CIPhoneSocket::ConnectTo( char * ip, int port )
	{
		if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_START)
		{
			struct sockaddr_in serv_addr;
			
			XP_API_MEMSET((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = inet_addr( ip );
			serv_addr.sin_port = htons( port );
			
			if(!SetNonBlocking())
			{
				XP_DEBUG_OUT( "CIPhoneSocket::ConnectTo() err : can not set unblock!!!\n" );
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			
			int iResult = connect( m_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );
			
			if( iResult < 0 )
			{
				// connection error
				if( GetLastError() != EINPROGRESS )
				{
					XP_DEBUG_OUT( "CIPhoneSocket::Connect() err = %d \n", GetLastError() );
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
			}
			
			m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_WAITING;
			return false;
		}
		else if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_WAITING)
		{
			int iResult = Select(XSOCKET_SELECT_WRITE);
			
			if(iResult < 0)
			{
				CloseSocket();
				m_state = XSOCKET_STATE_ERROR;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
				return false;
			}
			else if(iResult > 0) // ready for write -> connect is finished
			{
				int valopt; 
				socklen_t lon; 
				
				// check pending errors
				lon = sizeof(int);
				iResult = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
				
				if(iResult < 0)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				if(valopt)
				{
					CloseSocket();
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
					return false;
				}
				
				// remove non blocking flag
				{
					long arg = fcntl(m_socket, F_GETFL, NULL);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
					
					arg &= ~O_NONBLOCK;
					
					arg = fcntl(m_socket, F_SETFL, arg);
					if(arg < 0)
					{
						CloseSocket();
						m_state = XSOCKET_STATE_ERROR;
						m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_FAILED;
						return false;
					}
				}
				
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_SUCCESS;
				return true;
			}
		}
		
		return false;
	}
	
	bool CIPhoneSocket::CloseSocket()
	{
		if( m_socket >= 0 ) 
		{ 
			XP_DEBUG_OUT( "CloseSocket() [%d]\n", m_socket );
			close( m_socket ); 
			m_socket = -1;
		}
		
		return true;
	}
	
	int CIPhoneSocket::Select( XSocketSelectFlag flag )
	{
		struct timeval tval = { 0, 0 };
		
		if ( m_socket < 0 )
		{
			return -1;
		}
		
		FD_ZERO( &m_set );
		FD_SET( m_socket, &m_set );
		
		int iResult = -1;
		
		if ( XSOCKET_SELECT_READ == flag )
		{
			iResult = select( m_socket + 1, &m_set, NULL, NULL, &tval);
		}
		else if ( XSOCKET_SELECT_WRITE == flag )
		{
			iResult = select( m_socket + 1, NULL, &m_set, NULL, &tval);
		}
		
		return iResult;
	}
	
	int CIPhoneSocket::Send( char * data, int size )
	{
		//return send( m_socket, data, size, 0 );
		long sendTime = 0;	
		sendTime = XP_API_GET_TIME();
		
		int iRet = send( m_socket, data, size, 0 );
		
		if ( iRet < ONLINE_MAX_PACKET_SIZE )
		{
			char log[ONLINE_MAX_PACKET_SIZE];
			XP_API_MEMSET( log, 0, ONLINE_MAX_PACKET_SIZE );
			XP_API_MEMCPY( log, data, size );		
			XP_DEBUG_OUT( "CIPhoneSocket::Send() [time:%d]\n[%d] %s\n", sendTime, iRet, log );
		}
		else
		{
			XP_DEBUG_OUT( "CIPhoneSocket::Send() [time:%d]\n[%d]\n", sendTime, iRet );
		}
		
		return iRet;
	}
	
	int CIPhoneSocket::Recv( char * data, int size )
	{
		//return recv( m_socket, data, size, 0 );
		
		long recvTime = 0;
		
		int iRet = recv( m_socket, data, size, 0 );
		recvTime = XP_API_GET_TIME();
		
		if ( iRet >= 0 && iRet < ONLINE_MAX_PACKET_SIZE - 100)
		{
			XP_DEBUG_OUT( "CWin32Socket::Recv() [time:%d]\n[%d] %s\n", recvTime, iRet, data );
		}
		
		return iRet;
	}
	
	bool CIPhoneSocket::IsReadyForRW(  )
	{
		return ( FD_ISSET( m_socket, &m_set ) > 0 );
	}
	
	/**
	 *
	 *
	 */
	int CIPhoneSocket::Bind( char * ip, unsigned short port )
	{
		sockaddr_in addr = { 0 };
		
		addr.sin_family = AF_INET;
		if ( NULL == ip )
		{
			addr.sin_addr.s_addr = htonl( INADDR_ANY );
		}
		else
		{
			addr.sin_port = inet_addr( ip );
		}
		
		addr.sin_port = htons( port );
		
		int bOptVal = 1;
		setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, sizeof(int));
		
		int iResult = bind( m_socket, ( sockaddr* )&addr, sizeof( sockaddr ) );
		int iTry = 0;
		
		while ( iResult < 0 )
		{
			iTry++;
			addr.sin_port = htons( port + iTry );
			iResult = bind( m_socket, ( sockaddr* )&addr, sizeof( sockaddr ) );
		}
		
		return ( port + iTry );
	}
	
	bool CIPhoneSocket::CreateTcpSocket()
	{
		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		
		if ( m_socket < 0 ) 
		{
			return false;
		}
		
		m_type = XSOCKET_TYPE_TCP;
		return true;
	}
	
	bool CIPhoneSocket::CreateUdpSocket()
	{
		m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		
		if ( m_socket < 0 ) 
		{
			return false;
		}
		
		m_type = XSOCKET_TYPE_UDP;
		return true;
	}
	
	/**
	 * Send data to (ip:port) with udp protocol.
	 * <p>
	 * @param buffer: the data to send
	 *        size:   the size of the data
	 * @return:       the size to send
	 *
	 */
	int CIPhoneSocket::SendTo( const char * buffer, int size, const char * ip, int port )
	{
		sockaddr_in remoteAddr = { 0 };
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_addr.s_addr = inet_addr( ip );
		remoteAddr.sin_port = htons( port );
		
		int iRet = sendto( m_socket, buffer, size, 0, ( sockaddr * )&remoteAddr, sizeof( remoteAddr ) );
		
		XP_DEBUG_OUT( "CIPhoneSocket::SendTo()\n [%s:%d] [%d] %s\n", ip, port, iRet, buffer );
		
		return iRet;
	}
	
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
	int CIPhoneSocket::RecvFrom( char * buffer, int size, const char * ip, int port )
	{
		sockaddr_in remoteAddr = { 0 };
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_addr.s_addr = inet_addr( ip );
		remoteAddr.sin_port = htons( port );
		int sinlen = sizeof( remoteAddr );
		
		int iRet = recvfrom( m_socket, buffer, size, 0, (struct sockaddr *)&remoteAddr, 
							(socklen_t *)&sinlen );
		
		if ( iRet > 0 )
		{
			XP_DEBUG_OUT( "CIPhoneSocket::RecvFrom()\n [%s:%d] [%d] %s\n", ip, port, iRet, buffer );
		}
		
		if ( iRet < 0 )
		{
			XP_DEBUG_OUT( "CIPhoneSocket::RecvFrom() [%s:%d] last error = [%d]\n", ip, port, GetLastError() );
		}
		return iRet;
	}
	
	int CIPhoneSocket::RecvFromUnkownIP( char * buffer, int size, char ** ip, int * port )
	{
		sockaddr_in remoteAddr = { 0 };
		remoteAddr.sin_family = AF_INET;
		int sinlen = sizeof( remoteAddr );
		
		int iRet = recvfrom( m_socket, buffer, size, 0, (struct sockaddr *)&remoteAddr, 
							(socklen_t *)&sinlen );
		
		*ip = XP_API_STRNEW( inet_ntoa( remoteAddr.sin_addr ) );
		*port = ntohs( remoteAddr.sin_port );
		
		return iRet;
	}
	char * CIPhoneSocket::GetSocketAddr()
	{
		char local_name[256];
		
		XP_API_MEMSET( local_name, 0, 256 );
		
		if( gethostname( local_name, sizeof(local_name) ) != 0 )
		{
			return NULL;
		}
		
		struct hostent *hostinfo = gethostbyname( local_name );
		
		if ( NULL == hostinfo )
		{
			return NULL;
		}
		
		return inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
	}
	
	unsigned short CIPhoneSocket::GetSocketPort()
	{
		sockaddr_in localAddr = { 0 };
		int nLen = sizeof( localAddr );
		getsockname( m_socket, (struct sockaddr *)&localAddr, (socklen_t *)&nLen );
		return ntohs( localAddr.sin_port );
	}
	
	
	void CIPhoneSocket::Listen( int count )
	{
		int iResult = listen( m_socket, count );
		
		if ( iResult < 0)
		{
			XP_DEBUG_OUT( "Listen failed!\n" );
		}	
	}
	
	GLXPlayerSocket * CIPhoneSocket::Accept()
	{
		struct sockaddr_in addr;
		int len;  
		addr.sin_family = AF_INET;  
		len = sizeof( struct sockaddr );
		int s = accept( m_socket, (struct sockaddr*)&addr, (socklen_t *)&len );
		
		char * ip = inet_ntoa( addr.sin_addr );
		int port = ntohs( addr.sin_port );
		
		GLXPlayerSocket * soc = XP_API_NEW CIPhoneSocket();
		soc->SetSocket( s );
		soc->SetAcceptIP( ip );
		soc->SetAcceptPort( port );
		soc->SetSocketType( XSOCKET_TYPE_TCP );
		
		return soc;
	}
	
	void CIPhoneSocket::SetBroadcast()
	{
		int bBroadcast = 1;
		int result = setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bBroadcast, sizeof(int));
		if(result < 0)
		{
			XP_DEBUG_OUT( "CIPhoneSocket::SetBroadcast() last error = [%d]\n", GetLastError() );
		}
	}
	
	int CIPhoneSocket::SendBroadcast(const char * buffer, int size, int port)
	{
		sockaddr_in bcast = {0};
		
		bcast.sin_family = AF_INET;
		bcast.sin_addr.s_addr = htonl( INADDR_BROADCAST );
		bcast.sin_port = htons(port);
		
		int iRet = sendto( m_socket, buffer, size, 0, ( sockaddr * )&bcast, sizeof( bcast ) );
		
		if ( iRet > 0 )
		{
			XP_DEBUG_OUT( "CIPhoneSocket::SendBroadcast() [%d] [%d] %s\n", port, iRet, buffer );
		}
		else
		{
			XP_DEBUG_OUT( "CIPhoneSocket::SendBroadcast() [%d] last error = [%d]\n", port, GetLastError() );
		}
		
		return iRet;
	}
}

#endif // OS_IPHONE
