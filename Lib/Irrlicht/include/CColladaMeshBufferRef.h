//-*-c++-*-
#ifndef __C_COLLADA_MESH_BUFFER_REF_H_INCLUDED__
#define __C_COLLADA_MESH_BUFFER_REF_H_INCLUDED__

#include <IMeshBuffer.h>
#include <resFile.h>
#include <SDatabaseCollada.h>
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

class CColladaMeshBufferRef
	: public IMeshBuffer
{
public:

	CColladaMeshBufferRef(video::SMaterial& material,
						  video::SS3DVertexComponentArrays& vertices,
						  u32 startIndex,
						  u32 endIndex,
						  u16* indices,
						  u32 indexCount,
						  const core::aabbox3df& boundingBox);

	void setReferences(video::SMaterial& material,
					   video::SS3DVertexComponentArrays& vertices,
					   u32 startIndex,
					   u32 endIndex,
					   u16* indices,
					   u32 indexCount,
					   const core::aabbox3df& boundingBox);

	virtual scene::E_PRIMITIVE_TYPE getPrimitiveType() const;

	//! Get the material of this meshbuffer
	/** \return Material of this buffer. */
	virtual video::SMaterial& getMaterial();

	//! Get the material of this meshbuffer
	/** \return Material of this buffer. */
	virtual const video::SMaterial& getMaterial() const;

	//! Get type of vertex data which is stored in this meshbuffer.
	/** \return Vertex type of this buffer. */
	virtual video::E_VERTEX_TYPE getVertexType() const;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
	\return Pointer to array of vertices. */
	virtual const void* getVertices() const;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
	\return Pointer to array of vertices. */
	virtual void* getVertices();

	//! Get amount of vertices in meshbuffer.
	/** \return Number of vertices in this buffer. */
	virtual u32 getVertexCount() const;

	//! Returns the first index actually used in the vertex buffer to render
	//! the mesh buffer.
	virtual u32 getVertexIndexStart() const;

	//! Returns the end of the index range (i.e. the last index plus one)
	//! actually used in the vertex buffer to render the mesh buffer.
	virtual u32 getVertexIndexEnd() const;

	//! Get type of index data which is stored in this meshbuffer.
	/** \return Index type of this buffer. */
	virtual video::E_INDEX_TYPE getIndexType() const;

	//! Get access to Indices.
	/** \return Pointer to indices array. */
	virtual const u16* getIndices() const;

	//! Get access to Indices.
	/** \return Pointer to indices array. */
	virtual u16* getIndices();

	//! Get amount of indices in this meshbuffer.
	/** \return Number of indices in this buffer. */
	virtual u32 getIndexCount() const;

	//! Get the axis aligned bounding box of this meshbuffer.
	/** \return Axis aligned bounding box of this buffer. */
	virtual const core::aabbox3df& getBoundingBox() const;

	//! Set axis aligned bounding box
	/** Not supported. */
	virtual void setBoundingBox(const core::aabbox3df& box);

	//! Recalculates the bounding box. Should be called if the mesh changed.
	/** Not supported. */
	virtual void recalculateBoundingBox();

	//! returns position of vertex i
	virtual core::vector3df& getPosition(u32 i);

	//! returns normal of vertex i
	virtual core::vector3df& getNormal(u32 i);

	//! returns texture coord of vertex i
	virtual core::vector2df& getTCoords(u32 i, u32 set = 0);

	//! returns position of vertex i
	virtual const core::vector3df& getPosition(u32 i) const;

	//! returns normal of vertex i
	virtual const core::vector3df& getNormal(u32 i) const;

	//! returns texture coord of vertex i
	virtual const core::vector2df& getTCoords(u32 i, u32 set = 0) const;

	//! Append the vertices and indices to the current buffer
	/** Not supported  */
	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices);

	//! Append the meshbuffer to the current buffer
	/** Not supported  */
	virtual void append(const IMeshBuffer* const other);

	//! get the current hardware mapping hint
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const;

	//! get the current hardware mapping hint
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const;

	//! set the hardware mapping hint, for driver
	virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);

	//! flags the mesh as changed, reloads hardware buffers
	virtual void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);

	//! Get the currently used ID for identification of changes.
	/** This shouldn't be used for anything outside the VideoDriver. */
	virtual u32 getChangedID_Vertex() const;

	//! Get the currently used ID for identification of changes.
	/** This shouldn't be used for anything outside the VideoDriver. */
	virtual u32 getChangedID_Index() const;

protected:

	video::SMaterial* Material;
	video::SS3DVertexComponentArrays Vertices;
	u32 StartIndex;
	u32 EndIndex;
	u16* Indices;
	u32 IndexCount;
	const core::aabbox3df* BoundingBox;

	u32 ChangedID_Vertex;
	u32 ChangedID_Index;

	//! hardware mapping hint
	E_HARDWARE_MAPPING MappingHint_Vertex;
	E_HARDWARE_MAPPING MappingHint_Index;

}; // class CColladaMeshBufferRef

} // namespace scene
} // namespace irr

#endif // __C_COLLADA_MESH_BUFFER_REF_H_INCLUDED__
