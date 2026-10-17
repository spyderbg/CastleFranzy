#pragma once

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <list>

class CSceneGraph;
class CIrrLichtCanvas;
class CObjectNotebook;
class CAttributesGrid;
class CLogPanel;
class CColladaDatabaseGraph;
class CDrawCallsList;
class CTextureViewer;

enum
{
	FILE_OPEN,
	FILE_QUIT,
	EDIT_TEXTURE_FLIP,
	TB_PAUSE,
	TB_UNDO,
	TB_REDO,
	TB_MOVE,
	TB_SCALE,
	TB_ROTATE,
	TB_SELECTBYNAME,
	TB_LIGHT,
	TB_GEOMETRY,
	TB_HIERARCHY,
	TB_CAMERA,
	TB_REFRESH_PARAMETERS,
	SCENE_GRAPH_SELECTION_CHANGE,
	DCM_DEFAULT,
	DCM_OVERLAY,
};

extern const wxEventType SelectionChangedEvent;

class CMainFrame 
	: public wxFrame
{
public:
	CMainFrame(wxFrame *frame, const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~CMainFrame();
	void FileQuit(wxCommandEvent& WXUNUSED(event));
	void FileOpen(wxCommandEvent& WXUNUSED(event));
	void EditTextureFlip(wxCommandEvent& WXUNUSED(event));
	void Play(wxCommandEvent& WXUNUSED(event));
	void Pause(wxCommandEvent& WXUNUSED(event));
	void NextFrame(wxCommandEvent& WXUNUSED(event));
	void Mute(wxCommandEvent& event);
	void View(wxCommandEvent& event);
	void Capture(wxCommandEvent& WXUNUSED(event));
	void ListingFlagsChanged(wxCommandEvent& WXUNUSED(event));
	void RefreshParameters(wxCommandEvent & WXUNUSED(event));
	void OnSelectionChanged(wxCommandEvent& WXUNUSED(event));
	void DisplayCaptureModeChanged(wxCommandEvent &event);

	CTextureViewer *m_pTextureViewer;
	CDrawCallsList *m_pDrawCalls;
	CColladaDatabaseGraph	*m_pDBGraph;
	CSceneGraph				*m_pSceneGraph;
	//CAttributesGrid		*m_pSceneAttributesPanel;
	CIrrLichtCanvas		*m_pCanvas;
	CObjectNotebook		*m_pProps;
	CLogPanel			*m_pLog;
protected:
	void OnClose(wxCloseEvent&);

	wxAuiNotebook *m_pSceneNotebook;
	wxAuiManager m_mgr;
	wxToolBar *m_pToolbar;
	wxToolBar *m_pSceneGraphToolbar;
	wxToolBar *m_pDrawCallToolbar;
	wxToolBar *m_pParametersToolbar;
	wxStatusBar *m_pStatusBar;
	DECLARE_EVENT_TABLE()
};
