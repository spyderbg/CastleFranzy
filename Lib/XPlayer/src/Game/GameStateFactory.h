#ifndef __GAME_STATE_FACTORY_H__
#define __GAME_STATE_FACTORY_H__

#include "DEF.h"
#include "GameState.h"
#include "GameStateHandler.h"

/**
 * The class factory of game state.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GameStateFactory
{
public:

	static GameState * CreateGameState( GameStateHandler * handler, 
		DEF::GAME_STATE_ENUM state );

};

#endif // __GAME_STATE_FACTORY_H__