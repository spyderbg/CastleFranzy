#include "GLLiveStdafx.h"
#include "GLLiveMessageBox.h"
#include "GLLiveStateHandler.h"
#include "Porting.h"
#include "GLLive_Font.h"
#include "GLLiveMenu.h"


#define MESSAGEBOX_WIDTH            280
#define MESSAGEBOX_HEIGHT           148

CGLLiveMessageBox::CGLLiveMessageBox(const char *pTitle , const char *pText )
{
	m_pText = NULL;
	m_pTitle = NULL;

	m_rect.left = 100;
	m_rect.top =  86;
	m_rect.width = MESSAGEBOX_WIDTH;
	m_rect.height = MESSAGEBOX_HEIGHT;

	SetTitle(pTitle);
	SetText(pText);

	//m_frame = 0;
	//m_bShowTip = true;
	m_hasButton = false;
}

CGLLiveMessageBox::~CGLLiveMessageBox(void)
{
	XP_API_DELETE_ARRAY(m_pText);
	XP_API_DELETE_ARRAY(m_pTitle);
}

void CGLLiveMessageBox::SetText(const char *pText)
{
	if(pText != NULL)
	{
		//delete old string
		XP_API_DELETE_ARRAY(m_pText);
		//copy new string
		m_pText = XP_API_STRNEW(pText);
	}
	SetButtonType(0);
}

void CGLLiveMessageBox::SetTitle(const char *pTitle)
{
	if(pTitle != NULL)
	{
		//delete old string
		XP_API_DELETE_ARRAY(m_pTitle);
		//copy new string
		m_pTitle = XP_API_STRNEW(pTitle);
	}
}

void CGLLiveMessageBox::SetButtonType(int type)
{
	if(type == 0)
	{
		m_hasButton = false;
		m_rect.Set(99, 85, 280, 150);
	}	
	else
	{
		m_rect.Set(99, 75, 280, 165);
		m_hasButton = true;
	}
}

void CGLLiveMessageBox::Repaint(GLLiveStateHandler *handler)
{
	CGLLiveFont     *f_title = handler->GetFont(1);
	CGLLiveFont     *f_text = handler->GetFont(3);
	CGLLiveGraphics *g  = handler->GetGraphics();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	if(m_hasButton)
	{
		handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_MSG, 0, 0, 0, 0 ,0);
		//handler->DrawButton(m_rect);
		if(m_pTitle != NULL)
		{
			//handler->DrawString(m_rect.left + 5, m_rect.top + 5, m_pTitle);
			f_title->DrawString(g, m_pTitle, m_rect.left + 40, m_rect.top + 14, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP );
		}
		if(m_pText != NULL)
		{
			//handler->DrawString(m_rect.left + 30, m_rect.top + 30, m_pText);
			f_text->InitPage(m_pText, m_rect.width - 32);
			f_text->DrawPage(g, m_pText, SCREEN_WIDTH / 2, m_rect.top + 75, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER );
		}
	}
	else
	{
		handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_WARNING, 0, 0, 0, 0 ,0);
		//handler->DrawButton(m_rect);
		if(m_pTitle != NULL)
		{
			//handler->DrawString(m_rect.left + 5, m_rect.top + 5, m_pTitle);
			f_title->DrawString(g, m_pTitle, m_rect.left + 40, m_rect.top + 14, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP );
		}
		if(m_pText != NULL)
		{
			//handler->DrawString(m_rect.left + 30, m_rect.top + 30, m_pText);
			f_text->InitPage(m_pText, m_rect.width - 32);
			f_text->DrawPage(g, m_pText, SCREEN_WIDTH / 2, m_rect.top + 90, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER );
		}
	}
}

bool CGLLiveMessageBox::IsOn(const _TouchPoint &pt) const
{
	return m_rect.Contain(pt);
}

GLLiveMenu *CGLLiveMessageBox::GenerateConfirmMenu(GLXPlayerObserver *menuObserver)
{

	GLLiveMenu *confirmMenu = XP_API_NEW GLLiveMenu(0, 0);
	_TouchRect rect;
	rect.Set(141, 182, 63, 45 );
	confirmMenu->AddMenuItem(menuObserver, IDC_MSG_NO, NULL, rect);
	confirmMenu->GetItem(0)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_NO);
	confirmMenu->GetItem(0)->SetSelPic(SPRITE_MENU, Frame_menu_BUTTON_NO + 1);

	rect.Set(276, 182, 63, 45 );
	confirmMenu->AddMenuItem(menuObserver, IDC_MSG_YES,NULL, rect);
	confirmMenu->GetItem(1)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_YES);
	confirmMenu->GetItem(1)->SetSelPic(SPRITE_MENU, Frame_menu_BUTTON_YES + 1);

	return confirmMenu;
}