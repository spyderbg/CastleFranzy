
#include "PlatformSpecifics/Win32/stdafx.h"

#include "Core/irrlicht.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
#if _DEBUG
#pragma comment(lib, "Box2D_d.lib")
#else
#pragma comment(lib, "Box2D.lib")
#endif
*/


#include <windows.h>
#include "Core/Application.h"
#include "IO/Accelerometer/AccelerometerWin32.h"

#define START_APPLICATION_IN_WIDESCREEN	true
#define WINDOW_TITLE					L"ShrekParty - PC emu"




class ResizingWindow : public IEventReceiver
{
private:
	bool m_wide;
	bool m_frozen;
	mutable bool m_showOne;
	ResizingWindow() : m_wide(START_APPLICATION_IN_WIDESCREEN), m_frozen(false) {};
	static const int X_PADDING = 8;
	static const int Y_PADDING = 27;
public:
	static ResizingWindow* inst() {static ResizingWindow rw; return &rw;};
	virtual ~ResizingWindow() {};

	bool isFrozen() const
	{
		if(m_frozen && m_showOne)
		{
			m_showOne = false;
			return false;
		}

		return m_frozen;
	};

	void updateWindow()
	{
		HWND w = FindWindowW(NULL, WINDOW_TITLE);
		if(w)
		{
			WINDOWPLACEMENT p;
			GetWindowPlacement(w, &p);

			if(m_wide)	{MoveWindow(w, p.rcNormalPosition.left, p.rcNormalPosition.top, 480+X_PADDING, 320+Y_PADDING, TRUE);}
			else		{MoveWindow(w, p.rcNormalPosition.left, p.rcNormalPosition.top, 320+X_PADDING, 480+Y_PADDING, TRUE);}

			((AccelerometerWin32*)Application::GetInstance()->GetAccelerometer())->setWide(m_wide);
		}
	};

	bool OnEvent(const SEvent& ev)
	{
		if(ev.EventType == EET_KEY_INPUT_EVENT)
		{
			if(m_frozen)
			{
				if(ev.KeyInput.Key == VK_SPACE)
				{
					if(!ev.KeyInput.PressedDown)
					{
						m_showOne = true;
						return true;
					}
				}
				else
				{
					m_frozen = false;
					return true;
				}
			}
			else if(ev.KeyInput.Key == VK_PAUSE && !ev.KeyInput.PressedDown)
			{
				m_frozen = true;
				m_showOne = false;
				return true;
			}
			else if(ev.KeyInput.Key == VK_ESCAPE && !ev.KeyInput.PressedDown)
			{
//				Application::GetInstance()->GetDevice()->closeDevice();
				return true;
			}
			else if(ev.KeyInput.Key == VK_F1 && !ev.KeyInput.PressedDown)
			{
				m_wide = !m_wide;
				updateWindow();
				return true;
			}
		}

		return false;
	};
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int main(int argc, char* argv[])
{
	IrrlichtDevice *device = createDevice(video::EDT_OPENGL, dimension2d<s32>(480, 320), 16, false, false, false, 0);
	device->setWindowCaption(WINDOW_TITLE);
	device->setResizeAble(true);

	Application* app = Application::GetInstance();
	app->InitWin32(device);
	app->RegisterForIrrlichtEvents(ResizingWindow::inst());
	ResizingWindow::inst()->updateWindow();

	// end world app ...

	while(device->run())
	{
		if(!ResizingWindow::inst()->isFrozen())
			app->Update();
	}

	app->Quit();

	return 0;
}

