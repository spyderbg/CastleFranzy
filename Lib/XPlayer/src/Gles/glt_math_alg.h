#ifndef _GLT_MATH_ALG_H_
#define _GLT_MATH_ALG_H_

#include "gles/glt_math_fixed.h"

typedef struct
{
	GLfixed tu, tv;

} GLTvertexUV;

typedef struct
{
	short	x, y, z, w;

} GLTquat16;

#define GLT_QUAT16_SHIFT			15
#define GLT_QUAT16_FIXED_ONE		(1<<GLT_QUAT16_SHIFT)




class GLTvec2D
{
public:
	GLfixed	x, y;

	inline	GLTvec2D()	{}

	inline	GLTvec2D( GLfixed _x, GLfixed _y)
	{
		x = _x;
		y = _y;
	}
	inline void Set(const GLfixed _x, const GLfixed _y )
	{
		x = _x;
		y = _y;
	}
	inline GLTvec2D( const GLTvec2D& source )
	{
		x = source.x;
		y = source.y;
	}
	inline void Set( const GLTvec2D& source )
	{
		x = source.x;
		y = source.y;
	}

	inline GLTvec2D& operator=( const GLTvec2D& source )
	{
		x = source.x;
		y = source.y;
		return *this;
	}

	inline GLTvec2D operator+(const GLTvec2D& vec ) const 
	{
		return GLTvec2D(x + vec.x, y + vec.y);
	}

	inline GLTvec2D operator-(const GLTvec2D& vec) const 
	{
		return GLTvec2D(x - vec.x, y - vec.y);
	}

	inline GLTvec2D operator*(GLfixed factor) const 
	{
		return GLTvec2D( GLTMul(x, factor), GLTMul(y, factor));
	}

	inline GLTvec2D operator/(GLfixed factor) const 
	{
		return GLTvec2D( GLTDiv(x, factor), GLTDiv(y, factor));
	}

	inline GLfixed operator*(const GLTvec2D& vec) const 
	{
		return GLTMul(x, vec.x) + GLTMul(y, vec.y);
	}

	inline GLTvec2D& operator+=(const GLTvec2D& vec) 
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}

	inline GLTvec2D& operator-=(const GLTvec2D& vec) 
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	inline GLTvec2D& operator*=(GLfixed factor) 
	{
		x = GLTMul(x, factor);
		y = GLTMul(y, factor);
		return *this;
	}

//	inline GLTvec2D Cross(const GLTvec2D& vec) const 
//	{
//		return GLTvec2D(GLTMul(x, vec.y) - GLTMul(y, vec.x));
//	}

	inline GLTvec2D Scale(const GLTvec2D& vec) const 
	{
		return GLTvec2D( GLTMul(x, vec.x), GLTMul(y, vec.y));
	}

	inline	GLfixed Length2() const
	{
		GLfixed sq = GLTMul( x, x ) + GLTMul( y, y );
		return sq;
	}

	inline	GLfixed Length() const
	{
		GLfixed	sq = Length2();
		if (sq)
			return GLTSqrt( sq );
		else
			return 0;
	}

	inline GLfixed	Normalize()
	{
		GLfixed	len = Length();
		if ((len) && (len != GLT_FIXED_ONE))
		{
			int invLen = GLTDiv( GLT_FIXED_ONE, len );
			x = GLTMul( x, invLen );
			y = GLTMul( y, invLen );
		}

		return len;
	}

	inline GLfixed  operator[](int i) const {GLT_ASSERT(i>=0 && i<2);return *((&x) + i);}
	inline GLfixed& operator[](int i)		{GLT_ASSERT(i>=0 && i<2);return *((&x) + i);}
};





class GLTvec3D
{
public:
	GLfixed	x, y, z;

	inline	GLTvec3D()	{}

	inline	GLTvec3D( GLfixed _x, GLfixed _y, GLfixed _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
	inline void Set(const GLfixed _x, const GLfixed _y, const GLfixed _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
	inline GLTvec3D( const GLTvec3D& source )
	{
		x = source.x;
		y = source.y;
		z = source.z;
	}
	inline void Set( const GLTvec3D& source )
	{
		x = source.x;
		y = source.y;
		z = source.z;
	}

	inline GLTvec3D& operator=( const GLTvec3D& source )
	{
		x = source.x;
		y = source.y;
		z = source.z;
		return *this;
	}
	inline bool operator==( const GLTvec3D& source )
	{
		if(x == source.x &&	y == source.y &&z == source.z )
			return true;
		return false;
	}

	inline GLTvec3D operator+(const GLTvec3D& vec ) const 
	{
		return GLTvec3D(x + vec.x, y + vec.y, z + vec.z);
	}

	inline GLTvec3D operator-(const GLTvec3D& vec) const 
	{
		return GLTvec3D(x - vec.x, y - vec.y, z - vec.z);
	}

	inline GLTvec3D operator-(void) const
	{
		return GLTvec3D( -x, -y, -z );
	}

	inline GLTvec3D operator*(GLfixed factor) const 
	{
		return GLTvec3D( GLTMul(x, factor), GLTMul(y, factor), GLTMul(z, factor));
	}

	inline GLTvec3D operator/(GLfixed factor) const 
	{
		return GLTvec3D( GLTDiv(x, factor), GLTDiv(y, factor), GLTDiv(z, factor));
	}

	inline GLfixed operator*(const GLTvec3D& vec) const 
	{
		return GLTMul(x, vec.x) + GLTMul(y, vec.y) + GLTMul(z, vec.z);
	}

	inline GLTvec3D& operator+=(const GLTvec3D& vec) 
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	inline GLTvec3D& operator-=(const GLTvec3D& vec) 
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	inline GLTvec3D& operator*=(GLfixed factor) 
	{
		x = GLTMul(x, factor);
		y = GLTMul(y, factor);
		z = GLTMul(z, factor);
		return *this;
	}

	inline GLTvec3D Cross(const GLTvec3D& vec) const 
	{
		return GLTvec3D(	GLTMul(y, vec.z) - GLTMul(z, vec.y),
							-GLTMul(x, vec.z) + GLTMul(z, vec.x),
							GLTMul(x, vec.y) - GLTMul(y, vec.x)	);
	}

	inline static int	Dot(const GLTvec3D& v1,const GLTvec3D& v2)	{return (  (GLTMul(v1.x,v2.x) + GLTMul(v1.y,v2.y)) + GLTMul(v1.z,v2.z));}

	inline GLTvec3D Scale(const GLTvec3D& vec) const 
	{
		return GLTvec3D( GLTMul(x, vec.x), GLTMul(y, vec.y), GLTMul(z, vec.z) );
	}

	inline	GLfixed Length2() const
	{
		GLfixed sq = GLTMul( x, x ) + GLTMul( y, y ) + GLTMul( z, z );
		return sq;
	}

	inline	GLfixed Length() const
	{
		GLfixed	sq = Length2();
		if (sq)
			return GLTSqrt( sq );
		else
			return 0;
	}

	inline GLfixed	Normalize()
	{
		GLfixed	len = Length();
		if ((len) && (len != GLT_FIXED_ONE))
		{
			int invLen = GLTDiv( GLT_FIXED_ONE, len );
			x = GLTMul( x, invLen );
			y = GLTMul( y, invLen );
			z = GLTMul( z, invLen );
		}

		return len;
	}
	inline GLfixed  GetAbsMax()
	{
		GLfixed AbsMax = GLTAbs(x);
		if (GLTAbs(y) > AbsMax)
		{
			AbsMax = GLTAbs(y);
		}
		if (GLTAbs(z) > AbsMax)
		{
			AbsMax = GLTAbs(z);
		}
		return AbsMax;
	}

	inline GLfixed  NormalizeMax()
	{
		GLfixed AbsMax = GetAbsMax();
		if (AbsMax > GLT_FIXED_ONE)
		{
			int invLen = GLTDiv(GLT_FIXED_ONE, AbsMax);
			x = GLTMul( x, invLen );
			y = GLTMul( y, invLen );
			z = GLTMul( z, invLen );
		}
		return AbsMax;
		
	}

	inline GLfixed  operator[](int i) const {GLT_ASSERT(i>=0 && i<3);return *((&x) + i);}
	inline GLfixed& operator[](int i)		{GLT_ASSERT(i>=0 && i<3);return *((&x) + i);}

	inline static GLTvec3D GetReflexion(const GLTvec3D& v, const GLTvec3D& normal)
	{
		GLTvec3D proj = normal;
		proj.Normalize();
		int nDot = Dot(v, proj);

		proj.x = GLTMul(proj.x, nDot);
		proj.y = GLTMul(proj.y, nDot);
		proj.z = GLTMul(proj.z, nDot);

		return proj + (proj - v);
	}
};


class GLTvec4D
{
public:
	GLfixed	x, y, z, w;

	inline	GLTvec4D()	{}

	inline	GLTvec4D( GLfixed _x, GLfixed _y, GLfixed _z, GLfixed _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	inline GLTvec4D( const GLTvec4D& source )
	{
		x = source.x;
		y = source.y;
		z = source.z;
		w = source.w;
	}

	inline GLTvec4D& operator=( const GLTvec4D& source )
	{
		x = source.x;
		y = source.y;
		z = source.z;
		w = source.w;
		return *this;
	}

	inline GLTvec4D operator+(const GLTvec4D& vec ) const 
	{
		return GLTvec4D(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	inline GLTvec4D operator-(const GLTvec4D& vec) const 
	{
		return GLTvec4D(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	inline GLTvec4D operator*(GLfixed factor) const 
	{
		return GLTvec4D( GLTMul(x, factor), GLTMul(y, factor), GLTMul(z, factor), GLTMul(w, factor));
	}

	inline GLfixed operator*(const GLTvec4D& vec) const 
	{
		return GLTMul(x, vec.x) + GLTMul(y, vec.y) + GLTMul(z, vec.z) + GLTMul(w, vec.w);
	}

	inline GLTvec4D& operator+=(const GLTvec4D& vec) 
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}

	inline GLTvec4D& operator-=(const GLTvec4D& vec) 
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}

	inline GLTvec4D& operator*=(GLfixed factor) 
	{
		x = GLTMul(x, factor);
		y = GLTMul(y, factor);
		z = GLTMul(z, factor);
		w = GLTMul(w, factor);
		return *this;
	}

	inline	GLfixed Length()
	{
		GLfixed	sq = GLTMul( x, x ) + GLTMul( y, y ) + GLTMul( z, z ) + GLTMul( w, w );
		if (sq)
			return GLTSqrt( sq );
		else
			return 0;
	}

	inline void	Normalize()
	{
		GLfixed	len = Length();
		if ((len) && (len != GLT_FIXED_ONE))
		{
			len = GLTDiv( GLT_FIXED_ONE, len );
			x = GLTMul( x, len );
			y = GLTMul( y, len );
			z = GLTMul( z, len );
			w = GLTMul( w, len );
		}
	}

	inline GLfixed  operator[](int i) const {GLT_ASSERT(i>=0 && i<4);return *((&x) + i);}
	inline GLfixed& operator[](int i)		{GLT_ASSERT(i>=0 && i<4);return *((&x) + i);}
};


class GLTmatrix
{
public:

#if MATRIX_USE_16_ELEMENTS == 1

	GLfixed	m_elements[16];

#define MATRIX_ELEMENT(row,column)			m_elements[row + column * 4]
#define MATRIX_ELEMENT2(m,row,column)		m.m_elements[row + column * 4]

#else

	GLfixed	m_elements[12];

#define MATRIX_ELEMENT(row,column)			m_elements[row + column * 3]
#define MATRIX_ELEMENT2(m,row,column)		m.m_elements[row + column * 3]

#endif

/*	inline GLfixed	Element( int row, int column )
	{
		return MATRIX_ELEMENT(row, column);
	}*/

	GLTmatrix()	{};

	inline GLTmatrix(const GLTmatrix& mat) 
	{
		m_elements[0] = mat.m_elements[0];
		m_elements[1] = mat.m_elements[1];
		m_elements[2] = mat.m_elements[2];
		m_elements[3] = mat.m_elements[3];
		m_elements[4] = mat.m_elements[4];
		m_elements[5] = mat.m_elements[5];
		m_elements[6] = mat.m_elements[6];
		m_elements[7] = mat.m_elements[7];
		m_elements[8] = mat.m_elements[8];
		m_elements[9] = mat.m_elements[9];
		m_elements[10] = mat.m_elements[10];
		m_elements[11] = mat.m_elements[11];
#if MATRIX_USE_16_ELEMENTS == 1
		m_elements[12] = mat.m_elements[12];
		m_elements[13] = mat.m_elements[13];
		m_elements[14] = mat.m_elements[14];
		m_elements[15] = mat.m_elements[15];
#endif
	}

	inline GLTmatrix& operator=(const GLTmatrix& mat) 
	{
		m_elements[0] = mat.m_elements[0];
		m_elements[1] = mat.m_elements[1];
		m_elements[2] = mat.m_elements[2];
		m_elements[3] = mat.m_elements[3];
		m_elements[4] = mat.m_elements[4];
		m_elements[5] = mat.m_elements[5];
		m_elements[6] = mat.m_elements[6];
		m_elements[7] = mat.m_elements[7];
		m_elements[8] = mat.m_elements[8];
		m_elements[9] = mat.m_elements[9];
		m_elements[10] = mat.m_elements[10];
		m_elements[11] = mat.m_elements[11];
#if MATRIX_USE_16_ELEMENTS == 1
		m_elements[12] = mat.m_elements[12];
		m_elements[13] = mat.m_elements[13];
		m_elements[14] = mat.m_elements[14];
		m_elements[15] = mat.m_elements[15];
#endif
		return *this;
	}

	inline void	SetRotationFromMatrix(const GLTmatrix& mat) 
	{
		m_elements[0] = mat.m_elements[0];
		m_elements[1] = mat.m_elements[1];
		m_elements[2] = mat.m_elements[2];
		m_elements[3] = mat.m_elements[3];
		m_elements[4] = mat.m_elements[4];
		m_elements[5] = mat.m_elements[5];
		m_elements[6] = mat.m_elements[6];
		m_elements[7] = mat.m_elements[7];
		m_elements[8] = mat.m_elements[8];
	}

	GLTmatrix operator*(const GLTmatrix& mat) const 
	{
		GLTmatrix result;

#if MATRIX_USE_16_ELEMENTS == 1
	
		MATRIX_ELEMENT2( result, 0, 0) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 0) ) +
											GLTMul( MATRIX_ELEMENT(0, 3), MATRIX_ELEMENT2( mat, 3, 0) );

		MATRIX_ELEMENT2( result, 0, 1) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 1) ) +
											GLTMul( MATRIX_ELEMENT(0, 3), MATRIX_ELEMENT2( mat, 3, 1) );

		MATRIX_ELEMENT2( result, 0, 2) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 2) ) +
											GLTMul( MATRIX_ELEMENT(0, 3), MATRIX_ELEMENT2( mat, 3, 2) );

		MATRIX_ELEMENT2( result, 0, 3) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											GLTMul( MATRIX_ELEMENT(0, 3), MATRIX_ELEMENT2( mat, 3, 3) );

		MATRIX_ELEMENT2( result, 1, 0) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 0) ) +
											GLTMul( MATRIX_ELEMENT(1, 3), MATRIX_ELEMENT2( mat, 3, 0) );

		MATRIX_ELEMENT2( result, 1, 1) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 1) ) +
											GLTMul( MATRIX_ELEMENT(1, 3), MATRIX_ELEMENT2( mat, 3, 1) );

		MATRIX_ELEMENT2( result, 1, 2) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 2) ) +
											GLTMul( MATRIX_ELEMENT(1, 3), MATRIX_ELEMENT2( mat, 3, 2) );

		MATRIX_ELEMENT2( result, 1, 3) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											GLTMul( MATRIX_ELEMENT(1, 3), MATRIX_ELEMENT2( mat, 3, 3) );

		MATRIX_ELEMENT2( result, 2, 0) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 0) ) +
											GLTMul( MATRIX_ELEMENT(2, 3), MATRIX_ELEMENT2( mat, 3, 0) );

		MATRIX_ELEMENT2( result, 2, 1) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 1) ) +
											GLTMul( MATRIX_ELEMENT(2, 3), MATRIX_ELEMENT2( mat, 3, 1) );

		MATRIX_ELEMENT2( result, 2, 2) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 2) ) +
											GLTMul( MATRIX_ELEMENT(2, 3), MATRIX_ELEMENT2( mat, 3, 2) );

		MATRIX_ELEMENT2( result, 2, 3) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											GLTMul( MATRIX_ELEMENT(2, 3), MATRIX_ELEMENT2( mat, 3, 3) );

		MATRIX_ELEMENT2( result, 3, 0) =	GLTMul( MATRIX_ELEMENT(3, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(3, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(3, 2), MATRIX_ELEMENT2( mat, 2, 0) ) +
											GLTMul( MATRIX_ELEMENT(3, 3), MATRIX_ELEMENT2( mat, 3, 0) );

		MATRIX_ELEMENT2( result, 3, 1) =	GLTMul( MATRIX_ELEMENT(3, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(3, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(3, 2), MATRIX_ELEMENT2( mat, 2, 1) ) +
											GLTMul( MATRIX_ELEMENT(3, 3), MATRIX_ELEMENT2( mat, 3, 1) );

		MATRIX_ELEMENT2( result, 3, 2) =	GLTMul( MATRIX_ELEMENT(3, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(3, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(3, 2), MATRIX_ELEMENT2( mat, 2, 2) ) +
											GLTMul( MATRIX_ELEMENT(3, 3), MATRIX_ELEMENT2( mat, 3, 2) );

		MATRIX_ELEMENT2( result, 3, 3) =	GLTMul( MATRIX_ELEMENT(3, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(3, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(3, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											GLTMul( MATRIX_ELEMENT(3, 3), MATRIX_ELEMENT2( mat, 3, 3) );

#else	// 12 element matrix

		MATRIX_ELEMENT2( result, 0, 0) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 0) );

		MATRIX_ELEMENT2( result, 0, 1) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 1) );

		MATRIX_ELEMENT2( result, 0, 2) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 2) );

		MATRIX_ELEMENT2( result, 0, 3) =	GLTMul( MATRIX_ELEMENT(0, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(0, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(0, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											MATRIX_ELEMENT(0, 3);

		MATRIX_ELEMENT2( result, 1, 0) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 0) );

		MATRIX_ELEMENT2( result, 1, 1) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 1) );

		MATRIX_ELEMENT2( result, 1, 2) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 2) );

		MATRIX_ELEMENT2( result, 1, 3) =	GLTMul( MATRIX_ELEMENT(1, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(1, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(1, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											MATRIX_ELEMENT(1, 3);

		MATRIX_ELEMENT2( result, 2, 0) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 0) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 0) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 0) );

		MATRIX_ELEMENT2( result, 2, 1) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 1) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 1) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 1) );

		MATRIX_ELEMENT2( result, 2, 2) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 2) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 2) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 2) );

		MATRIX_ELEMENT2( result, 2, 3) =	GLTMul( MATRIX_ELEMENT(2, 0), MATRIX_ELEMENT2( mat, 0, 3) ) +
											GLTMul( MATRIX_ELEMENT(2, 1), MATRIX_ELEMENT2( mat, 1, 3) ) +
											GLTMul( MATRIX_ELEMENT(2, 2), MATRIX_ELEMENT2( mat, 2, 3) ) +
											MATRIX_ELEMENT(2, 3);


#endif


		return result;
	}

#if MATRIX_USE_16_ELEMENTS == 1

	inline GLTvec4D operator*(const GLTvec3D& vector) const 
	{
		return GLTvec4D
			(
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) + 
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) + 
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) + MATRIX_ELEMENT(0, 3),

			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) + 
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) + 
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) + MATRIX_ELEMENT(1, 3),

			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) + MATRIX_ELEMENT(2, 3),

			GLTMul(vector.x, MATRIX_ELEMENT(3, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(3, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(3, 2)) + MATRIX_ELEMENT(3, 3)
			);
	}

	inline GLTvec4D operator*(const GLTvec4D& vector) const 
	{
		return GLTvec4D
			(
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(0, 3)),

			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(1, 3)),

			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) + 
			GLTMul(vector.w, MATRIX_ELEMENT(2, 3)),

			GLTMul(vector.x, MATRIX_ELEMENT(3, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(3, 1)) + 
			GLTMul(vector.z, MATRIX_ELEMENT(3, 2)) + 
			GLTMul(vector.w, MATRIX_ELEMENT(3, 3))
			);
	}

	inline void Mul(const GLTvec4D& vector, GLTvec4D& result) const 
	{
		result.x = 
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(0, 3));

		result.y = 
			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(1, 3));

		result.z =
			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(2, 3));

		result.w =
			GLTMul(vector.x, MATRIX_ELEMENT(3, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(3, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(3, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(3, 3));
		}

	inline void Mul(const GLTvec3D& vector, GLTvec4D& result) const 
	{
		result.x = 
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
			MATRIX_ELEMENT(0, 3);

		result.y = 
			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
			MATRIX_ELEMENT(1, 3);

		result.z =
			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
			MATRIX_ELEMENT(2, 3);

		result.w =
			GLTMul(vector.x, MATRIX_ELEMENT(3, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(3, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(3, 2)) +
			MATRIX_ELEMENT(3, 3);
		}

	inline void Mul(const GLTvec4D& vector, GLTvec3D& result) const 
	{
		result.x =
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(0, 3));

		result.y =
			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(1, 3));

		result.z =
			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
			GLTMul(vector.w, MATRIX_ELEMENT(2, 3));
		}
#endif

	inline void Mul(const GLTvec3D& vector, GLTvec3D& result) const 
	{
		result.x =
			GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
			MATRIX_ELEMENT(0, 3);
		
		result.y =
			GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
			MATRIX_ELEMENT(1, 3);

		result.z =
			GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
			GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
			GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
			MATRIX_ELEMENT(2, 3);
		}

	inline void MulScaled(const GLTvec3D& vector, GLTvec3D& result, GLfixed scale) const 
	{
		result.x = GLTMul( scale,
				(GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
				 MATRIX_ELEMENT(0, 3))
				 );
		
		result.y = GLTMul( scale, 
				(GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
				 MATRIX_ELEMENT(1, 3))
				 );

		result.z = GLTMul( scale,
				(GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
				 MATRIX_ELEMENT(2, 3))
				 );
    }

	inline void MulAddScaled(const GLTvec3D& vector, GLTvec3D& result, GLfixed scale) const 
	{
		result.x += GLTMul( scale,
				(GLTMul(vector.x, MATRIX_ELEMENT(0, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(0, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(0, 2)) +
				 MATRIX_ELEMENT(0, 3))
				 );
		
		result.y += GLTMul( scale, 
				(GLTMul(vector.x, MATRIX_ELEMENT(1, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(1, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(1, 2)) +
				 MATRIX_ELEMENT(1, 3))
				 );

		result.z += GLTMul( scale,
				(GLTMul(vector.x, MATRIX_ELEMENT(2, 0)) +
				 GLTMul(vector.y, MATRIX_ELEMENT(2, 1)) +
				 GLTMul(vector.z, MATRIX_ELEMENT(2, 2)) +
				 MATRIX_ELEMENT(2, 3))
				 );
		}


	inline void Set( const GLfixed *array )
	{
		m_elements[0] = array[0];
		m_elements[1] = array[1];
		m_elements[2] = array[2];
		m_elements[3] = array[3];
		m_elements[4] = array[4];
		m_elements[5] = array[5];
		m_elements[6] = array[6];
		m_elements[7] = array[7];
		m_elements[8] = array[8];
		m_elements[9] = array[9];
		m_elements[10] = array[10];
		m_elements[11] = array[11];
#if MATRIX_USE_16_ELEMENTS == 1
		m_elements[12] = array[12];
		m_elements[13] = array[13];
		m_elements[14] = array[14];
		m_elements[15] = array[15];
#endif
	}

	inline void SetIdentity()
	{
#if MATRIX_USE_16_ELEMENTS == 1
		MATRIX_ELEMENT(0,0) = MATRIX_ELEMENT(1,1) = MATRIX_ELEMENT(2,2) = MATRIX_ELEMENT(3,3) = GLT_FIXED_ONE;
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = MATRIX_ELEMENT(0,3) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = MATRIX_ELEMENT(1,3) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = MATRIX_ELEMENT(2,3) = 
		MATRIX_ELEMENT(3,0) = MATRIX_ELEMENT(3,1) = MATRIX_ELEMENT(3,2) = 0;
#else
		MATRIX_ELEMENT(0,0) = MATRIX_ELEMENT(1,1) = MATRIX_ELEMENT(2,2) = GLT_FIXED_ONE;
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = MATRIX_ELEMENT(0,3) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = MATRIX_ELEMENT(1,3) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = MATRIX_ELEMENT(2,3) = 0;
#endif
	}

	inline void	SetScale( GLfixed x, GLfixed y, GLfixed z )
	{
		MATRIX_ELEMENT(0,0) = x;
		MATRIX_ELEMENT(1,1) = y;
		MATRIX_ELEMENT(2,2) = z;
#if MATRIX_USE_16_ELEMENTS == 1
		MATRIX_ELEMENT(3,3) = GLT_FIXED_ONE;
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = MATRIX_ELEMENT(0,3) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = MATRIX_ELEMENT(1,3) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = MATRIX_ELEMENT(2,3) = 
		MATRIX_ELEMENT(3,0) = MATRIX_ELEMENT(3,1) = MATRIX_ELEMENT(3,2) = 0;
#else
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = MATRIX_ELEMENT(0,3) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = MATRIX_ELEMENT(1,3) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = MATRIX_ELEMENT(2,3) = 0;
#endif
	}

	inline void	SetTranslate( GLfixed x, GLfixed y, GLfixed z )
	{
#if MATRIX_USE_16_ELEMENTS == 1
		MATRIX_ELEMENT(0,0) = MATRIX_ELEMENT(1,1) = MATRIX_ELEMENT(2,2) = MATRIX_ELEMENT(3,3) = GLT_FIXED_ONE;
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = 
		MATRIX_ELEMENT(3,0) = MATRIX_ELEMENT(3,1) = MATRIX_ELEMENT(3,2) = 0;
#else
		MATRIX_ELEMENT(0,0) = MATRIX_ELEMENT(1,1) = MATRIX_ELEMENT(2,2) = GLT_FIXED_ONE;
		MATRIX_ELEMENT(0,1) = MATRIX_ELEMENT(0,2) = 
		MATRIX_ELEMENT(1,0) = MATRIX_ELEMENT(1,2) = 
		MATRIX_ELEMENT(2,0) = MATRIX_ELEMENT(2,1) = 0;
#endif
		MATRIX_ELEMENT(0,3) = x;
		MATRIX_ELEMENT(1,3) = y;
		MATRIX_ELEMENT(2,3) = z;
	}

	inline void	SetTranslationOnly( GLfixed x, GLfixed y, GLfixed z )
	{
		MATRIX_ELEMENT(0,3) = x;
		MATRIX_ELEMENT(1,3) = y;
		MATRIX_ELEMENT(2,3) = z;
	}
	inline void GetTranslation(GLTvec3D &tran) const
	{
		GetColumn(tran, 3);
	}
	inline void GetColumn(GLTvec3D &col, u32 idx = 2 ) const
	{
		GLT_ASSERT(idx < 4);
		col.x = MATRIX_ELEMENT(0,idx);
		col.y = MATRIX_ELEMENT(1,idx);
		col.z = MATRIX_ELEMENT(2,idx);
	}
	void	SetRotateX( GLfixed x );
	void	SetRotateY( GLfixed y );
	void	SetRotateZ( GLfixed z );
	void	SetRotate( GLfixed x, GLfixed y, GLfixed z );
	void	SetRotate( GLfixed angle, GLfixed x, GLfixed y, GLfixed z );

	void	SetFromQuaternionAndPosition(const GLTvec4D& quaternion,GLTvec3D&);

	void	RotateX( GLfixed x );
	void	RotateY( GLfixed y );
	void	RotateZ( GLfixed z );

	void	SetFrustrum( GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed fnear, GLfixed ffar);
	void	SetOrtho(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed fnear, GLfixed ffar);
	

	// TBD inverse transpose
};

/*
class GLTmatrixStack
{
public:
	GLTmatrixStack( int depth )
	{
		GLT_ASSERT( depth >= 1 );

		m_depth = depth;
		m_pos = 0;

		m_matrix = GLT_NEW GLTmatrix[depth];
	}

	~GLTmatrixStack()
	{
		GLT_DELETE_ARRAY m_matrix;
	}

	GLTmatrix*	Get()		{	return m_matrix + m_pos;	}
	void		Push()		{	m_pos++;	GLT_ASSERT( m_pos < m_depth );	m_matrix[m_pos] = m_matrix[m_pos-1];	}
	void		Pop()		{	m_pos--;	GLT_ASSERT( m_pos >= 0 );	}

private:
	GLTmatrix*	m_matrix;
	int			m_pos;
	int			m_depth;
};*/



class GLTquat
{
public:
	GLfixed		x, y, z, w;

	inline		GLTquat(const  GLTquat16& q1,const  GLTquat16& q2, GLfixed t );	// interpolated from compressed s16 quaternions
	inline		GLTquat( GLfixed _x, GLfixed _y, GLfixed _z, GLfixed _w )	{	x = _x; y = _y; z = _z;	w = _w; }
	inline		GLTquat()	{}

	void		SetFromAxisAndAngle( GLTvec3D& axis, GLfixed angle )
	{
#ifdef WIPI
		const GLfixed	a2 = GLTDeg2FixedAngle( angle / 2 );
		const GLfixed	tsin = GLTSin( a2 );

		x = GLTMul(axis.x, tsin);
		y = GLTMul(axis.y, tsin);
		z = GLTMul(axis.z, tsin);
		w = GLTCos( a2 );
#else
		const GLfixed	a2 = GLTDeg2FixedAngle( angle / 2 );
		const GLfixed	sin = GLTSin( a2 );

		x = GLTMul(axis.x, sin);
		y = GLTMul(axis.y, sin);
		z = GLTMul(axis.z, sin);
		w = GLTCos( a2 );
#endif
	}

//	GLTquat		InterpolFast( GLTquat& q2, GLfixed t );

	void		SetInterpolFast(const  GLTquat& q1,const  GLTquat& q2, GLfixed t );
	void		SetInterpolFast(const  GLTquat16& q1,const  GLTquat16& q2, GLfixed t );	// interpolated from compressed s16 quaternions

	void		Quat2Mat( GLTmatrix& mtx )  const;

	inline GLTquat operator*(const GLTquat& q2) const 
	{
		return GLTquat
			(
				(GLTMul( w, q2.x ) + GLTMul( y, q2.w ) + GLTMul( z, q2.z ) - GLTMul( w, q2.y )),
				(GLTMul( w, q2.y ) - GLTMul( y, q2.z ) + GLTMul( z, q2.w ) + GLTMul( w, q2.x )),
				(GLTMul( w, q2.z ) + GLTMul( y, q2.y ) - GLTMul( z, q2.x ) + GLTMul( w, q2.w )),
				(GLTMul( w, q2.w ) - GLTMul( y, q2.x ) - GLTMul( z, q2.y ) - GLTMul( w, q2.z ))
			);
	}

	inline	GLfixed Length()
	{
		GLfixed	sq = GLTMul( x, x ) + GLTMul( y, y ) + GLTMul( z, z ) + GLTMul( w, w );
		if (sq)
			return GLTSqrt( sq );
		else
			return 0;
	}

	inline void	Normalize()
	{
		GLfixed	len = Length();
		if ((len) && (len != GLT_FIXED_ONE))
		{
			len = GLTInv( len );
			x = GLTMul( x, len );
			y = GLTMul( y, len );
			z = GLTMul( z, len );
			w = GLTMul( w, len );
		}
	}
};


//class GLTcoord// local coord
//{
//public:
//	GLTvec3D o;
//	GLTvec3D u; //length of u must be 1	
//	GLTvec3D w; //length of w must be 1
//
//	inline GLTcoord(void)
//		:o(GLT_FLOAT_TO_FIXED(0),GLT_FLOAT_TO_FIXED(0),GLT_FLOAT_TO_FIXED(0)),
//		u(GLT_FLOAT_TO_FIXED(1),GLT_FLOAT_TO_FIXED(0),GLT_FLOAT_TO_FIXED(0)),
//		w(GLT_FLOAT_TO_FIXED(0),GLT_FLOAT_TO_FIXED(0),GLT_FLOAT_TO_FIXED(1)){};
//	GLTcoord(const GLTvec3D& oo,const GLTvec3D& uu, const GLTvec3D& ww);
//	GLTmatrix GetMxWorldToLocal(void) const;
//	GLTmatrix GetMxLocalToWorld(void) const;
//	void TransWorldToLocal(GLTvec3D& v) const;
//	void TransLocalToWorld(GLTvec3D& v) const;
//	void TransWorldToLocal(GLTcoord& coord) const;
//	void TransLocalToWorld(GLTcoord& coord) const;
//	GLTvec3D v(void) const
//	{
//		return w.Cross(u);
//	};
//	void Translate(const GLTmatrix& m);
//
//	void Rotate(GLTvec3D pos,GLTvec3D dir_s,GLTvec3D dir_d);
//	void Rotate(GLTvec3D pos,GLTvec3D axis,GLfixed angle);
//
//
//	static GLTcoord Interpolation(const GLTcoord& c0,const GLTcoord& c1,double t);
//
//	
//
//
//};
#endif
