#include "CIrrLichtLogPanel.h"

BEGIN_EVENT_TABLE(CLogPanel, wxTextCtrl)
	EVT_IDLE(CLogPanel::OnIdle) 
END_EVENT_TABLE()


void 
CLogPanel::OnIdle(wxIdleEvent& event)
{
	
}