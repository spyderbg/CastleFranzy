#include "GLLiveStdAfx.h"
#include "GLLiveStateEdit.h"
#include "Porting.h"
#include "ViewController.h"
#include "GLLiveMenu.h"
GLLiveStateEdit::GLLiveStateEdit(GLLiveStateHandler * handler, char *pTitle, char *pText, bool isPwd, unsigned int maxLen, bool isView)
       : GLLiveState( handler )
{
	m_inputBox = [[GLLiveViewController alloc] init:pText LengthOfText:maxLen IsView:isView IsPwd:isPwd Width:280 HMiddle:240];
	[(GLLiveViewController*)m_inputBox performSelectorOnMainThread:@selector(myInit) withObject:nil waitUntilDone:YES];
	usleep(20000);
	
	m_title = pTitle;
	m_bIsPwd = isPwd;
	
	m_pLastState = NULL;
	
	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	_TouchRect rect;
	rect.Set(20, 127, 72, 32);
	m_menu->AddMenuItem(this, IDC_BACK, GetString(STR_BACK), rect);
	m_menu->GetItem(0)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BUTTON_SEND, 20, 127);
	m_menu->GetItem(0)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_SEND, 20 - 8, 127 - 8);
	m_menu->GetItem(0)->SetFont(2);

	rect.Set(388, 127, 72, 32);
	m_menu->AddMenuItem(this, IDC_GO, GetString(STR_GO), rect);
	m_menu->GetItem(1)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BUTTON_SEND, 388, 127);
	m_menu->GetItem(1)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_SEND, 388 - 8, 127 - 8);
	m_menu->GetItem(1)->SetFont(2);
	
	m_orientation = gll_isLandscapeRight;
}

GLLiveStateEdit::GLLiveStateEdit(GLLiveStateHandler *handler, char *pText, GLLiveState *pLastState) : GLLiveState(handler)
{
	m_pLastState = pLastState;
	
	m_inputBox = [[GLLiveViewController alloc] init:pText LengthOfText:127 IsView:true IsPwd:false Width:(480-115) HMiddle:(240-15)];
	[(GLLiveViewController*)m_inputBox performSelectorOnMainThread:@selector(myInit) withObject:nil waitUntilDone:YES];
	usleep(20000);
	
	m_title = "Message";
	m_bIsPwd = false;
	
	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	_TouchRect rect;
	rect.Set(1, 127, 44, 32);
	m_menu->AddMenuItem(this, IDC_BACK, NULL, rect);
	m_menu->GetItem(0)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BOTTION_DOWN, 1, 127);
	m_menu->GetItem(0)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_BUTTON, 1 - 8, 127 - 8);
	m_menu->GetItem(0)->SetFont(2);
	
	rect.Set(407, 127, 72, 32);
	m_menu->AddMenuItem(this, IDC_GO, GetString(STR_SEND), rect);
	m_menu->GetItem(1)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BUTTON_SEND, 407, 127);
	m_menu->GetItem(1)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_SEND, 407 - 8, 127 - 8);
	m_menu->GetItem(1)->SetFont(2);
	
	m_orientation = gll_isLandscapeRight;
}

GLLiveStateEdit::~GLLiveStateEdit(void)
{
	XP_API_DEL(m_menu);
	
	if(m_inputBox != NULL)
	{
		GLLiveViewController *pController = (GLLiveViewController*)m_inputBox;
		[pController performSelectorOnMainThread:@selector(endInput) withObject:nil waitUntilDone:YES];
		usleep(20000);
		[pController release];
	}
}

void GLLiveStateEdit::SwitchState()
{

}

bool GLLiveStateEdit::HandleEvent(_TOUCH_EVENT event)
{
#ifdef WIN32	
	if(event.eType == TET_Pressed)
	{
		if(m_inputBox->updateInputBox(event))
		{
			char *pText = m_inputBox->getInputBoxString();
			unsigned int len = strlen(pText);
			if(len > m_maxText)
			{
				m_inputBox->setInputBoxString(m_text);
			}
			else
			{
				strcpy(m_text, pText);
				m_frameCount = 0;
			}
			return true;
		}
	}
	return GLLiveState::HandleEvent(event);
#else
	return m_menu->HandleEvent(event);
#endif
}

void GLLiveStateEdit::Paint()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	if(m_pLastState == NULL)
	{
		m_handler->GetASprite(0)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
		g->BeginScale(0.8, 0.8, SCREEN_WIDTH, 0);
		m_handler->GetASprite(1)->PaintFrame(g, Frame_menu_BG_LOGO, 90, -20, 0, 0, 0);
		g->EndScale();
		m_handler->GetFont(4)->DrawString(g, m_title, 95, 60, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	}
	else
	{
		m_pLastState->Paint();
	}
	m_menu->Repaint(m_handler);
}

void GLLiveStateEdit::UpdateAnimation()
{
	if(m_orientation != gll_isLandscapeRight)
	{
		m_orientation = gll_isLandscapeRight;
		[(GLLiveViewController*)m_inputBox performSelectorOnMainThread:@selector(onRotateDevice) withObject:nil waitUntilDone:YES];
		usleep(20000);
	}
	m_menu->Update();
	GLLiveState::UpdateAnimation();
}

void GLLiveStateEdit::OnMenuItemClicked(int id)
{
	GLLiveViewController *pController = (GLLiveViewController*)m_inputBox;
	if(id == IDC_GO)
	{
		[pController performSelectorOnMainThread:@selector(clickOK) withObject:nil waitUntilDone:YES];
		usleep(20000);
		m_handler->PopGLLiveState();
	}
	else if(id == IDC_BACK)
	{
		[pController performSelectorOnMainThread:@selector(clickBack) withObject:nil waitUntilDone:YES];
		usleep(20000);
		m_handler->PopGLLiveState();
	}
}
