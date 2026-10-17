#ifndef TOUCH_EVENT_H_H
#define TOUCH_EVENT_H_H


#include "TouchRect.h"

namespace gllive
{
	typedef enum
	{
		TET_Idle,
		TET_Pressed,
		TET_Released,
		TET_Moving
	}TouchEventType;

	typedef struct _event
	{
		bool           isClicked;
		TouchEventType eType;
		_TouchPoint    position;
	}_TOUCH_EVENT;
}
#endif