#include "mainApp.h"
#include <irrlicht.h>
#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include "mainApp.h"
#include "CMainFrame.h"

#include "CMessageManager.h"
#include "CIrrLichtLogPanel.h"
#include "CIrrLichtSceneGraph.h"
#include "CIrrLichtObjectPropertiesPanel.h"
#include "CIrrLichtColladaDatabasePanel.h"
#include "CIrrLichtDrawCallsPanel.h"
#include "CIrrLichtCanvas.h"
#include "CIrrLichtEditorDevice.h"

BEGIN_EVENT_TABLE(mainApp, wxApp)
	EVT_IDLE(mainApp::OnIdle) 
	EVT_END_SESSION(mainApp::OnEndSession)
END_EVENT_TABLE()

mainApp::mainApp(void)
{
}

mainApp::~mainApp(void)
{
}

void
mainApp::ProcessMessages()
{
	int iId;
	void *pData;
	while(CMessageManager::GetMessage(iId, pData) == 0)
	{
		switch(iId)
		{
		case CMessageManager::EMT_LOG:
			m_pFrame->m_pLog->AppendText((const char *)pData);
			m_pFrame->m_pLog->AppendText("\n");
			break;
		case CMessageManager::EMT_SCENE_GRAPH_HAS_CHANGED:
			m_pFrame->m_pSceneGraph->RefreshData();
			break;
		case CMessageManager::EMT_ANIMATION_ADDED:
			m_pFrame->m_pProps->RefreshAnimation();
			break;
		case CMessageManager::EMT_DATABASE_ADDED:
			CColladaDatabaseGraph::getInstance().AddDatabase((irr::collada::CColladaDatabase *)pData);
			break;
		case CMessageManager::EMT_DISPLAY_CAPTURE:
			CDrawCallsList::getInstance().setList((std::vector<irr::CBatch *> *)pData);
			break;
		}

		CMessageManager::PopMessage();
	}
}

void 
mainApp::OnIdle(wxIdleEvent& event)
{
	ProcessMessages();
	wxApp::OnIdle(event);
}

bool 
mainApp::OnInit()
{
	/*const wxString name = wxString::Format("ColladaEditor-%s", wxGetUserId().c_str());
    m_pChecker = new wxSingleInstanceChecker(name);
    if ( m_pChecker->IsAnotherRunning() )
    {
        return false;
    }*/
	//SetupCurrentDirectory();

	// Create the main frame window
	m_pFrame = new CMainFrame(NULL, "Collada Editor");

	SetTopWindow(m_pFrame);
	SetExitOnFrameDelete(true);
	/* Show the frame */
	m_pFrame->Show(true);

	return true;
}

void
mainApp::OnEndSession(wxCloseEvent&)
{
	
}




int 
mainApp::OnExit()
{
    //delete m_pChecker;

    return 0;
}


IMPLEMENT_APP(mainApp)
