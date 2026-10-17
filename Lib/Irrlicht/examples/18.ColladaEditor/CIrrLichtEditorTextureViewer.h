#ifndef __CIRRLICHTEDITORTEXTUREVIEWER_H__
#define __CIRRLICHTEDITORTEXTUREVIEWER_H__

#include <wx/glCanvas.h>

static int attributes[] = 
{
	WX_GL_DOUBLEBUFFER,
	WX_GL_DEPTH_SIZE, 16,
	0
};

class CTextureViewer
	: public wxGLCanvas
{
protected:
	
public:
	CTextureViewer(wxWindow *pParent) 
		: wxGLCanvas(pParent, wxID_ANY, attributes)
	{
		s_Instance = this;
	}

	void OnPaint(wxPaintEvent& event);

	DECLARE_EVENT_TABLE()
	static CTextureViewer *s_Instance;
};

#endif