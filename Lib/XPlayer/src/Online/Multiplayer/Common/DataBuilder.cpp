//#include "stdafx.h"
#include "../../../../include/Multiplayer/Common/DataBuilder.h"

DataPacket * DataBuilder::BuildPushMessage( DataPacket * packet, char type )
{
	packet->setDataType( DATA_PACKET_TYPE_SESSION );
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
    packet->addByte( GAME_MESSAGE_PUSH );
    packet->addByte( type );
	return packet;
}

DataPacket * DataBuilder::BuildRequestSuccess( DataPacket * packet, char type )
{
	packet->setDataType( DATA_PACKET_TYPE_SESSION );
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
    packet->addByte( GAME_MESSAGE_RESPONSE );
    packet->addByte( type );
	packet->addByte( RESPONSE_MESSAGE_SUCCESS );
	return packet;
}

DataPacket * DataBuilder::BuildRequestError( DataPacket * packet, char type )
{
	packet->setDataType( DATA_PACKET_TYPE_SESSION );
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
    packet->addByte( GAME_MESSAGE_RESPONSE );
    packet->addByte( type );
	packet->addByte( RESPONSE_MESSAGE_ERROR );
	return packet;
}

DataPacket * DataBuilder::BuildGameMessage( DataPacket * packet, char type )
{
	packet->setDataType( DATA_PACKET_TYPE_SESSION );
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
	packet->addByte( GAME_MESSAGE_REQUEST );
	packet->addByte( type );
	return packet;
}

DataPacket * DataBuilder::BuildKeepAliveMessage( DataPacket * packet )
{
	packet->setDataType( DATA_PACKET_TYPE_SESSION );
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );
	packet->addByte( GAME_MESSAGE_KEEP_ALIVE );
	return packet;	
}

DataPacket * DataBuilder::BuildGameDataMessage( DataPacket * packet, char * data, int data_size, bool bReliable )
{
	char buffer[512] = { 0 };

	XP_API_MEMSET( buffer, 0, 512 );
	buffer[0] = BASIC_MESSAGE_TYPE_GAME;

	if ( bReliable )
	{
		buffer[1] = GAME_MESSAGE_IN_GAME_RELIABLE;
	}
	else
	{
		buffer[1] = GAME_MESSAGE_IN_GAME;
	}

	XP_API_MEMCPY( buffer+2, data, data_size );
	packet->setMessageBody( (BYTE *)buffer, data_size+2 );

	return packet;
}

DataPacket * DataBuilder::BuildGameDataMessage( DataPacket * packet, char * sender, DataPacket * data, bool bReliable )
{
	if ( bReliable )
	{
		packet->setDataType( DATA_PACKET_TYPE_GAME_RELIABLE );
	}
	else
	{
		packet->setDataType( DATA_PACKET_TYPE_GAME );
	}

	packet->addByte( BASIC_MESSAGE_TYPE_GAME );

	if ( bReliable )
	{
		packet->addByte( GAME_MESSAGE_IN_GAME_RELIABLE );
	}
	else
	{
		packet->addByte( GAME_MESSAGE_IN_GAME );
	}

	packet->addByteLenString( sender, XP_API_STRLEN( sender ) );
	packet->addPacket( data );
	return packet;
}

DataPacket * DataBuilder::BuildGameDataBasicMessage( DataPacket * packet, bool bReliable )
{
	packet->addByte( BASIC_MESSAGE_TYPE_GAME );

	if ( bReliable )
	{
		packet->addByte( GAME_MESSAGE_IN_GAME_RELIABLE );
	}
	else
	{
		packet->addByte( GAME_MESSAGE_IN_GAME );
	}

	return packet;
}