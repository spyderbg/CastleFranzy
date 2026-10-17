#include "CIrrLichtBatch.h"
#include "S3DVertexComponentArrays.h"

namespace irr
{
using namespace video;

CBatch::CBatch(const SInitData &initData)
: Material(initData.Material)
, WorldTransform(initData.WorldTransform)
, ProjectionTransform(initData.ProjectionTransform)
, ViewTransform(initData.ViewTransform)
, PrimitiveCount(initData.PrimitiveCount)
, StartIndex(initData.StartIndex)
, EndIndex(initData.EndIndex)
, vType(initData.vType)
, pType(initData.pType)
, iType(initData.iType)
, fVBOFlag(initData.fVBOFlag)
, Show(true)
, NearRange(initData.NearRange)
, FarRange(initData.FarRange)
{
	cloneData(initData.Vertices, initData.IndexList, initData.StartIndex, 
		initData.EndIndex, initData.PrimitiveCount, initData.vType, initData.pType, 
		initData.iType, initData.fVBOFlag);
}

CBatch::~CBatch()
{
	switch(Vertices.PositionType)
	{
	case S3DVertexComponentArrays::ECT_FLOAT:
		if(Vertices.Position) delete[] Vertices.Position;
		Vertices.Position = 0;
		break;
	case S3DVertexComponentArrays::ECT_SHORT:
		if(Vertices.PositionS16) delete[] Vertices.PositionS16;
		Vertices.PositionS16 = 0;
		break;
	}

	switch(Vertices.NormalType)
	{
	case S3DVertexComponentArrays::ECT_FLOAT:
		if(Vertices.Normal) delete[] Vertices.Normal;
		Vertices.Normal = 0;
		break;
	case S3DVertexComponentArrays::ECT_SHORT:
		if(Vertices.NormalS16) delete[] Vertices.NormalS16;
		Vertices.NormalS16 = 0;
		break;
	case S3DVertexComponentArrays::ECT_BYTE:
		if(Vertices.NormalS8) delete[] Vertices.NormalS8;
		Vertices.NormalS8 = 0;
		break;
	}

	for(int i = 0; i < 2; i++)
	{
		switch(Vertices.TexCoord[i].Type)
		{
		case S3DVertexComponentArrays::ECT_FLOAT:
			if(Vertices.TexCoord[i].Coord) delete[] Vertices.TexCoord[i].Coord;
			Vertices.TexCoord[i].Coord = 0;
			break;
		case S3DVertexComponentArrays::ECT_SHORT:
			if(Vertices.TexCoord[i].CoordS16) delete[]Vertices.TexCoord[i].CoordS16;
			Vertices.TexCoord[i].CoordS16 = 0;
			break;
		}
	}

	if(Vertices.Color0) delete[] Vertices.Color0;

	delete[] IndexListU16;
}

void
CBatch::cloneData(const void* vertices,
				 const void* indexList,
				 u32 startIndex,
				 u32 endIndex,
				 u32 primitiveCount,
				 E_VERTEX_TYPE vType,
				 scene::E_PRIMITIVE_TYPE pType,
				 E_INDEX_TYPE iType,
				 bool fVBOFlag)
{
	IndexListU16 = new unsigned short[primitiveCount * 3];
	memcpy(IndexListU16, indexList, primitiveCount * 3 * sizeof(unsigned short));

	_IRR_DEBUG_BREAK_IF(vType != EVT_COMPONENT_ARRAYS);
	_IRR_DEBUG_BREAK_IF(iType != EIT_16BIT);
	_IRR_DEBUG_BREAK_IF(fVBOFlag);
		
	const S3DVertexComponentArrays *inData = (const S3DVertexComponentArrays *)vertices;
	S3DVertexComponentArrays outData;
	{
		S3DVertexComponentArrays::SAccessorVector3df position;
		outData.PositionType = inData->PositionType;
		if(inData->getPositionAccessor(position) != -1)
		{
			outData.PositionStride = sizeof(*outData.Position);
			outData.Position = new core::vector3d<f32>[endIndex];
			for(int i = 0; i < endIndex; i++)
			{
				outData.Position[i] = position[i];
			}
		}
		else
		{
			S3DVertexComponentArrays::SAccessorVector3ds position;
			outData.PositionStride = sizeof(*outData.PositionS16);
			if(inData->getPositionAccessor(position) != -1)
			{
				outData.PositionS16 = new core::vector3d<s16>[endIndex];
				for(int i = 0; i < endIndex; i++)
				{
					outData.PositionS16[i] = position[i];
				}
				outData.setPositionOffset(*inData->getPositionOffset());
				outData.setPositionScale(*inData->getPositionScale());
			}
		}

		S3DVertexComponentArrays::SAccessorVector3df normal;
		outData.NormalType = inData->NormalType;
		if(inData->getNormalAccessor(normal) != -1)
		{
			outData.NormalStride = sizeof(*outData.Normal);
			outData.Normal = new core::vector3d<f32>[endIndex];
			for(int i = 0; i < endIndex; i++)
			{
				outData.Normal[i] = normal[i];
			}
		}
		else
		{
			S3DVertexComponentArrays::SAccessorVector3ds normal;
			if(inData->getNormalAccessor(normal) != -1)
			{
				outData.NormalStride = sizeof(*outData.NormalS16);
				outData.NormalS16 = new core::vector3d<s16>[endIndex];
				for(int i = 0; i < endIndex; i++)
				{
					outData.NormalS16[i] = normal[i];
				}
			}
			else
			{
				outData.NormalStride = sizeof(*outData.NormalS8);
				S3DVertexComponentArrays::SAccessorVector3db normal;
				if(inData->getNormalAccessor(normal) != -1)
				{
					outData.NormalS8 = new core::vector3d<char>[endIndex];
					for(int i = 0; i < endIndex; i++)
					{
						outData.NormalS8[i] = normal[i];
					}
				}
			}
		}

		for(int j = 0; j < 2; j++)
		{
			S3DVertexComponentArrays::SAccessorVector2df texCoord;
			outData.TexCoord[j].Type = inData->TexCoord[j].Type;
			if(inData->getTexCoordAccessor(j, texCoord) != -1)
			{
				outData.TexCoord[j].Stride = sizeof(*outData.TexCoord[j].Coord);
				outData.TexCoord[j].Coord = new core::vector2d<f32>[endIndex];
				for(int i = 0; i < endIndex; i++)
				{
					outData.TexCoord[j].Coord[i] = texCoord[i];
				}
			}
			else
			{
				S3DVertexComponentArrays::SAccessorVector2ds texCoord;
				
				if(inData->getTexCoordAccessor(j, texCoord) != -1)
				{
					outData.TexCoord[j].Stride = sizeof(*outData.TexCoord[j].CoordS16);
					outData.TexCoord[j].CoordS16 = new core::vector2d<s16>[endIndex];
					for(int i = 0; i < endIndex; i++)
					{
						outData.TexCoord[j].CoordS16[i] = texCoord[i];
					}
					outData.TexCoord[j].Offset = inData->TexCoord[j].Offset;
					outData.TexCoord[j].Scale = inData->TexCoord[j].Scale;
				}
			}
		}

		S3DVertexComponentArrays::SAccessorColor color;
		outData.Color0Type = inData->Color0Type;
		if(inData->getColor0Accessor(color) != -1)
		{
			outData.Color0Stride = sizeof(*outData.Color0);
			outData.Color0 = new video::SColor[endIndex];
			for(int i = 0; i < endIndex; i++)
			{
				outData.Color0[i] = color[i];
			}
		}
	}

	Vertices = outData;
}

}; // namespace irr
