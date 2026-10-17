#include "CIrrLichtAnimationPanel.h"
#include "CIrrLichtCanvas.h"

BEGIN_EVENT_TABLE(CAnimationGrid, wxPropertyGridPage)
    EVT_KEY_DOWN(CAnimationGrid::OnKeyEvent )
END_EVENT_TABLE()

void 
CAnimationGrid::OnKeyEvent( wxKeyEvent& event )
{
	switch(event.GetKeyCode())
	{
	case WXK_DELETE:
		break;
	default:
		event.Skip();
	}
}

BEGIN_EVENT_TABLE(CAnimationPanel, wxPropertyGridManager)
    EVT_KEY_DOWN(CAnimationPanel::OnKeyEvent )
	EVT_DROP_FILES(CAnimationPanel::OnDropFile)
END_EVENT_TABLE()

void 
CAnimationPanel::OnKeyEvent( wxKeyEvent& event )
{
	switch(event.GetKeyCode())
	{
	case WXK_DELETE:
		break;
	default:
		event.Skip();
	}
}

void 
CAnimationPanel::OnDropFile(wxDropFilesEvent& event)
{
	std::vector<wxString> validFiles;
	CIrrLichtCanvas::s_pInstance->BeginSceneModifications();
	for(int i = 0; i < event.GetNumberOfFiles(); i++)
	{
		if(event.GetFiles()[i].find(".bdae") != wxString::npos
			|| event.GetFiles()[i].find(".dae") != wxString::npos
			|| event.GetFiles()[i].find(".max") != wxString::npos)
		{
			validFiles.push_back(event.GetFiles()[i]);
		}
	}

	if(validFiles.size() > 0)
	{
		CIrrLichtCanvas::s_pInstance->LoadAnimation(validFiles, m_pRootSceneNode);
	}
	CIrrLichtCanvas::s_pInstance->EndSceneModifications();
}