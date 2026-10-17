#pragma once

#include "TouchRect.h"
#include "GLLiveMenu.h"

using namespace gllive;

class GLLiveStateHandler;
class GLLiveMenu;

class CGLLiveMessageBox
{
public:
	CGLLiveMessageBox(const char *pTitle = NULL, const char *pText = NULL);
	virtual ~CGLLiveMessageBox(void);

	void SetText(const char *pText);
	void SetTitle(const char *pText);
	void SetButtonType(int type = 0);
	void Repaint( GLLiveStateHandler * handler );
	bool IsOn(const _TouchPoint &pt) const;
	//void Update();
	//void SetShowTip(bool show);

	static GLLiveMenu *GenerateConfirmMenu(gllive::GLXPlayerObserver *menuObserver);
private:
	char *m_pText;
	char *m_pTitle;
	//unsigned long m_frame;
	//bool m_bShowTip;
	bool m_hasButton;

	_TouchRect m_rect;
};
