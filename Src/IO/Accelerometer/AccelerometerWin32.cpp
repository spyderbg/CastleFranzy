
#include "stdafx.h"

#include "AccelerometerWin32.h"

#include <windows.h>
#include "Core/irrlicht.h"



// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ctors / Dtor
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AccelerometerWin32::AccelerometerWin32() :
m_isWide(false)
{
}

AccelerometerWin32::~AccelerometerWin32()
{
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AccelerometerWin32::setWide(bool isWide)
{
	if(isWide)	{moved(-0.5f, 0.0f, 0.0f);}
	else		{moved(0.0f, -0.5f, 0.0f);}

	m_isWide = isWide;
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods (irr:IEventReceiver impl.)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AccelerometerWin32::OnEvent(const SEvent& ev)
{
	static bool weControl = false;
	static bool handledOnce = false;
	static bool skipNext = false;
	static s32 lastX = 0;
	static s32 lastY = 0;

	if(ev.EventType == EET_KEY_INPUT_EVENT)
	{
		// The !(...) is to make sure we don't catch the repeated Ctrl `down` state
		if(ev.KeyInput.Key == VK_CONTROL && !(weControl && ev.KeyInput.PressedDown))
		{
			if(weControl = ev.KeyInput.PressedDown)
			{
				HWND h = FindWindowW(NULL, L"Spiderman - PC emu");
				RECT rcWindow;
				GetWindowRect(h, &rcWindow);


				handledOnce = false;
				skipNext = false;

				//ShowCursor(FALSE);
				ClipCursor(&rcWindow);
				SetCapture(FindWindowW(NULL, L"Spiderman - PC emu"));
			}
			else
			{
				ReleaseCapture();
				ClipCursor(NULL);
				//ShowCursor(TRUE);

				setWide(m_isWide);
			}

			return true;
		}
	}
	else if(ev.EventType == EET_MOUSE_INPUT_EVENT && weControl)
	{
		if(ev.MouseInput.Event == EMIE_MOUSE_MOVED && !skipNext)
		{
			if(handledOnce)
			{
				s32 dx = ev.MouseInput.X - lastX;
				s32 dy = ev.MouseInput.Y - lastY;

				moved(float(dx), float(-dy), 0.0f);
			}

			lastX = ev.MouseInput.X;
			lastY = ev.MouseInput.Y;
			handledOnce = true;

			return true;
		}
	}

	return false;
}