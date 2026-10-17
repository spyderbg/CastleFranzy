#include "CIrrLichtEditorTextureViewer.h"
#include <wx/wx.h>

BEGIN_EVENT_TABLE(CTextureViewer, wxGLCanvas)
      EVT_PAINT(CTextureViewer::OnPaint)
END_EVENT_TABLE()

void 
CTextureViewer::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	this->SetCurrent();

	glViewport(0, 0, 10, 10);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glFinish();
	SwapBuffers();
}

CTextureViewer *CTextureViewer::s_Instance = 0;