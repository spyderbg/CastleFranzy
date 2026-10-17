#include "stdafx.h"
#include "MenuItem.h"
#include "../../include/Porting.h"

#define MENU_ITEM_WIDTH		100

MenuItem::MenuItem(int x, int y, char * title, MenuItemObserver * observer, bool selected)
{
	m_x = x;
	m_y = y;
	m_observer = observer;
	m_title = XP_API_NEW char[strlen(title)+1];
	memset(m_title,0,strlen(title)+1);
	memcpy(m_title,title,strlen(title));

	m_bSelected = selected;
}

MenuItem::~MenuItem(void)
{
	delete m_title;
}

void MenuItem::OnClicked()
{
	m_observer->OnMenuItemClicked( m_title );
}

int MenuItem::getWidth()
{
	return MENU_ITEM_WIDTH;
}

int MenuItem::getHeight()
{
	return DEF::LINE_HEIGHT;
}

void MenuItem::Repaint(GameStateHandler * handler)
{
	if ( m_bSelected )
	{
		handler->DrawString(m_x-DEF::LINE_HEIGHT,m_y,">");
	}
	
	handler->DrawString(m_x,m_y,m_title);

}