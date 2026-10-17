#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImage.h"
#include "irrString.h"
#include "CColorConverter.h"

//HACK:XBOX360 swizzle texutre:This is not the best solution for swizzled texture.
#ifdef _IRR_XBOX_PLATFORM_
#include <xtl.h>
#include <Xgraphics.h>
#endif

namespace irr
{
namespace video
{

namespace
{
// integer log2 of a float ieee 754. TODO: non ieee floating point
static inline s32 s32_log2_f32( f32 f)
{
	u32 x = IR ( f );

	return ((x & 0x7F800000) >> 23) - 127;
}

static inline s32 s32_log2_s32(u32 x)
{
	return s32_log2_f32( (f32) x);
}

//! a more useful memset for pixel
inline void memset32 ( void * dest, const u32 value, u32 bytesize )
{
	u32 * d = (u32*) dest;

	u32 i;

	i = bytesize >> ( 2 + 3 );
	while( i )
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2 ) & 7;
	while( i )
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}

}

// 1 - Bit Alpha Blending
inline u16 PixelBlend16 ( const u16 c2, const u16 c1 )
{
	u16 c = c1 & 0x8000;
	
	c >>= 15;
	c += 0x7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline u32 PixelBlend16_simd ( const u32 c2, const u32 c1 )
{
	u32 c = c1 & 0x80008000;
	
	c >>= 15;
	c += 0x7fff7fff;
	
	c &= c2;
	c |= c1;
	
	return c;
}

/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;32]
*/
inline u16 PixelBlend16 ( const u16 c2, const u32 c1, const u32 alpha )
{
	u32 srcRB = c1 & 0x7C1F;
	u32 srcXG = c1 & 0x03E0;

	u32 dstRB = c2 & 0x7C1F;
	u32 dstXG = c2 & 0x03E0;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 5;
	xg >>= 5;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x7C1F;
	xg &= 0x03E0;

	return rb | xg; 
}

/*!
	Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
*/
inline u32 PixelBlend32 ( const u32 c2, const u32 c1 )
{
	// alpha test
	u32 alpha = c1 & colormask::Alpha;

	if ( 0 == alpha )
		return c2;

	if ( colormask::Alpha == alpha )
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += ( alpha >> 7);

	u32 srcRB = c1 & colormask::RedBlue;
	u32 srcXG = c1 & colormask::Green;

	u32 dstRB = c2 & colormask::RedBlue;
	u32 dstXG = c2 & colormask::Green;

	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= colormask::RedBlue;
	xg &= colormask::Green;

	return rb | xg; 
}

/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;256]
*/
REALINLINE u32 PixelBlend32 ( const u32 c2, const u32 c1, u32 alpha )
{
	u32 srcRB = c1 & colormask::RedBlue;
	u32 srcXG = c1 & colormask::Green;

	u32 dstRB = c2 & colormask::RedBlue;
	u32 dstXG = c2 & colormask::Green;

	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= colormask::RedBlue;
	xg &= colormask::Green;

	return rb | xg; 
}

/*!
  Scale Color by (1/value)
  value 0 - 256 ( alpha )
*/
inline u32 PixelLerp32 ( const u32 source, const u32 value )
{
	u32 srcRB = source & colormask::RedBlue;
	u32 srcXG = (source & colormask::AlphaGreen) >> 8;

	srcRB *= value;
	srcXG *= value;

	srcRB >>= 8;
	//srcXG >>= 8;

	srcXG &= colormask::AlphaGreen;
	srcRB &= colormask::RedBlue;

	return srcRB | srcXG; 
}

/*
	return alpha in [0;256] Granularity
	 add highbit alpha ( alpha > 127 ? + 1 )
*/
inline u32 extractAlpha ( const u32 c )
{
	return ( c >> 24 ) + ( c >> 31 );
}

/*
	Pixel = c0 * (c1/31). 
*/
inline u16 PixelMul16_2 ( u16 c0, u16 c1)
{
	return	( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
			( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
			( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
			( c0 & c1 & 0x8000);
}

/*
	Pixel = c0 * (c1/255). 
*/
REALINLINE u32 PixelMul32_2 ( const u32 c0, const u32 c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = clamp ( c0 + c1, 0, 255 )
*/
REALINLINE u32 PixelAdd32 ( const u32 c2, const u32 c1)
{
	u32 sum = ( c2 & colormask::RGB )  + ( c1 & colormask::RGB );
	u32 low_bits = ( c2 ^ c1 ) & 0x00010101;
	s32 carries  = ( sum - low_bits ) & 0x01010100;
	u32 modulo = sum - carries;
	u32 clamp = carries - ( carries >> 8 );
	return modulo | clamp;
}

// some 2D Defines
struct AbsRectangle
{
	s32 x0;
	s32 y0;
	s32 x1;
	s32 y1;
};

inline void intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
{
	dest.x0 = core::s32_max( a.x0, b.x0 );
	dest.y0 = core::s32_max( a.y0, b.y0 );
	dest.x1 = core::s32_min( a.x1, b.x1 );
	dest.y1 = core::s32_min( a.y1, b.y1 );
}

inline bool isValid (const AbsRectangle& a)
{
	return a.x0 < a.x1 && a.y0 < a.y1;
}

struct SBlitJob
{
	AbsRectangle Dest;
	AbsRectangle Source;

	u32 argb;

	void * src;
	void * dst;

	s32 width;
	s32 height;

	u32 srcPitch;
	u32 dstPitch;

	u32 srcPixelMul;
	u32 dstPixelMul;
};

// Blitter Operation
enum eBlitter
{
	BLITTER_INVALID = 0,
	BLITTER_COLOR,
	BLITTER_COLOR_ALPHA,
	BLITTER_TEXTURE,
	BLITTER_TEXTURE_ALPHA_BLEND,
	BLITTER_TEXTURE_ALPHA_COLOR_BLEND
};

typedef void (*tExecuteBlit) ( const SBlitJob * job );


// Bitfields Cohen Sutherland
enum eClipCode
{
	CLIPCODE_EMPTY	=	0,
	CLIPCODE_BOTTOM	=	1,
	CLIPCODE_TOP	=	2,
	CLIPCODE_LEFT	=	4,
	CLIPCODE_RIGHT	=	8
};

inline u32 GetClipCode( const AbsRectangle &r, const core::position2d<s32> &p )
{
	u32 code = CLIPCODE_EMPTY;

	if ( p.X < r.x0 )
		code = CLIPCODE_LEFT;
	else
		if ( p.X > r.x1 )
			code = CLIPCODE_RIGHT;

	if ( p.Y < r.y0 )
		code |= CLIPCODE_TOP;
	else
		if ( p.Y > r.y1 )
			code |= CLIPCODE_BOTTOM;

	return code;
}


/*!
  Cohen Sutherland clipping
  @return: 1 if valid
*/

static int ClipLine(const AbsRectangle &clipping,
					core::position2d<s32> &p0,
					core::position2d<s32> &p1,
					const core::position2d<s32>& p0_in,
					const core::position2d<s32>& p1_in)
{
	u32 code0;
	u32 code1;
	u32 code;

	p0 = p0_in;
	p1 = p1_in;

	code0 = GetClipCode( clipping, p0 );
	code1 = GetClipCode( clipping, p1 );

	// trivial accepted
	while ( code0 | code1 )
	{
		s32 x=0;
		s32 y=0;

		// trivial reject
		if ( code0 & code1 )
			return 0;

		if ( code0 )
		{
			// clip first point
			code = code0;
		}
		else
		{
			// clip last point
			code = code1;
		}

		if ( (code & CLIPCODE_BOTTOM) == CLIPCODE_BOTTOM )
		{
			// clip bottom viewport
			y = clipping.y1;
			x = p0.X + ( p1.X - p0.X ) * ( y - p0.Y ) / ( p1.Y - p0.Y );
		}
		else
			if ( (code & CLIPCODE_TOP) == CLIPCODE_TOP )
			{
				// clip to viewport
				y = clipping.y0;
				x = p0.X + ( p1.X - p0.X ) * ( y - p0.Y ) / ( p1.Y - p0.Y );
			}
			else
				if ( (code & CLIPCODE_RIGHT) == CLIPCODE_RIGHT )
				{
					// clip right viewport
					x = clipping.x1;
					y = p0.Y + ( p1.Y - p0.Y ) * ( x - p0.X ) / ( p1.X - p0.X );
				}
				else
					if ( (code & CLIPCODE_LEFT) == CLIPCODE_LEFT )
					{
						// clip left viewport
						x = clipping.x0;
						y = p0.Y + ( p1.Y - p0.Y ) * ( x - p0.X ) / ( p1.X - p0.X );
					}

		if ( code == code0 )
		{
			// modify first point
			p0.X = x;
			p0.Y = y;
			code0 = GetClipCode( clipping, p0 );
		}
		else
		{
			// modify second point
			p1.X = x;
			p1.Y = y;
			code1 = GetClipCode( clipping, p1 );
		}
	}

	return 1;
}

/*
 */
inline void GetClip(AbsRectangle &clipping, video::IImage * t)
{
	clipping.x0 = 0;
	clipping.y0 = 0;
	clipping.x1 = t->getDimension().Width - 1;
	clipping.y1 = t->getDimension().Height - 1;
}

/*
 */
static void RenderLine32_Decal(video::IImage *t,
							   const core::position2d<s32> &p0,
							   const core::position2d<s32> &p1,
							   u32 argb )
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 4;
	s32 yInc = (s32) t->getPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u32 *dst;
	dst = (u32*) ( (u8*) t->lock() + ( p0.Y * t->getPitch() ) + ( p0.X << 2 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = argb;

		dst = (u32*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u32*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->unlock();
}


/*
 */
static void RenderLine32_Blend(video::IImage *t,
							   const core::position2d<s32> &p0,
							   const core::position2d<s32> &p1,
							   u32 argb, u32 alpha)
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 4;
	s32 yInc = (s32) t->getPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u32 *dst;
	dst = (u32*) ( (u8*) t->lock() + ( p0.Y * t->getPitch() ) + ( p0.X << 2 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = PixelBlend32( *dst, argb, alpha );

		dst = (u32*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u32*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->unlock();
}

/*
 */
static void RenderLine16_Decal(video::IImage *t,
							   const core::position2d<s32> &p0,
							   const core::position2d<s32> &p1,
							   u32 argb )
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 2;
	s32 yInc = (s32) t->getPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u16 *dst;
	dst = (u16*) ( (u8*) t->lock() + ( p0.Y * t->getPitch() ) + ( p0.X << 1 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = argb;

		dst = (u16*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u16*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->unlock();
}

/*
 */
static void RenderLine16_Blend(video::IImage *t,
							   const core::position2d<s32> &p0,
							   const core::position2d<s32> &p1,
							   u32 argb,
							   u32 alpha)
{
	s32 dx = p1.X - p0.X;
	s32 dy = p1.Y - p0.Y;

	s32 c;
	s32 m;
	s32 d = 0;
	s32 run;

	s32 xInc = 2;
	s32 yInc = (s32) t->getPitch();

	if ( dx < 0 )
	{
		xInc = -xInc;
		dx = -dx;
	}

	if ( dy < 0 )
	{
		yInc = -yInc;
		dy = -dy;
	}

	u16 *dst;
	dst = (u16*) ( (u8*) t->lock() + ( p0.Y * t->getPitch() ) + ( p0.X << 1 ) );

	if ( dy > dx )
	{
		s32 tmp;
		tmp = dx;
		dx = dy;
		dy = tmp;
		tmp = xInc;
		xInc = yInc;
		yInc = tmp;
	}

	c = dx << 1;
	m = dy << 1;

	run = dx;
	while ( run )
	{
		*dst = PixelBlend16( *dst, argb, alpha );

		dst = (u16*) ( (u8*) dst + xInc );	// x += xInc
		d += m;
		if ( d > dx )
		{
			dst = (u16*) ( (u8*) dst + yInc );	// y += yInc
			d -= c;
		}
		run -= 1;
	}

	t->unlock();
}


/*!
 */
static void executeBlit_TextureCopy_x_to_x( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	void *dst = (void*) job->dst;

	const u32 widthPitch = job->width * job->dstPixelMul;
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memcpy( dst, src, widthPitch );

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (void*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
 */
static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const u32 *src = static_cast<const u32*>(job->src);
	u16 *dst = static_cast<u16*>(job->dst);

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			//16 bit Blitter depends on pre-multiplied color
			const u32 s = PixelLerp32( src[dx] | colormask::Alpha, extractAlpha( src[dx] ) );
			dst[dx] = video::A8R8G8B8toA1R5G5B5( s );

		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	u16 *dst = (u16*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = video::RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
 */
static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = video::A1R5G5B5toA8R8G8B8( src[dx] );
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */

static void executeBlit_TextureCopy_16_to_32_RGBA( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = video::A1R5G5B5toR8G8B8A8( src[dx] );
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}
static void executeBlit_TextureCopy_32_to_32_RGBA( const SBlitJob * job )
{
	const u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = video::A8R8G8B8toR8G8B8A8( src[dx] );
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}
/*!
 */
static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	void *src = (void*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = colormask::Alpha | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
 */
static void executeBlit_TextureBlend_16_to_16( const SBlitJob * job )
{
	u32 dx;
	s32 dy;

	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;


	const u32 rdx = job->width >> 1;
	const u32 off = core::if_c_a_else_b( job->width & 1 ,job->width - 1, 0 );


	if ( 0 == off )
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
		}

	}
	else
	{
		for ( dy = 0; dy != job->height; ++dy )
		{
			for ( dx = 0; dx != rdx; ++dx )
			{
				dst[dx] = PixelBlend16_simd( dst[dx], src[dx] );
			}

			((u16*) dst)[off] = PixelBlend16( ((u16*) dst)[off], ((u16*) src)[off] );

			src = (u32*) ( (u8*) (src) + job->srcPitch );
			dst = (u32*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
 */
static void executeBlit_TextureBlend_32_to_32( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src[dx] );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static void executeBlit_TextureBlendColor_16_to_16( const SBlitJob * job )
{
	u16 *src = (u16*) job->src;
	u16 *dst = (u16*) job->dst;

	u16 blend = video::A8R8G8B8toA1R5G5B5 ( job->argb );
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], PixelMul16_2( src[dx], blend ) );
		}
		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}


/*!
 */
static void executeBlit_TextureBlendColor_32_to_32( const SBlitJob * job )
{
	u32 *src = (u32*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], PixelMul32_2( src[dx], job->argb ) );
		}
		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static void executeBlit_Color_16_to_16( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	u16 c0 = video::A8R8G8B8toA1R5G5B5( job->argb );
	u32 c = c0 | c0 << 16;

	if ( 0 == (job->srcPitch & 3 ) )
	{
		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}
	}
	else
	{
		s32 dx = job->width - 1;

		for ( s32 dy = 0; dy != job->height; ++dy )
		{
			memset32( dst, c, job->srcPitch );
			dst[dx] = c0;
			dst = (u16*) ( (u8*) (dst) + job->dstPitch );
		}

	}
}

/*!
 */
static void executeBlit_Color_32_to_32( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memset32( dst, job->argb, job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static void executeBlit_ColorAlpha_16_to_16( const SBlitJob * job )
{
	u16 *dst = (u16*) job->dst;

	const u32 alpha = extractAlpha( job->argb ) >> 3;
	const u32 src = video::A8R8G8B8toA1R5G5B5( job->argb );

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend16( dst[dx], src, alpha );
		}
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static void executeBlit_ColorAlpha_32_to_32( const SBlitJob * job )
{
	u32 *dst = (u32*) job->dst;

	const u32 alpha = extractAlpha( job->argb );
	const u32 src = job->argb;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = PixelBlend32( dst[dx], src, alpha );
		}
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

/*!
 */
static tExecuteBlit getBlitter( eBlitter operation,const video::IImage * dest,const video::IImage * source )
{
	video::ECOLOR_FORMAT sourceFormat = (video::ECOLOR_FORMAT) -1;
	video::ECOLOR_FORMAT destFormat = (video::ECOLOR_FORMAT) -1;

	if ( source )
		sourceFormat = source->getColorFormat();

	if ( dest )
		destFormat = dest->getColorFormat();

	switch ( operation )
	{
		case BLITTER_TEXTURE:
		{
			if ( sourceFormat == destFormat )
				return executeBlit_TextureCopy_x_to_x;

			if ( destFormat == video::ECF_A1R5G5B5 && sourceFormat == video::ECF_A8R8G8B8)
				return executeBlit_TextureCopy_32_to_16;

			if ( destFormat == video::ECF_A1R5G5B5 && sourceFormat == video::ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_16;

			if ( destFormat == video::ECF_A8R8G8B8 && sourceFormat == video::ECF_A1R5G5B5 )
				return executeBlit_TextureCopy_16_to_32;

            if ( destFormat == video::ECF_R8G8B8A8 && sourceFormat == video::ECF_A1R5G5B5 )
                return executeBlit_TextureCopy_16_to_32_RGBA;

			if ( destFormat == video::ECF_A8R8G8B8 && sourceFormat == video::ECF_R8G8B8 )
				return executeBlit_TextureCopy_24_to_32;

            
			if ( destFormat == video::ECF_R8G8B8A8 && sourceFormat == video::ECF_A8R8G8B8 )
				return executeBlit_TextureCopy_32_to_32_RGBA;

		} break;

		case BLITTER_TEXTURE_ALPHA_BLEND:
		{
			if ( destFormat == video::ECF_A1R5G5B5 && sourceFormat == video::ECF_A1R5G5B5 )
				return executeBlit_TextureBlend_16_to_16;

			if ( destFormat == video::ECF_A8R8G8B8 && sourceFormat == video::ECF_A8R8G8B8 )
				return executeBlit_TextureBlend_32_to_32;

		} break;

		case BLITTER_TEXTURE_ALPHA_COLOR_BLEND:
		{
			if ( destFormat == video::ECF_A1R5G5B5 && sourceFormat == video::ECF_A1R5G5B5 )
				return executeBlit_TextureBlendColor_16_to_16;

			if ( destFormat == video::ECF_A8R8G8B8 && sourceFormat == video::ECF_A8R8G8B8 )
				return executeBlit_TextureBlendColor_32_to_32;
		} break;

		case BLITTER_COLOR:
		{
			if ( destFormat == video::ECF_A1R5G5B5 )
				return executeBlit_Color_16_to_16;

			if ( destFormat == video::ECF_A8R8G8B8 )
				return executeBlit_Color_32_to_32;
		} break;

		case BLITTER_COLOR_ALPHA:
		{
			if ( destFormat == video::ECF_A1R5G5B5 )
				return executeBlit_ColorAlpha_16_to_16;

			if ( destFormat == video::ECF_A8R8G8B8 )
				return executeBlit_ColorAlpha_32_to_32;

		} break;

		case BLITTER_INVALID:
			break;
	}
/*
  char buf[64];
  sprintf( buf, "Blit: %d %d->%d unsupported",operation,sourceFormat,destFormat );
  os::Printer::log(buf );
*/
	return 0;

}



/*!
  a generic 2D Blitter
*/
static s32 Blit(eBlitter operation,
				video::IImage * dest,
				const core::rect<s32> *destClipping,
				const core::position2d<s32> *destPos,
				video::IImage * const source,
				const core::rect<s32> *sourceClipping,
				u32 argb)
{
	tExecuteBlit blitter = getBlitter( operation, dest, source );
	if ( 0 == blitter )
	{
		return 0;
	}

	// Clipping
	AbsRectangle sourceClip;
	AbsRectangle destClip;
	AbsRectangle v;

	SBlitJob job;

	if ( sourceClipping )
	{
		sourceClip.x0 = sourceClipping->UpperLeftCorner.X;
		sourceClip.y0 = sourceClipping->UpperLeftCorner.Y;
		sourceClip.x1 = sourceClipping->LowerRightCorner.X;
		sourceClip.y1 = sourceClipping->LowerRightCorner.Y;
	}
	else
	{
		sourceClip.x0 = 0;
		sourceClip.y0 = 0;
		sourceClip.x1 = source ? source->getDimension().Width : 0;
		sourceClip.y1 = source ? source->getDimension().Height : 0;
	}

	if ( destClipping )
	{
		destClip.x0 = destClipping->UpperLeftCorner.X;
		destClip.y0 = destClipping->UpperLeftCorner.Y;
		destClip.x1 = destClipping->LowerRightCorner.X;
		destClip.y1 = destClipping->LowerRightCorner.Y;
	}
	else
	{
		destClip.x0 = 0;
		destClip.y0 = 0;
		destClip.x1 = dest ? dest->getDimension().Width : 0;
		destClip.y1 = dest ? dest->getDimension().Height : 0;
	}

	v.x0 = destPos ? destPos->X : 0;
	v.y0 = destPos ? destPos->Y : 0;
	v.x1 = v.x0 + ( sourceClip.x1 - sourceClip.x0 );
	v.y1 = v.y0 + ( sourceClip.y1 - sourceClip.y0 );

	intersect( job.Dest, destClip, v );
	if ( !isValid( job.Dest ) )
		return 0;

	job.width = job.Dest.x1 - job.Dest.x0;
	job.height = job.Dest.y1 - job.Dest.y0;


	job.Source.x0 = sourceClip.x0 + ( job.Dest.x0 - v.x0 );
	job.Source.x1 = job.Source.x0 + job.width;

	job.Source.y0 = sourceClip.y0 + ( job.Dest.y0 - v.y0 );
	job.Source.y1 = job.Source.y0 + job.height;

	job.argb = argb;

	if ( source )
	{
		job.srcPitch = source->getPitch();
		job.srcPixelMul = source->getBytesPerPixel();
		job.src = (void*) ( (u8*) source->lock() + ( job.Source.y0 * job.srcPitch ) + ( job.Source.x0 * job.srcPixelMul ) );
	}
	else
	{
		// use srcPitch for color operation on dest
		job.srcPitch = job.width * dest->getBytesPerPixel();
	}

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBytesPerPixel();
	job.dst = (void*) ( (u8*) dest->lock() + ( job.Dest.y0 * job.dstPitch ) + ( job.Dest.x0 * job.dstPixelMul ) );

	blitter( &job );

	if ( source )
		source->unlock();

	if ( dest )
		dest->unlock();

	return 1;
}

}

//! constructor
CImage::CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size)
 :Data(0), Size(size), Format(format), DeleteMemory(true)
{
	initData();
}


//! constructor
CImage::CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size, void* data,
			   bool ownForeignMemory, bool deleteForeignMemory)
  : Data(0), Size(size), Format(format), DeleteMemory(deleteForeignMemory)
{
	if (ownForeignMemory)
	{
		Data = (void*)0xbadf00d;
		initData();
		Data = data;
	}
	else
	{
		Data = 0;
		initData();
		memcpy(Data, data, Size.Height * Pitch);
	}
}



//! constructor
CImage::CImage(ECOLOR_FORMAT format, IImage* imageToCopy)
  : Data(0), Format(format), DeleteMemory(true)
{
	if (!imageToCopy)
		return;

	Size = imageToCopy->getDimension();
	initData();

	// now copy data from other image

	Blit ( BLITTER_TEXTURE, this, 0, 0, imageToCopy, 0,0 );
}



//! constructor
CImage::CImage(IImage* imageToCopy, const core::position2d<s32>& pos,
			   const core::dimension2d<s32>& size)
	: Data(0), Size(0,0), DeleteMemory(true)
{
	if (!imageToCopy)
		return;

	Format = imageToCopy->getColorFormat();
	Size = size;

	initData();

	core::rect<s32> sClip( pos.X, pos.Y, pos.X + size.Width,pos.Y + size.Height );
	Blit (BLITTER_TEXTURE, this, 0, 0, imageToCopy, &sClip, 0);
}



//! assumes format and size has been set and creates the rest
void CImage::initData()
{
	setBitMasks();
	BitsPerPixel = getBitsPerPixelFromFormat(Format);
	BytesPerPixel = BitsPerPixel / 8;

	// Pitch should be aligned...
	Pitch = BytesPerPixel * Size.Width;

	if (!Data)
		Data = irrnew s8[Size.Height * Pitch];
}


//! destructor
CImage::~CImage()
{
	if ( DeleteMemory )
		delete [] (s8*)Data;
}


//! Returns width and height of image data.
const core::dimension2d<s32>& CImage::getDimension() const
{
	return Size;
}



//! Returns bits per pixel.
u32 CImage::getBitsPerPixel() const
{
	return BitsPerPixel;
}


//! Returns bytes per pixel
u32 CImage::getBytesPerPixel() const
{
	return BytesPerPixel;
}



//! Returns image data size in bytes
u32 CImage::getImageDataSizeInBytes() const
{
	return Pitch * Size.Height;
}



//! Returns image data size in pixels
u32 CImage::getImageDataSizeInPixels() const
{
	return Size.Width * Size.Height;
}



//! returns mask for red value of a pixel
u32 CImage::getRedMask() const
{
	return RedMask;
}



//! returns mask for green value of a pixel
u32 CImage::getGreenMask() const
{
	return GreenMask;
}



//! returns mask for blue value of a pixel
u32 CImage::getBlueMask() const
{
	return BlueMask;
}



//! returns mask for alpha value of a pixel
u32 CImage::getAlphaMask() const
{
	return AlphaMask;
}


void CImage::setBitMasks()
{
	switch(Format)
	{
		case ECF_A1R5G5B5:
			RedMask = 0x1F<<10;
			GreenMask = 0x1F<<5;
			BlueMask = 0x1F;
			AlphaMask = 0x1<<15;
			break;
		case ECF_R5G6B5:
			RedMask = 0x1F<<11;
			GreenMask = 0x3F<<5;
			BlueMask = 0x1F;
			AlphaMask = 0x0;
			break;
		case ECF_R8G8B8:
			RedMask   = 0x00FF0000;
			GreenMask = 0x0000FF00;
			BlueMask  = 0x000000FF;
			AlphaMask = 0x0;
			break;
		case ECF_A8R8G8B8:
			RedMask   = 0x00FF0000;
			GreenMask = 0x0000FF00;
			BlueMask  = 0x000000FF;
			AlphaMask = 0xFF000000;
			break;
        case ECF_R8G8B8A8:
			RedMask   = 0xFF000000;
			GreenMask = 0x00FF0000;
			BlueMask  = 0x0000FF00;
            AlphaMask = 0x000000FF;
            break;
		case ECF_A8:
			RedMask   = 0x0;
			GreenMask = 0x0;
			BlueMask  = 0x0;
			AlphaMask = 0xFF;
			break;
	}
}


u32 CImage::getBitsPerPixelFromFormat(ECOLOR_FORMAT format)
{
	switch(format)
	{
		case ECF_A1R5G5B5:
		case ECF_R5G6B5:
			return 16;
		case ECF_R8G8B8:
			return 24;
		case ECF_A8R8G8B8:
        case ECF_R8G8B8A8:
			return 32;
		case ECF_A8:
			return 8;
	}

	return 0;
}

//! sets a pixel
void CImage::setPixel(u32 x, u32 y, const SColor &color )
{
	if (x >= (u32)Size.Width || y >= (u32)Size.Height)
		return;

	switch(Format)
	{
		case ECF_A1R5G5B5:
		{
			u16 * dest = (u16*) ((u8*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = video::A1R5G5B5( color.A, color.R, color.G, color.B );
		} break;

		case ECF_R5G6B5:
		{
			u16 * dest = (u16*) ((u8*) Data + ( y * Pitch ) + ( x << 1 ));
			*dest = video::R5G6B5( color.R, color.G, color.B );
		} break;

		case ECF_R8G8B8:
		{
			u8* dest = (u8*) Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = color.R;
			dest[1] = color.G;
			dest[2] = color.B;
		} break;

		case ECF_A8R8G8B8:
		{
			u32 * dest = (u32*) ((u8*) Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.toARGB8();
		} break;

		case ECF_A8:
		{
			u8 * dest = (u8*) ((u8*) Data + ( y * Pitch ) + x);
			*dest = color.A;
		} break;
	}
}


//! returns a pixel
SColor CImage::getPixel(u32 x, u32 y) const
{
	if (x >= (u32)Size.Width || y >= (u32)Size.Height)
		return SColor(0,0,0,0);

	switch(Format)
	{
		case ECF_A1R5G5B5:
			return A1R5G5B5toSColor(((u16*)Data)[y*Size.Width + x]);
		case ECF_R5G6B5:
			return R5G6B5toSColor(((u16*)Data)[y*Size.Width + x]);
		case ECF_A8R8G8B8:
			return A8R8G8B8ToSColor(((u32*)Data)[y*Size.Width + x]);
		case ECF_R8G8B8:
		{
			u8* p = &((u8*)Data)[(y*3)*Size.Width + (x*3)];
			return SColor(255,p[0],p[1],p[2]);
		}
		case ECF_A8:
			return SColor(((u8*)Data)[y*Size.Width + x], 0, 0, 0);
	}

	return SColor(0,0,0,0);
}


//! returns the color format
ECOLOR_FORMAT CImage::getColorFormat() const
{
	return Format;
}

//! draws a rectangle
void CImage::drawRectangle(const core::rect<s32>& rect, const SColor &color)
{
	const u32 c = color.toARGB8(); 

	Blit(color.A == 0xFF ? BLITTER_COLOR : BLITTER_COLOR_ALPHA,
		 this, 0, &rect.UpperLeftCorner, 0, &rect, c);
}


//! copies this surface into another
void CImage::copyTo(IImage* target, const core::position2d<s32>& pos)
{
	Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0);
}


//! copies this surface into another
void CImage::copyTo(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect)
{
	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}



//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
void CImage::copyToWithAlpha(IImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect)
{
	const u32 c = (target->getColorFormat() == ECF_A8R8G8B8 
						? color.toARGB8() 
						: color.toRGBA8() ); 

	// color blend only necessary on not full spectrum aka. color.color != 0xFFFFFFFF
	Blit(color.A == 0xFF && color.R == 0xFF && color.G == 0xFF && color.B == 0xFF 
					? BLITTER_TEXTURE_ALPHA_BLEND
					: BLITTER_TEXTURE_ALPHA_COLOR_BLEND,
		 target, clipRect, &pos, this, &sourceRect, c);
}



//! draws a line from to with color
void CImage::drawLine(const core::position2d<s32>& from, const core::position2d<s32>& to, const SColor &color)
{
	AbsRectangle clip;
	GetClip( clip, this );

	core::position2d<s32> p[2];

	if ( ClipLine( clip, p[0], p[1], from, to ) )
	{
		// [0:265] : (color.A > 127 ? color.A + 1 : color.A )
		u32 alpha = color.A + ( color.A >> 7 );

		switch ( Format )
		{
			case ECF_A1R5G5B5:
				if ( alpha == 256 )
				{
					RenderLine16_Decal( this, p[0], p[1], video::A1R5G5B5( color.A, color.R, color.G, color.B ) );
				}
				else
				{
					RenderLine16_Blend( this, p[0], p[1], video::A1R5G5B5( color.A, color.R, color.G, color.B ), alpha >> 3 );
				}
				break;
			case ECF_A8R8G8B8:
				{
					const u32 c = color.toARGB8(); 
					if ( alpha == 256 )
					{
						RenderLine32_Decal( this, p[0], p[1], c );
					}
					else
					{
						RenderLine32_Blend( this, p[0], p[1], c, alpha );
					}
				}
				break;
		}
	}
}

//HACK:XBOX360 swizzle texutre:This is not the best solution for swizzled texture.
#ifdef _IRR_XBOX_PLATFORM_
//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void CImage::copyToScaling(void* target, s32 width, s32 height, ECOLOR_FORMAT format, u32 pitch)
{
	if (!target || !width || !height)
		return;

	const u32 bpp=getBitsPerPixelFromFormat(format)/8;
	if (0==pitch)
		pitch = width*bpp;

	if(bpp != 4)
	{
		return;
	}

	if (Format==format && Size.Width==width && Size.Height==height)
	{
		{
			//XGTileTextureLevel(width, height, 0, XGGetGpuFormat(D3DFMT_A8R8G8B8), 0, target, NULL, Data, pitch, NULL);
			u32* tgtpos = (u32*) target;
			u32* dstpos = (u32*) Data;
			const u32 bwidth = width*bpp;
			const s32 tex_w_pitch = pitch/bpp;
			for (s32 y=0; y<height; ++y)
			{
				dstpos = (((u32*)Data) + y*width);
				for (s32 x=0; x<width; ++x)
				{
					UINT tile_offset = XGAddress2DTiledOffset(x, y, tex_w_pitch, 4);
					tgtpos[tile_offset] = *dstpos++;
				}
			}
			return;
		}
	}
	const f32 sourceXStep = (f32)Size.Width / (f32)width;
	const f32 sourceYStep = (f32)Size.Height / (f32)height;
	s32 yval=0, syval=0;
	f32 sy = 0.0f;
	for (s32 y=0; y<height; ++y)
	{
		f32 sx = 0.0f;
		for (s32 x=0; x<width; ++x)
		{
			UINT tile_offset = XGAddress2DTiledOffset(x, y, width, 4);
			CColorConverter::convert_viaFormat(((u8*)Data)+ syval + ((s32)sx)*BytesPerPixel, Format, 1, ((u32*)target)+ tile_offset, format);	
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=((s32)sy)*Pitch;
		yval+=pitch;
	}
}

#else//_IRR_XBOX_PLATFORM_


//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void CImage::copyToScaling(void* target, s32 width, s32 height, ECOLOR_FORMAT format, u32 pitch)
{
	if (!target || !width || !height)
		return;

	const u32 bpp=getBitsPerPixelFromFormat(format)/8;
	if (0==pitch)
		pitch = width*bpp;

	if (Format==format && Size.Width==width && Size.Height==height)
	{
		if (pitch==Pitch)
		{
			memcpy(target, Data, height*pitch);
			return;
		}
		else
		{
			u8* tgtpos = (u8*) target;
			u8* dstpos = (u8*) Data;
			const u32 bwidth = width*bpp;
			for (s32 y=0; y<height; ++y)
			{
				memcpy(target, Data, height*pitch);
				memset(tgtpos+width, 0, pitch-bwidth);
				tgtpos += pitch;
				dstpos += Pitch;
			}
			return;
		}
	}

	const f32 sourceXStep = (f32)Size.Width / (f32)width;
	const f32 sourceYStep = (f32)Size.Height / (f32)height;
	s32 yval=0, syval=0;
	f32 sy = 0.0f;
	for (s32 y=0; y<height; ++y)
	{
		f32 sx = 0.0f;
		for (s32 x=0; x<width; ++x)
		{
			CColorConverter::convert_viaFormat(((u8*)Data)+ syval + ((s32)sx)*BytesPerPixel, Format, 1, ((u8*)target)+ yval + (x*bpp), format);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=((s32)sy)*Pitch;
		yval+=pitch;
	}
}
#endif //_IRR_XBOX_PLATFORM_
//! copies this surface into another, scaling it to the target image size
// note: this is very very slow. (i didn't want to write a fast version.
// but hopefully, nobody wants to scale surfaces every frame.
void CImage::copyToScaling(IImage* target)
{
	if (!target)
		return;

	const core::dimension2d<s32>& targetSize = target->getDimension();

	if (targetSize==Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->lock(), targetSize.Width, targetSize.Height, target->getColorFormat());
	target->unlock();
}

//! copies this surface into another, scaling it to fit it.
void CImage::copyToScalingBoxFilter(IImage* target, s32 bias)
{
	const core::dimension2d<s32> destSize = target->getDimension();

	const f32 sourceXStep = (f32) Size.Width / (f32) destSize.Width;
	const f32 sourceYStep = (f32) Size.Height / (f32) destSize.Height;

	target->lock();

	s32 fx = core::ceil32( sourceXStep );
	s32 fy = core::ceil32( sourceYStep );
	f32 sx;
	f32 sy;

	sy = 0.f;
	for ( s32 y = 0; y != destSize.Height; ++y )
	{
		sx = 0.f;
		for ( s32 x = 0; x != destSize.Width; ++x )
		{
			target->setPixel( x, y, getPixelBox( core::floor32( sx ), core::floor32( sy ), fx, fy, bias ) );
			sx += sourceXStep;
		}
		sy += sourceYStep;
	}

	target->unlock();
}


//! fills the surface with given color
void CImage::fill(const SColor &color)
{
	u32 c;

	switch ( Format )
	{
		case ECF_A1R5G5B5:
			c = video::A1R5G5B5( color.A, color.R, color.G, color.B  );
			c |= c << 16;
			break;
		case ECF_R5G6B5:
			c = video::R5G6B5( color.R, color.G, color.B );
			c |= c << 16;
			break;
		case ECF_A8R8G8B8:
			c = color.toARGB8();
			break;
		default:
//			os::Printer::log("CImage::Format not supported", ELL_ERROR);
			return;
	}

	memset32( Data, c, getImageDataSizeInBytes() );
}


//! get a filtered pixel
inline SColor CImage::getPixelBox( s32 x, s32 y, s32 fx, s32 fy, s32 bias ) const
{
	SColor c;
	s32 a = 0, r = 0, g = 0, b = 0;

	for ( s32 dx = 0; dx != fx; ++dx )
	{
		for ( s32 dy = 0; dy != fy; ++dy )
		{
			c = getPixel( x + dx , y + dy );

			a += c.A;
			r += c.R;
			g += c.G;
			b += c.B;
		}
	}

	s32 sdiv = s32_log2_s32(fx * fy);

	a = core::s32_clamp( ( a >> sdiv ) + bias, 0, 255 );
	r = core::s32_clamp( ( r >> sdiv ) + bias, 0, 255 );
	g = core::s32_clamp( ( g >> sdiv ) + bias, 0, 255 );
	b = core::s32_clamp( ( b >> sdiv ) + bias, 0, 255 );

	c.set( a, r, g, b );
	return c;
}


} // end namespace video
} // end namespace irr

