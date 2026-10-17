#pragma once

#include "TouchRect.h"

using namespace gllive;

#define MESSAGEBOX_WIDTH            288
#define MESSAGEBOX_HEIGHT           160

class GLLiveStateHandler;

class CGLLiveMessageBox
{
public:
	CGLLiveMessageBox(const char *pTitle = NULL, const char *pText = NULL);
	virtual ~CGLLiveMessageBox(void);

	void SetText(const char *pText);
	void SetTitle(const char *pText);
	void Repaint( GLLiveStateHandler * handler );
	bool IsOn(const _TouchPoint &pt) const;
	//void Update();
	//void SetShowTip(bool show);
private:
	char *m_pText;
	char *m_pTitle;
	//unsigned long m_frame;
	//bool m_bShowTip;

	_TouchRect m_rect;
};
