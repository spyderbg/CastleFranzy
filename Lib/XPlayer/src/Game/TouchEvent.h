#ifndef TOUCH_EVENT_H_H
#define TOUCH_EVENT_H_H


#include "TouchRect.h"

typedef struct _event
{
	bool isClicked;
	_TouchPoint position;
}_TOUCH_EVENT;

#endif