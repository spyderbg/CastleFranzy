#ifndef __MENU_H__
#define __MENU_H__

#include "MenuItem.h"
#include "TouchEvent.h"

#define MAX_MENU_ITEM_COUNT		10

/**
 * The menu class.
 * <p>
 *
 * @version 0.0.1
 *
 */
class Menu
{
public:
	Menu(int posX, int posY);
	~Menu(void);

	void Repaint( GameStateHandler * handler );
	bool HandleEvent( _TOUCH_EVENT event );
	void AddMenuItem( char * title, MenuItemObserver * observer );
	void RemoveAllMenuItem();
	int getSelectedIndex();
	int getItemCount()
	{ return m_count;}
	
private:
	int m_x;
	int m_y;

	MenuItem * m_vector[MAX_MENU_ITEM_COUNT];
	int m_count;
	int m_selected;

public:
	_TouchRect m_SoftLeft;
	_TouchRect m_UpKey;
	_TouchRect m_DownKey;
};

#endif // __MENU_H__