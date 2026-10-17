#include "GLLiveStdafx.h"
#include "TouchRect.h"

#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)

#define SMART_OFFSET			1

namespace gllive
{
	IconState CTouchRect::GetIconState(int x, int y)
	{
		const int left = m_left ;
		const int right = m_left + m_width;
		const int top = m_top;
		const int bottom = m_top + m_height;

		if(x < left  - SMART_OFFSET || y < top - SMART_OFFSET || x > right + SMART_OFFSET || y > bottom + SMART_OFFSET)
			return IconNone;
		
		if(abs( x -  right) <= SMART_OFFSET && abs( y -  bottom) <= SMART_OFFSET)
			return IconResizeWHRB;

		if(x <= left + SMART_OFFSET)
			return IconResizeWL;

		if( x >= right - SMART_OFFSET)
			return IconResizeWR;

		if(y <= top + SMART_OFFSET)
			return IconResizeHT;

		if( y >=  bottom - SMART_OFFSET)
			return IconResizeHB;

		return IconMove;
	}
}
#endif