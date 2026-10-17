//-*-c++-*-
#ifndef __C_SCENE_BATCH_BUFFER_H_INCLUDED__
#define __C_SCENE_BATCH_BUFFER_H_INCLUDED__

#include "IMaterialRenderer.h"
#include "IVideoDriver.h"
#include "IMeshBuffer.h"
#include "irrMath.h"

namespace irr
{
namespace scene
{

class CBatchBuffer
	: public scene::IMeshBuffer
{
public:

	explicit CBatchBuffer(video::IVideoDriver* driver);
	explicit CBatchBuffer(const CBatchBuffer* buf);
	virtual ~CBatchBuffer();

	// IMeshBuffer interface ---------------------------------------------------

	virtual E_PRIMITIVE_TYPE getPrimitiveType() const;

	/** \warning If the material's lighting flag or material type has changed,
		be sure to call commitMaterialChanges(); if the material has changed at
		all (except for diffuse color and texture matrices), be sure to call
		flush().  */
	virtual video::SMaterial& getMaterial();

	virtual const video::SMaterial& getMaterial() const;
	virtual video::E_VERTEX_TYPE getVertexType() const;
	virtual const void* getVertices() const;
	virtual void* getVertices();
	virtual u32 getVertexCount() const;
	virtual video::E_INDEX_TYPE getIndexType() const;
	virtual const u16* getIndices() const;
	virtual u16* getIndices();
	virtual u32 getIndexCount() const;
	virtual const core::aabbox3df& getBoundingBox() const;
	virtual void setBoundingBox(const core::aabbox3df& box);
	virtual void recalculateBoundingBox();
	virtual const core::vector3df& getPosition(u32 i) const;
	virtual core::vector3df& getPosition(u32 i);
	virtual const core::vector3df& getNormal(u32 i) const;
	virtual core::vector3df& getNormal(u32 i);
	virtual const core::vector2df& getTCoords(u32 i, u32 set = 0) const;
	virtual core::vector2df& getTCoords(u32 i, u32 set = 0);
	virtual void append(const void* const vertices,
						u32 numVertices,
						const u16* const indices,
						u32 numIndices);
	virtual void append(const IMeshBuffer* const other);
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const;
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const;
	virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint,
										E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);
	virtual void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX);
	virtual u32 getChangedID_Vertex() const;
	virtual u32 getChangedID_Index() const;

	// this class --------------------------------------------------------------

	//!
	void setVertexBuffer(void* buffer,
						 u32 capacity,
						 bool takeOwnership = false,
						 bool retainSize = false);

	//!
	void setIndexBuffer(void* buffer,
						u32 capacity,
						bool takeOwnership = false,
						bool retainSize = false);

	//!
	void* getVertexBuffer(u32* capacity = NULL, bool* owns = NULL) const
	{
		if (capacity)
		{
			*capacity = Buffers.getVertexMaxSize();
		}
		if (owns)
		{
			*owns = Buffers.ownsVertexBuffer();
		}
		return Buffers.getVertexStart();
	}

	//!
	void* getIndexBuffer(u32* capacity = NULL, bool* owns = NULL) const
	{
		if (capacity)
		{
			*capacity = Buffers.getIndexMaxSize() * sizeof(u16);
		}
		if (owns)
		{
			*owns = Buffers.ownsIndexBuffer();
		}
		return Buffers.getIndexStart();
	}

	//!
	u32 getIndexBufferCapacity() const
	{
		return Buffers.getIndexMaxSize();
	}

	//!
	void swapIndexBuffer(void*& buffer, u32& size, bool& owns)
	{
		u32 isize = size / sizeof(u16);
		Buffers.swapIndexBuffer(reinterpret_cast<u16*&>(buffer), isize, owns);
		size = isize * sizeof(u16);
	}

	//!
	void getBoundingBox(u32 vertexStart,
						u32 vertexEnd,
						core::aabbox3df& bbox) const;


	//!
	u32 append(const video::S3DVertexComponentArrays& vertices,
			   u16 startIndex,
			   u16 endIndex);

	//!
	u32 append(const u16* indexList,
			   u16 startIndex,
			   u32 primitiveCount,
			   E_PRIMITIVE_TYPE pType);

	//!
	void append(const video::S3DVertexComponentArrays& vertices,
				const u16* indexList,
				u16 startIndex,
				u16 endIndex,
				u32 primitiveCount,
				E_PRIMITIVE_TYPE pType,
				u32* vertexOffset = NULL,
				u32* indexOffset = NULL);

	//!
	void overwrite(const video::S3DVertexComponentArrays& vertices,
				   u16 vertexStart,
				   u16 vertexEnd,
				   u16 dest = 0,
				   u32 attributeMask = u32(-1),
				   const video::SMaterial* material = NULL)
	{
		_IRR_DEBUG_BREAK_IF(
			s32(vertexEnd - vertexStart)
			> s32((Buffers.getVertexSize() / Components.PositionStride) - dest)
		);

		_IRR_DEBUG_BREAK_IF(dest + (vertexEnd - vertexStart) > (s32)getVertexCount());
		overwrite(vertices,
				  vertexStart,
				  vertexEnd,
				  Buffers.getVertexStart() + dest * Components.PositionStride,
				  attributeMask,
				  material);
	}

	//!
	void overwrite(const u16* indexList,
				   u16 startIndex,
				   u32 primitiveCount,
				   E_PRIMITIVE_TYPE pType,
				   u32 dest = 0)
	{
		u32 count = 0;
		switch (pType)
		{
			case EPT_TRIANGLE_STRIP :
			case EPT_TRIANGLE_FAN :
			{
				count = primitiveCount + 2;
			}
			break;
			
			case EPT_TRIANGLES :
			{
				count = primitiveCount * 3;
			}
			break;

			default :
			{
				_IRR_DEBUG_BREAK_IF("not supported");
			}
		}
		_IRR_DEBUG_BREAK_IF(dest + count > getIndexCount());

		overwrite(indexList,
				  startIndex,
				  count,
				  pType,
				  Buffers.getIndexStart() + dest);
	}

	//! Convenience function to assign a new material 
	void setMaterial(const video::SMaterial& material)
	{
		Material = material;
		commitMaterialChanges();
	}

	//!
	void clear()
	{
		Buffers.resetVertices();
		Buffers.resetIndices();
	}

	//!
	void clearIndices()
	{
		Buffers.resetIndices();
	}

	//!
	void commitMaterialChanges()
	{
		video::IMaterialRenderer* mr
			= Driver->getMaterialRenderer(Material.getMaterialType());
		_IRR_DEBUG_BREAK_IF(mr == NULL);
		u32 requiredAttribs = mr->getVertexAttributeMask();
		if (Material.getFlag(video::EMF_LIGHTING))
		{
			//requiredAttribs |= video::EVA_NORMAL;
			if (!Material.getFlag(video::EMF_PER_VERTEX_MATERIAL_COLOR))
			{
				requiredAttribs &= ~video::EVA_COLOR0;
			}
		}
		// Check for material with no textures for required tex units
		u32 texMask = requiredAttribs & video::EVA_TEXCOORDS_MASK;
		for (u32 texUnit = 0; texMask; ++texUnit)
		{
			u32 texUnitMask = video::EVA_TEXCOORD0 << texUnit;
			texMask &= ~texUnitMask;
			if ((requiredAttribs & texUnitMask) != 0
				&& Material.getTexture(texUnit) == NULL)
			{
				requiredAttribs &= ~texUnitMask;
			}
		}
		if (requiredAttribs != RequiredVertexAttributes)
		{
			commitMaterialChanges(requiredAttribs);
		}
	}

	//!
	u32 getVertexAttributeMask() const
	{
		return RequiredVertexAttributes;
	}

	//!
	u32 getVertexBufferSize() const
	{
		return Buffers.getVertexSize();
	}

	//!
	u32 getMaxVertexBufferSize() const
	{
		return Buffers.getVertexMaxSize();
	}

	//!
	u32 getMaxVertexCount() const
	{
		return getMaxVertexBufferSize() / Components.PositionStride;
	}

	//!
	bool hasEnoughSpace(u32 vertexCount, u32 indexCount) const
	{
		return (Buffers.hasEnoughVertexSpace(vertexCount,
											 Components.PositionStride)
				&& Buffers.hasEnoughIndexSpace(indexCount));
	}

	//!
	void quantizeComponents(bool normalInShort = false,
							bool positionInShort = false);

	//!
	void transferToHardwareBuffer(E_HARDWARE_MAPPING mappingHint = EHM_STATIC);

protected:

	//!
	class CBuffers
	{
	public:

		CBuffers()
			: VertexStart(0)
			, VertexEnd(0)
			, VertexEndStorage(0)
			, IndexStart(0)
			, IndexEnd(0)
			, IndexEndStorage(0)
			, OwnsVertexData(false)
			, OwnsIndexData(false)
		{
		}

		~CBuffers()
		{
			clearVertices();
			clearIndices();
		}

		//! \param size Size of buffer in bytes.
		//! \param takeOwnership If true, the buffer must have been allocated
		//! with irrnew.
		void setVertexBuffer(u8* buffer,
							 u32 size,
							 bool takeOwnership,
							 bool retainSize)
		{
			u32 vertexSize = core::min_(getVertexSize(), size);
			clearVertices();
			VertexStart = buffer;
			VertexEnd = retainSize ? buffer + vertexSize : buffer;
			VertexEndStorage = buffer + size;
			OwnsVertexData = takeOwnership;
		}

		//!
		bool ownsVertexBuffer() const
		{
			return OwnsVertexData;
		}

		//!
		void expandVertices(u32 count, u32 componentSize)
		{
			_IRR_DEBUG_BREAK_IF(!hasEnoughVertexSpace(count, componentSize));
			VertexEnd += count * componentSize;
		}

		//!
		void clearVertices()
		{
			if (VertexStart && OwnsVertexData)
			{
				delete[] VertexStart;
			}
			VertexStart = VertexEnd = VertexEndStorage = 0;
		}

		//!
		void resetVertices()
		{
			VertexEnd = VertexStart;
		}

		//!
		u8* getVertexStart() const
		{
			return VertexStart;
		}

		//!
		u8* getVertexEnd() const
		{
			return VertexEnd;
		}

		u8* getVertexEndStorage() const
		{
			return VertexEndStorage;
		}

		//! Returns the current memory usage for vertices, in bytes.
		u32 getVertexSize() const
		{
			return VertexEnd - VertexStart;
		}
		
		//! Returns the current remaining memory for vertices, in bytes.
		u32 getRemainingVertexSize() const
		{
			return VertexEndStorage - VertexEnd;
		}

		//!
		bool hasEnoughVertexSpace(u32 count, u32 componentSize) const
		{
			u32 size = count * componentSize;
			return (size <= getRemainingVertexSize()
					&& getVertexSize() + size <= u16(-1) * componentSize);
		}

		//! Returns the maximum memory usage for vertices, in bytes.
		u32 getVertexMaxSize() const
		{
			return VertexEndStorage - VertexStart;
		}

		//! \param size Size of buffer in indices.
		void setIndexBuffer(u16* buffer,
							u32 size,
							bool takeOwnership,
							bool retainSize)
		{
			u32 indexSize = getIndexSize();
			clearIndices();
			IndexStart = buffer;
			IndexEnd = retainSize ? buffer + indexSize : buffer;
			IndexEndStorage = buffer + size;
			OwnsIndexData = takeOwnership;
		}

		//!
		bool ownsIndexBuffer() const
		{
			return OwnsIndexData;
		}

		//!
		void expandIndices(u32 size)
		{
			_IRR_DEBUG_BREAK_IF(!hasEnoughIndexSpace(size));
			IndexEnd += size;
		}

		//!
		void clearIndices()
		{
			if (IndexStart && OwnsIndexData)
			{
				delete[] IndexStart;
			}
			IndexStart = IndexEnd = IndexEndStorage = 0;
		}

		//!
		void resetIndices()
		{
			IndexEnd = IndexStart;
		}
		
		//!
		u16* getIndexStart() const
		{
			return IndexStart;
		}
 
		//!
		u16* getIndexEnd() const
		{
			return IndexEnd;
		}

		//!
		u16* getIndexEndStorage() const
		{
			return IndexEndStorage;
		}

		//! Returns the number of indices.
		u32 getIndexSize() const
		{
			return IndexEnd - IndexStart;
		}

		//! Returns the available number of indices.
		u32 getRemainingIndexSize() const
		{
			return IndexEndStorage - IndexEnd;
		}

		//!
		bool hasEnoughIndexSpace(u32 incomingSize) const
		{
			return incomingSize <= getRemainingIndexSize();
		}

		//! Returns the maximum number of indices.
		u32 getIndexMaxSize() const
		{
			return IndexEndStorage - IndexStart;
		}

		//! Exchanges the index buffer.
		/** It is assumed that the provided buffer is at least getIndexMaxSize()
			long. */
		void swapIndexBuffer(u16*& buffer, u32& size, bool& owns)
		{
			// swap buffer, where we need to update end pointers
			u32 sz = getIndexSize();
			u32 maxsz = getIndexMaxSize();
			_IRR_DEBUG_BREAK_IF(size > maxsz);
			u16* bufTmp = getIndexStart();
			IndexStart = buffer;
			IndexEnd = IndexStart + size;
			IndexEndStorage = IndexStart + maxsz;
			buffer = bufTmp;
			size = sz;

			// swap ownership
			bool ownTmp = ownsIndexBuffer();
			OwnsIndexData = owns;
			owns = ownTmp;
		}

	private:

		u8* VertexStart;
		u8* VertexEnd;
		u8* VertexEndStorage;
		u16* IndexStart;
		u16* IndexEnd;
		u16* IndexEndStorage;
		bool OwnsVertexData;
		bool OwnsIndexData;
	};

	//!
	void overwrite(const video::S3DVertexComponentArrays& vertices,
				   u16 startIndex,
				   u16 endIndex,
				   u8* dest,				   
				   u32 attributeMask = u32(-1),
				   const video::SMaterial* material = NULL);

	//!
	void overwrite(const u16* indexList,
				   s32 offset,
				   u32 count,
				   E_PRIMITIVE_TYPE pType,
				   u16* dest);

	//!
	u8* getAppendBuffer(u16 count)
	{
		return Buffers.getVertexEnd();
	}

	//!
	u32 commit(u8* buffer, u16 count)
	{
		_IRR_DEBUG_BREAK_IF(Buffers.getVertexEnd() != buffer);
		u32 offset = ((buffer - Buffers.getVertexStart())
					  / Components.PositionStride);
		Buffers.expandVertices(count, Components.PositionStride);
		_IRR_DEBUG_BREAK_IF((offset + count) * Components.PositionStride
							!= Buffers.getVertexSize());
		return offset;
	}

	//!
	u16* getIndexAppendBuffer(u16 count)
	{
		return Buffers.getIndexEnd();
	}

	//!
	u32 commitIndices(u16* indices, u32 count)
	{
		_IRR_DEBUG_BREAK_IF(Buffers.getIndexEnd() != indices);
		u32 offset = indices - Buffers.getIndexStart();
		Buffers.expandIndices(count);
		_IRR_DEBUG_BREAK_IF(offset + count != Buffers.getIndexSize());
		return offset;
	}

	//!
	void commitMaterialChanges(u32 requiredAttribs);

private:

	// non copyable
	CBatchBuffer(const CBatchBuffer&);
	CBatchBuffer& operator = (const CBatchBuffer&);

	video::IVideoDriver* Driver;
	video::SMaterial Material;
	video::S3DVertexComponentArrays Components;
	u32 RequiredVertexAttributes;
	core::aabbox3df BBox;

	// Memory implementation
	CBuffers Buffers;

	// test
	u32 ChangedID_Vertex;
	E_HARDWARE_MAPPING MappingHint_Vertex;
};

inline
void CBatchBuffer::append(const video::S3DVertexComponentArrays& vertices,
						  const u16* indexList,
						  u16 startIndex,
						  u16 endIndex,
						  u32 primitiveCount,
						  E_PRIMITIVE_TYPE pType,
						  u32* vertexOffset,
						  u32* indexOffset)
{
	//
	// Copy index data ---------------------------------------------------------
	//
	if (indexList)
	{
		// it is important to append index data first, because it uses the
		// current vertex count to compute the base index offset
		u32 offset = append(indexList, startIndex, primitiveCount, pType);
		if (indexOffset)
		{
			*indexOffset = offset;
		}
	}

	//
	// Copy vertex data --------------------------------------------------------
	//
	u32 offset = append(vertices, startIndex, endIndex);
	if (vertexOffset)
	{
		*vertexOffset = offset;
	}
}

} // end namespace scene
} // end namespace irr

#endif
