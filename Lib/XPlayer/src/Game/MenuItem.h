#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__

#include "MenuItemObserver.h"
#include "GameStateHandler.h"
#include <string>

/**
 * The item in menu.
 * <p>
 *
 * @version 0.0.1
 *
 */
class MenuItem
{
public:
	MenuItem(int x, int y, char * title, MenuItemObserver * observer, bool selected);
	~MenuItem(void);

	void Repaint(GameStateHandler * handler);

	static int getWidth();
	static int getHeight();

	inline void SetSelected(bool selected)
	{
		m_bSelected = selected;	
	};

	void OnClicked();

private:
	int m_x;
	int m_y;
	int m_w;
	int m_h;
	MenuItemObserver *m_observer;
	char *m_title;
	bool m_bSelected;
};

#endif // __MENU_ITEM_H__