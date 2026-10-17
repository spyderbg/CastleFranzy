#ifndef __GAME_DEFINES_H__
#define __GAME_DEFINES_H__

#include "XPlayer_defines.h"

#define MULTIPLAYER_BY_SERVER     0
#define MULTIPLAYER_P2P           1
#define MULTIPLAYER_LOBBY         2
#define MULTIPLAYER_TYPE          MULTIPLAYER_BY_SERVER

//>>for lobby second connect
#define	SECOND_CONNECTING_TIME_IN_GAME		( 2 * 60 * 1000 )
#define	SECOND_CONNECTING_TIME_IN_LOBBY		( 1 * 60 * 1000 )
//<<end

//>>get lobby server
#define	GET_SERVER_FROM_WEB
//#define LOBBY_LIST_WITH_GAME_PARAM
#define	LOBBY_SERVER_ADDRESS_HEAD			"socket://"
//<<end

#define	LAUNCH_GAME_TIME_OUT				( 30 * 1000 ) //the time is used that lobby server get the game server ip

#endif //__GAME_DEFINES_H__