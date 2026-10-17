#pragma once

#include "TouchRect.h"
#include "TouchEvent.h"
#include "GameStateHandler.h"

class InputBox
{
	char* m_keyNumChars;
	char* m_keyNumCharsShift;
	_TouchRect * m_keyTouchRect;
	char * m_textBoxString;
	int m_lastTextBoxLen;
	bool is_ShiftPressed;
	bool is_capital;
public:
	InputBox(int left =0, int top = 0);	
	virtual ~InputBox(void);
	bool updateInputBox(_TOUCH_EVENT event);
	void showTextBox(GameStateHandler * handler);
	char* getInputBoxString();
	void setInputBoxString( char * text );
	void deleteTextInTextBox();
	void delLastInputBoxCharactor();
	 void setInputBoxPos(int l,int t);

private:

	void SwitchKeyboard();

	int m_left;

	int m_top;

};
