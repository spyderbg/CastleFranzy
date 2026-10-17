#include "stdafx.h"
#include "CColladaMeshBuffer.h"
#include <SDatabaseCollada.h>
#include <resFile.h>
#include "coreutil.h"

namespace irr
{
namespace scene
{

CColladaMeshBuffer::CColladaMeshBuffer(const collada::SMeshBuffer& meshBuffer,
									   const collada::SMesh& vSources)
	: Buffer(meshBuffer)
	, Mesh(vSources)
	, ChangedID_Vertex(1)
	, ChangedID_Index(1)
	, MappingHint_Vertex(EHM_NEVER)
	, MappingHint_Index(EHM_NEVER)
{
	if(Buffer.positionSource != -1)
	{
		Vertices.PositionType = Mesh.getSourceType(Buffer.positionSource);
		Vertices.Position = (core::vector3df*)Mesh.getSourceBuffer(Buffer.positionSource);
		switch (Vertices.PositionType)
		{
			case video::S3DVertexComponentArrays::ECT_SHORT:
				Vertices.setPositionScale(Mesh.getSourceScale(Buffer.positionSource));
				Vertices.setPositionOffset(Mesh.getSourceOffset(Buffer.positionSource));
				break;
			default:
				break;
		}
	}
	if(Buffer.normalSource != -1)
	{
		Vertices.NormalType = Mesh.getSourceType(Buffer.normalSource);
		Vertices.Normal = (core::vector3df*)Mesh.getSourceBuffer(Buffer.normalSource);
	}
	if(Buffer.color0Source != -1)
	{
		Vertices.Color0Type = Mesh.getSourceType(Buffer.color0Source);
		Vertices.Color0 = (video::SColor*)Mesh.getSourceBuffer(Buffer.color0Source);
	}
	if(Buffer.texcoord0Source != -1)
	{
		Vertices.TexCoord[0].Type = Mesh.getSourceType(Buffer.texcoord0Source);
		Vertices.TexCoord[0].Coord = (core::vector2df*)Mesh.getSourceBuffer(Buffer.texcoord0Source);
		switch(Vertices.TexCoord[0].Type)
		{
		case video::S3DVertexComponentArrays::ECT_BYTE:
		case video::S3DVertexComponentArrays::ECT_SHORT:
			Vertices.setTexCoordScale(0, Mesh.getSourceScale(Buffer.texcoord0Source));
			Vertices.setTexCoordOffset(0, Mesh.getSourceOffset(Buffer.texcoord0Source));
			break;
		default:
			break;
		}
	}
	if(Buffer.texcoord1Source != -1)
	{
		Vertices.TexCoord[1].Type = Mesh.getSourceType(Buffer.texcoord1Source);
		Vertices.TexCoord[1].Coord = (core::vector2df*)Mesh.getSourceBuffer(Buffer.texcoord1Source);
	}

	if(Mesh.bInterleaved)
	{
		Vertices.PositionStride = Mesh.pInterleaved->stride;
		Vertices.NormalStride = Mesh.pInterleaved->stride;
		Vertices.Color0Stride = Mesh.pInterleaved->stride;
		Vertices.TexCoord[0].Stride = Mesh.pInterleaved->stride;
		Vertices.TexCoord[1].Stride = Mesh.pInterleaved->stride;
	}
}

video::SMaterial& CColladaMeshBuffer::getMaterial()
{
	return Material;
}

const video::SMaterial& CColladaMeshBuffer::getMaterial() const
{
	return Material;
}

video::E_VERTEX_TYPE CColladaMeshBuffer::getVertexType() const
{
	return video::EVT_COMPONENT_ARRAYS;	
}

const void* CColladaMeshBuffer::getVertices() const
{
	return &Vertices;	
}

void* CColladaMeshBuffer::getVertices()
{
	return &Vertices;	
}

u32 CColladaMeshBuffer::getVertexCount() const
{
	/** Not implemented  */
	return (Mesh.bInterleaved ? Mesh.getVertexCount() : Mesh.pSources[Buffer.positionSource].fData.size() / 3);	
}

u32 CColladaMeshBuffer::getVertexIndexStart() const
{
	return Buffer.minIndex;
}

u32 CColladaMeshBuffer::getVertexIndexEnd() const
{
	return Buffer.maxIndex + 1;
}

video::E_INDEX_TYPE CColladaMeshBuffer::getIndexType() const
{
	/** Not implemented  */
	return video::EIT_16BIT;	
}

const u16* CColladaMeshBuffer::getIndices() const
{
	/** Not implemented  */
	return (const u16*)Buffer.indices.ptr();		
}

u16* CColladaMeshBuffer::getIndices()
{
	/** Not implemented  */
	return (u16*)Buffer.indices.ptr();	
}	

u32 CColladaMeshBuffer::getIndexCount() const
{
	/** Not implemented  */
	return Buffer.indices.size();		
}
	
const core::aabbox3df& CColladaMeshBuffer::getBoundingBox() const
{
	return Buffer.boundingBox;
}

void CColladaMeshBuffer::setBoundingBox(const core::aabbox3df& box)
{
	/** Not supported  */
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

void CColladaMeshBuffer::recalculateBoundingBox()
{
	/** Not supported  */
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

const core::vector3df& CColladaMeshBuffer::getPosition(u32 i) const
{
	video::S3DVertexComponentArrays::SAccessorVector3df accessor;
	if(Vertices.getPositionAccessor(accessor) == 0)
	{
		return accessor[i];
	}
	else
	{
		return *(core::vector3df*)(0);
	}
}

const core::vector3df& CColladaMeshBuffer::getNormal(u32 i) const
{
	return *stepPointer(Vertices.Normal, i * Vertices.NormalStride);
}

const core::vector2df& CColladaMeshBuffer::getTCoords(u32 i, u32 set) const 
{
	_IRR_DEBUG_BREAK_IF(i >= video::S3DVertexComponentArrays::MAX_TEXTURES);
	return *stepPointer(Vertices.TexCoord[0].Coord, i * Vertices.TexCoord[0].Stride);
}

const video::SColor CColladaMeshBuffer::getColor(u32 i) 
{
	video::S3DVertexComponentArrays::SAccessorColor accessor;
	if(Vertices.getColor0Accessor(accessor) == 0)
	{
		return accessor[i];
	}
	else
	{
		return video::SColor(255, 255, 255, 255);
	}
}

core::vector3df& CColladaMeshBuffer::getPosition(u32 i)
{
	return *stepPointer(Vertices.Position, i * Vertices.PositionStride);
}

core::vector3df& CColladaMeshBuffer::getNormal(u32 i)
{
	return *stepPointer(Vertices.Normal, i * Vertices.NormalStride);
}

core::vector2df& CColladaMeshBuffer::getTCoords(u32 i, u32 set)
{
	_IRR_DEBUG_BREAK_IF(i >= video::S3DVertexComponentArrays::MAX_TEXTURES);
	return *stepPointer(Vertices.TexCoord[0].Coord, i * Vertices.TexCoord[0].Stride);
}
	
void CColladaMeshBuffer::append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices)
{
	_IRR_DEBUG_BREAK_IF("Not supported");	
}

void CColladaMeshBuffer::append(const IMeshBuffer* const other)
{
	_IRR_DEBUG_BREAK_IF("Not supported");
}

E_HARDWARE_MAPPING CColladaMeshBuffer::getHardwareMappingHint_Vertex() const
{
	return MappingHint_Vertex;
}

E_HARDWARE_MAPPING CColladaMeshBuffer::getHardwareMappingHint_Index() const
{
	return MappingHint_Index;
}

void CColladaMeshBuffer::setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer)
{
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_VERTEX)
		MappingHint_Vertex = newMappingHint;
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_INDEX)
		MappingHint_Index = newMappingHint;
}

void CColladaMeshBuffer::setDirty(E_BUFFER_TYPE buffer)
{
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_VERTEX)
		++ChangedID_Vertex;
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_INDEX)
		++ChangedID_Index;
}

u32 CColladaMeshBuffer::getChangedID_Vertex() const
{
	return ChangedID_Vertex;
}

u32 CColladaMeshBuffer::getChangedID_Index() const
{
	return ChangedID_Index;
}

}
}
