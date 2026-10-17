#include "GLLiveStdafx.h"
#include "GLLiveMenu.h"
#include "Porting.h"

const int ANI_TIME     =   5;

GLLiveMenu::GLLiveMenu(int posX, int posY)
{
	m_x = posX;
	m_y = posY;
	m_count = 0;
	m_selected = -1;
	for (int i=0; i<MAX_MENU_ITEM_COUNT; i++)
	{
		m_vector[i] = NULL;
	}

	m_selTimeCount = -1;
	m_bPressed = false;
	/*
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
	*/
}

GLLiveMenu::~GLLiveMenu(void)
{
	RemoveAll();
}

void GLLiveMenu::Repaint(  GLLiveStateHandler * handler )
{
	for(int i=0;i<m_count;i++)
	{
		m_vector[i]->Repaint(handler);
	}
}

bool GLLiveMenu::HandleEvent( _TOUCH_EVENT event )
{
	bool bHandled = false;

	switch(event.eType)
	{
		case TET_Pressed:
		case TET_Moving:
			m_selected = -1;
			for(int i = 0; i < MAX_MENU_ITEM_COUNT; i++)
			{
				if(m_vector[i])
					m_vector[i]->SetSelected(false);
				if( m_vector[i] != NULL && m_vector[i]->GetRect().Contain(event.position) && m_vector[i]->IsEnable())
				{
					m_bPressed = true;
					m_selected = i;
					m_vector[m_selected]->SetSelected(true);
					//Begin timer
					//m_selTimeCount = 0;
					//return true;
					bHandled = true;
				}
			}
			break;
		case TET_Released:
			m_bPressed = false;
			if(m_selected != -1 && m_vector[m_selected]->GetRect().Contain(event.position))
			{
				//m_bPressed = false;
				if(m_selTimeCount > ANI_TIME)
				{
					int selected = m_selected;
					m_vector[m_selected]->SetSelected(false);
					m_selected = -1;
					m_selTimeCount = -1;
					m_vector[selected]->OnClicked();
				}
				return true;
			}
			break;
		//case TET_Moving:
		//	if(m_selected != -1 && !(m_vector[m_selected]->GetRect().Contain(event.position)))
		//	{
		//		m_vector[m_selected]->SetSelected(false);
		//		m_selected = -1;
		//		//Disable timer
		//		m_selTimeCount = -1;
		//		return true;
		//	}
		//	break;
		case TET_Idle:
			break;
		default:
			break;
	}
	/*	
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
	*/
	return bHandled;
}

void GLLiveMenu::AddMenuItem(GLXPlayerObserver * observer, int id, const char *title, const _TouchRect &rect)
{
	_TouchRect itemRect(rect);
	itemRect.top += m_y;
	itemRect.left += m_x;
	if(m_count + 1 < MAX_MENU_ITEM_COUNT)
	{
		m_vector[m_count] = XP_API_NEW GLLiveMenuItem(observer, id, itemRect, title);
		m_count++;
	}
}

GLLiveMenuItem *GLLiveMenu::GetItem(int index)
{
	if(index >= 0 && index < MAX_MENU_ITEM_COUNT)
	{
		return m_vector[index];
	}
	return NULL;
}

GLLiveMenuItem *GLLiveMenu::GetItemById(int id)
{
	for(int i = 0; i < MAX_MENU_ITEM_COUNT; i++)
	{
		if(m_vector[i] != NULL && m_vector[i]->GetId() == id)
		{
			return m_vector[i];
		}
	}
	return NULL;
}

void GLLiveMenu::SetAllVisible(bool bVisible)
{
	for(int i = 0; i < MAX_MENU_ITEM_COUNT; i++)
	{
		if(m_vector[i] != NULL)
		{
			m_vector[i]->SetVisible(bVisible);
		}
	}
}

void GLLiveMenu::RemoveAll()
{
	for(int i = 0; i < MAX_MENU_ITEM_COUNT; i++)
	{
		if(m_vector[i] != NULL)
		{
			delete(m_vector[i]);
			m_vector[i] = NULL;
		}
	}
	m_count = 0;
}

bool GLLiveMenu::IsEmpty() const
{
	return m_count == 0;
}

bool GLLiveMenu::Update()
{
	if(m_selected > -1)
	{
		m_selTimeCount++;
		if(!m_bPressed && m_selTimeCount > ANI_TIME)
		{
			int selected = m_selected;
			m_vector[m_selected]->SetSelected(false);
			m_selected = -1;
			m_selTimeCount = -1;
			m_vector[selected]->OnClicked();
			return true;
		}
	}
	return false;
}