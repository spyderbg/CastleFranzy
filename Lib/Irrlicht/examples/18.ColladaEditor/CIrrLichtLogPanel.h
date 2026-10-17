#pragma once
#include <iostream>
#include <sstream>  
#include <wx/textctrl.h>
#include "CLogger.h"
#include "CMessageManager.h"

class CLogPanel
	: public wxTextCtrl
	, public irr::CLogger
{
public:
	CLogPanel(wxWindow *pParent) 
		: wxTextCtrl(pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
			wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP)
		, irr::CLogger(0)
		//, redirector(this)
	{
		InformationMask = (1 << irr::ELL_INFORMATION) 
			| (1 << irr::ELL_WARNING)
			| (1 << irr::ELL_ERROR) 
			| (1 << irr::ELL_NONE);
		irr::os::Printer::Logger = this;
		SetEditable(false);
	}

	void OnIdle(wxIdleEvent& event);
	//wxStreamToTextRedirector redirector;

	//! Prints out a text into the log
	virtual void log(const char* text, irr::ELOG_LEVEL ll=irr::ELL_INFORMATION)
	{
		if ((1 << (int)ll) & ~InformationMask)
			return;

		CMessageManager::LogMessage(text);
		irr::os::Printer::print(text);
	}

protected:
	int InformationMask;

	DECLARE_EVENT_TABLE();
	DECLARE_NO_COPY_CLASS(CLogPanel);
};