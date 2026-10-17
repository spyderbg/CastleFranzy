//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_3D_VERTEX_H_INCLUDED__
#define __S_3D_VERTEX_H_INCLUDED__

#include "S3DVertexComponentArrays.h"

namespace irr
{
namespace video
{

//! Array holding the built in vertex type names
const char* const sBuiltInVertexTypeNames[] =
{
	"standard",
	"2tcoords",
	"tangents",
	0
};

//! standard vertex used by the Irrlicht engine.
struct S3DVertex
{
	//! default constructor
	S3DVertex() {}

	//! constructor
	S3DVertex(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
		: Pos(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

	//! constructor
	S3DVertex(const core::vector3df& pos, const core::vector3df& normal,
		SColor color, const core::vector2d<f32>& tcoords)
		: Pos(pos), Normal(normal), Color(color), TCoords(tcoords) {}

	//! Position
	core::vector3df Pos;

	//! Normal vector
	core::vector3df Normal;

	//! Color
	SColor Color;

	//! Texture coordinates
	core::vector2d<f32> TCoords;

	bool operator==(const S3DVertex& other) const
	{
		return ((Pos == other.Pos) && (Normal == other.Normal) &&
			(Color == other.Color) && (TCoords == other.TCoords));
	}

	bool operator!=(const S3DVertex& other) const
	{
		return ((Pos != other.Pos) || (Normal != other.Normal) ||
			(Color != other.Color) || (TCoords != other.TCoords));
	}

	bool operator<(const S3DVertex& other) const
	{
		return ((Pos < other.Pos) ||
				((Pos == other.Pos) && (Normal < other.Normal)) ||
				((Pos == other.Pos) && (Normal == other.Normal) && (Color < other.Color)) ||
				((Pos == other.Pos) && (Normal == other.Normal) && (Color == other.Color) && (TCoords < other.TCoords)));
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_STANDARD;
	}
};


//! Vertex with two texture coordinates.
/** Usually used for geometry with lightmaps
or other special materials.
*/
struct S3DVertex2TCoords : public S3DVertex
{
	//! default constructor
	S3DVertex2TCoords() : S3DVertex() {}

	//! constructor with two different texture coords, but no normal
	S3DVertex2TCoords(f32 x, f32 y, f32 z, SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2)
		: S3DVertex(x,y,z, 0.0f, 0.0f, 0.0f, c, tu,tv), TCoords2(tu2,tv2) {}

	//! constructor with two different texture coords, but no normal
	S3DVertex2TCoords(const core::vector3df& pos, SColor color,
		const core::vector2d<f32>& tcoords, const core::vector2d<f32>& tcoords2)
		: S3DVertex(pos, core::vector3df(), color, tcoords), TCoords2(tcoords2) {}

	//! constructor with all values
	S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal, const SColor& color,
		const core::vector2d<f32>& tcoords, const core::vector2d<f32>& tcoords2)
		: S3DVertex(pos, normal, color, tcoords), TCoords2(tcoords2) {}

	//! constructor with all values
	S3DVertex2TCoords(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2)
		: S3DVertex(x,y,z, nx,ny,nz, c, tu,tv), TCoords2(tu2,tv2) {}

	//! constructor with the same texture coords and normal
	S3DVertex2TCoords(f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, SColor c, f32 tu, f32 tv)
		: S3DVertex(x,y,z, nx,ny,nz, c, tu,tv), TCoords2(tu,tv) {}

	//! constructor with the same texture coords and normal
	S3DVertex2TCoords(const core::vector3df& pos, const core::vector3df& normal,
		SColor color, const core::vector2d<f32>& tcoords)
		: S3DVertex(pos, normal, color, tcoords), TCoords2(tcoords) {}

	//! constructor from S3DVertex
	S3DVertex2TCoords(S3DVertex& o) : S3DVertex(o) {}

	//! Second set of texture coordinates
	core::vector2d<f32> TCoords2;

	//! Equality operator
	bool operator==(const S3DVertex2TCoords& other) const
	{
		return ((static_cast<S3DVertex>(*this)==other) &&
			(TCoords2 == other.TCoords2));
	}

	//! Inequality operator
	bool operator!=(const S3DVertex2TCoords& other) const
	{
		return ((static_cast<S3DVertex>(*this)!=other) ||
			(TCoords2 != other.TCoords2));
	}

	bool operator<(const S3DVertex2TCoords& other) const
	{
		return ((static_cast<S3DVertex>(*this) < other) ||
				((static_cast<S3DVertex>(*this) == other) && (TCoords2 < other.TCoords2)));
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_2TCOORDS;
	}
};


//! Vertex with a tangent and binormal vector.
/** Usually used for tangent space normal mapping. */
struct S3DVertexTangents : public S3DVertex
{
	//! default constructor
	S3DVertexTangents() : S3DVertex() { }

	//! constructor
	S3DVertexTangents(f32 x, f32 y, f32 z, f32 nx=0.0f, f32 ny=0.0f, f32 nz=0.0f,
			SColor c = SColor(0xFF,0xFF,0xFF,0xFF), f32 tu=0.0f, f32 tv=0.0f,
			f32 tanx=0.0f, f32 tany=0.0f, f32 tanz=0.0f,
			f32 bx=0.0f, f32 by=0.0f, f32 bz=0.0f)
		: S3DVertex(x,y,z, nx,ny,nz, c, tu,tv), Tangent(tanx,tany,tanz), Binormal(bx,by,bz) { }

	//! constructor
	S3DVertexTangents(const core::vector3df& pos, SColor c,
		const core::vector2df& tcoords)
		: S3DVertex(pos, core::vector3df(), c, tcoords) { }

	//! constructor
	S3DVertexTangents(const core::vector3df& pos,
		const core::vector3df& normal, SColor c,
		const core::vector2df& tcoords,
		const core::vector3df& tangent=core::vector3df(),
		const core::vector3df& binormal=core::vector3df())
		: S3DVertex(pos, normal, c, tcoords), Tangent(tangent), Binormal(binormal) { }

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	bool operator==(const S3DVertexTangents& other) const
	{
		return ((static_cast<S3DVertex>(*this)==other) &&
			(Tangent == other.Tangent) &&
			(Binormal == other.Binormal));
	}

	bool operator!=(const S3DVertexTangents& other) const
	{
		return ((static_cast<S3DVertex>(*this)!=other) ||
			(Tangent != other.Tangent) ||
			(Binormal != other.Binormal));
	}

	bool operator<(const S3DVertexTangents& other) const
	{
		return ((static_cast<S3DVertex>(*this) < other) ||
				((static_cast<S3DVertex>(*this) == other) && (Tangent < other.Tangent)) ||
				((static_cast<S3DVertex>(*this) == other) && (Tangent == other.Tangent) && (Binormal < other.Binormal)));
	}

	E_VERTEX_TYPE getType() const
	{
		return EVT_TANGENTS;
	}
};

#ifdef _IRR_COMPILE_WITH_PS3_
//! standard vertex used by the PS3 VBOs
struct S3DVertexPS3
{
	//! default constructor
	S3DVertexPS3() {}

	//! Position
	core::vector3df Pos;

	//! Normal vector
	core::vector3df Normal;

	//! Color
	SColorf Color;

	//! Texture coordinates
	core::vector2d<f32> TCoords;
};
#endif

inline u32 getVertexPitchFromType(E_VERTEX_TYPE vertexType)
{
	switch (vertexType)
	{
	case video::EVT_2TCOORDS:
		return sizeof(video::S3DVertex2TCoords);
	case video::EVT_TANGENTS:
		return sizeof(video::S3DVertexTangents);
	default:
		return sizeof(video::S3DVertex);
	}
}

const u32 VertexStrides[] =
{
	sizeof(S3DVertex),
	sizeof(S3DVertex2TCoords),
	sizeof(S3DVertexTangents),
	0
};

} // end namespace video
} // end namespace irr

#endif

