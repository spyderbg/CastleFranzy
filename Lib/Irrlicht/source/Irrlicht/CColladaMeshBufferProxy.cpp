#include "stdafx.h"
#include "CColladaMeshBufferProxy.h"
#include "coreutil.h"

namespace irr
{
namespace collada
{

//******************************************************************************
// Class CMeshBufferProxy

const video::SMaterial& CMeshBufferProxy::getMaterial() const
{
	return IndicesRef->getMaterial();
}

video::SMaterial& CMeshBufferProxy::getMaterial()
{
	return IndicesRef->getMaterial();
}

const void* CMeshBufferProxy::getVertices() const
{
	return &VerticesRef;
}

void* CMeshBufferProxy::getVertices()
{
	return &VerticesRef;
}

u32 CMeshBufferProxy::getVertexCount() const
{
	return IndicesRef->getVertexCount();
}

u32 CMeshBufferProxy::getVertexIndexStart() const
{
	return IndicesRef->getVertexIndexStart();
}

u32 CMeshBufferProxy::getVertexIndexEnd() const
{
	return IndicesRef->getVertexIndexEnd();
}

video::E_INDEX_TYPE CMeshBufferProxy::getIndexType() const
{
	return IndicesRef->getIndexType();
}

const u16* CMeshBufferProxy::getIndices() const
{
	return IndicesRef->getIndices();
}

u16* CMeshBufferProxy::getIndices()
{
	return IndicesRef->getIndices();
}

u32 CMeshBufferProxy::getIndexCount() const
{
	return IndicesRef->getIndexCount();
}

const core::aabbox3d<f32>& CMeshBufferProxy::getBoundingBox() const
{
	return *BBox;
}

void CMeshBufferProxy::setBoundingBox(const core::aabbox3df& box)
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

void CMeshBufferProxy::recalculateBoundingBox()
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

video::E_VERTEX_TYPE CMeshBufferProxy::getVertexType() const
{
	return video::EVT_COMPONENT_ARRAYS;
}

const core::vector3df& CMeshBufferProxy::getPosition(u32 i) const
{
	return *stepPointer(VerticesRef.Position, i * VerticesRef.PositionStride);
}

core::vector3df& CMeshBufferProxy::getPosition(u32 i)
{
	return *stepPointer(VerticesRef.Position, i * VerticesRef.PositionStride);
}

const core::vector3df& CMeshBufferProxy::getNormal(u32 i) const
{
	return *stepPointer(VerticesRef.Normal, i * VerticesRef.NormalStride);
}

core::vector3df& CMeshBufferProxy::getNormal(u32 i)
{
	return *stepPointer(VerticesRef.Normal, i * VerticesRef.NormalStride);
}

const core::vector2df& CMeshBufferProxy::getTCoords(u32 i, u32 set) const
{
	_IRR_DEBUG_BREAK_IF(set != 0);
	return *core::stepPointer(VerticesRef.TexCoord[set].Coord, i * VerticesRef.TexCoord[set].Stride);
}

core::vector2df& CMeshBufferProxy::getTCoords(u32 i, u32 set)
{
	_IRR_DEBUG_BREAK_IF(set != 0);
	return *core::stepPointer(VerticesRef.TexCoord[set].Coord, i * VerticesRef.TexCoord[set].Stride);
}

void CMeshBufferProxy::append(const void* const vertices,
							  u32 numVertices,
							  const u16* const indices,
							  u32 numIndices)
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

void CMeshBufferProxy::append(const IMeshBuffer* const other)
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

scene::E_HARDWARE_MAPPING CMeshBufferProxy::getHardwareMappingHint_Vertex() const
{
	return IndicesRef->getHardwareMappingHint_Vertex();
}

scene::E_HARDWARE_MAPPING CMeshBufferProxy::getHardwareMappingHint_Index() const
{
	return IndicesRef->getHardwareMappingHint_Index();
}

void CMeshBufferProxy::setHardwareMappingHint(scene::E_HARDWARE_MAPPING newMappingHint,
											  scene::E_BUFFER_TYPE buffer)
{
	IndicesRef->setHardwareMappingHint(newMappingHint, buffer);
}

void CMeshBufferProxy::setDirty(scene::E_BUFFER_TYPE buffer)
{
	IndicesRef->setDirty(buffer);
}

u32 CMeshBufferProxy::getChangedID_Vertex() const 
{
	return IndicesRef->getChangedID_Vertex();
}

u32 CMeshBufferProxy::getChangedID_Index() const 
{
	return IndicesRef->getChangedID_Index();
}

} // namespace collada
} // namespace irr
