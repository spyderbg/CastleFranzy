#pragma once

#include "TouchRect.h"
#include "TouchEvent.h"
#include "GLLiveStateHandler.h"

class GLLiveInputBox
{
	char* m_keyNumChars;
	_TouchRect * m_keyTouchRect;	
	char * m_textBoxString;
	int m_lastTextBoxLen;
public:
	GLLiveInputBox(int left =0, int top = 0);	
	virtual ~GLLiveInputBox(void);
	bool updateInputBox(_TOUCH_EVENT event);
	void showTextBox(GLLiveStateHandler * handler);
	char* getInputBoxString();
	void setInputBoxString( char * text );
	void deleteTextInTextBox();
};
