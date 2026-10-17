#include "stdafx.h"

#include "S3DVertexComponentArrays.h"

#include "S3DVertex.h"

namespace irr
{
namespace video
{

S3DVertexComponentArrays&
S3DVertexComponentArrays::operator = (S3DVertex* vertices)
{
	PositionType = ECT_FLOAT;
	Position = &vertices->Pos;
	PositionStride = sizeof(S3DVertex);

	NormalType = ECT_FLOAT;
	Normal = &vertices->Normal;
	NormalStride = sizeof(S3DVertex);

	TexCoord[0].Type = ECT_FLOAT;
	TexCoord[0].Coord = &vertices->TCoords;
	TexCoord[0].Stride = sizeof(S3DVertex);

	TexCoord[1].Type = ECT_FLOAT; 
	TexCoord[1].Coord = &vertices->TCoords;
	TexCoord[1].Stride = sizeof(S3DVertex);
	//IMPORTANT: all texcoord should be set as the first default texture coordinate, nerver set as NULL.
	//This is convenient using STANDARD vertex make simple multi texture, without any performance pain.

	Color0Type = ECT_UNSIGNED_BYTE;
	Color0 = &vertices->Color;
	Color0Stride = sizeof(S3DVertex);

	Color1Type = ECT_UNSIGNED_BYTE;
	Color1 = NULL;
	Color1Stride = sizeof(S3DVertex);

	return *this;
}

S3DVertexComponentArrays&
S3DVertexComponentArrays::operator = (S3DVertex2TCoords* vertices)
{
	PositionType = ECT_FLOAT;
	Position = &vertices->Pos;
	PositionStride = sizeof(S3DVertex2TCoords);

	NormalType = ECT_FLOAT;
	Normal = &vertices->Normal;
	NormalStride = sizeof(S3DVertex2TCoords);

	TexCoord[0].Type = ECT_FLOAT;
	TexCoord[0].Coord = &vertices->TCoords;
	TexCoord[0].Stride = sizeof(S3DVertex2TCoords);

	TexCoord[1].Type = ECT_FLOAT;
	TexCoord[1].Coord = &vertices->TCoords2;
	TexCoord[1].Stride = sizeof(S3DVertex2TCoords);

	Color0Type = ECT_UNSIGNED_BYTE;
	Color0 = &vertices->Color;
	Color0Stride = sizeof(S3DVertex2TCoords);

	Color1Type = ECT_UNSIGNED_BYTE;
	Color1 = NULL;
	Color1Stride = sizeof(S3DVertex2TCoords);

	return *this;
}

S3DVertexComponentArrays& S3DVertexComponentArrays::operator = (S3DVertexTangents* vertices)
{
	// if we want to support this, we need to increment MAX_TEXTURES to at least 3
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	
	PositionType = ECT_FLOAT;
	Position = &vertices->Pos;
	PositionStride = sizeof(S3DVertexTangents);

	NormalType = ECT_FLOAT;
	Normal = &vertices->Normal;
	NormalStride = sizeof(S3DVertexTangents);

	TexCoord[0].Type = ECT_FLOAT;
	TexCoord[0].Coord = &vertices->TCoords;
	TexCoord[0].Stride = sizeof(S3DVertexTangents);

	TexCoord[1].Type = ECT_FLOAT;
	TexCoord[1].Coord = NULL;
	TexCoord[1].Stride = sizeof(S3DVertexTangents);

	Color0Type = ECT_UNSIGNED_BYTE;
	Color0 = &vertices->Color;
	Color0Stride = sizeof(S3DVertexTangents);

	Color1Type = ECT_UNSIGNED_BYTE;
	Color1 = NULL;
	Color1Stride = sizeof(S3DVertexTangents);

	return *this;
}

S3DVertexComponentArrays&
S3DVertexComponentArrays::assign(void* vertices,
								 E_VERTEX_TYPE type)
{
	switch (type)
	{
		case EVT_STANDARD :
		{
			*this = reinterpret_cast<S3DVertex*>(vertices);
		}
		break;

		case EVT_2TCOORDS :
		{
			*this = reinterpret_cast<S3DVertex2TCoords*>(vertices);
		}
		break;

		case EVT_TANGENTS :
		{
			*this = reinterpret_cast<S3DVertexTangents*>(vertices);
		}
		break;

		case EVT_COMPONENT_ARRAYS :
		{
			*this = *reinterpret_cast<S3DVertexComponentArrays*>(vertices);
		}
		break;

		default :
		{
			_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
		}
		break;
	}
	return *this;
}

int
S3DVertexComponentArrays::getPositionAccessor(SAccessorVector3df& acc) const
{
	if (Position == 0)
		return -1;

	if (PositionType == ECT_FLOAT)
	{
		acc = SAccessorVector3df(PositionType, PositionStride, Position);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getPositionAccessor(SAccessorVector3ds& acc) const
{
	if (Position && PositionType == ECT_SHORT)
	{
		acc = SAccessorVector3ds(PositionType, PositionStride, Position, PositionScale, PositionOffset);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getNormalAccessor(SAccessorVector3df& acc) const
{
	if (Normal && NormalType == ECT_FLOAT)
	{
		acc = SAccessorVector3df(NormalType, NormalStride, Normal);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getNormalAccessor(SAccessorVector3ds& acc) const
{
	if (Normal && NormalType == ECT_SHORT)
	{
		acc = SAccessorVector3ds(NormalType, NormalStride, Normal);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getNormalAccessor(SAccessorVector3db& acc) const
{
	if (Normal && NormalType == ECT_BYTE)
	{
		acc = SAccessorVector3db(NormalType, NormalStride, Normal);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getTexCoordAccessor(u32 unit, SAccessorVector2df& acc) const
{
	_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
	if (TexCoord[unit].Coord && TexCoord[unit].Type == ECT_FLOAT)
	{
		acc = SAccessorVector2df(TexCoord[unit].Type,
								 TexCoord[unit].Stride,
								 TexCoord[unit].Coord);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getTexCoordAccessor(u32 unit, SAccessorVector2ds& acc) const
{
	_IRR_DEBUG_BREAK_IF(unit >= MAX_TEXTURES);
	if (TexCoord[unit].Coord && TexCoord[unit].Type == ECT_SHORT)
	{
		acc = SAccessorVector2ds(TexCoord[unit].Type,
								 TexCoord[unit].Stride,
								 TexCoord[unit].Coord);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getColor0Accessor(SAccessorColor &acc) const
{
	if (Color0 && (Color0Type == ECT_UNSIGNED_BYTE || Color0Type == ECT_BYTE))
	{
		acc = SAccessorColor(Color0Type, Color0Stride, Color0);
		return 0; // Succeed
	}
	return -1; // Failed
}

int
S3DVertexComponentArrays::getColor1Accessor(SAccessorColor &acc) const
{
	if (Color1 && Color1Type == ECT_BYTE)
	{
		acc = SAccessorColor(Color1Type, Color1Stride, Color1);
		return 0; // Succeed
	}
	return -1; // Failed
}

} // namespace video
} // namespace irr
