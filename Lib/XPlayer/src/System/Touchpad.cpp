#include "Stdafx.h"
#include "Touchpad.h"

#if TOUCH_MODE > TOUCH_NONE
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)

CTouchpad::CTouchpad()
{
	mbTouched = false;	
	mbPrvTouched = false;
	moDispPoint.x = 0;
	moDispPoint.y = 0;
	moLastDispPoint = moDispPoint;
}

CTouchpad::~CTouchpad()
{

}

extern unsigned int g_mouseX;
extern unsigned int g_mouseY;
extern bool	g_bMousePressed;

void CTouchpad::Update()
{
	miFrameCounter++;

	bool bTouched = mbTouched;

	moLastDispPoint = moDispPoint;

	mbPrvTouched  = mbTouched;

	mbTouched	  = g_bMousePressed;		
	moDispPoint.x = g_mouseX;
	moDispPoint.y = g_mouseY;
	
	
	if(!bTouched && mbTouched)
	{
		moLastDispPoint = moDispPoint;
		miClickFrame1 = miFrameCounter;
	}
	else
	{
		miClickFrame2 = miClickFrame1;
	}
}

void CTouchpad::SetTouchEvent(_TOUCH_EVENT& evn)
{
	evn.isClicked = IsClicked();
	evn.position.x = GetPosition().x;
	evn.position.y = GetPosition().y;
}

#endif
#endif