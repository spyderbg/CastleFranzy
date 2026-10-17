#ifdef WIN32
#include "GLLiveStdafx.h"
#include "GLLiveStateEdit.h"
#include "Porting.h"

GLLiveStateEdit::GLLiveStateEdit(GLLiveStateHandler * handler, char *pTitle, char *pText, bool isPwd, unsigned int maxLen, bool isView)
       : GLLiveState( handler )
{
	m_inputBox = XP_API_NEW GLLiveInputBox(60, 180);

	m_maxText = maxLen;

	m_text = XP_API_NEW char[m_maxText + 1];
	XP_API_MEMSET(m_text, 0, sizeof(char)  * (m_maxText + 1));
	XP_API_STRCPY(m_text, pText);
	m_inputBox->setInputBoxString(m_text);

	m_title = XP_API_STRNEW(pTitle);
	m_targetText = pText;

	m_bIsPwd = isPwd;
}

GLLiveStateEdit::~GLLiveStateEdit(void)
{
	XP_API_DEL(m_inputBox);
	XP_API_DELETE_ARRAY(m_title);
	XP_API_DELETE_ARRAY(m_text);
}

void GLLiveStateEdit::SwitchState()
{

}

bool GLLiveStateEdit::HandleEvent(_TOUCH_EVENT event)
{
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
}

void GLLiveStateEdit::Paint()
{
	//Paint title
	int x = 100;
	int y = 70;
	char temp[512];

	CGLLiveFont     *f = m_handler->GetFont(FONT_MID);
	CGLLiveGraphics *g = m_handler->GetGraphics();

	m_handler->ClearScreen();

	if(m_title != NULL)
	{
		sprintf(temp, "--%s--", m_title);
		f->DrawString(g, temp, x, y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	}
	//Paint text
	y += 50;

	XP_API_STRCPY(temp, m_text);
	//Password shows as *
	if(m_bIsPwd)
	{
		if(m_frameCount > 20)
		{
			XP_API_STRSET(temp, '*');
		}
		else
		{
			XP_API_STRNSET(temp, '*', XP_API_STRLEN(m_text) - 1);
		}
	}
	if((m_frameCount / 15) % 2 == 0)
	{
		XP_API_STRCAT(temp, "-");
	}
	f->DrawString(g, temp, x, y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

	//Paint input box
	m_inputBox->showTextBox(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateEdit::UpdateAnimation()
{
	GLLiveState::UpdateAnimation();
}

void GLLiveStateEdit::OnMenuItemClicked(int id)
{
	if(id == IDC_GO)
	{
		XP_API_STRCPY(m_targetText, m_text);
		m_handler->PopGLLiveState();
	}

	else if(id == IDC_BACK)
	{
		m_handler->PopGLLiveState();
	}
}
#endif