#ifndef __CTIMECONTROLLER_H__
#define __CTIMECONTROLLER_H__

#include <ITimeController.h>

class CTimeController
	: public ITimeController
{
	//! Get a transformated time based on the current time
	virtual void Update(s32 timeMs) 
	{
		SetCtrlTime(timeMs);
	};
}

#endif