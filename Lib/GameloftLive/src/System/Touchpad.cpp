#include "GLLiveStdafx.h"
#include "Touchpad.h"

#if TOUCH_MODE > TOUCH_NONE
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)

namespace gllive
{
	CGLLiveTouchpad::CGLLiveTouchpad()
	{
		mbTouched = false;	
		mbPrvTouched = false;
		moDispPoint.x = 0;
		moDispPoint.y = 0;
		moLastDispPoint = moDispPoint;
	}

	CGLLiveTouchpad::~CGLLiveTouchpad()
	{

	}


	void CGLLiveTouchpad::Update(float x, float y, bool pressed)
	{
		miFrameCounter++;

		bool bTouched = mbTouched;

		moLastDispPoint = moDispPoint;

		mbPrvTouched  = mbTouched;

		mbTouched	  = pressed;

		moDispPoint.x = x;
		moDispPoint.y = y;
		
		
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

	void CGLLiveTouchpad::SetTouchEvent(_TOUCH_EVENT& evn)
	{
		evn.isClicked = mbTouched && !mbPrvTouched;
		evn.position.x = GetPosition().x;
		evn.position.y = GetPosition().y;
		if(mbPrvTouched)
		{
			if(mbTouched)
			{
				evn.eType = TET_Moving;
			}
			else
			{
				evn.eType = TET_Released;
			}
		}
		else
		{
			if(mbTouched)
			{
				evn.eType = TET_Pressed;
			}
			else
			{
				evn.eType = TET_Idle;
			}
		}
	}
}

#endif
#endif