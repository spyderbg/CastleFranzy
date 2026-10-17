#include "Win32Socket.h"
#include "XPlayer_defines.h"

#pragma comment( lib, "WS2_32.lib" )

const int HOST_BUF_LEN = 4;

struct hostent * l_host[HOST_BUF_LEN];
int l_count = 0;

bool g_output_ga_infor = true;

void CWin32Socket::Startup()
{
	WSADATA wsaData;

	// Initialize Winsock.
	WSAStartup( MAKEWORD(2,2), &wsaData );

	for ( int i=0; i<HOST_BUF_LEN; i++ )
	{
		l_host[i] = NULL;
	}
}

void CWin32Socket::Clearup()
{
	WSACleanup();

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

CWin32Socket::CWin32Socket( char * server_name, int server_port, GLXPlayerSocketObserver * observer )
	: GLXPlayerSocket( server_name, server_port, observer )
{
	m_state = XSOCKET_STATE_NONE;
}

CWin32Socket::~CWin32Socket(void)
{
	CloseSocket();
}

bool CWin32Socket::GetLocalIP(char * szLocalIP)
{
	// Get the local hostname
	char szHostName[255];
	gethostname(szHostName, 255);
	
	struct hostent *host_entry;
	host_entry = gethostbyname(szHostName);
	
	char* localIP = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
	
	strcpy(szLocalIP, localIP);
	return true;	
}

bool CWin32Socket::GetServerIP(char * serverName, char *serverIP)
{	
	if(serverName == NULL || serverIP == NULL)
		return false;

	struct hostent *host_entry = (struct hostent *)GetHostByName( serverName );
	
	char* tmpIP = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
	
	strcpy(serverIP, tmpIP);
	return true;	
}

bool CWin32Socket::CreateSocket()
{
	//initialize socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket < 0 ) 
	{
		XP_DEBUG_OUT( "CWin32Socket::CreateSocket() error [%d] \n", GetLastError() );
		m_state = XSOCKET_STATE_ERROR;
		return false;
	}

	XP_DEBUG_OUT( "CWin32Socket::CreateSocket( %d ) \n", m_socket );
	return true;
}

void CWin32Socket::Listen( int count )
{
	if ( listen( m_socket, count ) != 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::Listen() error [%d] \n", GetLastError() );
	}
}

GLXPlayerSocket * CWin32Socket::Accept()
{
	struct sockaddr_in addr;
	int len;  
    addr.sin_family = AF_INET;  
    len = sizeof( struct sockaddr );
	SOCKET s = accept( m_socket, (struct sockaddr*)&addr, &len );

	char * ip = inet_ntoa( addr.sin_addr );
	int port = ntohs( addr.sin_port );

	GLXPlayerSocket * soc = XP_API_NEW CWin32Socket();
	soc->SetSocket( s );
	soc->SetAcceptIP( ip );
	soc->SetAcceptPort( port );
	soc->SetSocketType( XSOCKET_TYPE_TCP );

	return soc;
}

void * CWin32Socket::GetHostName( char ** name )
{
	char szHostName[255];
	gethostname(szHostName, 255);

	*name = XP_API_STRNEW( szHostName );
	return name;
}

void * CWin32Socket::GetHostByName( char *server_name )
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

	if ( NULL == server )
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

int CWin32Socket::GetLastError()
{
	return WSAGetLastError();
}

bool CWin32Socket::SetNonBlocking()
{
	int timeout = 30000;
	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char   *)&timeout, sizeof(timeout));
	return true;
}

bool CWin32Socket::ConnectByName( char *server_name )
{
	m_server_addr = GetHostByName( server_name );
	if ( NULL == m_server_addr )
	{
		return false;
	}

	return Connect();
}

bool CWin32Socket::Connect()
{
	//create a sockaddr_in structure for the connect
	struct sockaddr_in serv_addr;
			
	XP_API_MEMSET((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	XP_API_MEMCPY((char *)&serv_addr.sin_addr.s_addr,
		(char *)((struct hostent *)m_server_addr)->h_addr,
		((struct hostent *)m_server_addr)->h_length);
	serv_addr.sin_port = htons(m_server_port);
			
	//attempt connection to server
	int iResult = connect( m_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );

	if(iResult < 0)
	{
		XP_DEBUG_OUT( "CWin32Socket::Connect() err = %d \n", GetLastError() );
		CloseSocket();
		m_state = XSOCKET_STATE_ERROR;
		return false;
	}

	return true;
}

bool CWin32Socket::ConnectTo( char * ip, int port )
{
	struct sockaddr_in serv_addr;
			
	XP_API_MEMSET((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr = inet_addr( ip );
	serv_addr.sin_port = htons( port );

	int iResult = connect( m_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );

	if( iResult < 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::ConnectTo(%s:%d) error [%d] \n", ip,port,GetLastError() );
		CloseSocket();
		m_state = XSOCKET_STATE_ERROR;
		return false;
	}

	return true;
}

bool CWin32Socket::CloseSocket()
{
	if( m_socket >= 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::CloseSocket( %d ) \n", m_socket );
		closesocket( m_socket ); 
		m_socket = -1; 
	}

	return true;
}

int CWin32Socket::Select( XSocketSelectFlag flag )
{
	int value = 0;
	struct timeval tval = { 0, 0 };
	 
	FD_ZERO( &m_set );
	FD_SET( m_socket, &m_set );

	if ( XSOCKET_SELECT_READ == flag )
	{
		value = select( m_socket + 1, &m_set, NULL, NULL, &tval);
	}
	else if ( XSOCKET_SELECT_WRITE == flag )
	{
		//value = select( m_socket + 1, NULL, &m_set, NULL, &tval);
		value = 1;
	}
	else
	{
		value = -1;
	}

	if ( value > 0 )
	{
//		XP_DEBUG_OUT( "CWin32Socket::Select() = [%d] \n", value );
	}

	if ( FD_ISSET( m_socket, &m_set ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}

//	return value;
}

bool CWin32Socket::IsReadyForRW()
{
	return ( FD_ISSET( m_socket, &m_set ) > 0 );
}

int CWin32Socket::Send( char * data, int size )
{
	char log[ONLINE_MAX_PACKET_SIZE];
	long sendTime = 0;

	int iRet = send( m_socket, data, size, 0 );
	if ( size < ONLINE_MAX_PACKET_SIZE )
	{
		sendTime = XP_API_GET_TIME();
		XP_API_MEMSET( log, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_MEMCPY( log, data, size );

		if ( g_output_ga_infor )
		{
			XP_DEBUG_OUT( "CWin32Socket::Send() [time:%d]\n[%d] %s\n", sendTime, iRet, log );
		}
		else
		{
			if ( XP_API_STRNCMP( data+1, "ga", 2 ) != 0 )
			{
				XP_DEBUG_OUT( "CWin32Socket::Send() [time:%d]\n[%d] %s\n", sendTime, iRet, log );
			}
		}
	}
	else
	{
		
	}

	return iRet;
}

int CWin32Socket::Recv( char * data, int size )
{
	long recvTime = 0;

	int iRet = recv( m_socket, data, size, 0 );
	recvTime = XP_API_GET_TIME();

//	int iRet = 0;
//	if ( recv( m_socket, data, size, MSG_PEEK ) )
//	{
//		iRet = recv( m_socket, data, size, 0 );
//	}

	if ( iRet >= 0 )
	{
		if ( g_output_ga_infor && ( iRet < ONLINE_MAX_PACKET_SIZE ) )
		{
			XP_DEBUG_OUT( "CWin32Socket::Recv() [time:%d]\n[%d] %s\n", recvTime, iRet, data );
		}
		else
		{
			if ( XP_API_STRNCMP( data+1, "ga", 2 ) != 0 && ( iRet < ONLINE_MAX_PACKET_SIZE ) )
			{
				XP_DEBUG_OUT( "CWin32Socket::Recv() [time:%d]\n[%d] %s\n", recvTime, iRet, data );
			}
		}
	}
	//else
	//{
	//	XP_DEBUG_OUT( "CWin32Socket::Recv()\n error [%d]\n", GetLastError() );
	//}
	
	return iRet;
}

/**
 *
 *
 */
int CWin32Socket::Bind( char * ip, unsigned short port )
{
	SOCKADDR_IN addr = { 0 };

	addr.sin_family = AF_INET;
	if ( NULL == ip )
	{
		addr.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
	}
	else
	{
		addr.sin_addr.S_un.S_addr = inet_addr( ip );
	}
	
	addr.sin_port = htons( port );

	int iRet = bind( m_socket, ( SOCKADDR* )&addr, sizeof( SOCKADDR ) );
	int iTry = 0;

	while( iRet != 0 && iTry < 100 )
	{
		iTry++;
		addr.sin_port = htons( port + iTry );
		iRet = bind( m_socket, ( SOCKADDR* )&addr, sizeof( SOCKADDR ) );	
	}

	return ( port + iTry );
}

bool CWin32Socket::CreateTcpSocket()
{
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket < 0 ) 
	{
		return false;
	}

	XP_DEBUG_OUT( "CWin32Socket::CreateTcpSocket( %d ) \n", m_socket );
	m_type = XSOCKET_TYPE_TCP;
	return true;
}

bool CWin32Socket::CreateUdpSocket()
{
	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( m_socket < 0 ) 
	{
		return false;
	}

	XP_DEBUG_OUT( "CWin32Socket::CreateUdpSocket( %d ) \n", m_socket );
	m_type = XSOCKET_TYPE_UDP;

//
//	int opt = 1;
//	setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof (opt));

	return true;
}

char * CWin32Socket::GetSocketAddr()
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

unsigned short CWin32Socket::GetSocketPort()
{
	SOCKADDR_IN localAddr = { 0 };
	int nLen = sizeof( localAddr );
	getsockname( m_socket, ( PSOCKADDR )&localAddr, &nLen );
	return ntohs( localAddr.sin_port );
}

/**
 * Send data to (ip:port) with udp protocol.
 * <p>
 * @param buffer: the data to send
 *        size:   the size of the data
 * @return:       the size to send
 *
 */
int CWin32Socket::SendTo( const char * buffer, int size, const char * ip, int port )
{
	SOCKADDR_IN remoteAddr = { 0 };
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = inet_addr( ip );
	remoteAddr.sin_port = htons( port );

	int iRet = sendto( m_socket, buffer, size, 0, ( PSOCKADDR )&remoteAddr, sizeof( remoteAddr ) );

	if ( iRet > 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::SendTo() [%s:%d] [%d] %s\n", ip, port, iRet, buffer );
	}
	
	if ( SOCKET_ERROR == iRet )
	{
		XP_DEBUG_OUT( "CWin32Socket::SendTo() [%s:%d] last error = [%d]\n", ip, port, GetLastError() );
	}
	
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
int CWin32Socket::RecvFrom( char * buffer, int size, const char * ip, int port )
{
	SOCKADDR_IN remoteAddr = { 0 };
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_addr.s_addr = inet_addr( ip );
	remoteAddr.sin_port = htons( port );
	int sinlen = sizeof( remoteAddr );

	int iRet = recvfrom( m_socket, buffer, size, 0, ( PSOCKADDR )&remoteAddr, &sinlen );
	
	if ( iRet > 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::RecvFrom()\n [%s:%d] [%d] %s\n", ip, port, iRet, buffer );
	}
	
	if ( SOCKET_ERROR == iRet )
	{
		XP_DEBUG_OUT( "CWin32Socket::RecvFrom() [%s:%d] last error = [%d]\n", ip, port, GetLastError() );
	}
	return iRet;
}

int CWin32Socket::RecvFromUnkownIP( char * buffer, int size, char ** ip, int * port )
{
	SOCKADDR_IN remoteAddr = { 0 };
	remoteAddr.sin_family = AF_INET;
	int sinlen = sizeof( remoteAddr );
	int iRet = recvfrom( m_socket, buffer, size, 0, ( PSOCKADDR )&remoteAddr, &sinlen );	

	if ( iRet > 0 )
	{
		*ip = XP_API_STRNEW( inet_ntoa( remoteAddr.sin_addr ) );
		*port = ntohs( remoteAddr.sin_port );
		XP_DEBUG_OUT( "CWin32Socket::RecvFromUnknownIP() [%s:%d] [%s]\n", *ip, *port, buffer );
	}

	return iRet;
}

void CWin32Socket::SetBroadcast()
{
	bool bBroadcast = true;
	setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(bool));
}

int CWin32Socket::SendBroadcast(const char * buffer, int size, int port)
{
	SOCKADDR_IN bcast;

	bcast.sin_family = AF_INET;
	bcast.sin_addr.s_addr = INADDR_BROADCAST;
	bcast.sin_port = htons(port);

	int iRet = sendto( m_socket, buffer, size, 0, ( PSOCKADDR )&bcast, sizeof( bcast ) );

	if ( iRet > 0 )
	{
		XP_DEBUG_OUT( "CWin32Socket::SendBroadcast() [%d] [%d] %s\n", port, iRet, buffer );
	}
	
	if ( SOCKET_ERROR == iRet )
	{
		XP_DEBUG_OUT( "CWin32Socket::SendBroadcast() [%d] last error = [%d]\n", port, GetLastError() );
	}
	
	return iRet;
}