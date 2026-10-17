#ifndef __TOUCHPAD_H__
#define __TOUCHPAD_H__

#include "TouchRect.h"
#include "TouchEvent.h"

extern bool gll_isLandscapeRight;

namespace gllive
{
	#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)

	typedef struct 
	{
		bool _touched;
		unsigned int _pointX;
		unsigned int _pointY;
		unsigned int _unused;
	}PadState;

	class CGLLiveTouchpad
	{
	public:
		CGLLiveTouchpad();
		~CGLLiveTouchpad();

		void Update(float x, float y, bool pressed);

		inline int ScreenOffsetX()
		{
			return IsTouched() ? moDispPoint.x - moLastDispPoint.x : 0;
		}

		inline int ScreenOffsetY()
		{
			return IsTouched() ? moDispPoint.y - moLastDispPoint.y : 0;
		}
		
		inline _TouchPoint GetPosition()
		{
	#if PLATFORM == PLATFORM_MAC_OS_X
			_TouchPoint tp;
			tp.x = moLastDispPoint.y;
			tp.y = 320 - moLastDispPoint.x;
			
			if(gll_isLandscapeRight)
			{
				tp.x = 480 - tp.x;
				tp.y = 320 - tp.y;
			}
			return tp;
	#else
			return moDispPoint;
	#endif
		}

		inline _TouchPoint GetLastPosition()
		{
			return moLastDispPoint;
		}
		
		inline unsigned int ScreenX(){ return moDispPoint.x;	}
		inline unsigned int ScreenY(){ return moDispPoint.y;	}


		inline bool IsTouched(){return mbTouched;}

		
		inline bool IsClicked()
		{
			return mbTouched && !mbPrvTouched;
		}

		inline bool IsDoubleClicked()
		{
			return IsClicked() && (miFrameCounter - miClickFrame2) < 10;
		}

		inline bool IsMoving()
		{
			return IsTouched() && (moDispPoint.x != moLastDispPoint.x || moDispPoint.y != moLastDispPoint.y);
		}

		inline bool IsUp()
		{
			return !mbTouched && mbPrvTouched;
		}

		inline PadState GetTouchPadState()
		{
			PadState state;

			state._touched = mbTouched;
			state._pointX = moDispPoint.x;
			state._pointY = moDispPoint.y;

			return state;
		}

		void SetTouchEvent(_TOUCH_EVENT& evn);

	private:
		unsigned int miFrameCounter;

		_TouchPoint moRawPoint;
		_TouchPoint moDispPoint;
		_TouchPoint moLastDispPoint;

		bool  mbTouched;
		bool  mbPrvTouched;
		int   miClickFrame1;
		int   miClickFrame2;
	};

	#else

	class CGLLiveTouchpad
	{
	public:
		CGLLiveTouchpad();
		~CGLLiveTouchpad();

		void Update();

		inline int ScreenOffsetX()
		{
			return 0;
		}

		inline int ScreenOffsetY()
		{
			return 0;
		}

		inline _TouchPoint GetPosition()
		{
			_TouchPoint pt;
			pt.x = 0;
			pt.y = 0;
			return pt;
		}

		inline unsigned int ScreenX(){ return 0;}
		inline unsigned int ScreenY(){ return 0;}


		inline bool IsTouched(){return false;}

		inline bool IsClicked()
		{
			return false;
		}

		inline bool IsDoubleClicked()
		{
			return false;
		}

		inline bool IsMoving()
		{
			return false;
		}

		inline bool IsUp()
		{
			return false;
		}
	};

	#endif
}

#endif
