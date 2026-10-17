#ifndef __DATABUILDER_H__
#define __DATABUILDER_H__

#include "../../Game_defines.h"
#include "DataPacket.h"

class DataBuilder
{

public:

	static DataPacket * BuildPushMessage( DataPacket * packet, char type );
	static DataPacket * BuildRequestSuccess( DataPacket * packet, char type );
	static DataPacket * BuildRequestError( DataPacket * packet, char type );
	static DataPacket * BuildGameMessage( DataPacket * packet, char type );
	static DataPacket * BuildKeepAliveMessage( DataPacket * packet );
	static DataPacket * BuildGameDataMessage( DataPacket * packet, char * data, int data_size, bool bReliable );
	static DataPacket * BuildGameDataMessage( DataPacket * packet, char * sender, DataPacket * data, bool bReliable );
	static DataPacket * BuildGameDataBasicMessage( DataPacket * packet, bool bReliable );
};

#endif