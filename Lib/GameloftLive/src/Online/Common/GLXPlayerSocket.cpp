#include "GLXPlayerSocket.h"
#include "Porting.h"
#include "Memory.h"  //byhy for debug memory leak

GLXPlayerSocket::GLXPlayerSocket( char * server_name, int server_port, GLXPlayerSocketObserver * observer )
	:m_server_name( server_name ),m_server_port(server_port),m_observer( observer )
{
	m_server_addr = NULL;
	m_socket = -1;
	m_accept_ip = NULL;
	m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;
	m_gethostbynameState = GETHOSTBYNAME_STATE_INIT;
	m_connectTimeStart = 0;
	m_gethostbynametimeStart = 0;
}

GLXPlayerSocket::~GLXPlayerSocket(void)
{
	XP_API_DELETE_ARRAY( m_accept_ip );
}
//
//bool XSocket::GetLocalIP(char * szLocalIP)
//{
//	return false;
//}

char * GLXPlayerSocket::GetAcceptIP()
{
	return m_accept_ip;
}

int GLXPlayerSocket::GetAcceptPort()
{
	return m_accept_port;
}

void GLXPlayerSocket::SetAcceptIP( char * ip )
{
	m_accept_ip = XP_API_STRNEW( ip );
}

void GLXPlayerSocket::SetAcceptPort( int port )
{
	m_accept_port = port;
}

void GLXPlayerSocket::SetSocket( int soc )
{
	m_socket = soc;	
}

int GLXPlayerSocket::GetSocketType()
{
	return m_type;
}

void GLXPlayerSocket::SetSocketType( int type )
{
	m_type = ( XSocketType ) type;
}

void GLXPlayerSocket::Start()
{
	m_nReceived = 0;
	XP_API_MEMSET( m_abBuffer, 0, RECV_ONCE_BUFFER_SIZE );
	m_state = XSOCKET_STATE_START_CONNECT;
	m_server_addr = NULL;
	m_socket = -1;
}

void GLXPlayerSocket::Run()
{
	if( m_state == XSOCKET_STATE_NONE || 
		m_state == XSOCKET_STATE_ERROR || 
		m_state == XSOCKET_STATE_CANCELLED )
	{
		XP_DEBUG_OUT("GLXPlayerSocket::Run(): m_state is: %d, quit Run()!!", m_state);
		return;
	}

	switch( m_state )
	{
		case XSOCKET_STATE_START_CONNECT:
		{
			if ( NULL == m_server_addr )
			{
				m_server_addr = GetHostByName( m_server_name );
				if ( NULL == m_server_addr )
				{
					if(XP_API_GET_TIME() - m_gethostbynametimeStart < GETHOSTBYNAME_TIMEOUT)
					{
						return;
					}
					else
					{
						XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
						m_state	= XSOCKET_STATE_ERROR;
						return;
					}
				}				
			}

			if ( !CreateSocket() )
			{
				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
				return;
			}
/*
			if ( !SetNonBlocking() )
			{
				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
				return;				
			}
*/
			//if( !Connect() )
			//{
			//	XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
			//	return;
			//}

			m_state = XSOCKET_STATE_WAIT_CONNECTING;
		}
		break;

		case XSOCKET_STATE_WAIT_CONNECTING:
		{
			if( !Connect() )
			{
				if(m_nonBlockConnectState == NONBLOCK_CONNECT_STATE_WAITING)
				{
					XP_DEBUG_OUT( "XSocket::Run(): Connecting... \n" );
					break;
				}
				else
				{
					XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
					CloseSocket( );
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;
					return;
				}
			}
			else
			{
				XP_DEBUG_OUT( "XSocket::Run(): Connected \n" );
				m_state = XSOCKET_STATE_SEND_REQUEST;
				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;
			}

//			int iResult = Select( XSOCKET_SELECT_WRITE );
//#ifdef OS_WIN32
//			if( iResult < 0 )
//			{
//				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
//				CloseSocket( );
//				m_state = XSOCKET_STATE_ERROR;
//				return;
//			}
//#endif
//			if( iResult > 0 )	// ready for write -> connect is finished
//			{
//				m_state = XSOCKET_STATE_SEND_REQUEST;
//			}
//			else					// waiting
//			{
//				;
//			}
		}
		break;

		case XSOCKET_STATE_SEND_REQUEST:
		{
			//sending a message to a server
			int iResult = 0;
			iResult = Send( m_observer->GetRequestData(), m_observer->GetRequestLength() );

			if ( iResult < 0 )
			{
				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
				CloseSocket( );
				m_state = XSOCKET_STATE_ERROR;
				return;
			}

			XP_DEBUG_OUT( "XSocket::Run(): Send successful. \n" );
			m_nReceived = 0;
			m_buffer = "";
			m_buffer_used = false;
			m_bReceiveHttpHeader = true;

			m_state = XSOCKET_STATE_WAIT_RESPONSE;
		}
		break;

		case XSOCKET_STATE_WAIT_RESPONSE:
		{
			int iResult = Select( XSOCKET_SELECT_READ );
#ifdef OS_WIN32
			if( iResult < 0 )
			{
				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
				CloseSocket( );
				m_state = XSOCKET_STATE_ERROR;
				return;
			}
#endif
			if( iResult > 0 )
			{
				XP_DEBUG_OUT( "XSocket::Run(): Have someting to receive. \n" );
				XP_API_MEMSET( m_abBuffer, 0, RECV_ONCE_BUFFER_SIZE );
				iResult = Recv( m_abBuffer, RECV_ONCE_BUFFER_SIZE );

				if( iResult <= 0 )
				{
					XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
					CloseSocket( );
					m_state = XSOCKET_STATE_ERROR;
					return;
				}

				std::string data_once = m_abBuffer;
				m_buffer.append( data_once );

				if ( m_bReceiveHttpHeader )
				{
					m_total_length = CalculateTotalLength();
					RemoveHttpHeader();
					m_bReceiveHttpHeader = false;
				}

				if ( m_buffer.length() == m_total_length )
				{
					m_state = XSOCKET_STATE_CLOSE_CONNECTION;
				}
			}
		}
		break;

		case XSOCKET_STATE_CLOSE_CONNECTION:
		{
			XP_DEBUG_OUT( "XSocket::Run(): Close connection \n" );
			CloseSocket();
			m_observer->SetResponseData( (char *)m_buffer.c_str(), m_buffer.length() );
			m_buffer = "";
			m_state = XSOCKET_STATE_REQUEST_COMPLETE;
		}
		break;

		default:
			break;
	}	
}

void GLXPlayerSocket::RemoveHttpHeader()
{
	int pos = m_buffer.find("\r\n\r\n");
	if ( pos > 0 )
	{
		m_buffer = m_buffer.substr( pos + XP_API_STRLEN( "\r\n\r\n" ) );
	}
}

int GLXPlayerSocket::CalculateTotalLength()
{
	char CONTENT_LENGTH[] = "\r\nContent-Length:";

	int pos = m_buffer.find("\r\n\r\n");
	int length = 0;

	if ( pos < 0 )
	{
		return -1;
	}

	std::string header = m_buffer.substr( 0, pos );
	pos = header.find( CONTENT_LENGTH );
	if ( pos < 0 )
	{
		
	}
	else
	{
		std::string str_length = header.substr( pos + XP_API_STRLEN( CONTENT_LENGTH ) );
		pos = str_length.find( "\r\n" );
		if ( pos < 0 )
		{
			return -1;
		}
		str_length = str_length.substr( 0, pos );
		length = XP_API_ATOI( str_length.c_str() );
	}

	return length;
}

void GLXPlayerSocket::Cancel()
{
	if( m_state == XSOCKET_STATE_NONE || 
		m_state == XSOCKET_STATE_ERROR ||
		m_state == XSOCKET_STATE_REQUEST_COMPLETE )
	{
		return;
	}

	m_state = XSOCKET_STATE_CANCELLED;

	CloseSocket( );
}

bool GLXPlayerSocket::IsInProgress()
{
	if( m_state == XSOCKET_STATE_NONE || 
		m_state == XSOCKET_STATE_ERROR ||
		m_state == XSOCKET_STATE_CANCELLED ||
		m_state == XSOCKET_STATE_REQUEST_COMPLETE )
	{
		return false;
	}

	return true;	
}

bool GLXPlayerSocket::IsErrorOccurred()
{
	return m_state == XSOCKET_STATE_ERROR;
}