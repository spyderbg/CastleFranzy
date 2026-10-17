///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

class TextInput;

///////////////////////////////////////////////////////////////////////

class WTextInput
{
protected:

	TextInput* m_textInputInstance;

public:

	WTextInput();
	~WTextInput();

	void Init(/*unsigned short* */ char* textBuf, unsigned int bufSize, int x, int y, int w, int h, bool pass, int Orientation);
	void mySetOrientation(int orientationNormal,int x, int y, int w, int h);
	void Enable();
	void Disable();
	void Remove();

};
