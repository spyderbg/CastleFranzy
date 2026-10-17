#ifndef __ONLINE_PLAYER_LIST_H__
#define __ONLINE_PLAYER_LIST_H__

#include "OnlinePlayer.h"

class COnlinePlayer;

/**
 * Represent all the players in the game.
 * <p>
 * 
 */
class COnlinePlayerList
{

public:

	COnlinePlayerList(void);

	~COnlinePlayerList(void);

	/**
	 * Get the amount of players.
	 *
	 */
	int Count()
	{
		return m_count;
	}

	/**
	 * Add new player.
	 *
	 */
	COnlinePlayer * AddPlayer( char * name, char * country, DataPacket * data, 
		char * exter_ip, char * inter_ip, int exter_tcp_port, int exter_udp_port,
		int inter_tcp_port, int inter_udp_port );

	/**
	 * Get player.
	 *
	 */
	COnlinePlayer * Get( int index );

	COnlinePlayer * GetPlayer( int index );

	int GetPlayerIndex( char * name );

	/**
	 * Delete player.
	 *
	 */
	bool RemovePlayer( int index );

	bool RemovePlayer( char * name );

	COnlinePlayer * GetLastPlayer();

	int GetPlayerCount();

private:

	/**
	 * The players' list.
	 *
	 */
	COnlinePlayer ** m_list;

	/**
	 * The amount of the players.
	 *
	 */
	int m_count;

};

#endif