#include "CIrrLichtObjectMeshBufferPanel.h"

CObjectMeshBufferPanel *CObjectMeshBufferPanel::s_pInstance = 0;

BEGIN_EVENT_TABLE(CMaterialChoise, wxChoice)
	EVT_CHOICE(-1, CMaterialChoise::OnSelect)
END_EVENT_TABLE()

void 
CMaterialChoise::OnSelect(wxCommandEvent&)
{
	CObjectMeshBufferPanel::s_pInstance->m_pGridManager->SelectPage(GetSelection());
}