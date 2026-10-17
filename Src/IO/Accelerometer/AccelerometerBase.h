// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AccelerometerBase
//
//		Common implementation for accelerometers
//
//	Notes
//		- The X, Y, Z should be values corresponding to a "current positioning"; i.e. it gives the position of the device
//		  rather than its displcament. It is up to us to keep track of the previous positions, and extrapolate the
//		  displacement. The X, Y, Z should respect a left-handed system, where X expands to the right, Y to the top and
//		  Z away from the "viewer".
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IUpdatable.h"


class AccelerometerBase : public IUpdatable
{
// ** Constants
// ////////////////////////////////////////////////////////
protected:
	static const bool			SHOW_TRACE			= true;

	static const bool			USE_ROLL			= true;	// To disable the calculation of the angle(s) to avoid the
	static const bool			USE_PITCH			= true;	// arctangent calculation.
	static const bool			USE_YAW				= true;


// ** Typedefs
// ////////////////////////////////////////////////////////
public:
	typedef float				MOVE_TYPE;
	typedef float				ANGLE_TYPE;


// ** Members
// ////////////////////////////////////////////////////////
protected:
	MOVE_TYPE					m_x, m_y, m_z;
	MOVE_TYPE					m_prevX, m_prevY, m_prevZ;
	MOVE_TYPE					m_minX, m_minY, m_minZ;
	MOVE_TYPE					m_maxX, m_maxY, m_maxZ;
	ANGLE_TYPE					m_prevAngleXY, m_angleXY;	// roll
	ANGLE_TYPE					m_prevAngleYZ, m_angleYZ;	// pitch
	ANGLE_TYPE					m_prevAngleZX, m_angleZX;	// yaw


// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
protected:
								AccelerometerBase();
public:
	virtual						~AccelerometerBase();


// ** Methods
// ////////////////////////////////////////////////////////
protected:
	void						_trace(const char* msg) const;
	void						_calcAngles();

public:
	void						clear();

	// Events
	virtual void				moved(MOVE_TYPE x, MOVE_TYPE y, MOVE_TYPE z);

	// Utilities
	MOVE_TYPE					getX()				const	{return m_x;};
	MOVE_TYPE					getY()				const	{return m_y;};
	MOVE_TYPE					getZ()				const	{return m_z;};

	MOVE_TYPE					getDX()				const	{return m_x-m_prevX;};
	MOVE_TYPE					getDY()				const	{return m_y-m_prevY;};
	MOVE_TYPE					getDZ()				const	{return m_z-m_prevZ;};

	ANGLE_TYPE					getRoll()			const	{return m_angleXY;};
	ANGLE_TYPE					getPitch()			const	{return m_angleYZ;};
	ANGLE_TYPE					getYaw()			const	{return m_angleZX;};

	ANGLE_TYPE					getDRoll()			const	{return m_angleXY-m_prevAngleXY;};
	ANGLE_TYPE					getDPitch()			const	{return m_angleYZ-m_prevAngleYZ;};
	ANGLE_TYPE					getDYaw()			const	{return m_angleZX-m_prevAngleZX;};

	//Point3D<MOVE_TYPE>			getPosition()		const	{return Point3D<MOVE_TYPE>(m_x, m_y, m_z);};
	//Point3D<MOVE_TYPE>			getDisplacement()	const	{return Point3D<MOVE_TYPE>(getDX(), getDY(), getDY());};

	// ** IUpdatable impl.
	void						update(TimeStamp timestamp);
};
