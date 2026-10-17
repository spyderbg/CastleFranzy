#include "stdafx.h"
#include "CColladaMeshBufferRef.h"
#include "coreutil.h"

namespace irr
{
namespace scene
{

CColladaMeshBufferRef::CColladaMeshBufferRef(
	video::SMaterial& material,
	video::SS3DVertexComponentArrays& vertices,
	u32 startIndex,
	u32 endIndex,
	u16* indices,
	u32 indexCount,
	const core::aabbox3df& boundingBox
)
	: Material(&material)
	, Vertices(vertices)
	, StartIndex(startIndex)
	, EndIndex(endIndex)
	, Indices(indices)
	, IndexCount(indexCount)
	, BoundingBox(&boundingBox)
	, ChangedID_Vertex(1)
	, ChangedID_Index(1)
	, MappingHint_Vertex(EHM_NEVER)
	, MappingHint_Index(EHM_NEVER)
{
}

void CColladaMeshBufferRef::setReferences(
	video::SMaterial& material,
	video::SS3DVertexComponentArrays& vertices,
	u32 startIndex,
	u32 endIndex,
	u16* indices,
	u32 indexCount,
	const core::aabbox3df& boundingBox
)
{
	Material = &material;
	Vertices = vertices;
	StartIndex = startIndex;
	EndIndex = endIndex;
	Indices = indices;
	IndexCount = indexCount;
	BoundingBox = &boundingBox;

	++ChangedID_Vertex;
	++ChangedID_Index;
}

scene::E_PRIMITIVE_TYPE
CColladaMeshBufferRef::getPrimitiveType() const
{
	return EPT_TRIANGLES;
}

video::SMaterial&
CColladaMeshBufferRef::getMaterial()
{
	return *Material;
}

const video::SMaterial&
CColladaMeshBufferRef::getMaterial() const
{
	return *Material;
}

video::E_VERTEX_TYPE
CColladaMeshBufferRef::getVertexType() const
{
	return video::EVT_COMPONENT_ARRAYS;	
}

const void*
CColladaMeshBufferRef::getVertices() const
{
	return &Vertices;	
}

void*
CColladaMeshBufferRef::getVertices()
{
	return &Vertices;	
}

u32
CColladaMeshBufferRef::getVertexCount() const
{
	return EndIndex - StartIndex;
}

u32
CColladaMeshBufferRef::getVertexIndexStart() const
{
	return StartIndex;
}

u32
CColladaMeshBufferRef::getVertexIndexEnd() const
{
	return EndIndex;
}

video::E_INDEX_TYPE
CColladaMeshBufferRef::getIndexType() const
{
	return video::EIT_16BIT;	
}

const u16*
CColladaMeshBufferRef::getIndices() const
{
	return Indices;
}

u16*
CColladaMeshBufferRef::getIndices()
{
	return Indices;
}	

u32
CColladaMeshBufferRef::getIndexCount() const
{
	return IndexCount;
}
	
const core::aabbox3df&
CColladaMeshBufferRef::getBoundingBox() const
{
	return *BoundingBox;
}

void
CColladaMeshBufferRef::setBoundingBox(const core::aabbox3df& box)
{
	/** Not supported  */
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

void
CColladaMeshBufferRef::recalculateBoundingBox()
{
	/** Not supported  */
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

const core::vector3df&
CColladaMeshBufferRef::getPosition(u32 i) const
{
	return *stepPointer(Vertices.Position, i * Vertices.PositionStride);
}

const core::vector3df&
CColladaMeshBufferRef::getNormal(u32 i) const
{
	return *stepPointer(Vertices.Normal, i * Vertices.NormalStride);
}

const core::vector2df&
CColladaMeshBufferRef::getTCoords(u32 i, u32 set) const 
{
	_IRR_DEBUG_BREAK_IF(set != 0);
	return *core::stepPointer(Vertices.TexCoord[set].Coord,
							  i * Vertices.TexCoord[set].Stride);
}

core::vector3df&
CColladaMeshBufferRef::getPosition(u32 i)
{
	return *core::stepPointer(Vertices.Position, i * Vertices.PositionStride);
}

core::vector3df&
CColladaMeshBufferRef::getNormal(u32 i)
{
	return *core::stepPointer(Vertices.Normal, i * Vertices.NormalStride);
}

core::vector2df&
CColladaMeshBufferRef::getTCoords(u32 i, u32 set)
{
	_IRR_DEBUG_BREAK_IF(set != 0);
	return *core::stepPointer(Vertices.TexCoord[set].Coord,
							  i * Vertices.TexCoord[set].Stride);
}
	
void
CColladaMeshBufferRef::append(const void* const vertices,
							  u32 numVertices,
							  const u16* const indices,
							  u32 numIndices)
{
	_IRR_DEBUG_BREAK_IF("Not supported");	
}

void
CColladaMeshBufferRef::append(const IMeshBuffer* const other)
{
	_IRR_DEBUG_BREAK_IF("Not supported");
}

E_HARDWARE_MAPPING
CColladaMeshBufferRef::getHardwareMappingHint_Vertex() const
{
	return MappingHint_Vertex;
}

E_HARDWARE_MAPPING
CColladaMeshBufferRef::getHardwareMappingHint_Index() const
{
	return MappingHint_Index;
}

void
CColladaMeshBufferRef::setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint,
											  E_BUFFER_TYPE buffer)
{
	if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_VERTEX)
		MappingHint_Vertex = newMappingHint;
	if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_INDEX)
		MappingHint_Index = newMappingHint;
}

void
CColladaMeshBufferRef::setDirty(E_BUFFER_TYPE buffer)
{
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_VERTEX)
		++ChangedID_Vertex;
	if (buffer == EBT_VERTEX_AND_INDEX || buffer == EBT_INDEX)
		++ChangedID_Index;
}

u32
CColladaMeshBufferRef::getChangedID_Vertex() const
{
	return ChangedID_Vertex;
}

u32
CColladaMeshBufferRef::getChangedID_Index() const
{
	return ChangedID_Index;
}

} // end namespace scene
} // end namespace irr
