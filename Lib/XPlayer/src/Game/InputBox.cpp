#include "StdAfx.h"
#include "InputBox.h"
#include "../../include/Porting.h"
#include "memory.h"

InputBox::InputBox(int left, int top)
{
	m_textBoxString = NULL;
	m_lastTextBoxLen = 0;	

	m_keyNumChars = XP_API_NEW char[128];
	XP_API_MEMSET( m_keyNumChars, 0, 128 );
	XP_API_SPRINTF( m_keyNumChars,"abcdefgh+|ijklmnop|qrstuvw<C");

	m_keyNumCharsShift = XP_API_NEW char[128];
	XP_API_MEMSET( m_keyNumCharsShift, 0, 128 );
	XP_API_SPRINTF( m_keyNumCharsShift,"xyz12345+|67890!@,|.:&*() <C");

	is_ShiftPressed = true;
	is_capital = false;
	m_keyTouchRect = NULL;
	m_left = left;
	m_top = top;

	SwitchKeyboard();

}

InputBox::~InputBox(void)
{
	XP_API_DELETE_ARRAY(m_keyTouchRect);
	m_keyTouchRect = NULL;
	XP_API_DEL(m_keyNumChars);
	XP_API_DEL(m_keyNumCharsShift);
	m_keyNumChars = NULL;
	XP_API_DEL(m_textBoxString);
	m_textBoxString = NULL;
}

void InputBox::SwitchKeyboard()
{
	int keyNumLen = 0;
	char * tempKeyBoard;

	is_ShiftPressed = !is_ShiftPressed;

	if( is_ShiftPressed )
	{
		tempKeyBoard = m_keyNumCharsShift;
	}
	else
	{
		tempKeyBoard = m_keyNumChars;
	}

	keyNumLen = XP_API_STRLEN( tempKeyBoard );

	XP_API_DELETE_ARRAY( m_keyTouchRect );
	m_keyTouchRect = XP_API_NEW _TouchRect[ keyNumLen - 2];

	int i = 0;
	int j = 0;
	int top = m_top;
	int left = m_left;
	int l = m_left;

	for (i=0; i < keyNumLen;i++)
	{					 
		if( tempKeyBoard[i] == '|' )
		{
			top += 35;
			l = left;
		}
		else
		{
			l += 32;
			m_keyTouchRect[j].left = l;
			m_keyTouchRect[j].top = top;
			m_keyTouchRect[j].width = 30;
			m_keyTouchRect[j].height = 33;
			j++;
		}
	}	
}

void InputBox::showTextBox(GameStateHandler *handler)
{		
	int keyNumLen = 0;	

	if ( !is_ShiftPressed )
	{
		keyNumLen = XP_API_STRLEN( m_keyNumChars );
	}
	else
	{
		keyNumLen = XP_API_STRLEN( m_keyNumCharsShift );
	}

	char tmp[2];
	int j = 0;
	for ( int i=0; i<keyNumLen; i++ )
	{					 
		XP_API_MEMSET( tmp, 0, 2 );
		if( is_ShiftPressed == true )
		{
			tmp[0] = m_keyNumCharsShift[i];
		}
		else
		{
			tmp[0] = m_keyNumChars[i];
		}			
		tmp[1] = '\0';

		if( tmp[0] != '|' )
		{
			handler->DrawButton(m_keyTouchRect[j]);
			handler->DrawString(m_keyTouchRect[j].left + 2,m_keyTouchRect[j].top + 2, tmp );
			j++;
		}
	}	
}

bool InputBox::updateInputBox(_TOUCH_EVENT event)
{
	bool bHandled = false;
	int keyNumLen = 0;	
	char * tempKeyBoard;

	if( is_ShiftPressed )
	{
		tempKeyBoard = m_keyNumCharsShift;
	}
	else
	{
		tempKeyBoard = m_keyNumChars;
	}
	keyNumLen = XP_API_STRLEN( tempKeyBoard );

	for ( int i=0,j=0; i<keyNumLen; i++ )
	{
		if(tempKeyBoard[i] != '|')
		{
			if(tempKeyBoard[i] == '+')
			{
				if(event.isClicked && m_keyTouchRect[j].Contain(event.position))
				{
					SwitchKeyboard();
					bHandled = false;
				}
			}			
			else if(tempKeyBoard[i] == '<')
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
					delete ( m_textBoxString );
					m_textBoxString = tmp;
					bHandled = true;
				}
			}
			else if(tempKeyBoard[i] == 'C')
			{
				is_capital = !is_capital;
			}
			else
			{
				if ( event.isClicked && m_keyTouchRect[j].Contain(event.position) )
				{
					if ( tempKeyBoard[i] == '^' )
					{
						int len = XP_API_STRLEN( "gameloft.com" );
						m_lastTextBoxLen += len;
						char* tmp = XP_API_NEW char[m_lastTextBoxLen+1];
						XP_API_MEMCPY( tmp, m_textBoxString, m_lastTextBoxLen - len );
						XP_API_MEMCPY( tmp+m_lastTextBoxLen-len, "gameloft.com", len );
						tmp[m_lastTextBoxLen] = '\0';
						delete ( m_textBoxString );
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
						tmp[ti] = tempKeyBoard[i];					
						tmp[ti+1] = '\0';
						delete ( m_textBoxString );
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

void InputBox::deleteTextInTextBox()
{
	XP_API_DEL( m_textBoxString );
	m_textBoxString = XP_API_NEW char[1];
	m_textBoxString[0] = '\0';
	m_lastTextBoxLen = 0;
}

char* InputBox::getInputBoxString()
{
	if ( NULL == m_textBoxString )
	{
		return NULL;
	}
	return m_textBoxString;
}

void InputBox::setInputBoxString( char * text )
{
	XP_API_DEL( m_textBoxString );
	m_lastTextBoxLen = 0;

	if ( NULL != text && XP_API_STRLEN( text ) > 0 )
	{		
		m_textBoxString = XP_API_STRNEW( text );
		m_lastTextBoxLen = XP_API_STRLEN( m_textBoxString );
	}
}
void InputBox::delLastInputBoxCharactor()
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
	delete ( m_textBoxString );
	m_textBoxString = tmp;
}
void InputBox::setInputBoxPos(int l,int t)
{
	m_left=l;
	m_top=t;
	SwitchKeyboard(); 
}
