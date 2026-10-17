#include "../../../../include/Multiplayer/Common/DefaultDataPacket.h"

#include "../../../../include/Game_defines.h"

#ifdef ENABLE_MULTIPLAYER

#include "../../../../include/Multiplayer/Common/DataPacket.h"

DefaultDataPacket::DefaultDataPacket(void)
{
	m_ip = NULL;
	m_port = 0;
    messagelen = 0;
    currentpos = 0;
    nextPacket = NULL;
	m_data_type = DATA_PACKET_TYPE_SESSION;
	XP_API_MEMSET( messagebody, 0, sizeof(BYTE) * ONLINE_MAX_PACKET_SIZE );
}

DefaultDataPacket::DefaultDataPacket(DataPacket * packet)
{
	m_ip = NULL;
	m_port = 0;
	m_data_type = packet->getDataType();
	setMessageBody( packet->getMessageBody(), packet->getMessageLen() );
	nextPacket = NULL;
}

DefaultDataPacket::~DefaultDataPacket(void)
{
	XP_API_DEL( m_ip );
}

bool DefaultDataPacket::addByte(BYTE data)
{
    if ( messagelen >= ONLINE_MAX_PACKET_SIZE )
	{
        return false;
	}

    messagebody[messagelen++] = data;
    return true;
}

bool DefaultDataPacket::getByte(BYTE &data)
{
    if (currentpos >= messagelen)
    {
        data = 0;
        return false;
    }

    data = messagebody[currentpos++];
    return true;
}

bool DefaultDataPacket::addShort(short data)
{
	// we need 4 bytes for an integer ... 
	if (messagelen > (ONLINE_MAX_PACKET_SIZE - 2 ))
	{
		return false;
	}

	messagebody[messagelen++] = (BYTE)((data & 0x0000FF00)>>8);
	messagebody[messagelen++] = (BYTE)((data & 0x000000FF));

	return true;
}

bool DefaultDataPacket::getShort(short &data)
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

bool DefaultDataPacket::addInt(int data)
{
    // we need 4 bytes for an integer ... 
    if (messagelen > (ONLINE_MAX_PACKET_SIZE - 4 ))
	{
        return false;
	}

    messagebody[messagelen++] = (BYTE)((data & 0xFF000000)>>24);
    messagebody[messagelen++] = (BYTE)((data & 0x00FF0000)>>16);
    messagebody[messagelen++] = (BYTE)((data & 0x0000FF00)>>8);
    messagebody[messagelen++] = (BYTE)((data & 0x000000FF));

    return true;
}

bool DefaultDataPacket::getInt(int &data)
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

bool DefaultDataPacket::addFloat(float data)
{
    // we need 4 bytes for an float ... 
    if (messagelen > (ONLINE_MAX_PACKET_SIZE - 4 ))
	{
        return false;
	}

	char *p = (char *)&data;

	messagebody[messagelen++] = *(p);
	messagebody[messagelen++] = *(p+1);
	messagebody[messagelen++] = *(p+2);
	messagebody[messagelen++] = *(p+3);

    return true;
}

bool DefaultDataPacket::getFloat(float &data)
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

bool DefaultDataPacket::addByteLenString(const char* data,BYTE len)
{
 //   if (data == NULL || len == 0)  //to enable string with lentgth = 0
	//{
 //       return false;
	//}

    // for the string , we need space for the data and the first byte for the string length
    if (messagelen > (ONLINE_MAX_PACKET_SIZE - len))
	{
        return false;
	}

    messagebody[messagelen++] = len;
	if (data != NULL && len > 0)
	{
		XP_API_MEMCPY((messagebody+messagelen),data,len);
	}
    messagelen += len;

    return true;
}


// The caller MUST free "data" once it is finished using it.
bool DefaultDataPacket::getByteLenString(char* &data,BYTE& len)
{
    // first , put the length of the data ... 
    len = messagebody[currentpos++];

	if (currentpos > (messagelen - len))
	{
		return false;
	}

    //if (len == 0)
    //{
    //    data = NULL;
    //    return false;
    //}

    data = XP_API_NEW char[(len+1)];
    if (len > 0)
	{
		XP_API_MEMCPY(data,(messagebody+currentpos),len);
	}
    data[len] = 0;//put end separator ..
    currentpos+=len;
    return true;
}

bool DefaultDataPacket::addByteLenWideString(const wchar_t* data,BYTE len)
{
    // for the string , we need space for the data and the first byte for the string length
    if (messagelen > (ONLINE_MAX_PACKET_SIZE - len * sizeof(wchar_t)))
	{
        return false;
	}

    messagebody[messagelen++] = len;
	if (data != NULL && len > 0)
	{
		XP_API_MEMCPY((messagebody+messagelen),data,len * sizeof(wchar_t));
	}
    messagelen += len * sizeof(wchar_t);

    return true;
}

bool DefaultDataPacket::getByteLenWideString(wchar_t* &data,BYTE& len)
{
	    // first , put the length of the data ... 
    len = messagebody[currentpos++];

	if (currentpos > (messagelen - len * sizeof(wchar_t)))
	{
		return false;
	}

    data = XP_API_NEW wchar_t[(len+1)];
	if (len > 0)
	{
		XP_API_MEMCPY(data,(messagebody+currentpos),len * sizeof(wchar_t));
	}
    data[len] = 0;//put end separator ..
    currentpos+=len * sizeof(wchar_t);
    return true;
}
int DefaultDataPacket::getMessageLen()
{
    return messagelen;
}

BYTE* DefaultDataPacket::getMessageBody()
{
    return messagebody;
}

void DefaultDataPacket::setMessageBody(BYTE* body, int body_len)
{
    XP_API_MEMCPY(messagebody,body,body_len);
    messagelen = body_len;
    currentpos = 0;
}

void DefaultDataPacket::packMessage()
{
    //messagelen = currentpos;
}


DataPacket* DefaultDataPacket::getNextPacket()
{
    return nextPacket;
}

void DefaultDataPacket::setNextPacket(DataPacket* next)
{
    nextPacket = next;
}

void DefaultDataPacket::addPacket( DataPacket * packet )
{
	int len = packet->getMessageLen();

	messagebody[messagelen++] = (BYTE)((len & 0x0000FF00)>>8);
	messagebody[messagelen++] = (BYTE)((len & 0x000000FF));
	XP_API_MEMCPY( messagebody+messagelen, packet->getMessageBody(), len );
	messagelen += len;
}

void DefaultDataPacket::getPacket( DataPacket * &packet )
{
	short len = 0;
	packet = XP_API_NEW DefaultDataPacket();

	len |= ( messagebody[currentpos++] & 0xFF ) << 8;
	len |= ( messagebody[currentpos++] & 0xFF );

	packet->setMessageBody( messagebody+currentpos, len );
	currentpos += len;
}

bool DefaultDataPacket::IsSendByTcp()
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

bool DefaultDataPacket::IsSendByUdp()
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

int DefaultDataPacket::getDataType()
{
	return m_data_type;
}

void DefaultDataPacket::setDataType( int type )
{
	m_data_type = type;
}

int DefaultDataPacket::getPort()
{
	return m_port;
}

void DefaultDataPacket::setPort( int port )
{
	m_port = port;
}

char * DefaultDataPacket::getIP()
{
	return m_ip;
}

void DefaultDataPacket::setIP( char * ip )
{
	XP_API_DEL( m_ip );
	m_ip = XP_API_STRNEW( ip );
}

void DefaultDataPacket::Packet2String( char * buffer, int &len )
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

int DefaultDataPacket::String2Packet( char * buffer, int len )
{
	short length = 0;
	int currentpos = 0;
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