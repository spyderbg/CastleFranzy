#include "CIrrLichtEditorDevice.h"
#include "CIrrLichtEditorTextureViewer.h"
#include "CIrrLichtCanvas.h"

#include "irros.h"
#include <wx/thread.h>

extern LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace irr
{

IrrlichtDevice* createEditorDeviceEx(
	const SIrrlichtCreationParameters& parameters)
{
	CEditorDevice* dev = irrnew CEditorDevice(parameters);

	if (dev && !dev->getVideoDriver() && parameters.DriverType != video::EDT_NULL)
	{
		dev->closeDevice(); // destroy window
		dev->run(); // consume quit message
		dev->drop();
		dev = 0;
	}

	return dev;
}

CEditorDevice *CEditorDevice::s_Instance = 0;



bool 
CEditorDevice::run()
{
	os::Timer::tick();
	
	MSG msg;

	CIrrLichtCanvas::s_pInstance->EndSceneModifications();

	CIrrLichtCanvas::s_pInstance->BeginSceneModifications();

	
	bool quit = wxThread::This()->TestDestroy();

	wxMutexLocker lock(MessageMutex);
	for(int i = 0; i < Messages.size(); i++)
	{
		SMessage &msg = Messages[i];
		if (msg.HWnd == HWnd && msg.Msg != WM_PAINT)
			WndProc(HWnd, msg.Msg, msg.WParam, msg.LParam);
		
		if (msg.Msg == WM_QUIT)
		{
			quit = true;
		}
	}

	Messages.clear();

	if (!quit)
		resizeIfNecessary();

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return !quit;
}

};
