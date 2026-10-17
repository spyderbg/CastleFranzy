#ifndef __CIRRLICHTEDITORDEVICE_H__
#define __CIRRLICHTEDITORDEVICE_H__

#include "CIrrDeviceWin32.h"
#include <vector>
#include <wx/thread.h>

namespace irr
{

class CEditorDevice
	: public CIrrDeviceWin32
{
protected:

	struct SMessage
	{
		SMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			: HWnd(hWnd)
			, Msg(message)
			, WParam(wParam)
			, LParam(lParam)
		{
		}

		HWND	HWnd;
		UINT	Msg;
		WPARAM	WParam;
		LPARAM	LParam;
	};

	std::vector<SMessage> Messages;
	wxMutex MessageMutex;
public:
	CEditorDevice(const SIrrlichtCreationParameters& params)
		: CIrrDeviceWin32(params)
		, MustQuit(false)
	{
		s_Instance = this;
		s_Instance->grab();
	}

	virtual ~CEditorDevice() {}

	void pushMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		SMessage msg(hWnd, message, wParam, lParam);
		wxMutexLocker lock(MessageMutex);
		Messages.push_back(msg);
	}

	//! runs the device. Returns false if device wants to be deleted
	virtual bool run();

	static CEditorDevice *s_Instance;
	bool MustQuit;
};

IrrlichtDevice* createEditorDeviceEx(const SIrrlichtCreationParameters& parameters);

};

#endif