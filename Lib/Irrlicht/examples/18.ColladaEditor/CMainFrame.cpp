#include "CMainFrame.h"
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/bitmap.h>

#include <Irrlicht.h>
#include <CColladaDatabase.h>
#include "CIrrlichtSceneGraph.h"
#include "CIrrLichtCanvas.h"
#include "CIrrLichtAnimationPanel.h"
#include "CIrrLichtObjectPropertiesPanel.h"
#include "CIrrLichtAttributesPanel.h"
#include "CIrrLichtLogPanel.h"
#include "CIrrLichtObjectMeshBufferPanel.h"
#include <Winreg.h>
#include "CIrrLichtColladaDatabasePanel.h"
#include "CIrrLichtEditorDriver.h"
#include "CIrrLichtDrawCallsPanel.h"
#include "CIrrLichtEditorTextureViewer.h"
#include "CIrrLichtEditorDevice.h"

using namespace irr;
using namespace video;


const wxEventType SelectionChangedEvent = wxNewEventType();


BEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_MENU(FILE_QUIT, CMainFrame::FileQuit)
	EVT_MENU(FILE_OPEN, CMainFrame::FileOpen)
	EVT_MENU(EDIT_TEXTURE_FLIP, CMainFrame::EditTextureFlip)
	EVT_TOOL(TB_PAUSE, CMainFrame::Pause)
	EVT_TOOL(TB_LIGHT, CMainFrame::ListingFlagsChanged)
	EVT_TOOL(TB_GEOMETRY, CMainFrame::ListingFlagsChanged)
	EVT_TOOL(TB_HIERARCHY, CMainFrame::ListingFlagsChanged)
	EVT_TOOL(TB_CAMERA, CMainFrame::ListingFlagsChanged)
	EVT_TOOL(TB_REFRESH_PARAMETERS, CMainFrame::RefreshParameters)
	EVT_TOOL(DCM_DEFAULT, CMainFrame::DisplayCaptureModeChanged)
	EVT_TOOL(DCM_OVERLAY, CMainFrame::DisplayCaptureModeChanged)
	EVT_COMMAND(wxID_ANY, SelectionChangedEvent, CMainFrame::OnSelectionChanged)
	EVT_CLOSE(CMainFrame::OnClose)
END_EVENT_TABLE()

wxBitmap g_bmp;

std::string initIrrLichtDirectory()
{
	HKEY hk; 
	DWORD dwData, dwDisp;

	
	if (RegOpenKeyExA(HKEY_CURRENT_USER, 
		"Software\\Gameloft\\Irrlicht", 
          0, KEY_READ, &hk))
	{
		printf("Could not create the registry key."); 
		return std::string();
	}

	std::wstring irrlicht;

	DWORD dwBufLen = 1024;
	char szPath[1024];
	

	if (RegQueryValueEx(hk,             // subkey handle 
          "Path",					// value name 
          NULL, NULL, 
          (LPBYTE) szPath,				// pointer to value data 
          (DWORD*) (&dwBufLen)))			// data size
	{
	  RegCloseKey(hk); 
	  return std::string();
	}

	RegCloseKey(hk); 
	return szPath;
}

const std::string g_IrrLichtDirectory = initIrrLichtDirectory();



CMainFrame::CMainFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style)
: wxFrame(frame, wxID_ANY, title, pos, wxSize(800, 600), style)
{
	m_mgr.SetManagedWindow(this);
	
	wxMenuBar *menubar = new wxMenuBar;

	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append(FILE_OPEN, _T("&Open"));
	menuFile->Append(FILE_QUIT, _T("&Quit"));
	
	menubar->Append(menuFile, _T("&File"));

//-- Menu Edit
	wxMenu *menuEdit = new wxMenu;
	menubar->Append(menuEdit, _T("&Edit"));
	
	wxMenu *menuEditTexture = new wxMenu;
	menuEdit->Append(-1, _T("&Texture"), menuEditTexture);

	menuEditTexture->Append(EDIT_TEXTURE_FLIP, _T("&Flip"));
//-- Menu View
	wxMenu *menuView = new wxMenu;

	menubar->Append(menuView, _T("&View"));
//-- End of Menu View
	SetMenuBar(menubar);

	m_pToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT );
		
	m_pToolbar->SetToolBitmapSize(wxSize(30,30));
	m_pToolbar->AddTool(TB_PAUSE, "Pause", wxBitmap(g_IrrLichtDirectory + "/media/ColladaEditor/pause.bmp", wxBITMAP_TYPE_BMP ));
	m_pToolbar->Realize();
	
	m_mgr.AddPane(m_pToolbar, wxAuiPaneInfo().
                  Name(wxT("tb1")).Caption(wxT("Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));

	m_pLog = new CLogPanel(this);

	m_mgr.AddPane(m_pLog, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("Log")).Caption(wxT("Log")).
                  Bottom().Layer(0).Position(0).BestSize(256,256).
                  CloseButton(false).Show());

	

	m_pCanvas = new CIrrLichtCanvas(this);


	if(__argc)
	{
		for(int i = 0; i < __argc; i++)
		{
			if(strstr(__argv[i], ".dae") ||
				strstr(__argv[i], ".bdae") ||
				strstr(__argv[i], ".max"))
				m_pCanvas->LoadDatabase(__argv[i]);
			if(strcmp(__argv[i], "/c") == 0)
			{
				m_pCanvas->SetActiveCamera(__argv[i+1]);
				i++;
			}
		}
	}
	//m_pCanvas->LoadDatabase();

	m_pProps = new CObjectNotebook(this);

	m_pSceneNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TAB_SPLIT | 
		wxAUI_NB_TAB_EXTERNAL_MOVE | 
		wxAUI_NB_SCROLL_BUTTONS | 
		wxAUI_NB_WINDOWLIST_BUTTON | 
		wxAUI_NB_BOTTOM | 
		wxBORDER_NONE);

	
	// Scene Graph Panel Configuration
	wxPanel *pSceneGraphPanel = new wxPanel(this);
	wxBoxSizer *pSceneGraphPanelSizer = new wxBoxSizer( wxVERTICAL );

	
	m_pSceneGraphToolbar = new wxToolBar(pSceneGraphPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT );
	m_pSceneGraphToolbar->SetToolBitmapSize(wxSize(16,15));
	m_pSceneGraphToolbar->AddCheckTool(TB_LIGHT, "Light", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/light.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pSceneGraphToolbar->AddCheckTool(TB_GEOMETRY, "Geometry", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/geometry.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pSceneGraphToolbar->AddCheckTool(TB_HIERARCHY, "Hierarchie", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/hierarchy.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pSceneGraphToolbar->AddCheckTool(TB_CAMERA, "Camera", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/camera.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pSceneGraphToolbar->Realize();
	pSceneGraphPanelSizer->Add(m_pSceneGraphToolbar, 0, wxEXPAND);
	
	wxBoxSizer *pSceneGraphSize = new wxBoxSizer( wxVERTICAL );
	m_pSceneGraph = new CSceneGraph(pSceneGraphPanel);
		
	m_pSceneGraph->setPropertyPanel(m_pProps);
	pSceneGraphSize->Add(m_pSceneGraph, 1, wxEXPAND);
	pSceneGraphPanelSizer->Add(pSceneGraphSize, 1, wxEXPAND);
	
	pSceneGraphPanel->SetSizer(pSceneGraphPanelSizer);
	pSceneGraphPanel->Layout();

	m_pSceneNotebook->AddPage(pSceneGraphPanel, "Graph");
	
	// DB Graph Panel Configuration
	wxPanel *pDBGraphPanel = new wxPanel(this);
	wxBoxSizer *pDBGraphPanelSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *pDBGraphSize = new wxBoxSizer( wxVERTICAL );
	m_pDBGraph = new CColladaDatabaseGraph(pDBGraphPanel);

	//m_pDBGraph->setPropertyPanel(m_pProps);
	pDBGraphSize->Add(m_pDBGraph, 1, wxEXPAND);
	pDBGraphPanelSizer->Add(pDBGraphSize, 1, wxEXPAND);
	
	pDBGraphPanel->SetSizer(pDBGraphPanelSizer);
	pDBGraphPanel->Layout();

	m_pSceneNotebook->AddPage(pDBGraphPanel, "Databases");

	// DrawCalls Panel Configuration
	//wxPanel *pDCPanel = new wxPanel(this);
	//wxBoxSizer *pDCPanelSizer = new wxBoxSizer( wxVERTICAL );
	//wxBoxSizer *pDCSize = new wxBoxSizer( wxVERTICAL );
	// Scene Graph Panel Configuration
	wxPanel *pDrawCallPanel = new wxPanel(this);
	wxBoxSizer *pDrawCallPanelSizer = new wxBoxSizer( wxVERTICAL );

	
	m_pDrawCallToolbar = new wxToolBar(pDrawCallPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT );
	m_pDrawCallToolbar->SetToolBitmapSize(wxSize(16,15));
	m_pDrawCallToolbar->AddRadioTool(DCM_DEFAULT, "Draw Calls", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/light.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pDrawCallToolbar->AddRadioTool(DCM_OVERLAY, "Overdraw", wxBitmap( (g_IrrLichtDirectory + "/media/ColladaEditor/geometry.bmp").c_str(), wxBITMAP_TYPE_BMP ));
	m_pDrawCallToolbar->Realize();
	pDrawCallPanelSizer->Add(m_pDrawCallToolbar, 0, wxEXPAND);
	
	wxBoxSizer *pDrawCallSize = new wxBoxSizer( wxVERTICAL );

	m_pDrawCalls = new CDrawCallsList(pDrawCallPanel);
	
	//m_pDBGraph->setPropertyPanel(m_pProps);
	//pDCSize->Add(m_pDrawCalls, 1, wxEXPAND);
	pDrawCallPanelSizer->Add(m_pDrawCalls, 1, wxEXPAND);
	
	pDrawCallPanel->SetSizer(pDrawCallPanelSizer);
	pDrawCallPanel->Layout();

	m_pSceneNotebook->AddPage(pDrawCallPanel, "Draw Calls");

	m_pTextureViewer = new CTextureViewer(this);

	m_pSceneNotebook->AddPage(m_pTextureViewer, "Textures");

	// Scene parameters
	//wxPanel *pParameterPanel = new wxPanel(this);
	//wxBoxSizer *pParameterPanelSizer = new wxBoxSizer( wxVERTICAL );

	//m_pParametersToolbar = new wxToolBar(pParameterPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_FLAT );
	//m_pParametersToolbar->SetToolBitmapSize(wxSize(16,15));
	//m_pParametersToolbar->AddTool(TB_REFRESH_PARAMETERS, "Refresh", wxBitmap( "../../media/ColladaEditor/refresh.bmp", wxBITMAP_TYPE_BMP ));
	//m_pParametersToolbar->Realize();
	//pParameterPanelSizer->Add(m_pParametersToolbar, 0, wxEXPAND);

	//wxBoxSizer *pParameterSize = new wxBoxSizer( wxVERTICAL );

	//m_pSceneAttributesPanel = new CAttributesGrid(pParameterPanel);
	//pParameterPanelSizer->Add(m_pSceneAttributesPanel, 1, wxEXPAND);


	//pParameterPanel->SetSizer(pParameterPanelSizer);
	//pParameterPanel->Layout();

	//m_pSceneNotebook->AddPage(pParameterPanel, "Params");

	m_mgr.AddPane(m_pSceneNotebook, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("ScnGraph")).Caption(wxT("Scene")).
                  Right().Layer(1).Position(0).BestSize(256, 100).
                  CloseButton(false).Show());

	while(m_pCanvas->m_pDevice == 0 || m_pCanvas->m_pDevice->getSceneManager() == 0)
	{
		wxThread::Yield();
	}
	m_pSceneGraph->setSceneManager(m_pCanvas->m_pDevice->getSceneManager());
	//m_pSceneAttributesPanel->setAttributes(m_pCanvas->getSceneManager()->getParameters());
		
	
	
	m_mgr.AddPane(m_pProps, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("ObjProp")).Caption(wxT("Properties")).
                  Right().Layer(1).Position(1).
                  CloseButton(false).Show());

	/*m_mgr.AddPane(m_pProps, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("ObjProp")).Caption(wxT("Object Properties")).
                  Right().Layer(1).Position(1).
                  CloseButton(false).Show());*/

	m_mgr.AddPane(m_pCanvas, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("test9")).Center().Layer(0).Position(0).
                  CloseButton(false).Show());

	
	/*m_pCanvas = new CIrrLichtCanvas(this);

	m_mgr.AddPane(m_pCanvas, wxAuiPaneInfo().Resizable(true).
                  Name(wxT("test10")).Center().Layer(0).Position(0).
                  CloseButton(false).Show());*/

	// Status bar
	m_pStatusBar = new wxStatusBar(this);
	SetStatusBar(m_pStatusBar);

	m_mgr.Update();
}

void
CMainFrame::OnSelectionChanged(wxCommandEvent& event)
{
	m_pSceneGraph->Select((irr::scene::ISceneNode *)event.GetClientData());
	m_pProps->setObject((irr::scene::ISceneNode *)event.GetClientData());
}

void
CMainFrame::RefreshParameters(wxCommandEvent &event)
{
	//m_pSceneAttributesPanel->refreshAttributes();
}

void
CMainFrame::ListingFlagsChanged(wxCommandEvent &event)
{
	int id = event.GetId();
	int iInt = event.GetInt();
	switch(event.GetId())
	{
	case TB_LIGHT:
		m_pSceneGraph->SetListingFlags(CSceneGraph::ELF_LIGHT, (bool)iInt);
		break;
	case TB_CAMERA:
		m_pSceneGraph->SetListingFlags(CSceneGraph::ELF_CAMERA, (bool)iInt);
		break;
	case TB_GEOMETRY:
		m_pSceneGraph->SetListingFlags(CSceneGraph::ELF_GEOMETRY, (bool)iInt);
		break;
	case TB_HIERARCHY:
		m_pSceneGraph->SetListingFlags(CSceneGraph::ELF_HIERARCHY, (bool)iInt);
		break;
	}
}

void
CMainFrame::DisplayCaptureModeChanged(wxCommandEvent &event)
{
	int id = event.GetId();
	int iInt = event.GetInt();
	switch(event.GetId())
	{
	case DCM_DEFAULT:
		((irr::video::CEditorDriver*)m_pCanvas->m_pDevice->getVideoDriver())->DisplayCaptureMode 
			= irr::video::CEditorDriver::EDCM_SELECTED_DRAW_CALL;
		break;
	case DCM_OVERLAY:
		((irr::video::CEditorDriver*)m_pCanvas->m_pDevice->getVideoDriver())->DisplayCaptureMode 
			= irr::video::CEditorDriver::EDCM_OVERDRAW;
		break;
	}
}

CMainFrame::~CMainFrame()
{
	//irr::CEditorDevice::s_Instance->MustQuit = true;
	/*CIrrLichtCanvas::s_pInstance->Wait();
	exit(0);*/
	m_mgr.UnInit();
	//exit(0);
}

void
CMainFrame::OnClose(wxCloseEvent& evt)
{
	// DO TO
	if(CIrrLichtCanvas::s_pInstance)
	{
		CIrrLichtCanvas::s_pInstance->OnClose(evt);
	}
	wxFrame::OnCloseWindow(evt);
}

void
CMainFrame::FileQuit(wxCommandEvent& WXUNUSED(event))
{
	// DO TO
	exit(0); 
}

void
CMainFrame::FileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog file(this);
	file.SetWildcard("*.bdae;*.dae;*.max");
	if(file.ShowModal() == wxID_OK)
	{
		
		wxArrayString paths;
		file.GetPaths(paths);
		CIrrLichtCanvas::SLockSceneForModifications lock(m_pCanvas);
		for(int i = 0; i < paths.size(); i++)
		{
			m_pCanvas->LoadDatabase(paths[i]);
		}
		m_pSceneGraph->RefreshData();
	}
}

extern bool EditorFlipValue;
void
CMainFrame::EditTextureFlip(wxCommandEvent& WXUNUSED(event))
{
	EditorFlipValue = !EditorFlipValue;
}

void
CMainFrame::View(wxCommandEvent& event)
{
}

void
CMainFrame::Capture(wxCommandEvent& WXUNUSED(event))
{
}

void
CMainFrame::Play(wxCommandEvent& WXUNUSED(event))
{
}

void
CMainFrame::NextFrame(wxCommandEvent& WXUNUSED(event))
{
	//m_pToolbar->ToggleTool(TB_PAUSE, true);
}

void
CMainFrame::Pause(wxCommandEvent& WXUNUSED(event))
{
	if(((irr::video::CEditorDriver*)m_pCanvas->m_pDevice->getVideoDriver())->Mode == irr::video::CEditorDriver::E_DEFAULT)
	{
		((irr::video::CEditorDriver*)m_pCanvas->m_pDevice->getVideoDriver())->Mode = 
			irr::video::CEditorDriver::E_BEGIN_CAPTURE_FRAME;
	}
	else
	{
		((irr::video::CEditorDriver*)m_pCanvas->m_pDevice->getVideoDriver())->Mode = 
			irr::video::CEditorDriver::E_DEFAULT;
	}
}

void
CMainFrame::Mute(wxCommandEvent& e)
{
}
