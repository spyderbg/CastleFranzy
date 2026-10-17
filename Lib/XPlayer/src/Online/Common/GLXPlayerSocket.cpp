#include "../../../include/Common/GLXPlayerSocket.h"
#include "../../../include/Porting.h"
//#include "Memory.h"  //byhy for debug memory leak

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

	m_sentLen = 0;

	tmpSendStartTime = 0;
	tmpRecvStartTime = 0;
}

GLXPlayerSocket::~GLXPlayerSocket(void)
{
	XP_API_DEL( m_accept_ip );
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
		XP_DEBUG_OUT("GLXPlayerSocket::Run(): m_state is %d, quit Run()!!\n", m_state);
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
						m_gethostbynameState = GETHOSTBYNAME_STATE_INIT;
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
#ifdef ENABLE_HTTP_REQUEST_QUEUE
				if(!m_observer->GetRequest())
				{
					XP_DEBUG_OUT("XSocket::Run() The request queue is empty!\n");
					CloseSocket( );
					m_state = XSOCKET_STATE_ERROR;
					m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;
					return;
				}
#endif
				XP_DEBUG_OUT( "XSocket::Run(): Connected \n" );
				m_sentLen = 0;
				
				tmpSendStartTime = XP_API_GET_TIME();

				m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;
				m_state = XSOCKET_STATE_SEND_REQUEST;
			}
		}
		break;

		case XSOCKET_STATE_SEND_REQUEST:
		{
			int iResult = 0;
			int totleLen = 0;
			char *sendBuffer = NULL;

			int iselectResult = Select( XSOCKET_SELECT_WRITE );
			
			if(iselectResult<0)
			{
				XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
				CloseSocket( );
				m_state = XSOCKET_STATE_ERROR;
				return;
			}

			if(iselectResult>0)
			{
				//sending a message to a server
				totleLen = m_observer->GetRequestLength();
				sendBuffer = m_observer->GetRequestData();

				if(totleLen-m_sentLen>SEND_ONCE_BUFFER_SIZE)
				{
					iResult = Send( sendBuffer+m_sentLen, SEND_ONCE_BUFFER_SIZE );
				}
				else
				{
					iResult = Send( sendBuffer+m_sentLen, totleLen-m_sentLen );
				}

				if ( iResult < 0 )
				{
					XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
					CloseSocket( );
					m_state = XSOCKET_STATE_ERROR;
					return;
				}

				m_sentLen += iResult;

				if(m_sentLen == totleLen)
				{
					XP_DEBUG_OUT( "XSocket::Run(): Send successful. \n" );
					m_nReceived = 0;
					m_buffer = "";
					m_buffer_used = false;
					m_bReceiveHttpHeader = true;
					m_bLengthKnown = false;

					XP_DEBUG_OUT( "XSocket::Run(): The amount of sent is: %d\n", totleLen);
					XP_DEBUG_OUT( "XSocket::Run(): The time of sent is: %d\n", XP_API_GET_TIME()-tmpSendStartTime);
					
					tmpRecvStartTime = XP_API_GET_TIME();
					m_state = XSOCKET_STATE_WAIT_RESPONSE;
				}
			}
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
				m_observer->SetResponseLen(0); //get the length of file in download process
				return;
			}
#endif
			if( iResult > 0 )
			{
				XP_DEBUG_OUT( "XSocket::Run(): Have someting to receive. \n" );
				XP_API_MEMSET( m_abBuffer, 0, RECV_ONCE_BUFFER_SIZE );
				iResult = Recv( m_abBuffer, RECV_ONCE_BUFFER_SIZE );

				if( iResult < 0 )
				{
					XP_DEBUG_OUT( "XSocket::Run() err = %d \n", GetLastError() );
					CloseSocket( );
					m_state = XSOCKET_STATE_ERROR;
					m_observer->SetResponseLen(0); //get the length of file in download process
					return;
				}

				if( iResult == 0 ) //the server disconnect the connection
				{
					XP_DEBUG_OUT( "XSocket::Run(): Close connection \n" );
					if(!m_bLengthKnown)
					{
						ParseChunkedContent();
					}
					m_observer->SetResponseData( (char *)m_buffer.c_str(), m_buffer.length() );
					m_buffer = "";
					m_state = XSOCKET_STATE_REQUEST_COMPLETE;
					return;
				}

				//std::string data_once = m_abBuffer;
				//m_buffer.append( data_once );
				m_buffer.append( m_abBuffer, iResult );

				if(m_bReceiveHttpHeader)
				{
					if(m_buffer.find("\r\n\r\n") > 0)
					{
						if(ParseHttpHeader("Content-Length") >= 0)
						{
							m_total_length = CalculateTotalLength();
							m_bLengthKnown = true;
						}

						RemoveHttpHeader();
						m_bReceiveHttpHeader = false;
					}
					else
					{
						break;
					}
				}

				int len = m_buffer.length();
				m_observer->SetResponseLen(len); //get the length of file in download process

				if(m_bLengthKnown)
				{
					if ( len == m_total_length )
					{
						m_state = XSOCKET_STATE_CLOSE_CONNECTION;
						XP_DEBUG_OUT( "XSocket::Run(): The amount of recv is: %d\n", m_total_length);
						XP_DEBUG_OUT( "XSocket::Run(): The time of recv is: %d\n", XP_API_GET_TIME()-tmpRecvStartTime);
					}
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
#ifdef ENABLE_HTTP_REQUEST_QUEUE
			m_observer->PopRequest();
#endif
		}
		break;

		default:
			break;
	}	
}

int GLXPlayerSocket::ParseHttpHeader(char *content)
{
	int result = -1;
	
	int contentLen = XP_API_STRLEN(content);
	char *dest = XP_API_NEW char[contentLen+1];
	XP_API_MEMSET(dest, 0, contentLen+1);
	XP_API_TOLOWER(content, dest);

	int pos = m_buffer.find("\r\n\r\n");
	char *headSrc = XP_API_NEW char[pos+1];
	char *headDst = XP_API_NEW char[pos+1];
	XP_API_MEMSET(headSrc, 0, pos+1);
	XP_API_MEMSET(headDst, 0, pos+1);
	XP_API_MEMCPY(headSrc, m_buffer.c_str(), pos);
	XP_API_TOLOWER(headSrc, headDst);
	std::string head = headDst;

	result = head.find(dest);

	XP_API_DELETE_ARRAY(dest);
	XP_API_DELETE_ARRAY(headSrc);
	XP_API_DELETE_ARRAY(headDst);

	return result;
}

bool GLXPlayerSocket::ParseChunkedContent()
{
	std::string srcBuf = m_buffer;
	m_buffer = "";
	std::string chunkSizeBuf = "";
	std::string chunkDataBuf = "";
	int currentPos = 0;
	int posChunkSize = 0;
	int chunkSize = 0;
	
	XP_DEBUG_OUT("The http content is:\n%s\n", srcBuf.c_str());
	

	posChunkSize = srcBuf.find("\r\n"); //first chunk size line
	XP_DEBUG_OUT("The posChunkSize is: %d", posChunkSize);

	if(posChunkSize<=0)
	{
		return false;
	}
	
	chunkSizeBuf = srcBuf.substr(0, posChunkSize);
	
	for(int i=0; i<posChunkSize; ++i)
	{
		XP_DEBUG_OUT("The charactor is:\n%d\n", chunkSizeBuf.at(i));
	}
	
	int tmpPosChunkSize = chunkSizeBuf.find(" ");
	XP_DEBUG_OUT("The tmpPosChunkSize is: %d", tmpPosChunkSize);
	
	if(tmpPosChunkSize>0)
	{
		chunkSizeBuf = chunkSizeBuf.substr(0, tmpPosChunkSize);
		
		for(int i=0; i<tmpPosChunkSize; ++i)
		{
			XP_DEBUG_OUT("The charactor is:\n%d\n", chunkSizeBuf.at(i));
		}
	}

	//chunkSize = XP_API_ATOI(chunkSizeBuf.c_str());
	//chunkSize = StrToNum16(chunkSizeBuf.c_str());
	chunkSize = strtol(chunkSizeBuf.c_str(), NULL, 16);
	XP_DEBUG_OUT("The chunk size is: %d\n", chunkSize);

	currentPos += posChunkSize+2;
	XP_DEBUG_OUT("current pos is: %d\n", currentPos);

	while(chunkSize>0)
	{
		chunkDataBuf = srcBuf.substr(currentPos, chunkSize);
		XP_DEBUG_OUT("chunkDataBuf is:\n%s\n", chunkDataBuf.c_str());

		m_buffer.append(chunkDataBuf);
		XP_DEBUG_OUT("current whole content is:\n%s\n", m_buffer.c_str());

		currentPos += chunkSize+2;//skip the chunksize and \r\n
		XP_DEBUG_OUT("current pos is: %d\n", currentPos);

		posChunkSize = srcBuf.find("\r\n", currentPos); //chunk size line
		XP_DEBUG_OUT("The posChunkSize is: %d\n", posChunkSize);

		if(posChunkSize<=0)
		{
			return false;
		}
	
		chunkSizeBuf = srcBuf.substr(currentPos, posChunkSize);
		XP_DEBUG_OUT("The chunkSizeBuf is: %s\n", chunkSizeBuf.c_str());
		for(int i=currentPos; i<posChunkSize; ++i)
		{
			XP_DEBUG_OUT("The charactor is:\n%d\n", chunkSizeBuf.at(i-currentPos));
		}
		
		tmpPosChunkSize = chunkSizeBuf.find(" ");
		XP_DEBUG_OUT("The tmpPosChunkSize is: %d\n", tmpPosChunkSize);
		
		if(tmpPosChunkSize>0)
		{
			chunkSizeBuf = chunkSizeBuf.substr(0, tmpPosChunkSize);
			
			for(int i=0; i<tmpPosChunkSize; ++i)
			{
				XP_DEBUG_OUT("The charactor is:\n%d\n", chunkSizeBuf.at(i));
			}
		}

		//chunkSize = XP_API_ATOI(chunkSizeBuf.c_str());
		chunkSize = strtol(chunkSizeBuf.c_str(), NULL, 16);
		XP_DEBUG_OUT("The chunk size is: %d\n", chunkSize);

		currentPos += posChunkSize+2;
		XP_DEBUG_OUT("current pos is: %d\n", currentPos);
	}

	return true;
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
	char CONTENT_LENGTH_1[] = "\r\nContent-Length:";
	char CONTENT_LENGTH_2[] = "\r\nContent-length:";

	int pos = m_buffer.find("\r\n\r\n");
	int length = 0;

	if ( pos < 0 )
	{
		return -1;
	}

	std::string header = m_buffer.substr( 0, pos );
	pos = header.find( CONTENT_LENGTH_1 );
	if ( pos < 0 )
	{
		pos = header.find( CONTENT_LENGTH_2 );
		if ( pos < 0 )
		{
			
		}
		else
		{
			std::string str_length = header.substr( pos + XP_API_STRLEN( CONTENT_LENGTH_2 ) );
			pos = str_length.find( "\r\n" );
			if ( pos < 0 )
			{
				return -1;
			}
			str_length = str_length.substr( 0, pos );
			length = XP_API_ATOI( str_length.c_str() );
		}
	}
	else
	{
		std::string str_length = header.substr( pos + XP_API_STRLEN( CONTENT_LENGTH_1 ) );
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
	m_nonBlockConnectState = NONBLOCK_CONNECT_STATE_START;

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