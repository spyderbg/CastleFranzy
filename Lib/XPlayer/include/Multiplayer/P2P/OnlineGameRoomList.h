#ifndef __ONLINE_GAME_ROOM_LIST_H__
#define __ONLINE_GAME_ROOM_LIST_H__

#include "OnlineGameRoom.h"
#include "../Common/DataPacket.h"

/**
 * Represent all the game rooms.
 * <p>
 * 
 */
class COnlineGameRoomList
{

public:

	COnlineGameRoomList(void);

	~COnlineGameRoomList(void);

	COnlineGameRoomList( DataPacket * packet );

	/**
	 * Get the amount of game rooms.
	 *
	 */
	int Count()
	{
		return m_count;
	}

	/**
	 * Get game room.
	 *
	 */
	COnlineGameRoom * Get( int index );
	COnlineGameRoom * Get( char * name );

	bool AddRoom(DataPacket * currentPacket, long time);


	bool ClearAllLostRoom( long time );
	bool SaveRoomData( DataPacket * currentPacket, int index );

private:

	bool ClearOneLostRoom( long time );
	/**
	 * The list of game rooms.
	 *
	 */
	COnlineGameRoom ** m_list;

	/**
	 * The amount of game rooms.
	 *
	 */
	int m_count;

};

#endif