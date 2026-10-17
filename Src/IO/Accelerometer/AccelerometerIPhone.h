// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AccelerometerIPhone
//
//		Implementation of AccelerometerBase, for the iPhone
//
//	Notes:
//		If the handheld is held straight up, the accelerometer values are the opposite of a left-handed axis system.
//		(i.e. X go to the left, Y go down and Z toward the viewer... so to keep a standard axis system, we revert the values)
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IO_ACCELEROMETER_ACCELEROMETERIPHONE_H
#define IO_ACCELEROMETER_ACCELEROMETERIPHONE_H

#include "IO/Accelerometer/AccelerometerBase.h"



class AccelerometerIPhone : public AccelerometerBase
{
// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
public:
						AccelerometerIPhone();
	virtual				~AccelerometerIPhone();


// ** Methods
// ////////////////////////////////////////////////////////
public:
	// AccelerometerBase impl.
	void				moved(MOVE_TYPE x, MOVE_TYPE y, MOVE_TYPE z);
};



#endif