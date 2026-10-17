#include "GLLiveStdafx.h"
#include "glt_math_alg.h"

//=============================================================================================
//=============================================================================================

void	GLTmatrix::SetRotateX( GLfixed angle )
{
	//angle = GLTDeg2FixedAngle( angle );
	GLfixed sine = GLTSin(angle);
	GLfixed cosine = GLTCos(angle);

	MATRIX_ELEMENT(0, 0) = GLT_FIXED_ONE;
	MATRIX_ELEMENT(1, 1) = cosine;
	MATRIX_ELEMENT(2, 1) = sine;
	MATRIX_ELEMENT(1, 2) = -sine;
	MATRIX_ELEMENT(2, 2) = cosine;

#if MATRIX_USE_16_ELEMENTS == 1
	MATRIX_ELEMENT(3, 3) = GLT_FIXED_ONE;
	MATRIX_ELEMENT(1, 0) = MATRIX_ELEMENT(2, 0) = MATRIX_ELEMENT(3, 0) = 
	MATRIX_ELEMENT(0, 1) = MATRIX_ELEMENT(3, 1) = 
	MATRIX_ELEMENT(0, 2) = MATRIX_ELEMENT(3, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#else
	MATRIX_ELEMENT(1, 0) = MATRIX_ELEMENT(2, 0) = 
	MATRIX_ELEMENT(0, 1) = MATRIX_ELEMENT(0, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#endif
}

//=============================================================================================
//=============================================================================================

void	GLTmatrix::SetRotateY( GLfixed angle )
{
	//angle = GLTDeg2FixedAngle( angle );
	GLfixed sine = GLTSin(angle);
	GLfixed cosine = GLTCos(angle);

	MATRIX_ELEMENT(1, 1) = GLT_FIXED_ONE;
	MATRIX_ELEMENT(0, 0) = cosine;
	MATRIX_ELEMENT(2, 0) = -sine;
	MATRIX_ELEMENT(0, 2) = sine;
	MATRIX_ELEMENT(2, 2) = cosine;

#if MATRIX_USE_16_ELEMENTS == 1
	MATRIX_ELEMENT(3, 3) = GLT_FIXED_ONE;
	MATRIX_ELEMENT(1, 0) = MATRIX_ELEMENT(3, 0) = 
	MATRIX_ELEMENT(0, 1) = MATRIX_ELEMENT(2, 1) = MATRIX_ELEMENT(3, 1) = 
	MATRIX_ELEMENT(1, 2) = MATRIX_ELEMENT(3, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#else
	MATRIX_ELEMENT(1, 0) = MATRIX_ELEMENT(0, 1) = 
	MATRIX_ELEMENT(2, 1) = MATRIX_ELEMENT(1, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#endif
}

//=============================================================================================
//=============================================================================================

void	GLTmatrix::SetRotateZ( GLfixed angle )
{
	//angle = GLTDeg2FixedAngle( angle );
	GLfixed sine = GLTSin(angle);
	GLfixed cosine = GLTCos(angle);

	MATRIX_ELEMENT(0, 0) = cosine;
	MATRIX_ELEMENT(1, 0) = sine;
	MATRIX_ELEMENT(0, 1) = -sine;
	MATRIX_ELEMENT(1, 1) = cosine;
	MATRIX_ELEMENT(2, 2) = GLT_FIXED_ONE;

#if MATRIX_USE_16_ELEMENTS == 1
	MATRIX_ELEMENT(3, 3) = GLT_FIXED_ONE;
	MATRIX_ELEMENT(2, 0) = MATRIX_ELEMENT(3, 0) = 
	MATRIX_ELEMENT(2, 1) = MATRIX_ELEMENT(3, 1) = 
	MATRIX_ELEMENT(0, 2) = MATRIX_ELEMENT(1, 2) = MATRIX_ELEMENT(3, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#else
	MATRIX_ELEMENT(2, 0) = MATRIX_ELEMENT(2, 1) = 
	MATRIX_ELEMENT(0, 2) = MATRIX_ELEMENT(1, 2) = 
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#endif
}

//=============================================================================================
//=============================================================================================


void GLTmatrix::RotateX( GLfixed x )
{
	GLTmatrix rx;

	rx.SetRotateX(x);
//	rx.MATRIX_ELEMENT(0,3) = 0;
//	rx.MATRIX_ELEMENT(1,3) = 0;
//	rx.MATRIX_ELEMENT(2,3) = 0;
	*this = *this * rx;
}
void GLTmatrix::RotateY( GLfixed y )
{
	GLTmatrix ry;

	ry.SetRotateY(y);
//	ry.MATRIX_ELEMENT(0,3) = 0;
//	ry.MATRIX_ELEMENT(1,3) = 0;
//	ry.MATRIX_ELEMENT(2,3) = 0;
	*this = *this * ry;
}

void GLTmatrix::RotateZ( GLfixed z )
{
	GLTmatrix rz;

	rz.SetRotateZ(z);
//	rz.MATRIX_ELEMENT(0,3) = 0;
//	rz.MATRIX_ELEMENT(1,3) = 0;
//	rz.MATRIX_ELEMENT(2,3) = 0;
	*this = *this * rz;
}


//=============================================================================================
//=============================================================================================

void	GLTmatrix::SetRotate( GLfixed x, GLfixed y, GLfixed z )
{
	GLTmatrix	matz, maty, matx;

	matx.SetRotateX( x );
	maty.SetRotateY( y );
	matz.SetRotateZ( z );

	*this = (matx * maty) * matz;
}

//=============================================================================================
//=============================================================================================

void	GLTmatrix::SetRotate( GLfixed angle, GLfixed x, GLfixed y, GLfixed z )
{
	GLTvec3D	axis(x, y, z);

	axis.Normalize();

	angle = GLTDeg2FixedAngle( angle );
	GLfixed sine = GLTSin(angle);
	GLfixed cosine = GLTCos(angle);
	
	GLfixed one_minus_cosine = GLT_FIXED_ONE - cosine;

	MATRIX_ELEMENT(0, 0) = cosine + GLTMul(one_minus_cosine, GLTMul(axis.x, axis.x));
	MATRIX_ELEMENT(0, 1) = GLTMul(one_minus_cosine, GLTMul(axis.x, axis.y)) - GLTMul(axis.z, sine);
	MATRIX_ELEMENT(0, 2) = GLTMul(GLTMul(one_minus_cosine, axis.x), axis.z) + GLTMul(axis.y, sine);

	MATRIX_ELEMENT(1, 0) = GLTMul(GLTMul(one_minus_cosine, axis.x),  axis.y) + GLTMul(axis.z, sine);
	MATRIX_ELEMENT(1, 1) = cosine + GLTMul(GLTMul(one_minus_cosine, axis.y), axis.y);
	MATRIX_ELEMENT(1, 2) = GLTMul(GLTMul(one_minus_cosine, axis.y), axis.z) - GLTMul(axis.x, sine);

	MATRIX_ELEMENT(2, 0) = GLTMul(GLTMul(one_minus_cosine, axis.x), axis.z) - GLTMul(axis.y, sine);
	MATRIX_ELEMENT(2, 1) = GLTMul(GLTMul(one_minus_cosine, axis.y), axis.z) + GLTMul(axis.x, sine);
	MATRIX_ELEMENT(2, 2) = cosine + GLTMul(GLTMul(one_minus_cosine, axis.z), axis.z);

#if MATRIX_USE_16_ELEMENTS == 1
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 
	MATRIX_ELEMENT(3, 0) = MATRIX_ELEMENT(3, 1) = MATRIX_ELEMENT(3, 2) = 0;
	MATRIX_ELEMENT(3, 3) = GLT_FIXED_ONE;
#else
	MATRIX_ELEMENT(0, 3) = MATRIX_ELEMENT(1, 3) = MATRIX_ELEMENT(2, 3) = 0;
#endif

}

//=============================================================================================
//=============================================================================================

void GLTmatrix::SetFrustrum( GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f) 
{
#if MATRIX_USE_16_ELEMENTS == 1			// NO REAL USE IF 12 ELEMENTS MATRIX ARE USED !
	GLfixed		inv_width = (r - l) ? GLTInv(r - l) : 0;
	GLfixed		inv_height = (t - b) ? GLTInv(t - b) : 0;
	GLfixed		inv_depth = (f - n) ? GLTInv(f - n) : 0;
	GLfixed		two_n = n * 2;
	
	MATRIX_ELEMENT( 0, 0) = GLTMul(two_n, inv_width);
	MATRIX_ELEMENT( 0, 2) = GLTMul(r + l, inv_width);
	
	MATRIX_ELEMENT( 1, 1) = GLTMul(two_n, inv_height);
	MATRIX_ELEMENT( 1, 2) = GLTMul(t + b, inv_height);
	
	MATRIX_ELEMENT( 2, 2) = GLTMul(-f - n, inv_depth);
	MATRIX_ELEMENT( 2, 3) = GLTMul(GLTMul(-two_n, inv_depth), f);
	
	MATRIX_ELEMENT( 3, 2) = -GLT_FIXED_ONE;
	MATRIX_ELEMENT( 3, 3) = 0;	

	MATRIX_ELEMENT( 0, 1 ) = MATRIX_ELEMENT( 0, 3 ) = 
	MATRIX_ELEMENT( 1, 0 ) = MATRIX_ELEMENT( 1, 3 ) = 
	MATRIX_ELEMENT( 2, 0 ) = MATRIX_ELEMENT( 2, 1 ) = 
	MATRIX_ELEMENT( 3, 0 ) = MATRIX_ELEMENT( 3, 1 ) = 0;
#endif
}

//=============================================================================================
//=============================================================================================

void GLTmatrix::SetOrtho(GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f) 
{
#if MATRIX_USE_16_ELEMENTS == 1			// NO REAL USE IF 12 ELEMENTS MATRIX ARE USED !
	GLfixed		inv_width = (r - l) ? GLTInv(r - l) : 0;
	GLfixed		inv_height = (t - b) ? GLTInv(t - b) : 0;
	GLfixed		inv_depth = (f - n) ? GLTInv(f - n) : 0;
	GLfixed		two_n = n * 2;
	
	MATRIX_ELEMENT( 0, 0) = 2 * inv_width;
	MATRIX_ELEMENT( 0, 3) = -GLTMul(r + l, inv_width);
	
	MATRIX_ELEMENT( 1, 1) = 2 * inv_height;
	MATRIX_ELEMENT( 1, 3) = -GLTMul(t + b, inv_height);
	
	MATRIX_ELEMENT( 2, 2) = -2 * inv_depth;
	MATRIX_ELEMENT( 2, 3) = GLTMul(-f - n, inv_depth);

	MATRIX_ELEMENT( 0, 1 ) = MATRIX_ELEMENT( 0, 2 ) = 
	MATRIX_ELEMENT( 1, 0 ) = MATRIX_ELEMENT( 1, 2 ) = 
	MATRIX_ELEMENT( 2, 0 ) = MATRIX_ELEMENT( 2, 1 ) = 
	MATRIX_ELEMENT( 3, 0 ) = MATRIX_ELEMENT( 3, 1 ) = MATRIX_ELEMENT( 3, 2 ) = 0;
	MATRIX_ELEMENT( 3, 3 ) = GLT_FIXED_ONE;
#endif
}



//=============================================================================================
//=============================================================================================
void GLTmatrix::SetFromQuaternionAndPosition(const GLTvec4D& q,GLTvec3D& p)
{
	GLTquat& quat = *( (GLTquat*) & q);

	quat.Quat2Mat(*this);
	
	MATRIX_ELEMENT(0, 3) = p.x;
	MATRIX_ELEMENT(1, 3) = p.y;
	MATRIX_ELEMENT(2, 3) = p.z;
}




//=============================================================================================
//=============================================================================================
// QUATERNIONS

GLTquat::GLTquat(const GLTquat16& q1, const GLTquat16& q2, GLfixed t )
{
	SetInterpolFast( q1, q2, t );
}

//=============================================================================================
//=============================================================================================

void	GLTquat::SetInterpolFast(const GLTquat16& q1, const GLTquat16& q2, GLfixed t )
{
//	t = 0;
	if (t == 0)
	{
		x = q1.x << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		y = q1.y << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		z = q1.z << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		w = q1.w << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
//		Normalize();
	}
	else if (t == GLT_FIXED_ONE)
	{
		x = q2.x << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		y = q2.y << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		z = q2.z << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
		w = q2.w << (GLT_FIXED_SHIFT - GLT_QUAT16_SHIFT);
//		Normalize();
	}
	else
	{
		GLfixed sclq = t;
		GLfixed sclp = (GLT_FIXED_ONE - t);
		
	//  [2006-6-21  13:33:12   baibo.zhang]
		// a and/or b maybe overflow!
		// 	These are rewritten as below.
// 		GLfixed	a, b, s0, s1, s2, s3;
// 
// 		// decide if one of the quaternions is backwards
// 		s0 = q1.x - q2.x;
// 		s1 = q1.y - q2.y;
// 		s2 = q1.z - q2.z;
// 		s3 = q1.w - q2.w;
// 		a  = (s0*s0 + s1*s1 + s2*s2 +  s3*s3);// >> GLT_QUAT16_SHIFT;
//  
// 		s0 = q1.x + q2.x;
// 		s1 = q1.y + q2.y;
// 		s2 = q1.z + q2.z;
// 		s3 = q1.w + q2.w;
// 		b = (s0*s0 + s1*s1 + s2*s2 + s3*s3);// >> GLT_QUAT16_SHIFT;
// 		
// 		if (a > b)
//			sclq = -sclq;
		GLfixed	a = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z  + q1.w * q2.w;
#ifdef _DEBUG
		double b = (double)q1.x * q2.x + (double)q1.y * q2.y + (double)q1.z * q2.z  + (double)q1.w * q2.w;
		if((double)a != b)
		{
			GLT_ASSERT(0);
		}
#endif
		if (a < 0)
		{
			sclq = -sclq;
		}

		
		x = (sclp*q1.x + sclq*q2.x) >> GLT_QUAT16_SHIFT;
		y = (sclp*q1.y + sclq*q2.y) >> GLT_QUAT16_SHIFT;
		z = (sclp*q1.z + sclq*q2.z) >> GLT_QUAT16_SHIFT;
		w = (sclp*q1.w + sclq*q2.w) >> GLT_QUAT16_SHIFT;

		Normalize();
	}
}


//=============================================================================================
//=============================================================================================

void	GLTquat::SetInterpolFast(const GLTquat& q1, const GLTquat& q2, GLfixed t )
{
	if (t == 0)
	{
		x = q1.x;
		y = q1.y;
		z = q1.z;
		w = q1.w;
//		Normalize();
	}
	else if (t == GLT_FIXED_ONE)
	{
		x = q2.x;
		y = q2.y;
		z = q2.z;
		w = q2.w;
//		Normalize();
	}
	else
	{
		GLfixed sclq = t;
		GLfixed sclp = (GLT_FIXED_ONE - t);
//		[8/16/2006 baibo.zhang]		
		//similarly, I rewritten these part as in 
		//SetInterpolFast( GLTquat16& q1, GLTquat16& q2, GLfixed t ) function:
// 		GLfixed	a, b, s0, s1, s2, s3;
		
		// decide if one of the quaternions is backwards
// 		s0 = x - q2.x;	//wrong: x should be q1.x
// 		s1 = y - q2.y;	//wrong: y should be q1.y
// 		s2 = z - q2.z;	//wrong: z should be q1.z
// 		s3 = w - q2.w;	//wrong: w should be q1.w
// 		a  = GLTMul( s0, s0 ) + GLTMul( s1, s1 ) + GLTMul( s2, s2 ) + GLTMul( s3, s3 );
// 		
// 		s0 = x + q2.x;	//wrong: x should be q1.x
// 		s1 = y + q2.y;	//wrong: y should be q1.y
// 		s2 = z + q2.z;	//wrong: z should be q1.z
// 		s3 = w + q2.w;	//wrong: w should be q1.w
// 		b = GLTMul( s0, s0 ) + GLTMul( s1, s1 ) + GLTMul( s2, s2 ) + GLTMul( s3, s3 );
// 		
// 		if (a > b)
//			sclq = -sclq;
		GLfixed	a = GLTMul(q1.x, q2.x) + GLTMul(q1.y, q2.y) + GLTMul(q1.z, q2.z) + GLTMul(q1.w, q2.w);
		if (a < 0)
		{
			sclq = -sclq;
		}

		
		x = (GLTMul(sclp, q1.x)) + (GLTMul(sclq, q2.x));
		y = (GLTMul(sclp, q1.y)) + (GLTMul(sclq, q2.y));
		z = (GLTMul(sclp, q1.z)) + (GLTMul(sclq, q2.z));
		w = (GLTMul(sclp, q1.w)) + (GLTMul(sclq, q2.w));

		Normalize();
	}
}

//=============================================================================================
//=============================================================================================

void	GLTquat::Quat2Mat( GLTmatrix &mtx ) const
{
	GLfixed	xx = GLTMul(x, x);
    GLfixed	xy = GLTMul(x, y);
    GLfixed	xz = GLTMul(x, z);
    GLfixed	xw = GLTMul(x, w);

    GLfixed	yy = GLTMul(y, y);
    GLfixed	yz = GLTMul(y, z);
    GLfixed	yw = GLTMul(y, w);

    GLfixed	zz = GLTMul(z, z);
    GLfixed	zw = GLTMul(z, w);

	GLfixed	ww = GLTMul(w, w);
	
	mtx.m_elements[0] = (ww + xx - yy - zz);
    mtx.m_elements[3] = (2 * ( xy - zw ));
    mtx.m_elements[6] = (2 * ( xz + yw ));

    mtx.m_elements[1] = (2 * ( xy + zw ));
	mtx.m_elements[4] = (ww - xx + yy - zz);
    mtx.m_elements[7] = (2 * ( yz - xw ));

    mtx.m_elements[2] = (2 * ( xz - yw ));
    mtx.m_elements[5] = (2 * ( yz + xw ));
	mtx.m_elements[8] = (ww - xx - yy + zz);

	mtx.m_elements[9] = 0;
	mtx.m_elements[10] = 0;
	mtx.m_elements[11] = 0;
}

////=============================================================================================
////=============================================================================================
//GLTcoord::GLTcoord(const GLTvec3D& oo,const GLTvec3D& uu, const GLTvec3D& ww)
//{
//	o = oo;
//	u = uu;
//	w = ww;
//	int s = u.Length();
//
//	//GLT_ASSERT( u.Length() - GLT_FIXED_ONE < 100 );
//	//GLT_ASSERT( u.Length() - GLT_FIXED_ONE > -100 );
//	//GLT_ASSERT( w.Length() - GLT_FIXED_ONE < 100 );
//	//GLT_ASSERT( w.Length() - GLT_FIXED_ONE > -100 );
//
//}
//GLTmatrix GLTcoord::GetMxWorldToLocal(void) const
//{
//	GLTmatrix m;
//	GLTvec3D v = w.Cross(u);
//	MATRIX_ELEMENT2(m,0,0) = u.x;
//	MATRIX_ELEMENT2(m,0,1) = u.y;
//	MATRIX_ELEMENT2(m,0,2) = u.z;
//	MATRIX_ELEMENT2(m,0,3) = -GLTvec3D::Dot(o,u);
//		
//	MATRIX_ELEMENT2(m,1,0) = v.x;
//	MATRIX_ELEMENT2(m,1,1) = v.y;
//	MATRIX_ELEMENT2(m,1,2) = v.z;
//	MATRIX_ELEMENT2(m,1,3) = -GLTvec3D::Dot(o,v);
//
//	MATRIX_ELEMENT2(m,2,0) = w.x;
//	MATRIX_ELEMENT2(m,2,1) = w.y;
//	MATRIX_ELEMENT2(m,2,2) = w.z;
//	MATRIX_ELEMENT2(m,2,3) = -GLTvec3D::Dot(o,w);
//
//#if MATRIX_USE_16_ELEMENTS == 1
//	MATRIX_ELEMENT2(m,3,0) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,1) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,2) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,3) = GLT_FLOAT_TO_FIXED(1);	
//#endif	
//	return m;
//}
//GLTmatrix GLTcoord::GetMxLocalToWorld(void) const
//{
//	GLTmatrix m;
//	GLTvec3D v = w.Cross(u);
//
//	MATRIX_ELEMENT2(m,0,0) = u.x;
//	MATRIX_ELEMENT2(m,0,1) = v.x;
//	MATRIX_ELEMENT2(m,0,2) = w.x;
//	MATRIX_ELEMENT2(m,0,3) = o.x;
//
//	MATRIX_ELEMENT2(m,1,0) = u.y;
//	MATRIX_ELEMENT2(m,1,1) = v.y;
//	MATRIX_ELEMENT2(m,1,2) = w.y;
//	MATRIX_ELEMENT2(m,1,3) = o.y;
//
//	MATRIX_ELEMENT2(m,2,0) = u.z;
//	MATRIX_ELEMENT2(m,2,1) = v.z;
//	MATRIX_ELEMENT2(m,2,2) = w.z;
//	MATRIX_ELEMENT2(m,2,3) = o.z;
//
//#if MATRIX_USE_16_ELEMENTS == 1
//	MATRIX_ELEMENT2(m,3,0) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,1) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,2) = GLT_FLOAT_TO_FIXED(0);
//	MATRIX_ELEMENT2(m,3,3) = GLT_FLOAT_TO_FIXED(1);	
//#endif	
//	return m;
//}

//void GLTcoord::TransWorldToLocal(GLTvec3D& v) const
//{	
//	GLTmatrix m = GetMxWorldToLocal();
//	GLTvec3D vv = v;
//	m.Mul(vv,v);	
//}
//
//void GLTcoord::TransLocalToWorld(GLTvec3D& v) const
//{
//	GLTmatrix m = GetMxLocalToWorld();
//	GLTvec3D vv = v;
//	m.Mul(vv,v);	
//}
//void GLTcoord::Translate(const GLTmatrix& m)
//{
//	GLTvec3D oo=o;
//	GLTvec3D uu=u+o;
//	GLTvec3D ww=w+o;
//	m.Mul(oo,o);
//	m.Mul(uu,u);
//	m.Mul(ww,w);
//	u -= o;
//	w -= o;
//
//}
//void GLTcoord::Rotate(GLTvec3D pos,GLTvec3D dir_s,GLTvec3D dir_d)
//{
//
//	dir_s.Normalize();
//	dir_d.Normalize();
//	GLTvec3D axis = dir_s.Cross(dir_d);
//	axis.Normalize();
//
//	GLTcoord coord0(pos,dir_s,axis);
//	GLTcoord coord1(pos,dir_d,axis);
//
//
//
//	u += o;
//	w += o;
//	coord0.TransWorldToLocal(o);
//	coord0.TransWorldToLocal(u);
//	coord0.TransWorldToLocal(w);
//	coord1.TransLocalToWorld(o);
//	coord1.TransLocalToWorld(u);
//	coord1.TransLocalToWorld(w);
//
//	u -= o;
//	w -= o;
//}
//
//void GLTcoord::Rotate(GLTvec3D pos,GLTvec3D axis,GLfixed angle)
//{
//	GLTquat quat;
//	GLTmatrix m;
//
//	quat.SetFromAxisAndAngle(axis,angle);
//	quat.Quat2Mat(m);
//
//	GLTvec3D oo=o;
//	GLTvec3D uu=u+o;
//	GLTvec3D ww=w+o;
//	m.Mul(oo,o);
//	m.Mul(uu,u);
//	m.Mul(ww,w);
//
//	u -= o;
//	w -= o;
//}
//
//void GLTcoord::TransWorldToLocal(GLTcoord& coord) const
//{
//	coord.u += coord.o;
//	coord.w += coord.o;
//	TransWorldToLocal(coord.o);
//	TransWorldToLocal(coord.u);
//	TransWorldToLocal(coord.w);
//	coord.u -= coord.o;
//	coord.w -= coord.o;
//}
//
//void GLTcoord::TransLocalToWorld(GLTcoord& coord) const
//{
//	coord.u += coord.o;
//	coord.w += coord.o;
//	TransLocalToWorld(coord.o);
//	TransLocalToWorld(coord.u);
//	TransLocalToWorld(coord.w);
//	coord.u -= coord.o;
//	coord.w -= coord.o;
//
//}

//GLTcoord GLTcoord::Interpolation(const GLTcoord& c0,const GLTcoord& c1,GLfixed t)
//{
//	GLT_ASSERT( t >= 0 );
//	GLT_ASSERT( t <= GLT_FIXED_ONE );
//
//	GLTcoord result;
//	result.o.x = GLTInterpolate( c0.o.x, c1.o.x, t );
//	result.o.y = GLTInterpolate( c0.o.y, c1.o.y, t );
//	result.o.z = GLTInterpolate( c0.o.z, c1.o.z, t );
//
//	GLTmatrix mx0 = c0.GetMxLocalToWorld();
//	GLTmatrix mx1 = c1.GetMxLocalToWorld();
//
//	mx0.
//
//
//    GLTquat q0,q1,quat;
//	quat.SetInterpolFast(q0,q1,t);
//	quat.Quat2Mat(result);
//
//	return ;
//}