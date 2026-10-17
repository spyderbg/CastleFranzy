#include "stdafx.h"
#include "Menu.h"
#include "../../include/Porting.h"

Menu::Menu(int posX, int posY)
{
	m_x = posX;
	m_y = posY;
	m_count = 0;
	m_selected = 0;
	for (int i=0; i<MAX_MENU_ITEM_COUNT; i++)
	{
		m_vector[i] = NULL;
	}

	m_SoftLeft.left = DEF::BUTTON_LEFT_X;
	m_SoftLeft.top = DEF::BUTTON_LEFT_Y;
	m_SoftLeft.width = DEF::BUTTON_LEFT_W;
	m_SoftLeft.height = DEF::BUTTON_LEFT_H;	

	m_UpKey.left = DEF::BUTTON_UP_X;
	m_UpKey.top = DEF::BUTTON_UP_Y;
	m_UpKey.width = DEF::BUTTON_UP_W;
	m_UpKey.height = DEF::BUTTON_UP_H;

	m_DownKey.left = DEF::BUTTON_DOWN_X;
	m_DownKey.top = DEF::BUTTON_DOWN_Y;
	m_DownKey.width = DEF::BUTTON_DOWN_W;
	m_DownKey.height = DEF::BUTTON_DOWN_H;
}

Menu::~Menu(void)
{
	for (int i=0; i<m_count; i++)
	{
		delete(m_vector[i]);
	}
}

void Menu::Repaint(  GameStateHandler * handler )
{
	for(int i=0;i<m_count;i++)
	{
		m_vector[i]->Repaint(handler);
	}
}

bool Menu::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;
	
	if(event.isClicked)
	{		
		if ( m_UpKey.Contain(event.position) )
		{
			m_vector[m_selected]->SetSelected(false);
			m_selected--;
			if (m_selected < 0)
			{
				m_selected = m_count - 1;
			}
			m_vector[m_selected]->SetSelected(true);
			bHandled = true;
		}		
		else if ( m_DownKey.Contain(event.position) )
		{
			m_vector[m_selected]->SetSelected(false);
			m_selected++;
			if (m_selected == m_count)
			{
				m_selected = 0;
			}
			m_vector[m_selected]->SetSelected(true);
			bHandled = true;
		}		
		else if ( m_SoftLeft.Contain(event.position) )
		{
			m_vector[m_selected]->OnClicked();
			bHandled = true;
		}
	}

	return bHandled;
}

void Menu::AddMenuItem(char * title, MenuItemObserver * observer )
{
	MenuItem *pItem = XP_API_NEW MenuItem(m_x,m_y+m_count*MenuItem::getHeight(),title,observer,m_count == 0);
	m_vector[m_count] = pItem;
	m_count++;
}

void Menu::RemoveAllMenuItem()
{
	for (int i=0; i<m_count; i++)
	{
		delete(m_vector[i]);
	}

	m_count = 0;
}

int Menu::getSelectedIndex()
{
	return m_selected;
}