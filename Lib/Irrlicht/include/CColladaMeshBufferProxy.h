//-*-c++-*-
#ifndef __CCOLLADAMESHBUFFERPROXY_H__
#define __CCOLLADAMESHBUFFERPROXY_H__

#include "IMeshBuffer.h"

namespace irr
{
namespace collada
{

class CMeshBufferProxy
	: public scene::IMeshBuffer
{
protected:

	IMeshBuffer* IndicesRef;
	video::S3DVertexComponentArrays VerticesRef;
	const core::aabbox3df* BBox;

public:

	//! Default constructor for empty meshbuffer
	CMeshBufferProxy()
		: IndicesRef(0)
	{
		#ifdef _DEBUG
		setDebugName("CMeshBufferProxy");
		#endif
	}

	virtual scene::E_PRIMITIVE_TYPE getPrimitiveType() const
	{
		return scene::EPT_TRIANGLES;
	}

	void setReferences(IMeshBuffer* indices,
					   const video::S3DVertexComponentArrays* vertices = NULL,
					   const core::aabbox3df* bbox = NULL) 
	{
		IndicesRef = indices;
		if (vertices)
		{
			VerticesRef = *vertices;
		}
		else
		{
			_IRR_DEBUG_BREAK_IF(
				indices->getVertexType() != video::EVT_COMPONENT_ARRAYS
			);
			VerticesRef = *reinterpret_cast<video::S3DVertexComponentArrays*>(
				indices->getVertices()
			);
		}
		if (bbox)
		{
			BBox = bbox;
		}
		else
		{
			BBox = &IndicesRef->getBoundingBox();
		}
	}

	virtual const video::SMaterial& getMaterial() const;
	virtual video::SMaterial& getMaterial();

	virtual video::E_VERTEX_TYPE getVertexType() const;
	virtual const void* getVertices() const;
	virtual void* getVertices();
	virtual u32 getVertexCount() const;
	virtual u32 getVertexIndexStart() const;
	virtual u32 getVertexIndexEnd() const;

	virtual video::E_INDEX_TYPE getIndexType() const;
	virtual const u16* getIndices() const;
	virtual u16* getIndices();
	virtual u32 getIndexCount() const;

	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual void setBoundingBox(const core::aabbox3df& box);
	virtual void recalculateBoundingBox();

	virtual const core::vector3df& getPosition(u32 i) const;
	virtual core::vector3df& getPosition(u32 i);

	virtual const core::vector3df& getNormal(u32 i) const;
	virtual core::vector3df& getNormal(u32 i);

	virtual const core::vector2df& getTCoords(u32 i, u32 set = 0) const;
	virtual core::vector2df& getTCoords(u32 i, u32 set = 0);

	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices);
	virtual void append(const IMeshBuffer* const other);

	virtual scene::E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const;
	virtual scene::E_HARDWARE_MAPPING getHardwareMappingHint_Index() const;
	virtual void setHardwareMappingHint(scene::E_HARDWARE_MAPPING newMappingHint,
										scene::E_BUFFER_TYPE buffer = scene::EBT_VERTEX_AND_INDEX);

	virtual void setDirty(scene::E_BUFFER_TYPE buffer = scene::EBT_VERTEX_AND_INDEX);

	virtual u32 getChangedID_Vertex() const;
	virtual u32 getChangedID_Index() const;
};

} // namespace collada
} // namespace irr

#endif // __CCOLLADAMESHBUFFERPROXY_H__
