#include "GLLiveStdafx.h"
#include "GLLiveMessageBox.h"
#include "GLLiveStateHandler.h"
#include "Porting.h"
#include "GLLive_Font.h"

CGLLiveMessageBox::CGLLiveMessageBox(const char *pTitle , const char *pText )
{
	m_pText = NULL;
	m_pTitle = NULL;

	m_rect.left = 100;
	m_rect.top =  76;
	m_rect.width = MESSAGEBOX_WIDTH;
	m_rect.height = MESSAGEBOX_HEIGHT;

	SetTitle(pTitle);
	SetText(pText);

	//m_frame = 0;
	//m_bShowTip = true;
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
}

void CGLLiveMessageBox::SetTitle(const char *pTitle)
{
	//delete old string
	XP_API_DELETE_ARRAY(m_pTitle);
	if(pTitle != NULL)
	{
		//copy new string
		m_pTitle = XP_API_STRNEW(pTitle);
	}
}

void CGLLiveMessageBox::Repaint(GLLiveStateHandler *handler)
{
	CGLLiveFont     *f3 = handler->GetFont(2);
	CGLLiveFont     *f7 = handler->GetFont(6);
	CGLLiveGraphics *g  = handler->GetGraphics();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_BG_MSG, 0, 0, 0, 0 ,0);
	//handler->DrawButton(m_rect);
	if(m_pTitle != NULL)
	{
		//handler->DrawString(m_rect.left + 5, m_rect.top + 5, m_pTitle);
		f3->DrawString(g, m_pTitle, m_rect.left + 5, m_rect.top + 3, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP );
	}
	if(m_pText != NULL)
	{
		//handler->DrawString(m_rect.left + 30, m_rect.top + 30, m_pText);
		f7->InitPage(m_pText, m_rect.width - 32);
		f7->DrawPage(g, m_pText, SCREEN_WIDTH / 2, m_rect.top + m_rect.height / 2 - 5, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER );
	}

	//if(m_bShowTip && m_frame % 10 < 5)
	//{
	//	f7->DrawString(g, GetString(STR_CONTINUE), 240, m_rect.top + m_rect.height - 10, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE );
	//}
}

bool CGLLiveMessageBox::IsOn(const _TouchPoint &pt) const
{
	return m_rect.Contain(pt);
}

//void CGLLiveMessageBox::Update()
//{
//	m_frame++;
//}

//void CGLLiveMessageBox::SetShowTip(bool show)
//{
//	m_bShowTip = show;
//}