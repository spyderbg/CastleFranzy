#include "GLLiveStdafx.h"
#include "GLLiveMenuItem.h"
#include "Porting.h"
#include "GLLive_Font.h"
#include "GLLive_ASprite.h"
//#define MENU_ITEM_WIDTH		100

GLLiveMenuItem::GLLiveMenuItem(GLXPlayerObserver * observer, int id, const _TouchRect &rect, const char * title)
{
	m_rect = rect;
	m_observer = observer;
	m_title = NULL;
	m_id = id;
	m_font = 2;
	m_pic = NULL;
	m_selPic = NULL;

	if(title != NULL)
	{
		m_title = XP_API_STRNEW(title);
	}
	m_bSelected = false;
    m_bVisible = true;
	m_bEnable = true;
}

GLLiveMenuItem::~GLLiveMenuItem(void)
{
	XP_API_DELETE_ARRAY(m_title);
	XP_API_DEL(m_pic);
	XP_API_DEL(m_selPic);
}

void GLLiveMenuItem::OnClicked()
{
	m_observer->OnMenuItemClicked( m_id );
}

/*
int GLLiveMenuItem::getWidth()
{
	return MENU_ITEM_WIDTH;
}

int GLLiveMenuItem::getHeight()
{
	return DEF::LINE_HEIGHT;
}
*/

const _TouchRect& GLLiveMenuItem::GetRect() const
{
	return m_rect;
}

void GLLiveMenuItem::Repaint(GLLiveStateHandler * handler)
{
	CGLLiveFont     *f = handler->GetFont(m_font);
	CGLLiveGraphics *g = handler->GetGraphics();

	if(m_bVisible)
	{
		//Draw frame
		//handler->DrawButton(m_rect);


		if(m_pic != NULL && m_pic->CanShow())
		{
			handler->GetASprite(m_pic->asprite)->PaintFrame(g, m_pic->frame, m_pic->x, m_pic->y, 0 , 0, 0);
		}

		if(m_bSelected && m_selPic != NULL && m_selPic->CanShow())
		{
			handler->GetASprite(m_selPic->asprite)->PaintFrame(g, m_selPic->frame, m_selPic->x, m_selPic->y, 0 , 0, 0);
		}

		if(m_title != NULL)
		{
			int oldLineSpacing = f->GetLineSpacing();
			if(m_font == 9)
			{
				f->SetLineSpacing(-7);
			}
			//f->DrawString(g, m_title, m_rect.left + m_rect.width / 2, m_rect.top + m_rect.height / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);
			f->InitPage(m_title, m_rect.width);
			f->DrawPage(g, m_title, m_rect.left + m_rect.width / 2, m_rect.top + m_rect.height / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);
			if(m_font == 9)
			{
				f->SetLineSpacing(oldLineSpacing);
			}
		}


		//Show touch rect
#ifdef SHOW_TOUCH_RECT
		g->EndDraw2D();
		g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
		g->SetARGBColor(0x88880000);
		g->FillRect(m_rect.left, m_rect.top, m_rect.width, m_rect.height);
#endif
	}
}

void GLLiveMenuItem::SetEnable(bool bEnable)
{
	if(m_bVisible)
	{
		m_bEnable = bEnable;
	}
}

void GLLiveMenuItem::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
	m_bEnable  = bVisible;
}

 bool GLLiveMenuItem::IsEnable() const
{
	return m_bEnable;
}

 bool GLLiveMenuItem::IsVisible() const
{
	return m_bVisible;
}

 int GLLiveMenuItem::GetId()
{
	return m_id;
}

 bool GLLiveMenuItem::IsSelected() const
{
	return m_bSelected;
}

void GLLiveMenuItem::SetRect(const _TouchRect &rect)
{
	m_rect.left = rect.left;
	m_rect.top  = rect.top;
	m_rect.height = rect.height;
	m_rect.width = m_rect.width;
}

void GLLiveMenuItem::SetTitle(const char *title)
{
	if(title != NULL)
	{
		XP_API_DELETE_ARRAY(m_title);
		m_title = XP_API_NEW char[strlen(title)+1];
		XP_API_STRCPY(m_title, title);
	}
}

void GLLiveMenuItem::SetPic(int asprite, int frame, int x, int y)
{
	if(m_pic == NULL)
	{
		m_pic = XP_API_NEW ButtonPic();
	}
	m_pic->asprite = asprite;
	m_pic->frame = frame;
	m_pic->x = x;
	m_pic->y = y;
}

void GLLiveMenuItem::SetSelPic(int asprite, int frame, int x, int y)
{
	if(m_selPic == NULL)
	{
		m_selPic = XP_API_NEW ButtonPic();
	}
	m_selPic->asprite = asprite;
	m_selPic->frame = frame;
	m_selPic->x = x;
	m_selPic->y = y;
}

void GLLiveMenuItem::SetFont(int font)
{
	if(font >= 0)
	{
		m_font = font;
	}
}