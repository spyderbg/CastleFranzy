#include "GLLiveStdafx.h"
#include "GLLiveInputBox.h"
#include "Porting.h"
#include "GLLive_Memory.h"
#include "GLLive_Font.h"

GLLiveInputBox::GLLiveInputBox(int left, int top)
{
	m_textBoxString = NULL;
	m_lastTextBoxLen = 0;	

	m_keyNumChars = XP_API_NEW char[128];
	XP_API_MEMSET( m_keyNumChars, 0, 128 );
	XP_API_SPRINTF( m_keyNumChars,"1234567890<|qwertyuiop(|asdfghjkl@)|zxcvbnm .:^");
	
	int keyNumLen = XP_API_STRLEN(m_keyNumChars);
	
	m_keyTouchRect = XP_API_NEW _TouchRect[ keyNumLen - 3];
	int i=0;
	int j = 0, l = left;	
	 for (i=0; i < keyNumLen;i++)
	 {					 
		 if(m_keyNumChars[i] == '|')
		 {
			top += 27;			
			l = left;
		 }
		 else
		 {
			 l = l + 27;
			 m_keyTouchRect[j].left = l;
			 m_keyTouchRect[j].top = top;
			 m_keyTouchRect[j].width = 25;
			 m_keyTouchRect[j].height = 25;
			 j++;
		 }
	 }
}

GLLiveInputBox::~GLLiveInputBox(void)
{
	XP_API_DELETE_ARRAY(m_keyTouchRect);
	m_keyTouchRect = NULL;
	XP_API_DELETE_ARRAY(m_keyNumChars);
	m_keyNumChars = NULL;
	XP_API_DELETE_ARRAY(m_textBoxString);
	m_textBoxString = NULL;
}

void GLLiveInputBox::showTextBox(GLLiveStateHandler *handler)
{
	CGLLiveFont     *f = handler->GetFont(FONT_SMALL);
	CGLLiveGraphics *g = handler->GetGraphics();
	int keyNumLen = XP_API_STRLEN(m_keyNumChars);
	char tmp[2];
	int j=0;
	for (int i=0; i < keyNumLen;i++)
	{					 
		if(m_keyNumChars[i] != '|')
		{
			handler->DrawButton(m_keyTouchRect[j]);
			XP_API_MEMSET(tmp,0,2);
			tmp[0] = m_keyNumChars[i];
			tmp[1] = '\0';
			f->DrawString(g, tmp, m_keyTouchRect[j].left + 2,m_keyTouchRect[j].top + 2, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
			j++;
		}
	}	
}
bool GLLiveInputBox::updateInputBox(_TOUCH_EVENT event)
{
	bool bHandled = false;
	int keyNumLen = XP_API_STRLEN(m_keyNumChars);

	for (int i=0,j=0; i < keyNumLen; i++)
	{
		if(m_keyNumChars[i] != '|')
		{
			if(m_keyNumChars[i] == '<')
			{
				if(event.isClicked && m_keyTouchRect[j].Contain(event.position))
				{
					if(m_lastTextBoxLen > 0)
						m_lastTextBoxLen -= 1;
					char* tmp = XP_API_NEW char[m_lastTextBoxLen+1];
					int ti;
					for(ti=0;ti<m_lastTextBoxLen;ti++)
					{
						tmp[ti] =  m_textBoxString[ti];
					}			
					tmp[ti] = '\0';	
					delete [] ( m_textBoxString );
					m_textBoxString = tmp;
					bHandled = true;
				}
			}
			else
			{
				if ( event.isClicked && m_keyTouchRect[j].Contain(event.position) )
				{
					if ( m_keyNumChars[i] == '^' )
					{
						int len = XP_API_STRLEN( "gameloft.com" );
						m_lastTextBoxLen += len;
						char* tmp = XP_API_NEW char[m_lastTextBoxLen+1];
						XP_API_MEMCPY( tmp, m_textBoxString, m_lastTextBoxLen - len );
						XP_API_MEMCPY( tmp+m_lastTextBoxLen-len, "gameloft.com", len );
						tmp[m_lastTextBoxLen] = '\0';
						delete [] ( m_textBoxString );
						m_textBoxString = tmp;
					}
					else
					{
						m_lastTextBoxLen += 1;
						char* tmp = XP_API_NEW char[m_lastTextBoxLen+1];						
						int ti;
						for(ti=0;ti<m_lastTextBoxLen - 1;ti++)
						{
							tmp[ti] =  m_textBoxString[ti];
						}
						tmp[ti] = m_keyNumChars[i];					
						tmp[ti+1] = '\0';	
						delete [] ( m_textBoxString );
						m_textBoxString = tmp;
					}
					bHandled = true;
				}			
			}
			j++;
		}			
	}	
	return bHandled;
}	

void GLLiveInputBox::deleteTextInTextBox()
{
	m_textBoxString = new char[1];
	m_textBoxString[0] = '\0';
	m_lastTextBoxLen = 0;
}

char* GLLiveInputBox::getInputBoxString()
{
	if(m_textBoxString == NULL)
	{
		return "";
	}
	return m_textBoxString;
}

void GLLiveInputBox::setInputBoxString( char * text )
{
	XP_API_DELETE_ARRAY( m_textBoxString );
	m_lastTextBoxLen = 0;

	if ( NULL != text && XP_API_STRLEN( text ) > 0 )
	{		
		m_textBoxString = XP_API_STRNEW( text );
		m_lastTextBoxLen = XP_API_STRLEN( m_textBoxString );
	}
}