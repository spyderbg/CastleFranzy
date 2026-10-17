// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __COLOR_H_INCLUDED__
#define __COLOR_H_INCLUDED__

#include "irrTypes.h"
#include "irrMath.h"

namespace irr
{
namespace video
{	
namespace colormask
{
namespace ARGB
{
static const u32 Alpha = 0xFF000000;
static const u32 Red   = 0x00FF0000;
static const u32 Green = 0x0000FF00;
static const u32 Blue  = 0x000000FF;
}
namespace RGBA
{
static const u32 Red   = 0x000000FF;
static const u32 Green = 0x0000FF00;
static const u32 Blue  = 0x00FF0000;
static const u32 Alpha = 0xFF000000;
}
static const u32 RedBlue    = 0x00FF00FF;
static const u32 AlphaGreen = 0xFF00FF00;
static const u32 RGB        = 0x00FFFFFF;
static const u32 &Alpha     = ARGB::Alpha;
static const u32 &Green     = ARGB::Green;
}

//! Creates a 16 bit A1R5G5B5 color
inline u16 RGBA16(u32 r, u32 g, u32 b, u32 a=0xFF)
{
	return ((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}


//! Creates a 16 bit A1R5G5B5 color
inline u16 RGB16(u32 r, u32 g, u32 b)
{
	return RGBA16(r,g,b, 0xFF);
}


//! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
inline u16 X8R8G8B8toA1R5G5B5(u32 color)
{
	return (0x8000 |
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}


inline u16 A1R5G5B5(u8 r, u8 g, u8 b)
{
	return (0x8000 | (u16)r >> 9 | (u16)g >> 6 | (u16)b >> 3);
}


//! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
inline u16 A8R8G8B8toA1R5G5B5(u32 color)
{
	return (( color & 0x80000000) >> 16|
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}
inline u32 A8R8G8B8toR8G8B8A8(u32 color)
{
	return (color<<8)|(color>>24);
}
inline u16 A1R5G5B5(u8 a, u8 r, u8 g, u8 b)
{
	return ( a ? 0x8000 : 0 | (u16)r >> 9 | (u16)g >> 6 | (u16)b >> 3);
}


//! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
inline u16 A8R8G8B8toR5G6B5(u32 color)
{
	return (( color & 0x00F80000) >> 8 |
		( color & 0x0000FC00) >> 5 |
		( color & 0x000000F8) >> 3);
}

//! Create a 16bit R5G6B5 color
inline u16 R5G6B5(u8 r, u8 g, u8 b)
{
	return ((u16)r >> 8 | (u16)g >> 5 | (u16)b >> 3);
}


//! Convert A8R8G8B8 Color from A1R5G5B5 color
/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
inline u32 A1R5G5B5toA8R8G8B8(u16 color)
{
	return ( (( -( (s32) color & 0x00008000 ) >> (s32) 31 ) & 0xFF000000 ) |
			 (( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
			 (( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
		 	 (( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
		   );
}

//! Convert R8G8B8A8 Color from A1R5G5B5 color
/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
inline u32 A1R5G5B5toR8G8B8A8(u16 color)
{
	u32 argb = A1R5G5B5toA8R8G8B8(color);
    return (argb<<8) | (argb>>24);
}

//! Returns A8R8G8B8 Color from R5G6B5 color
inline u32 R5G6B5toA8R8G8B8(u16 color)
{
	return 0xFF000000 |
		((color & 0xF800) << 8)|
		((color & 0x07E0) << 5)|
		((color & 0x001F) << 3);
}


//! Returns A1R5G5B5 Color from R5G6B5 color
inline u16 R5G6B5toA1R5G5B5(u16 color)
{
	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}


//! Returns R5G6B5 Color from A1R5G5B5 color
inline u16 A1R5G5B5toR5G6B5(u16 color)
{
	return (((color & 0x7FE0) << 1) | (color & 0x1F));
}


//! Returns the alpha component from A1R5G5B5 color
inline u8 getA1R5G5BAlpha(u16 color)
{
	return ((color >> 15)&0x1);
}


//! Returns the red component from A1R5G5B5 color.
/** Shift left by 3 to get 8 bit value. */
inline u8 getA1R5G5BRed(u16 color)
{
	return ((color >> 10)&0x1F);
}


//! Returns the green component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline u8 getA1R5G5BGreen(u16 color)
{
	return ((color >> 5)&0x1F);
}


//! Returns the blue component from A1R5G5B5 color
/** Shift left by 3 to get 8 bit value. */
inline u8 getA1R5G5BBlue(u16 color)
{
	return (color & 0x1F);
}


//! Returns the average from a 16 bit A1R5G5B5 color
inline u8 getA1R5G5BAverage(s16 color)
{
	return ((getA1R5G5BRed(color)<<3) + (getA1R5G5BGreen(color)<<3) + (getA1R5G5BBlue(color)<<3)) / 3;
}


//! Class representing a 32 bit ARGB color.
/** The color values for alpha, red, green, and blue are
stored in four u8. So all four values may be between 0 and 255.
This class is used by most parts of the Irrlicht Engine
to specify a color. Another way is using the class SColorf, which
stores the color values in 4 floats.
*/
class SColor
{
public:

	//! Constructor of the Color. Does nothing.
	/** The color value is not initialized to save time. */
	SColor() {}

	//! Constructs the color from 4 values representing the alpha, red, green and blue component.
	/** Must be values between 0 and 255. */
	SColor (u8 a, u8 r, u8 g, u8 b)
#if defined(_IRR_SCOLOR_USE_ARGB_)
#    ifdef _IRR_BIG_ENDIAN_
		:A(a)
		, R(r)
		, G(g)
		, B(b)
#    else
		: B(b)
		, G(g)
		, R(r)
		, A(a)
#    endif
#else
		: R(r)
		, G(g)
		, B(b)
		, A(a)
#endif
	{

	}

#ifdef _IRR_KEEP_DEPRECATED_
	//! Constructs the color from a 32 bit value. Could be another color.
	//! @deprecated this could cause problem with endianness
	SColor(u32 clr)
	{
		DEPRECATED("This constructor is deprecated, use	SColor(u8 a, u8 r, u8 g, u8 b)", "1.5.0.0");
		//*(u32*)this = clr;
		set((clr & colormask::ARGB::Alpha) >> 24,
		    (clr & colormask::ARGB::Red)   >> 16,
		    (clr & colormask::ARGB::Green) >> 8,
		    (clr & colormask::ARGB::Blue));
	}

	//! Returns the alpha component of the color.
	/** The alpha component defines how transparent a color should
	be. 255 means not transparent (opaque), 0 means fully
	transparent. */
	//! @deprecated 
	u8 getAlpha() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return A; 
	}

	//! Returns the red component of the color.
	/** \return Value between 0 and 255, specifying how red the color is.
	0 means no red, 255 means full red. */
	//! @deprecated 
	u8 getRed() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return R; 
	}

	//! Returns the green component of the color.
	/** \return Value between 0 and 255, specifying how green the color is.
	0 means no green, 255 means full green. */
	//! @deprecated 
	u8 getGreen() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return G; 
	}

	//! Returns the blue component of the color.
	/** \return Value between 0 and 255, specifying how blue the color is.
	0 means no blue, 255 means full blue. */
	//! @deprecated 
	u8 getBlue() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return B; 
	}

	//! Sets the alpha component of the Color.
	/** The alpha component defines how transparent a color should
	be.
	\param a: Has to be a value between 0 and 255.
	255 means not transparent (opaque), 0 means fully transparent. */
	//! @deprecated 
	void setAlpha(u8 a) 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		A = a; 
	}

	//! Sets the red component of the Color.
	/** \param r: Has to be a value between 0 and 255.
	0 means no red, 255 means full red. */
	//! @deprecated 
	void setRed(u8 r) 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		R = r; 
	}

	//! Sets the green component of the Color.
	/** \param g: Has to be a value between 0 and 255.
	0 means no green, 255 means full green. */
	//! @deprecated 
	void setGreen(u8 g) 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		G = g; 
	}

	//! Sets the blue component of the Color.
	/** \param b: Has to be a value between 0 and 255.
	0 means no blue, 255 means full blue. */
	//! @deprecated 
	void setBlue(u8 b) 
	{  
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		B = b; 
	}
#endif
	//! Returns the luminance of the color.
	f32 getLuminance() const
	{
		return 0.3f*R + 0.59f*G + 0.11f*B;
	}

	//! Returns the average intensity of the color.
	u8 getAverage() const
	{
		return (u8)( (u32)R + G + B ) / 3;
	}

	//! Calculates a 16 bit A1R5G5B5 value of this color.
	/** \return 16 bit A1R5G5B5 value of this color. */
	u16 toA1R5G5B5() const 
	{ 
		return A1R5G5B5(A, R, G, B); 
	}

	u16 toR5G6B5() const 
	{ 
		return R5G6B5(R, G, B); 
	}
	
	//! Converts color to DirectX color format
	/** To ARGB in 4 byte components for endian aware passing to DirectX
	\param dest: address where the 4x8 bit DirectX color is stored. */
	inline void toARGB8(u8* dest) const
	{
#ifdef _IRR_SCOLOR_USE_ARGB_
		*reinterpret_cast<SColor*>(dest) = *this;
#elif defined(_IRR_BIG_ENDIAN_)
		*dest =  A;
		*++dest =  R;
		*++dest =  B;
		*++dest =  G;
#else
		*dest =  B;
		*++dest =  G;
		*++dest =  R;
		*++dest =  A;
#endif
	}

	u32 toARGB8() const
	{
		u32 data;
		toARGB8((u8*)&data);
		return data;
	};

	//! Converts color to OpenGL color format
	/** To RGBA in 4 byte components for endian aware passing to OpenGL
	\param dest: address where the 4x8 bit OpenGL color is stored. */
	inline void toRGBA8(u8* dest) const
	{
#ifdef _IRR_SCOLOR_USE_ARGB_
		*dest =  R;
		*++dest =  G;
		*++dest =  B;
		*++dest =  A;
#else
		*reinterpret_cast<SColor*>(dest) = *this;
#endif
	}

	u32 toRGBA8() const
	{
		u32 data;
		toRGBA8((u8*)&data);
		return data;
	};

#ifdef _IRR_KEEP_DEPRECATED_
	//! Converts color to OpenGL color format
	//! @deprecated renamed to toRGBA8
	void toOpenGLColor(u8* dest) const
	{
		DEPRECATED("toOpenGLColor renamed to toRGBA8", "1.4.1.0");
		toRGBA8(dest);
	};
#endif

	//! Sets all four components of the color at once.
	/** Constructs the color from 4 values representing the alpha,
	red, green and blue components of the color. Must be values
	between 0 and 255.
	\param a: Alpha component of the color. The alpha component
	defines how transparent a color should be. Has to be a value
	between 0 and 255. 255 means not transparent (opaque), 0 means
	fully transparent.
	\param r: Sets the red component of the Color. Has to be a
	value between 0 and 255. 0 means no red, 255 means full red.
	\param g: Sets the green component of the Color. Has to be a
	value between 0 and 255. 0 means no green, 255 means full
	green.
	\param b: Sets the blue component of the Color. Has to be a
	value between 0 and 255. 0 means no blue, 255 means full blue. */
	void set(u8 a, u8 r, u8 g, u8 b)
	{
		A = a;
		R = r;
		G = g;
		B = b;
	}

#ifdef _IRR_KEEP_DEPRECATED_
	void set(u32 col) 
	{ 
		DEPRECATED("Setting a color with an int is deprecated as it's prone to endiannes problems", "1.5.0.0");
		//*(u32*)this = col;
		set((col & colormask::ARGB::Alpha) >> 24,
		    (col & colormask::ARGB::Red)   >> 16,
		    (col & colormask::ARGB::Green) >> 8,
		    (col & colormask::ARGB::Blue));
	}
#endif

	//! Compares the color to another color.
	/** \return True if the colors are the same, and false if not. */
	bool operator==(const SColor& other) const { return (*(u32*)this) == (*(u32*)&other); }

	//! Compares the color to another color.
	/** \return True if the colors are different, and false if they are the same. */
	bool operator!=(const SColor& other) const { return (*(u32*)this) != (*(u32*)&other); }

	//! comparison operator
	/** \return True if this color is smaller than the other one */
	bool operator<(const SColor& other) const { return (*(u32*)this) < (*(u32*)&other);; }

	//! Adds two colors, result is clamped to 0..255 values
	/** \param other Color to add to this color
	\return Addition of the two colors, clamped to 0..255 values */
	SColor operator+(const SColor& other) const
	{
		return SColor(core::min_((u32)A + other.A, (u32)255u),
					  core::min_((u32)R + other.R, (u32)255u),
					  core::min_((u32)G + other.G, (u32)255u),
					  core::min_((u32)B + other.B, (u32)255u));
	}

	//! Adds two colors, result is clamped to 0..255 values
	/** \param other Color to add to this color
	\return Addition of the two colors, clamped to 0..255 values */
	SColor& operator+=(const SColor& other)
	{
		A = core::min_((u32)A + other.A, (u32)255u);
		R = core::min_((u32)R + other.R, (u32)255u);
		G = core::min_((u32)G + other.G, (u32)255u);
		B = core::min_((u32)B + other.B, (u32)255u);
		return *this;
	}

	//! Adds two colors, result is clamped to 0..255 values
	/** \param other Color to add to this color
	\return Addition of the two colors, clamped to 0..255 values */
	SColor operator*(f32 factor) const
	{
		return SColor(core::min_((u32)((u32)A * factor), (u32)255u),
					  core::min_((u32)((u32)R * factor), (u32)255u),
					  core::min_((u32)((u32)G * factor), (u32)255u),
					  core::min_((u32)((u32)B * factor), (u32)255u));
	}

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f
	\return Interpolated color. */
	SColor getInterpolated(const SColor &other, f32 d) const
	{
		d = 1.0f - core::clamp(d, 0.f, 1.f);
		return SColor((u8)core::clamp<f32>(core::lerp<f32>(A, other.A, d), (f32)0.0f, (f32)255.0f ),
					  (u8)core::clamp<f32>(core::lerp<f32>(R, other.R, d), (f32)0.0f, (f32)255.0f ),
					  (u8)core::clamp<f32>(core::lerp<f32>(G, other.G, d), (f32)0.0f, (f32)255.0f ),
					  (u8)core::clamp<f32>(core::lerp<f32>(B, other.B, d), (f32)0.0f, (f32)255.0f ));
	}

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	SColor getInterpolated_quadratic(const SColor& c1, const SColor& c2, f32 d) const
	{
		// this*(1-d)*(1-d) + 2 * c1 * (1-d) + c2 * d * d;
		d = core::clamp(d, 0.f, 1.f);
		const f32 inv = 1.f - d;
		const f32 mul0 = inv * inv;
		const f32 mul1 = 2.f * d * inv;
		const f32 mul2 = d * d;

		return SColor(core::clamp<s32>(core::floor32(A * mul0 + c1.A * mul1 + c2.A * mul2 ), 
									   (irr::s32)0, (irr::s32)255 ),
					  core::clamp<s32>(core::floor32(R * mul0 + c1.R * mul1 + c2.R * mul2 ), 
									   (irr::s32)0, (irr::s32)255 ),
					  core::clamp<s32>(core::floor32(G * mul0 + c1.G * mul1 + c2.G * mul2 ), 
									   (irr::s32)0, (irr::s32)255 ),
					  core::clamp<s32>(core::floor32(B * mul0 + c1.B * mul1 + c2.B * mul2 ), 
									   (irr::s32)0, (irr::s32)255 ));
	}
	
	inline u32* getDataPtr()
	{
		return (u32*)this;
	}

#ifdef _IRR_SCOLOR_USE_ARGB_ 

#    ifdef _IRR_BIG_ENDIAN_
	u8 A, R, G, B;
#    else

	u8 B, G, R, A;
#    endif

#else // then it's _IRR_SCOLOR_USE_RGBA_
	u8 R, G, B, A;
#endif
};

inline SColor A8R8G8B8ToSColor(u32 color)
{
	return SColor((color & colormask::ARGB::Alpha) >> 24,
				  (color & colormask::ARGB::Red)   >> 16,
				  (color & colormask::ARGB::Green) >> 8,
				  (color & colormask::ARGB::Blue));
}

//! Convert SColor from A1R5G5B5 color
/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
inline SColor A1R5G5B5toSColor(u16 color)
{
	return ( SColor( ( color & 0x8000 ) ? 0xff : 0,
					 (( color & 0x7C00 ) >> 7) | (( color & 0x7000 ) >> 12),
					 (( color & 0x03E0 ) >> 2) | (( color & 0x0380 ) >> 3), 
					 (( color & 0x001F ) << 3) | (( color & 0x001C ) >> 2)
				    )
			);
}

//! Returns SColor Color from R5G6B5 color
inline SColor R5G6B5toSColor(u16 color)
{
	return SColor(0xFF, (color & 0xF800) >> 8,(color & 0x07E0) >> 3, (color & 0x001F) << 3 );
}



//! Class representing a color with four floats.
/** The color values for red, green, blue
and alpha are each stored in a 32 bit floating point variable.
So all four values may be between 0.0f and 1.0f.
Another, faster way to define colors is using the class SColor, which
stores the color values in a single 32 bit integer.
*/
class SColorf
{
public:
	//! Constructs a color from up to four color values: red, green, blue, and alpha.
	/** \param r: Red color component. Should be a value between
	0.0f meaning no red and 1.0f, meaning full red.
	\param g: Green color component. Should be a value between 0.0f
	meaning no green and 1.0f, meaning full green.
	\param b: Blue color component. Should be a value between 0.0f
	meaning no blue and 1.0f, meaning full blue.
	\param a: Alpha color component of the color. The alpha
	component defines how transparent a color should be. Has to be
	a value between 0.0f and 1.0f, 1.0f means not transparent
	(opaque), 0.0f means fully transparent. */
	SColorf(f32 r=0.f, f32 g=0.f, f32 b=0.f, f32 a=1.f) 
#ifdef _IRR_SCOLOR_USE_ARGB_ 
#    ifdef _IRR_BIG_ENDIAN_
		: R(r), G(g), B(b), A(a) 
#    else
		: B(b), G(g), R(r), A(a) 
#    endif
#else
		: R(r), G(g), B(b), A(a) 
#endif
	{}

	//! Constructs a color from 32 bit Color.
	/** \param c: 32 bit color from which this SColorf class is
	constructed from. */
	SColorf(SColor c)
	{
		const f32 inv = 1.0f / 255.0f;
		R = c.R * inv;
		G = c.G * inv;
		B = c.B * inv;
		A = c.A * inv;
	}

	//! Converts this color to a SColor without floats.
	SColor toSColor() const
	{
		return SColor((u8)(A*255.0f), (u8)(R*255.0f), (u8)(G*255.0f), (u8)(B*255.0f));
	}

	//! Sets three color components to new values at once.
	/** \param rr: Red color component. Should be a value between 0.0f meaning
	no red (=black) and 1.0f, meaning full red.
	\param gg: Green color component. Should be a value between 0.0f meaning
	no green (=black) and 1.0f, meaning full green.
	\param bb: Blue color component. Should be a value between 0.0f meaning
	no blue (=black) and 1.0f, meaning full blue. */
	void set(f32 r, f32 g, f32 b) {R = r; G = g; B = b; }

	//! Sets all four color components to new values at once.
	/** \param aa: Alpha component. Should be a value between 0.0f meaning
	fully transparent and 1.0f, meaning opaque.
	\param rr: Red color component. Should be a value between 0.0f meaning
	no red and 1.0f, meaning full red.
	\param gg: Green color component. Should be a value between 0.0f meaning
	no green and 1.0f, meaning full green.
	\param bb: Blue color component. Should be a value between 0.0f meaning
	no blue and 1.0f, meaning full blue. */
	void set(f32 a, f32 r, f32 g, f32 b) {A = a; R = r; G = g; B = b; }

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f
	\return Interpolated color. */
	SColorf getInterpolated(const SColorf &other, f32 d) const
	{
		d = core::clamp(d, 0.f, 1.f);
		const f32 inv = 1.0f - d;
		return SColorf(other.R*inv + R*d,
					   other.G*inv + G*d, 
					   other.B*inv + B*d, 
					   other.A*inv + A*d);
	}

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	inline SColorf getInterpolated_quadratic(const SColorf& c1, const SColorf& c2,
			f32 d) const
	{
		d = core::clamp(d, 0.f, 1.f);
		// this*(1-d)*(1-d) + 2 * c1 * (1-d) + c2 * d * d;
		const f32 inv = 1.f - d;
		const f32 mul0 = inv * inv;
		const f32 mul1 = 2.f * d * inv;
		const f32 mul2 = d * d;

		return SColorf (R * mul0 + c1.R * mul1 + c2.R * mul2,
						G * mul0 + c1.G * mul1 + c2.G * mul2,
						B * mul0 + c1.B * mul1 + c2.B * mul2,
						A * mul0 + c1.A * mul1 + c2.A * mul2);
	}


	//! Sets a color component by index. R=0, G=1, B=2, A=3
	void setColorComponentValue(u8 index, f32 value)
	{
		switch(index)
		{
		case 0: R = value; break;
		case 1: G = value; break;
		case 2: B = value; break;
		case 3: A = value; break;
		}
	}

#ifdef _IRR_KEEP_DEPRECATED_
	//! Returns the alpha component of the color in the range 0.0 to 1.0
	f32 getAlpha() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return A; 
	}

	//! Returns the red component of the color in the range 0.0 to 1.0
	f32 getRed() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return R; 
	}

	//! Returns the green component of the color in the range 0.0 to 1.0
	f32 getGreen() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return G; 
	}

	//! Returns the blue component of the color in the range 0.0 to 1.0
	f32 getBlue() const 
	{ 
		DEPRECATED("This accessor is deprecated, you shoudl access the veriable directly", "1.5.0.0");
		return B; 
	}
#endif

#ifdef _IRR_SCOLOR_USE_ARGB_ // this is somewhat a lie here...
//#    ifdef _IRR_BIG_ENDIAN_
	//! red color component
	f32 R;
	//! green color component
	f32 G;
	//! blue component
	f32 B;
	//! alpha color component
	f32 A;
//#    else // needs validation below with DirectX on PC: is it really BGRA or actually RBGA?
//	//! alpha color component
//	f32 A;
//
//	f32 B;
//	//! red color component
//	f32 G;
//	//! green color component
//	f32 R;
//	//! blue component
//	f32 A;
//
//#    endif

#else
	//! red color component
	f32 R;
	//! green color component
	f32 G;
	//! blue component
	f32 B;
	//! alpha color component
	f32 A;
#endif
};


//! Class representing a color in HSV format
/** The color values for hue, saturation, value
are stored in a 32 bit floating point variable.
*/
class SColorHSL
{
public:
	SColorHSL ( f32 h = 0.f, f32 s = 0.f, f32 l = 0.f )
		: Hue ( h ), Saturation ( s ), Luminance ( l ) {}

	void fromRGB(const SColor &color);
	void toRGB(SColor &color) const;

	f32 Hue;
	f32 Saturation;
	f32 Luminance;

private:
	inline u32 toRGB1(f32 rm1, f32 rm2, f32 rh) const;

};

inline void SColorHSL::fromRGB(const SColor &color)
{
	const f32 maxVal = (f32)core::max_(color.R, color.G, color.B);
	const f32 minVal = (f32)core::min_(color.R, color.G, color.B);
	Luminance = (maxVal/minVal)*0.5f;
	if (maxVal==minVal)
	{
		Hue=0.f;
		Saturation=0.f;
		return;
	}

	const f32 delta = maxVal-minVal;
	if ( Luminance <= 0.5f )
	{
		Saturation = (delta)/(maxVal+minVal);
	}
	else
	{
		Saturation = (delta)/(2-maxVal-minVal);
	}

	if (maxVal==color.R)
		Hue = (color.R-color.B)/delta;
	else if (maxVal==color.G)
		Hue = 2+(color.B-color.R)/delta;
	else if (maxVal==color.B)
		Hue = 4+(color.R-color.G)/delta;

	Hue *= (60.0f * core::DEGTORAD);
	while ( Hue < 0.f )
		Hue += 2.f * core::PI;
}


inline void SColorHSL::toRGB(SColor &color) const
{
	if ( Saturation == 0.0f) // grey
	{
		u8 c = (u8) ( Luminance * 255.0 );
		color.R = c;
		color.G = c;
		color.B = c;
		return;
	}

	f32 rm2;

	if ( Luminance <= 0.5f )
	{
		rm2 = Luminance + Luminance * Saturation;
	}
	else
	{
		rm2 = Luminance + Saturation - Luminance * Saturation;
	}

	const f32 rm1 = 2.0f * Luminance - rm2;

	color.R = toRGB1(rm1, rm2, Hue + (120.0f * core::DEGTORAD ));
	color.G = toRGB1(rm1, rm2, Hue);
	color.B = toRGB1(rm1, rm2, Hue - (120.0f * core::DEGTORAD) );
}


inline u32 SColorHSL::toRGB1(f32 rm1, f32 rm2, f32 rh) const
{
	while ( rh > 2.f * core::PI )
		rh -= 2.f * core::PI;

	while ( rh < 0.f )
		rh += 2.f * core::PI;

	if (rh < 60.0f * core::DEGTORAD )
		rm1 = rm1 + (rm2 - rm1) * rh / (60.0f * core::DEGTORAD);
	else if (rh < 180.0f * core::DEGTORAD )
		rm1 = rm2;
	else if (rh < 240.0f * core::DEGTORAD )
		rm1 = rm1 + (rm2 - rm1) * ( ( 240.0f * core::DEGTORAD ) - rh) /
			(60.0f * core::DEGTORAD);

	return (u32) (rm1 * 255.f);
}

} // end namespace video
} // end namespace irr

#endif

