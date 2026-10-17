// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_QUATERNION_H_INCLUDED__
#define __IRR_QUATERNION_H_INCLUDED__

#include "irrTypes.h"
#include "irrMath.h"
#include "matrix4.h"
#include "vector3d.h"

namespace irr
{
namespace core
{

//! Quaternion class for representing rotations.
/** It provides cheap combinations and avoids gimbal locks.
Also useful for interpolations. */
class quaternion
{
	public:

		//! Default Constructor
		quaternion() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}

		//! Constructor
		quaternion(f32 x, f32 y, f32 z, f32 w) : X(x), Y(y), Z(z), W(w) { }

		//! Constructor which converts euler angles (radians) to a quaternion
		quaternion(f32 x, f32 y, f32 z);

		//! Constructor which converts euler angles (radians) to a quaternion
		quaternion(const vector3df& vec);

		//! Constructor which converts a matrix to a quaternion
		quaternion(const matrix4& mat);

		//! Equalilty operator
		bool operator==(const quaternion& other) const;

		//! Assignment operator
		inline quaternion& operator=(const quaternion& other);

		//! Matrix assignment operator
		inline quaternion& operator=(const matrix4& other);

		//! Add operator
		quaternion operator+(const quaternion& other) const;

		//! Multiplication operator
		quaternion operator*(const quaternion& other) const;

		//! Multiplication operator with scalar
		quaternion operator*(f32 s) const;

		//! Multiplication operator with scalar
		quaternion& operator*=(f32 s);

		//! Multiplication operator
		vector3df operator*(const vector3df& v) const;

		//! Multiplication operator
		quaternion& operator*=(const quaternion& other);

		//! Calculates the dot product
		inline f32 dotProduct(const quaternion& other) const;

		//! Sets new quaternion
		inline quaternion& set(f32 x, f32 y, f32 z, f32 w);

		//! Sets new quaternion based on euler angles (radians)
		inline quaternion& set(f32 x, f32 y, f32 z);

		//! Sets new quaternion based on euler angles (radians)
		inline quaternion& set(const core::vector3df& vec);

		//! Normalizes the quaternion
		inline quaternion& normalize();

		//! Creates a matrix from this quaternion
		matrix4 getMatrix() const;
		matrix4 getMatrix_transposed() const;

		//! Creates a matrix from this quaternion
		void getMatrix( matrix4 &dest ) const;

		//! Creates a matrix from this quaternion
		void getMatrix_transposed( matrix4 &dest ) const;

		//! Inverts this quaternion
		quaternion& makeInverse();

		//! Set this quaternion to the result of the interpolation between two quaternions
		quaternion& slerp( quaternion q1, quaternion q2, f32 interpolate );

		//! Create quaternion from rotation angle and rotation axis.
		/** Axis must be unit length.
		The quaternion representing the rotation is
		q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
		\param angle Rotation Angle in radians.
		\param axis Rotation axis. */
		quaternion& fromAngleAxis (f32 angle, const vector3df& axis);

		//! Fills an angle (radians) around an axis (unit vector)
		void toAngleAxis (f32 &angle, core::vector3df& axis) const;

		//! Output this quaternion to an euler angle (radians)
		void toEuler(vector3df& euler) const;

		//! Output this quaternion to an euler angle (360 degree)
		void toEulerDegree(vector3df& euler) const;

		//! Set quaternion to identity
		quaternion& makeIdentity();

		//! Set quaternion to represent a rotation from one vector to another.
		quaternion& rotationFromTo(const vector3df& from, const vector3df& to);

		//! Quaternion elements.
		f32 X, Y, Z, W;
};


// Constructor which converts euler angles to a quaternion
inline quaternion::quaternion(f32 x, f32 y, f32 z)
{
	set(x,y,z);
}


// Constructor which converts euler angles to a quaternion
inline quaternion::quaternion(const vector3df& vec)
{
	set(vec.X,vec.Y,vec.Z);
}


// Constructor which converts a matrix to a quaternion
inline quaternion::quaternion(const matrix4& mat)
{
	(*this) = mat;
}


// equal operator
inline bool quaternion::operator==(const quaternion& other) const
{
	return ((X == other.X) &&
		(Y == other.Y) &&
		(Z == other.Z) &&
		(W == other.W));
}


// assignment operator
inline quaternion& quaternion::operator=(const quaternion& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	W = other.W;
	return *this;
}


// matrix assignment operator
inline quaternion& quaternion::operator=(const matrix4& m)
{
	const f32 diag = m(0,0) + m(1,1) + m(2,2);

	if( diag > 0.0f )
	{
		f32 scale = sqrtf(1.0f + diag); // get scale from diagonal

		// TODO: speed this up
		W = 0.5f * scale;
		scale = 0.5f / scale;
		X = ( m(2,1) - m(1,2)) * scale;
		Y = ( m(0,2) - m(2,0)) * scale;
		Z = ( m(1,0) - m(0,1)) * scale;
		
	}
	else if ( m(0,0) > m(1,1) && m(0,0) > m(2,2))
	{
		// 1st element of diag is greatest value
		// find scale according to 1st element
		f32 scale = sqrtf( 1.0f + m(0,0) - m(1,1) - m(2,2));

		X = 0.5f * scale;
		scale = 0.5f / scale;
		Y = (m(0,1) + m(1,0)) * scale;
		Z = (m(2,0) + m(0,2)) * scale;
		W = (m(2,1) - m(1,2)) * scale;
	}
	else if ( m(1,1) > m(2,2))
	{
		// 2nd element of diag is greatest value
		// find scale according to 2nd element
		f32 scale = sqrtf( 1.0f + m(1,1) - m(0,0) - m(2,2));

		// TODO: speed this up
		Y = 0.5f * scale;
		scale = 0.5f / scale;
		X = (m(0,1) + m(1,0) ) * scale;
		Z = (m(1,2) + m(2,1) ) * scale;
		W = (m(0,2) - m(2,0) ) * scale;
	}
	else
	{
		// 3rd element of diag is greatest value
		// find scale according to 3rd element
		f32 scale = sqrtf( 1.0f + m(2,2) - m(0,0) - m(1,1));

		// TODO: speed this up
		Z = 0.5f * scale;
		scale = 0.5f / scale;
		X = (m(0,2) + m(2,0)) * scale;
		Y = (m(1,2) + m(2,1)) * scale;
		W = (m(1,0) - m(0,1)) * scale;
	}
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
	makeInverse();
#endif

	return normalize();
}


// multiplication operator
inline quaternion quaternion::operator*(const quaternion& other) const
{
	quaternion tmp;
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
	tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
	tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
	tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
	tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);
#else
	tmp.W = (W * other.W) - (X * other.X) - (Y * other.Y) - (Z * other.Z);
	tmp.X = (W * other.X) + (X * other.W) + (Y * other.Z) - (Z * other.Y);
	tmp.Y = (W * other.Y) + (Y * other.W) + (Z * other.X) - (X * other.Z);
	tmp.Z = (W * other.Z) + (Z * other.W) + (X * other.Y) - (Y * other.X);
#endif
	return tmp;
}


// multiplication operator
inline quaternion quaternion::operator*(f32 s) const
{
	return quaternion(s*X, s*Y, s*Z, s*W);
}

// multiplication operator
inline quaternion& quaternion::operator*=(f32 s)
{
	X*=s;
	Y*=s;
	Z*=s;
	W*=s;
	return *this;
}

// multiplication operator
inline quaternion& quaternion::operator*=(const quaternion& other)
{
	return (*this = other * (*this));
}

// add operator
inline quaternion quaternion::operator+(const quaternion& b) const
{
	return quaternion(X+b.X, Y+b.Y, Z+b.Z, W+b.W);
}


// Creates a matrix from this quaternion
inline matrix4 quaternion::getMatrix() const
{
	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
	getMatrix_transposed(m);
#else
	getMatrix(m);
#endif
	return m;
}

inline matrix4 quaternion::getMatrix_transposed() const
{
	core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
	getMatrix_transposed(m);
	return m;
}

// Creates a matrix from this quaternion
inline void quaternion::getMatrix( matrix4 &dest ) const
{
	const f32 _2xx = 2.0f*X*X;
	const f32 _2yy = 2.0f*Y*Y;
	const f32 _2zz = 2.0f*Z*Z;
	const f32 _2xy = 2.0f*X*Y;
	const f32 _2xz = 2.0f*X*Z;
	const f32 _2xw = 2.0f*X*W;
	const f32 _2yz = 2.0f*Y*Z;
	const f32 _2yw = 2.0f*Y*W;
	const f32 _2zw = 2.0f*Z*W;

	dest[0] = 1.0f - _2yy - _2zz;
	dest[1] = _2xy + _2zw;
	dest[2] = _2xz - _2yw;
	dest[3] = 0.0f;

	dest[4] = _2xy - _2zw;
	dest[5] = 1.0f - _2xx - _2zz;
	dest[6] = _2yz + _2xw;
	dest[7] = 0.0f;

	dest[8] = _2xz + _2yw;
	dest[9] = _2yz - _2xw;
	dest[10] = 1.0f - _2yy - _2xx;
	dest[11] = 0.0f;

	dest[12] = 0.f;
	dest[13] = 0.f;
	dest[14] = 0.f;
	dest[15] = 1.f;
}

// Creates a matrix from this quaternion
inline void quaternion::getMatrix_transposed( matrix4 &dest ) const
{
	const f32 _2xx = 2.0f*X*X;
	const f32 _2yy = 2.0f*Y*Y;
	const f32 _2zz = 2.0f*Z*Z;
	const f32 _2xy = 2.0f*X*Y;
	const f32 _2xz = 2.0f*X*Z;
	const f32 _2xw = 2.0f*X*W;
	const f32 _2yz = 2.0f*Y*Z;
	const f32 _2yw = 2.0f*Y*W;
	const f32 _2zw = 2.0f*Z*W;

	dest[0] = 1.0f - _2yy - _2zz;
	dest[4] = _2xy + _2zw;
	dest[8] = _2xz - _2yw;
	dest[12] = 0.0f;

	dest[1] = _2xy - _2zw;
	dest[5] = 1.0f - _2xx - _2zz;
	dest[9] = _2yz + _2xw;
	dest[13] = 0.0f;

	dest[2] = _2xz + _2yw;
	dest[6] = _2yz - _2xw;
	dest[10] = 1.0f - _2yy - _2xx;
	dest[14] = 0.0f;

	dest[3] = 0.f;
	dest[7] = 0.f;
	dest[11] = 0.f;
	dest[15] = 1.f;
}



// Inverts this quaternion
inline quaternion& quaternion::makeInverse()
{
	X = -X; Y = -Y; Z = -Z;
	return *this;
}

// sets new quaternion
inline quaternion& quaternion::set(f32 x, f32 y, f32 z, f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
	return *this;
}


// sets new quaternion based on euler angles
inline quaternion& quaternion::set(f32 x, f32 y, f32 z)
{
	f64 angle;

	angle = x * 0.5;
	const f64 sr = sin(angle);
	const f64 cr = cos(angle);

	angle = y * 0.5;
	const f64 sp = sin(angle);
	const f64 cp = cos(angle);

	angle = z * 0.5;
	const f64 sy = sin(angle);
	const f64 cy = cos(angle);

	const f64 cpcy = cp * cy;
	const f64 spcy = sp * cy;
	const f64 cpsy = cp * sy;
	const f64 spsy = sp * sy;

	X = (f32)(sr * cpcy - cr * spsy);
	Y = (f32)(cr * spcy + sr * cpsy);
	Z = (f32)(cr * cpsy - sr * spcy);
	W = (f32)(cr * cpcy + sr * spsy);

	return normalize();
}

// sets new quaternion based on euler angles
inline quaternion& quaternion::set(const core::vector3df& vec)
{
	return set(vec.X, vec.Y, vec.Z);
}

// normalizes the quaternion
inline quaternion& quaternion::normalize()
{
	const f32 n = X*X + Y*Y + Z*Z + W*W;

	if (n == 1)
		return *this;

	//n = 1.0f / sqrtf(n);
	return (*this *= reciprocal_squareroot ( n ));
}


// set this quaternion to the result of the interpolation between two quaternions
inline quaternion& quaternion::slerp(quaternion q1, quaternion q2, f32 time)
{
	f32 angle = q1.dotProduct(q2);

	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	f32 scale;
	f32 invscale;

	if ((angle + 1.0f) > 0.05f)
	{
		if ((1.0f - angle) >= 0.05f) // spherical interpolation
		{
			const f32 theta = acosf(angle);
			const f32 invsintheta = reciprocal(sinf(theta));
			scale = sinf(theta * (1.0f-time)) * invsintheta;
			invscale = sinf(theta * time) * invsintheta;
			*this = (q1*scale) + (q2*invscale);
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
			*this = (q1*scale) + (q2*invscale);
			normalize();
		}
	}
	else
	{
		q2.set(-q1.Y, q1.X, -q1.W, q1.Z);
		scale = sinf(PI * (0.5f - time));
		invscale = sinf(PI * time);
		*this = (q1*scale) + (q2*invscale);
	}

	return *this;
}


// calculates the dot product
inline f32 quaternion::dotProduct(const quaternion& q2) const
{
	return (X * q2.X) + (Y * q2.Y) + (Z * q2.Z) + (W * q2.W);
}


//! axis must be unit length
//! angle in radians
inline quaternion& quaternion::fromAngleAxis(f32 angle, const vector3df& axis)
{
	const f32 fHalfAngle = 0.5f*angle;
	const f32 fSin = sinf(fHalfAngle);
	W = cosf(fHalfAngle);
	X = fSin*axis.X;
	Y = fSin*axis.Y;
	Z = fSin*axis.Z;
	return *this;
}


inline void quaternion::toAngleAxis(f32 &angle, core::vector3df &axis) const
{
	const f32 scale = sqrtf(X*X + Y*Y + Z*Z);

	if (core::iszero(scale) || W > 1.0f || W < -1.0f)
	{
		angle = 0.0f;
		axis.X = 0.0f;
		axis.Y = 1.0f;
		axis.Z = 0.0f;
	}
	else
	{
		const f32 invscale = reciprocal(scale);
		angle = 2.0f * acosf(W);
		axis.X = X * invscale;
		axis.Y = Y * invscale;
		axis.Z = Z * invscale;
	}
}

inline void quaternion::toEuler(vector3df& euler) const
{
	//TODO:yin ze hong: optimize
	toEulerDegree(euler);
	euler *= core::DEGTORAD;

	//const f64 sqw = W*W;
	//const f64 sqx = X*X;
	//const f64 sqy = Y*Y;
	//const f64 sqz = Z*Z;

	//// heading = rotation about z-axis
	//euler.Z = (f32) (atan2(2.0 * (X*Y +Z*W),(sqx - sqy - sqz + sqw)));

	//// bank = rotation about x-axis
	//euler.X = (f32) (atan2(2.0 * (Y*Z +X*W),(-sqx - sqy + sqz + sqw)));

	//// attitude = rotation about y-axis
	//euler.Y = asinf( clamp(-2.0f * (X*Z - Y*W), -1.0f, 1.0f) );
}

inline void quaternion::toEulerDegree(vector3df& euler) const
{
	//TODO:yin ze hong: optimize
	core::matrix4 m;
	m.makeIdentity();
	getMatrix(m);
	euler = m.getRotationDegrees();
}

inline vector3df quaternion::operator* (const vector3df& v) const
{
	// nVidia SDK implementation

	vector3df uv, uuv;
	vector3df qvec(X, Y, Z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * W);
	uuv *= 2.0f;

	return v + uv + uuv;
}

// set quaternion to identity
inline core::quaternion& quaternion::makeIdentity()
{
	W = 1.f;
	X = 0.f;
	Y = 0.f;
	Z = 0.f;
	return *this;
}

inline core::quaternion& quaternion::rotationFromTo(const vector3df& from, const vector3df& to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	vector3df v0 = from;
	vector3df v1 = to;
	v0.normalize();
	v1.normalize();

	const f32 d = v0.dotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return makeIdentity();
	}

	if (d <= -1.0f) // if dot == -1, vectors are opossite
	{
		vector3df axis(1, 0, 0);
		axis = axis.crossProduct(from);
		if (axis.getLengthSQ() == 0) // pick another if colinear
		{
			axis.set(0, 1, 0);
			axis = axis.crossProduct(from);
		}
		axis.normalize();
		return fromAngleAxis(core::PI, axis);
	}

	const f32 s = sqrtf( (1+d)*2 ); // optimize inv_sqrt
	const f32 invs = 1.f / s;
	const vector3df c = // v0.crossProduct(v1)*invs;
		vector3df(v0.Y * v1.Z - v0.Z * v1.Y, v0.Z * v1.X - v0.X * v1.Z, v0.X * v1.Y - v0.Y * v1.X)*invs;
		

	X = c.X;
	Y = c.Y;
	Z = c.Z;
	W = s * 0.5f;

	return *this;
}

#if 0
//I suggest you copy following unit test to your cpp in order to increase the building speed
//all in this macro for unit test by coder
#include <d3dx9math.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

inline void quat_unit_test()
{
	{//Unit Test for quat
		core::matrix4 matI;
		matI.makeIdentity();
		core::quaternion quatk(matI);


		core::matrix4 matT;
		matT.setTranslation(core::vector3df(-30, 80, 45));
		core::matrix4 matR;
		matR.setRotationDegrees(core::vector3df(-30, 80, 45));
		core::matrix4 matM1 = matR*matT;
		core::matrix4 matM2 = matT*matR;
		//confirm TRS sequence. 

		//1.0-------------------------------------------------------------
		//1.0 VEC -> Matrix -> VEC
		core::matrix4 mat;
		mat.setRotationDegrees(core::vector3df(-30, 80, 45));//VEC->Matrix
		core::vector3df v1 = mat.getRotationDegrees();
		//Check: v1 should core::vector3df(-30, 80, 45) or (330, 80, 45)
		//1.0-----------------------End-----------------------------------

		//2.0-------------------------------------------------------------
		//2.0 
		//VEC -> Quat2 -> Matrix -> VEC
		//VEC -> Matrix -> Quat1 -> VEC
		//VEC -> Matrix -> Quat1 -> Matrix -> VEC
		core::quaternion quat1(mat);//Matrix->Quat
		core::quaternion quat2(core::vector3df(-30, 80, 45)*core::DEGTORAD);//VEC->Quat
		//Check:quat1 should equal quat2

		core::vector3df quat1vec;
		quat1.toEulerDegree(quat1vec);
		core::matrix4 quat2mat;
		quat2.getMatrix(quat2mat);
		core::vector3df v2 = quat2mat.getRotationDegrees();
		//Check:v1 should equal v2 and equal quat1vec (330, 80, 45) or (-30, 80, 45)
		//Check:quat2mat should equal with mat
		//2.0-----------------------End-----------------------------------


		core::quaternion quat3,quat4,quat5;
		irr::f32 angle,angle2,angle3; 
		core::vector3df axis,axis2,axis3;
		core::vector3df euler, euler2,euler3;

		//3.0-------------------------------------------------------------
		//other quat algorithm
		// VEC -> Quat ->VEC
		// fromAnlgeAxis ->Quat -> toAngleAxis & VEC

		quat3.fromAngleAxis(45*core::DEGTORAD, core::vector3df(1, 0, 0));
		quat4.fromAngleAxis(20*core::DEGTORAD, core::vector3df(0, 0, 1));

		quat3.toAngleAxis(angle, axis);
		quat4.toAngleAxis(angle2, axis2);

		quat3.toEulerDegree(euler);//(45, 0, 0)
		quat4.toEulerDegree(euler2);//(0, 0, 20)
		//3.0----------------------End------------------------------------
		
		//4.0-------------------------------------------------------------
		quat5 = quat4*quat3;
		quat5.toAngleAxis(angle3, axis3);
		//Check:Confirm: sequence?
		
		quat5.toEulerDegree(euler3);
		//Check:euler3 should be (45, 20, 0)

		core::matrix4 m_q5;
		quat5.getMatrix(m_q5);

		core::matrix4 m_q3, m_q4;
		quat3.getMatrix(m_q3);
		quat4.getMatrix(m_q4);
		core::vector3df mq3vec = m_q3.getRotationDegrees();
		core::vector3df mq4vec = m_q4.getRotationDegrees();
		//Check: mq3vec == (45, 0, 0) && mq4vec == (0, 0, 20)

		core::matrix4 m_q6 = m_q4 * m_q3;
		core::vector3df v5 =m_q5.getRotationDegrees();
		core::quaternion quat6(m_q6);
		core::vector3df v6 =m_q6.getRotationDegrees();
		//Check:quat5 should equal quat6
		//Check:v5 should equal v6 (45, 20, 0)

		//4.0-------------------------End---------------------------------

		//5.0-------------------------------------------------------------
		core::quaternion quat7;
		core::matrix4 m1,m2;
		quat7.rotationFromTo(core::vector3df(0,0,1),core::vector3df(0,1,0)).getMatrix(m1);
		quat7.rotationFromTo(core::vector3df(0,0,1),core::vector3df(0,1,0)).getMatrix_transposed(m2);
		core::vector3df  vv1(0,0,1), vv2(0, 0, 1);
		m1.transformVect(vv1);//vv1 should be (0,0,1) -- the right one
		m2.transformVect(vv2);//vv2 should be (0,0,-1) -- the wrong one
		//5.0-------------------------End---------------------------------

		//6.0-------------------------------------------------------------
		//DirectX quat check

		D3DXMATRIX DXt, DXr, DXf;
		D3DXMatrixTranslation(&DXt, -99, 99, 20);
		D3DXMatrixRotationY(&DXr, 45*core::DEGTORAD);
		D3DXMatrixMultiply(&DXf, &DXr, &DXt);
		//SRT

		D3DXMATRIX DXrx, DXry, DXrz, DXff;
		D3DXMatrixRotationX(&DXrx, -30*core::DEGTORAD);
		D3DXMatrixRotationY(&DXry, 80*core::DEGTORAD);
		D3DXMatrixRotationZ(&DXrz, 45*core::DEGTORAD);
		D3DXMatrixMultiply(&DXff, &DXrx, &DXry);
		D3DXMatrixMultiply(&DXff, &DXff, &DXry);

		core::matrix4 mat1,mat2,mat3;
		mat1.setRotationDegrees(core::vector3df(-30, 0, 0));
		mat2.setRotationDegrees(core::vector3df(0, 80, 0));
		mat3.setRotationDegrees(core::vector3df(0, 0, 45));
		core::matrix4 ourMat1 = mat3*mat2*mat1;

		D3DXQUATERNION q1,q2,q3,qout, q4;
		D3DXVECTOR3 axisxx1(1, 0, 0);
		D3DXVECTOR3 axisxx2(0, 1, 0);
		D3DXVECTOR3 axisxx3(0, 0, 1);
		core::vector3df dxv = core::vector3df(-30, 80, 45)*core::DEGTORAD;
		D3DXQuaternionRotationAxis(&q1, &axisxx1, dxv.X);
		D3DXQuaternionRotationAxis(&q2, &axisxx2, dxv.Y);
		D3DXQuaternionRotationAxis(&q3, &axisxx3, dxv.Z);
		D3DXQuaternionMultiply(&qout, &q1, &q2);
		D3DXQuaternionMultiply(&qout, &qout, &q3);


		D3DXMATRIX DxMat;
		D3DXMatrixRotationQuaternion(&DxMat, &qout);
		D3DXQuaternionRotationMatrix(&q4, &DxMat);
		//Check: DxMat should equal ourMat1 and quat2mat and mat
		//Check: qout should equal quat1 and q4


		D3DXQUATERNION q_y_45;
		D3DXQuaternionRotationAxis(&q_y_45, &axisxx2, 45*core::DEGTORAD);
		D3DXMATRIX mrq;
		D3DXVECTOR4 dxy45v(1, 0, 0, 1);
		D3DXVECTOR4 dxy45out;
		D3DXMatrixRotationQuaternion(&mrq, &q_y_45);
		D3DXVec4Transform(&dxy45out, &dxy45v, &mrq);

		core::quaternion quat12;
		quat12.fromAngleAxis(45*core::DEGTORAD, core::vector3df(0, 1, 0));
		core::matrix4 mat33;
		quat12.getMatrix(mat33);
		core::vector3df out, in(1, 0, 0);
		mat33.transformVect(out, in);
		//Check: Out equal qout
		//6.0-------------------------End---------------------------------
		int a= 0;
	}
}
#endif//#if 0
} // end namespace core
} // end namespace irr

#endif

