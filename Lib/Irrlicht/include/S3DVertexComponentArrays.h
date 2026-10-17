//-*-c++-*-
#ifndef __S_3D_VERTEX_COMPONENT_ARRAY_H_INCLUDED__
#define __S_3D_VERTEX_COMPONENT_ARRAY_H_INCLUDED__

#include "vector3d.h"
#include "vector2d.h"
#include "SColor.h"
#include "EVertexType.h"

namespace irr
{
namespace video
{

struct S3DVertex;
struct S3DVertex2TCoords;
struct S3DVertexTangents;

struct S3DVertexComponentArrays
{
	enum E_COMPONENT_TYPE
	{
		ECT_BYTE = 0,
		ECT_UNSIGNED_BYTE = 1,
		ECT_SHORT = 2,
		ECT_UNSIGNED_SHORT = 3,
		ECT_INT = 4,
		ECT_UNSIGNED_INT = 5,
		ECT_FLOAT = 6,
	};

	static const u32 MAX_TEXTURES = 2; // MATERIAL_MAX_TEXTURES is 4...

	// Maximum possible stride value (i.e. vertex size)
	static const u32 MAX_STRIDE = 2 * (sizeof(core::vector3df) + sizeof(core::vector2df)) + sizeof(SColor);

	template < typename T >
	struct SAccessor
	{
	public:

		SAccessor() {}

		SAccessor(int type, int stride, void* data, core::vector3df scale = core::vector3df(1.0), core::vector3df offset = core::vector3df(0)) 
			: Type(type), Stride(stride), Data(data), Scale(scale), Offset(offset) {}

		// Access the current component value
		T& operator*()
		{
			return *(T*)Data;
		}

		// Return the next component ptr
		SAccessor<T>& operator++()
		{
			Data = static_cast<char*>(Data) + Stride;
			return *this;
		}

		// Access the ith component
		T& operator[] (u32 i)
		{
			return *(T*)(((char*)Data) + (Stride * i));
		}

	protected:

		u32 Type;
		u32 Stride;
		void* Data;
		core::vector3df Scale;
		core::vector3df Offset;
	};

	template < template < typename V > class T, typename U >
	struct SAccessorEx
		: public SAccessor < T < U > >
	{
	public:
		SAccessorEx() {};

		SAccessorEx(u32 type, u32 stride, void* data, core::vector3df scale = core::vector3df(1.0), core::vector3df offset = core::vector3df(0) ) 
			: SAccessor< T < U > >( type, stride, data, scale, offset) {}

		// Return the next component ptr
		SAccessorEx &operator++(int)
		{
			SAccessor < T < U > >::Data = ((char*)SAccessor < T < U > >::Data) + SAccessor < T < U > >::Stride;
			return *this;
		}

		void get(u32 i, core::vector2d<float> &value)
		{
			T<U> &v = SAccessor < T < U > >::operator[](i);
			value.X = v.X * SAccessor < T < U > >::Scale.X + SAccessor < T < U > >::Offset.X;
			value.Y = v.Y * SAccessor < T < U > >::Scale.Y + SAccessor < T < U > >::Offset.Y;
		};

		void get(u32 i, core::vector3d<float> &value)
		{
			T<U>& v = SAccessor < T < U > >::operator[](i);
			value.X = v.X * SAccessor < T < U > >::Scale.X + SAccessor < T < U > >::Offset.X;
			value.Y = v.Y * SAccessor < T < U > >::Scale.Y + SAccessor < T < U > >::Offset.Y;
			value.Z = v.Z * SAccessor < T < U > >::Scale.Z + SAccessor < T < U > >::Offset.Z;
		};
	};

	typedef SAccessorEx< core::vector3d, float >	SAccessorVector3df;
	typedef SAccessorEx< core::vector3d, short >	SAccessorVector3ds;
	typedef SAccessorEx< core::vector3d, char >		SAccessorVector3db;
	typedef SAccessorEx< core::vector2d, float >	SAccessorVector2df;
	typedef SAccessorEx< core::vector2d, short >	SAccessorVector2ds;
	typedef SAccessor< video::SColor >				SAccessorColor;

	S3DVertexComponentArrays() 
		: Position(0)
		, PositionStride(sizeof(*Position))
		, PositionType(ECT_FLOAT)
		, Normal(0)
		, NormalStride(sizeof(*Normal))
		, NormalType(ECT_FLOAT)
		, Color0(0)
		, Color0Stride(sizeof(*Color0))
		, Color0Type(ECT_UNSIGNED_BYTE)
		, Color1(0)
		, Color1Stride(sizeof(*Color1))
		, Color1Type(ECT_UNSIGNED_BYTE)
	{
	}

	u32 PositionType;
	union				
	{
		core::vector3d<short>* PositionS16;
		core::vector3d<f32>* Position;
	};
	u32 PositionStride;

	u32 NormalType;
	union				
	{
		core::vector3df* Normal;
		core::vector3d<short>* NormalS16;
		core::vector3d<char>* NormalS8;
	};
	u32 NormalStride;

	// This is what we should eventually do:
	struct STexCoordComponent
	{
		u32 Type;
		union				
		{
			core::vector2d<short>* CoordS16;
			core::vector2df* Coord;
		};
		u32 Stride;
		core::vector3df Scale;
		core::vector3df Offset;

		STexCoordComponent()
			: Type(ECT_FLOAT)
			, Coord(0)
			, Stride(sizeof(*Coord))
		{
		}
	};

 	STexCoordComponent TexCoord[MAX_TEXTURES];

	u32 Color0Type;
	video::SColor* Color0;
	u32 Color0Stride;

	u32 Color1Type;
	video::SColor* Color1;
	u32 Color1Stride;

	/* ... */

	S3DVertexComponentArrays& operator = (S3DVertex* vertices);
	S3DVertexComponentArrays& operator = (S3DVertex2TCoords* vertices);
	S3DVertexComponentArrays& operator = (S3DVertexTangents* vertices);

	S3DVertexComponentArrays& assign(void* vertices, E_VERTEX_TYPE type);

	int getPositionAccessor(SAccessorVector3df& acc) const;
	int getPositionAccessor(SAccessorVector3ds& acc) const;
	int getNormalAccessor(SAccessorVector3df& acc) const;
	int getNormalAccessor(SAccessorVector3ds& acc) const;
	int getNormalAccessor(SAccessorVector3db& acc) const;
	int getTexCoordAccessor(u32 unit, SAccessorVector2df& acc) const;
	int getTexCoordAccessor(u32 unit, SAccessorVector2ds& acc) const;
	int getColor0Accessor(SAccessorColor& acc) const;
	int getColor1Accessor(SAccessorColor& acc) const;

	// Return a pointer to the scale/offset if they are not float, otherwise it return 0
	const core::vector3df* getPositionScale() const
	{
		return PositionType == ECT_FLOAT ? NULL : &PositionScale; 
	}

	const core::vector3df* getPositionOffset() const
	{
		return PositionType == ECT_FLOAT ? NULL : &PositionOffset; 
	}

	const core::vector3df* getTexCoordScale(u32 unit) const
	{ 
		_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
		return TexCoord[unit].Type == ECT_FLOAT ? NULL : &TexCoord[unit].Scale;
	}

	const core::vector3df* getTexCoordOffset(u32 unit) const
	{
		_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
		return TexCoord[unit].Type == ECT_FLOAT ? NULL : &TexCoord[unit].Offset;
	}

	// Set scale and offset if the components type are not float
	void setPositionScale(const core::vector3df& v)
	{
		PositionScale = v;
	}
	void setPositionOffset(const core::vector3df& v)
	{
		PositionOffset = v;
	}
	void setTexCoordScale(u32 unit, const core::vector3df& v)
	{
		_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
		TexCoord[unit].Scale = v;
	}
	void setTexCoordOffset(u32 unit, const core::vector3df& v)
	{
		_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
		TexCoord[unit].Offset = v;
	}

	bool isNull() const
	{
		bool result = (Position == NULL
					   && Normal == NULL
					   && Color0 == NULL
					   && Color1 == NULL);
		for (u32 i = 0; result && i < MAX_TEXTURES; ++i)
		{
			result &= TexCoord[i].Coord == NULL;
		}
		return result;
	}

protected: 

	// Those variables will be put in an external structure.
	// Use the accessor instead
	core::vector3df PositionScale;
	core::vector3df PositionOffset;
};

typedef S3DVertexComponentArrays SS3DVertexComponentArrays;

} // namespace video
} // namespace irr

#endif // __S_3D_VERTEX_COMPONENT_ARRAY_H_INCLUDED__
