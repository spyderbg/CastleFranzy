#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include "GameStateHandler.h"
#include "TouchEvent.h"

/**
 * The abstract basic class of game state.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GameState
{

public:

	/**
	 * The type of command bar.
	 *
	 */
	enum CommandBarState
	{
		CB_NUMM_NULL = 0,
		CB_GO_NULL,
		CB_NULL_BACK,
		CB_GO_BACK,
		CB_REMOVE_BACK,
		CB_CANCEL,
		CB_JOIN_BACK,
		CB_LEAVE_NULL,
		CB_START_KICKOUT,
		CB_FINISH_LEAVE,
		CB_CONTINUE_BACK,
		CB_CHAT_BUDDY_BACK,
		CB_SEND_BACK,
		CB_ACCEPT_IGNORE
	};

	GameState( GameStateHandler * handler );

	/**
	 * Event handler.
     *
	 */
	virtual bool HandleEvent( _TOUCH_EVENT event ) = 0;

	/**
	 * Every frame, you have a chance to switch to another game state.
	 *
	 */
    virtual void SwitchState() = 0;

	/**
	 * Drawing.
	 *
	 */
    virtual void Paint() = 0;

	/**
	 * Do your logic process at this time.
     *
	 */
	virtual void UpdateAnimation() = 0;

	virtual bool UpdateQuitAnimation()
	{ return true;}

	virtual void BeginQuitAnimation(){}

	void SetGameState( DEF::GAME_STATE_ENUM state );

	virtual ~GameState(void);

	void DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool drawUpDown = true );

protected:

	GameStateHandler * m_handler;

	_TouchRect m_SoftLeft;
	_TouchRect m_SoftRight;
	_TouchRect m_SoftMid;
	_TouchRect m_UpKey;
	_TouchRect m_DownKey;

};

#endif // __GAME_STATE_H__