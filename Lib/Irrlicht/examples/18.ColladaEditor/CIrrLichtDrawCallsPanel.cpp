#include "CIrrLichtDrawCallsPanel.h"
#include "CIrrLichtEditorDriver.h"
#include "CIrrLichtCanvas.h"

BEGIN_EVENT_TABLE(CDrawCallElements, CAttributesGrid)
	  EVT_PG_SELECTED(wxID_ANY, CDrawCallElements::OnSelected)
END_EVENT_TABLE()

void CDrawCallElements::OnSelected(wxPropertyGridEvent &event)
{
	CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);

	if(m_pList)
	{
		if(event.GetProperty())
		{
			for(int i = 0; i < m_pList->size(); i++)
			{
				(*m_pList)[i]->Show = false;
			}

			int i = event.GetProperty()->GetArrIndex();
			if(m_pList->size() > i) 
			{
				(*m_pList)[i]->Show = true;
			}
		}
		else
		{
			for(int i = 0; i < m_pList->size(); i++)
			{
				(*m_pList)[i]->Show = true;
			}
		}
	}
}

CDrawCallsList * CDrawCallsList::s_pInstance = 0;

CDrawCallFilters::CDrawCallFilters()
{
	irr::video::CEditorDrawFilter::s_Instance.serializeAttributes(this, 0);
}
	
void 
CDrawCallFilters::OnPostPropertyGridChanged()
{
	irr::video::CEditorDrawFilter::s_Instance.deserializeAttributes(this, 0);
}