#pragma once

#include <wx/app.h>
#include <wx/snglinst.h>

class CMainFrame;

class mainApp :
	public wxApp
{
public:
	mainApp(void);
	virtual ~mainApp(void);

	bool OnInit();
	void OnIdle(wxIdleEvent& event);
	int OnExit();
	void OnEndSession(wxCloseEvent&);

protected:
	wxSingleInstanceChecker *m_pChecker;
	void ProcessMessages();
	CMainFrame *m_pFrame;


	DECLARE_EVENT_TABLE();
	DECLARE_NO_COPY_CLASS(mainApp);
};
