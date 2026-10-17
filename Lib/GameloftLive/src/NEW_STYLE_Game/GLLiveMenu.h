#ifndef __MENU_H__
#define __MENU_H__

#include "GLLiveMenuItem.h"
#include "TouchEvent.h"

#define MAX_MENU_ITEM_COUNT		10

/**
 * The menu class.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GLLiveMenu
{
public:
	GLLiveMenu(int posX, int posY);
	~GLLiveMenu(void);

	void Repaint( GLLiveStateHandler * handler );
	bool HandleEvent( _TOUCH_EVENT event );
	void AddMenuItem(GLXPlayerObserver * observer, int id, const char *title, const _TouchRect &pRect);
	GLLiveMenuItem *GetItem(int index);
	GLLiveMenuItem *GetItemById(int id);
	void SetAllVisible(bool bVisible);
	void RemoveAll();
	bool IsEmpty() const;
	bool Update();

	int  m_selTimeCount;
private:
	int m_x;
	int m_y;

	GLLiveMenuItem * m_vector[MAX_MENU_ITEM_COUNT];
	int m_count;
	int m_selected;
	//Time count for show visual feedback
	bool m_bPressed;
};

#endif // __MENU_H__