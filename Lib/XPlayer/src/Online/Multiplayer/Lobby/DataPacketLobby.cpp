#include "../../../../include/Multiplayer/Lobby/DataPacketLobby.h"

#include "../../../../include/Game_defines.h"

#ifdef ENABLE_MULTIPLAYER

#include "../../../../include/Multiplayer/Common/DataPacket.h"

DataPacketLobby::DataPacketLobby(void)
{
	m_ip = NULL;
	m_port = 0;
	messagelen = 0;
	currentpos = sizeof(short);
	nextPacket = NULL;
	m_data_type = DATA_PACKET_TYPE_SESSION;
	//XP_API_MEMSET( messagebody, 0, sizeof(BYTE) * ONLINE_MAX_PACKET_SIZE );
	m_max_len = ONLINE_MAX_PACKET_SIZE;
	messagebody = XP_API_NEW BYTE[m_max_len];
	XP_API_MEMSET( messagebody, 0, m_max_len );
}

DataPacketLobby::DataPacketLobby(DataPacket * packet)
{
	m_ip = NULL;
	m_port = 0;
	m_data_type = packet->getDataType();
	setMessageBody( packet->getMessageBody(), packet->getMessageLen() );
	nextPacket = NULL;
}

DataPacketLobby::~DataPacketLobby(void)
{
	XP_API_DEL( m_ip );
	XP_API_DEL( messagebody );
}

void DataPacketLobby::InitMessage( int type )
{
	addByte( 1 );
	addByte( 2 );
	addInt( type );
}

void DataPacketLobby::InitMMOMessage( short type )
{
	//addByte( 1 );
	//addByte( 2 );
	addShort( type );
}
int DataPacketLobby::GetMessageType()
{
	int type = 0;
	getInt( type );
	return type;
}
short DataPacketLobby::GetMMOMessageType()
{

	short type = 0;
	getShort( type );
	return type;
}
bool DataPacketLobby::WillBeFull( int nToBeAdded )
{
	if ( ( currentpos + nToBeAdded > m_max_len ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DataPacketLobby::AllocateMoreMomery()
{
	int len = m_max_len * 2;

	BYTE * body = XP_API_NEW BYTE[len];
	if ( NULL == body )
	{
		return false;
	}

	XP_API_MEMSET( body, 0, len );
	XP_API_MEMCPY( body, messagebody, currentpos );
	XP_API_DEL( messagebody );
	messagebody = body;
	m_max_len = len;
	return true;
}

bool DataPacketLobby::addByte(BYTE data)
{
	if ( WillBeFull( sizeof(BYTE) ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	messagebody[currentpos++] = data;
	return true;
}

bool DataPacketLobby::getByte(BYTE &data)
{
	if (currentpos >= messagelen)
	{
		data = 0;
		return false;
	}

	data = messagebody[currentpos++];
	return true;
}

bool DataPacketLobby::addShort(short data)
{
	if ( WillBeFull( sizeof(short) ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}
	messagebody[currentpos++] = (BYTE)((data & 0x0000FF00)>>8);
	messagebody[currentpos++] = (BYTE)((data & 0x000000FF));

	return true;
}

bool DataPacketLobby::getShort(short &data)
{
	data = 0;

	if (currentpos > (messagelen - 2))
	{
		return false;
	}

	data |= (messagebody[currentpos++] & 0xFF) << 8;
	data |= (messagebody[currentpos++] & 0xFF);

	return true;
}

bool DataPacketLobby::addInt(int data)
{
	if ( WillBeFull( sizeof(int) ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	messagebody[currentpos++] = (BYTE)((data & 0xFF000000)>>24);
	messagebody[currentpos++] = (BYTE)((data & 0x00FF0000)>>16);
	messagebody[currentpos++] = (BYTE)((data & 0x0000FF00)>>8);
	messagebody[currentpos++] = (BYTE)((data & 0x000000FF));

	return true;
}

bool DataPacketLobby::getInt(int &data)
{
	data = 0;

	if (currentpos > (messagelen - 4))
	{
		return false;
	}

	data |= (messagebody[currentpos++] & 0xFF) << 24;
	data |= (messagebody[currentpos++] & 0xFF) << 16;
	data |= (messagebody[currentpos++] & 0xFF) << 8;
	data |= (messagebody[currentpos++] & 0xFF);

	return true;
}

bool DataPacketLobby::addFloat(float data)
{
	if ( WillBeFull( sizeof(float) ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	char *p = (char *)&data;

	messagebody[currentpos++] = *(p);
	messagebody[currentpos++] = *(p+1);
	messagebody[currentpos++] = *(p+2);
	messagebody[currentpos++] = *(p+3);

	return true;
}

bool DataPacketLobby::getFloat(float &data)
{
	data = 0.0;

	if (currentpos > (messagelen - 4))
	{
		return false;
	}

	char *p = (char *)&data;

	*(p) = messagebody[currentpos++];
	*(p+1) = messagebody[currentpos++];
	*(p+2) = messagebody[currentpos++];
	*(p+3) = messagebody[currentpos++];

	return true;
}

bool DataPacketLobby::addBlob( char* data, short len )
{
	if( data == NULL || len < 0 ) len = 0;

	if( WillBeFull( len + sizeof(short) ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	addShort( len );
	if( len > 0 )
	{
		XP_API_MEMCPY( ( messagebody + currentpos ), data, len );
		currentpos += len;
	}

	return true;
}

bool DataPacketLobby::getBlob( char*& data, short & len )
{
	// first , put the length of the data ... 
	if( !getShort( len ) || len < 0 )
	{
		data = NULL;
		len = 0;
		return false;
	}

	if( currentpos > ( m_max_len - len ) )
	{
		data = NULL;
		len = 0;
		return false;
	}

	data = XP_API_NEW char[len+1];
	data[len] = '\0'; // if str, then add null-terminated
	if( len > 0 )
	{
		XP_API_MEMCPY( data, messagebody + currentpos, len);
		currentpos += len;
	}
	return true;
}

bool DataPacketLobby::addQueryString(const char* data, short len)
{
	if ( data == NULL || len == 0 )
	{
		return false;
	}

	if ( WillBeFull( len ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	//messagebody[currentpos++] = (BYTE)((len & 0x0000FF00)>>8);
	//messagebody[currentpos++] = (BYTE)((len & 0x000000FF));
	XP_API_MEMCPY((messagebody+currentpos),data,len);
	currentpos += len;

	return true;
}

bool DataPacketLobby::addShortLenString(const char* data, short len)
{
	if ( data == NULL || len == 0 )
	{
		return false;
	}

	if ( WillBeFull( len ) )
	{
		if( !AllocateMoreMomery() )
		{
			return false;
		}
	}

	messagebody[currentpos++] = (BYTE)((len & 0x0000FF00)>>8);
	messagebody[currentpos++] = (BYTE)((len & 0x000000FF));
	XP_API_MEMCPY((messagebody+currentpos),data,len);
	currentpos += len;

	return true;
}

bool DataPacketLobby::getShortLenString(char* &data, short& len)
{
	// first , put the length of the data ... 
	len = 0;

	len |= ( messagebody[currentpos++] & 0xFF ) << 8;
	len |= ( messagebody[currentpos++] & 0xFF );


	if (currentpos > (messagelen - len))
	{
		return false;
	}

	if (len == 0)
	{
		data = NULL;
		return false;
	}

	data = XP_API_NEW char[(len+1)];
	XP_API_MEMCPY(data,(messagebody+currentpos),len);
	data[len] = 0;//put end separator ..
	currentpos+=len;
	return true;
}

int DataPacketLobby::getMessageLen()
{
	return messagelen;
}

BYTE* DataPacketLobby::getMessageBody()
{
	return messagebody;
}

void DataPacketLobby::setMessageBody(BYTE* body, int body_len)
{
	XP_API_MEMCPY(messagebody,body,body_len);
	messagelen = body_len;
	currentpos = 0;
}

void DataPacketLobby::packMessage()
{
	messagelen = currentpos;
	messagebody[0] = (BYTE)(((messagelen - 2) & 0x0000FF00)>>8);
	messagebody[1] = (BYTE)(((messagelen - 2) & 0x000000FF));


}


DataPacket* DataPacketLobby::getNextPacket()
{
	return nextPacket;
}

void DataPacketLobby::setNextPacket(DataPacket* next)
{
	nextPacket = next;
}

void DataPacketLobby::addPacket( DataPacket * packet )
{
	int len = packet->getMessageLen();

	if ( WillBeFull( len ) )
	{
		if( !AllocateMoreMomery() )
		{			
			XP_DEBUG_OUT( "addPacket failed : memory leak\n");
			return;
		}
	}

	messagebody[currentpos++] = (BYTE)((len & 0x0000FF00)>>8);
	messagebody[currentpos++] = (BYTE)((len & 0x000000FF));
	XP_API_MEMCPY( messagebody+currentpos, packet->getMessageBody(), len );
	currentpos += len;
}

void DataPacketLobby::getPacket( DataPacket * &packet )
{
	short len = 0;
	packet = XP_API_NEW DataPacketLobby();

	len |= ( messagebody[currentpos++] & 0xFF ) << 8;
	len |= ( messagebody[currentpos++] & 0xFF );

	packet->setMessageBody( messagebody+currentpos, len );
	currentpos += len;
}

bool DataPacketLobby::IsSendByTcp()
{
	if ( m_data_type == DATA_PACKET_TYPE_SESSION ||
		m_data_type == DATA_PACKET_TYPE_GAME_RELIABLE )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DataPacketLobby::IsSendByUdp()
{
	if ( m_data_type == DATA_PACKET_TYPE_GAME )
	{
		return true;
	}
	else
	{
		return false;
	}	
}

int DataPacketLobby::getDataType()
{
	return m_data_type;
}

void DataPacketLobby::setDataType( int type )
{
	m_data_type = type;
}

int DataPacketLobby::getPort()
{
	return m_port;
}

void DataPacketLobby::setPort( int port )
{
	m_port = port;
}

char * DataPacketLobby::getIP()
{
	return m_ip;
}

void DataPacketLobby::setIP( char * ip )
{
	XP_API_DEL( m_ip );
	m_ip = XP_API_STRNEW( ip );
}

void DataPacketLobby::Packet2String( char * buffer, int &len )
{
	int currentpos = 0;
	buffer[currentpos++] = (BYTE)((messagelen & 0x0000FF00)>>8);
	buffer[currentpos++] = (BYTE)((messagelen & 0x000000FF));

	len = 0;
	len += 2;

	XP_API_MEMCPY( buffer+len, messagebody, messagelen );
	len += messagelen;

	return;
}

int DataPacketLobby::String2Packet( char * buffer, int len )
{
	short length = 0;

	length |= (buffer[currentpos++] & 0xFF) << 8;
	length |= (buffer[currentpos++] & 0xFF);

	if ( len < ( length + 2 ) )
	{
		return -1;
	}

	setMessageBody( (BYTE *)buffer+2, length );
	return ( length + 2 );
}

#endif//ENABLE_MULTIPLAYER