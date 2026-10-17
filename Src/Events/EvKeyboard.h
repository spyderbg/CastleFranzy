// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EvKeyboard
//
//		Keyboard events
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Events/Events.h"
#include "Entities/IEvent.h"


#if defined(WIN32)
#include <windows.h>
#endif

///////////////////////////////////////////////////////////////////////

// ** Main Class
// ////////////////////////////////////////////////////////
class EvKeyboard : public IEvent
{
// ** Enums
// ////////////////////////////////////////////////////////
public:
	enum Keys
	{
		KEY_SPACE	= ' ',

		KEY_0		= '0',
		KEY_1		= '1',
		KEY_2		= '2',
		KEY_3		= '3',
		KEY_4		= '4',
		KEY_5		= '5',
		KEY_6		= '6',
		KEY_7		= '7',
		KEY_8		= '8',
		KEY_9		= '9',

		KEY_A		= 'A',
		KEY_B		= 'B',
		KEY_C		= 'C',
		KEY_D		= 'D',
		KEY_E		= 'E',
		KEY_F		= 'F',
		KEY_G		= 'G',
		KEY_H		= 'H',
		KEY_I		= 'I',
		KEY_J		= 'J',
		KEY_K		= 'K',
		KEY_L		= 'L',
		KEY_M		= 'M',
		KEY_N		= 'N',
		KEY_O		= 'O',
		KEY_P		= 'P',
		KEY_Q		= 'Q',
		KEY_R		= 'R',
		KEY_S		= 'S',
		KEY_T		= 'T',
		KEY_U		= 'U',
		KEY_V		= 'V',
		KEY_W		= 'W',
		KEY_X		= 'X',
		KEY_Y		= 'Y',
		KEY_Z		= 'Z',

#if defined(WIN32) || defined(OS_LINUX)
		KEY_F1		= irr::KEY_F1,
		KEY_F2		= irr::KEY_F2,
		KEY_F3		= irr::KEY_F3,
		KEY_F4		= irr::KEY_F4,

		KEY_F5		= irr::KEY_F5,
		KEY_F6		= irr::KEY_F6,
		KEY_F7		= irr::KEY_F7,
		KEY_F8		= irr::KEY_F8,

		KEY_F9		= irr::KEY_F9,
		KEY_F10		= irr::KEY_F10,
		KEY_F11		= irr::KEY_F11,
		KEY_F12		= irr::KEY_F12,

		KEY_N0		= irr::KEY_NUMPAD0,
		KEY_N1		= irr::KEY_NUMPAD1,
		KEY_N2		= irr::KEY_NUMPAD2,
		KEY_N3		= irr::KEY_NUMPAD3,
		KEY_N4		= irr::KEY_NUMPAD4,
		KEY_N5		= irr::KEY_NUMPAD5,
		KEY_N6		= irr::KEY_NUMPAD6,
		KEY_N7		= irr::KEY_NUMPAD7,
		KEY_N8		= irr::KEY_NUMPAD8,
		KEY_N9		= irr::KEY_NUMPAD9,

		KEY_DECIMAL		= irr::KEY_DECIMAL,
		KEY_DIVIDE		= irr::KEY_DIVIDE,
		KEY_MULTIPLY	= irr::KEY_MULTIPLY,
		KEY_SUBTRACT	= irr::KEY_SUBTRACT,
		KEY_ADD			= irr::KEY_ADD,

		KEY_UP			= irr::KEY_UP,
		KEY_DOWN		= irr::KEY_DOWN,
		KEY_LEFT		= irr::KEY_LEFT,
		KEY_RIGHT		= irr::KEY_RIGHT,

		KEY_INSERT		= irr::KEY_INSERT,
		KEY_DELETE		= irr::KEY_DELETE,
		KEY_HOME		= irr::KEY_HOME,
		KEY_END			= irr::KEY_END,
		KEY_PGUP		= irr::KEY_PRIOR,
		KEY_PGDN		= irr::KEY_NEXT,

		KEY_ESC			= irr::KEY_ESCAPE,
		KEY_PRINT		= irr::KEY_SNAPSHOT,
		KEY_SLOCK		= irr::KEY_SCROLL,
		KEY_PAUSE		= irr::KEY_SLEEP,
		KEY_NLOCK		= irr::KEY_NUMLOCK,

		KEY_SHIFT		= irr::KEY_SHIFT,
		KEY_CONTROL		= irr::KEY_CONTROL,
#else
		KEY_F1		= 'Z'+1,
		KEY_F2		= 'Z'+2,
		KEY_F3		= 'Z'+3,
		KEY_F4		= 'Z'+4,

		KEY_F5		= 'Z'+5,
		KEY_F6		= 'Z'+6,
		KEY_F7		= 'Z'+7,
		KEY_F8		= 'Z'+8,

		KEY_F9		= 'Z'+9,
		KEY_F10		= 'Z'+10,
		KEY_F11		= 'Z'+11,
		KEY_F12		= 'Z'+12,

		KEY_N0		= 'Z'+30,
		KEY_N1		= 'Z'+31,
		KEY_N2		= 'Z'+32,
		KEY_N3		= 'Z'+33,
		KEY_N4		= 'Z'+34,
		KEY_N5		= 'Z'+35,
		KEY_N6		= 'Z'+36,
		KEY_N7		= 'Z'+37,
		KEY_N8		= 'Z'+38,
		KEY_N9		= 'Z'+39,

		KEY_DECIMAL		= 'Z'+40,
		KEY_DIVIDE		= 'Z'+41,
		KEY_MULTIPLY	= 'Z'+42,
		KEY_SUBTRACT	= 'Z'+43,
		KEY_ADD			= 'Z'+44,

		KEY_UP			= 'Z'+45,
		KEY_DOWN		= 'Z'+46,
		KEY_LEFT		= 'Z'+47,
		KEY_RIGHT		= 'Z'+48,

		KEY_INSERT		= 'Z'+49,
		KEY_DELETE		= 'Z'+50,
		KEY_HOME		= 'Z'+51,
		KEY_END			= 'Z'+52,
		KEY_PGUP		= 'Z'+53,
		KEY_PGDN		= 'Z'+54,

		KEY_SHIFT		= 'Z'+55,
		KEY_CONTROL		= 'Z'+56,
#endif		
	};


// ** Members
// ////////////////////////////////////////////////////////
public:
	wchar_t				chr;
	int					code;
	bool				pressed;		// key released if false
	bool				shiftPressed;
	bool				ctrlPressed;
	bool				altPressed;


// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
public:
						EvKeyboard(int kCode, wchar_t kChr, bool kPressed, bool sPressed, bool cPressed, bool aPressed)
							: IEvent(Events::KEYBOARD), code(kCode), chr(kChr), pressed(kPressed),
							shiftPressed(sPressed), ctrlPressed(cPressed), altPressed(aPressed)
						{};
	virtual				~EvKeyboard() {};
};

