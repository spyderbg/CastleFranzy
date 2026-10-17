#pragma once

#include "threadex.h"
#include <wx/frame.h>
#include <vector>
#include <string>

namespace irr
{
	class IrrlichtDevice;
namespace video
{
class IVideoDriver;
};
namespace scene
{
class ICameraSceneNode;
class ISceneManager;
class ISceneNode;
};
};

class CIrrLichtCanvas;

struct Loading
{
	std::vector<wxString> Filenames;
	irr::scene::ISceneNode *pSceneNode;
};

class CIrrLichtLoadingThread
	: public wxThreadEx
{
public:
	CIrrLichtLoadingThread() : wxThreadEx(wxTHREAD_JOINABLE), m_pDedicatedCanvas(0) {}
	virtual ~CIrrLichtLoadingThread()
	{

	}
	virtual ExitCode Entry();

	void setDedicatedCanvas(CIrrLichtCanvas *pDedicatedCanvas) { m_pDedicatedCanvas = pDedicatedCanvas; }

protected:
	CIrrLichtCanvas *m_pDedicatedCanvas;
};

class CIrrLichtGameThread
	: public wxThreadEx
{
public:
	CIrrLichtGameThread() : wxThreadEx(), m_pDedicatedCanvas(0) {}

	virtual ExitCode Entry();

	void setDedicatedCanvas(CIrrLichtCanvas *pDedicatedCanvas) { m_pDedicatedCanvas = pDedicatedCanvas; }

	static CIrrLichtGameThread *s_pInstance;
protected:
	CIrrLichtCanvas *m_pDedicatedCanvas;
};

class CIrrLichtCanvas
	: public wxWindow
	//, public wxThread
{
public:
	friend class CIrrLichtLoadingThread;

	CIrrLichtCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("Gameplay"));

	virtual ~CIrrLichtCanvas();
	void OnClick();
protected:
	void OnDropFile(wxDropFilesEvent&);
	void OnResize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	void Paint();
	
	void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);
	

	void pushLoading(Loading& loading);
	bool popLoading(bool validity, Loading& loading);

public:
	static CIrrLichtCanvas *s_pInstance;
	void OnClose(wxCloseEvent &);
	//virtual ExitCode Entry();
	
	irr::scene::ISceneManager *getSceneManager() const { return m_pSceneManager; }
	void LoadDatabase(const char *pFilename);
	void LoadAnimation(std::vector<wxString> Filenames, irr::scene::ISceneNode *pNode);
	
	struct SLockSceneForModifications
	{
		SLockSceneForModifications(CIrrLichtCanvas *canvas)
			: Canvas(canvas)
		{
			Canvas->BeginSceneModifications();
		};

		~SLockSceneForModifications()
		{
			Canvas->EndSceneModifications();
		};

		CIrrLichtCanvas *Canvas;
	};

	//! Restrict scene modifications to the current thread
	void BeginSceneModifications()
	{
		 s_SceneModiticationLock.Lock();
	}

	//! Release lock on the scene modifications
	void EndSceneModifications()
	{
		s_SceneModiticationLock.Unlock();
	}

	//! Switch the current camera to use the selected camera
	void SetActiveCamera(const char *pUID);

	bool m_bSizeHasChanged;
	int m_iWidth;
	int m_iHeight;

	irr::scene::ICameraSceneNode	*m_pCamera;
	irr::scene::ISceneManager		*m_pSceneManager;
	irr::IrrlichtDevice				*m_pDevice;
	irr::video::IVideoDriver		*m_pIrrlichtDriver;

	bool m_bActiveCameraIsSet;
	std::string m_ActiveCameraUID;

	void ProcessLoading();
	
	

	std::vector<Loading>	m_Loadings;
	int						m_iLoadingThread;
	CIrrLichtLoadingThread	*m_pLoadingThread;
	CIrrLichtGameThread		*m_pGameThread;


	static wxMutex s_SceneModiticationLock;
	wxCondition m_ThreadWakeCondition;
	wxMutex m_ThreadWakeMutex;

	DECLARE_EVENT_TABLE()
};
