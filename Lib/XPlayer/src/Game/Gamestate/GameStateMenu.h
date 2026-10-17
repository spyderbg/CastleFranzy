#ifndef __GAME_STATE_MENU_H__
#define __GAME_STATE_MENU_H__

#include "gamestate.h"
#include "Menu.h"

/**
 * The main menu.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GameStateMenu :
	public GameState, public MenuItemObserver
{
public:
	GameStateMenu( GameStateHandler * handler );
	~GameStateMenu(void);

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

    void Paint();

	void UpdateAnimation();

	void OnMenuItemClicked( char * manu_name );

	DEF::GAME_STATE_ENUM m_state;

	void DrawCommandBar( GameStateHandler * handler, CommandBarState state );

private:

	Menu * m_menu;
};

#endif // __GAME_STATE_MENU_H__