//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_BUFFER_H_INCLUDED__
#define __I_MESH_BUFFER_H_INCLUDED__

#include "aabbox3d.h"
#include "S3DVertex.h"
#include "SMaterial.h"
#include "SVertexIndex.h"
#include "EHardwareBufferFlags.h"
#include "EPrimitiveType.h"
#include "EVertexType.h"
#include "IDriverBinding.h"

namespace irr
{
namespace video
{

class IVideoDriver;

} // end namespace video

namespace scene
{

enum E_BUFFER_TYPE
{
	EBT_NONE = 0,
	EBT_VERTEX,
	EBT_INDEX,
	EBT_VERTEX_AND_INDEX
};

//! Struct for holding a mesh with a single material
/** SMeshBuffer is a simple implementation of a MeshBuffer.

	Since meshbuffers are used for drawing, and hence will be exposed to the
	driver, chances are high that they are grab()'ed from somewhere.  It's
	therefore required to dynamically allocate meshbuffers which are passed to a
	video driver and only drop hte buffer once it's not used in the current code
	block anymore.
*/
class IMeshBuffer : public virtual IReferenceCounted
{
public:

	virtual ~IMeshBuffer();


	//! Called by drop just before deleting.
	/** Useful if some virtual calls are required to clean up. */
	virtual void onDelete();

	//! Get the primitive type of this meshbuffer
	virtual E_PRIMITIVE_TYPE getPrimitiveType() const = 0;

	//! Get the material of this meshbuffer
	/** \return Material of this buffer. */
	virtual video::SMaterial& getMaterial() = 0;

	//! Get the material of this meshbuffer
	/** \return Material of this buffer. */
	virtual const video::SMaterial& getMaterial() const = 0;

	//! Get type of vertex data which is stored in this meshbuffer.
	/** \return Vertex type of this buffer. */
	virtual video::E_VERTEX_TYPE getVertexType() const = 0;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
	virtual const void* getVertices() const = 0;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
	virtual void* getVertices() = 0;

	//! Get amount of vertices in meshbuffer.
	/** \return Number of vertices in this buffer. */
	virtual u32 getVertexCount() const = 0;

	//! Returns the first index actually used in the vertex buffer to render
	//! the mesh buffer.
	virtual u32 getVertexIndexStart() const;

	//! Returns the end of the index range (i.e. the last index plus one)
	//! actually used in the vertex buffer to render the mesh buffer.
	virtual u32 getVertexIndexEnd() const;

	//! Get type of index data which is stored in this meshbuffer.
	/** \return Index type of this buffer. */
	virtual video::E_INDEX_TYPE getIndexType() const = 0;

	//! Get access to Indices.
	/** \return Pointer to indices array. */
	virtual const u16* getIndices() const = 0;

	//! Get access to Indices.
	/** \return Pointer to indices array. */
	virtual u16* getIndices() = 0;

	//! Get amount of indices in this meshbuffer.
	/** \return Number of indices in this buffer. */
	virtual u32 getIndexCount() const = 0;

	//! Get the axis aligned bounding box of this meshbuffer.
	/** \return Axis aligned bounding box of this buffer. */
	virtual const core::aabbox3df& getBoundingBox() const = 0;

	//! Set axis aligned bounding box
	/** \param box User defined axis aligned bounding box to use
		for this buffer. */
	virtual void setBoundingBox(const core::aabbox3df& box) = 0;

	//! Recalculates the bounding box. Should be called if the mesh changed.
	virtual void recalculateBoundingBox() = 0;

	//! returns position of vertex i
	virtual const core::vector3df& getPosition(u32 i) const = 0;

	//! returns position of vertex i
	virtual core::vector3df& getPosition(u32 i) = 0;

	//! returns normal of vertex i
	virtual const core::vector3df& getNormal(u32 i) const = 0;

	//! returns normal of vertex i
	virtual core::vector3df& getNormal(u32 i) = 0;

	//! returns texture coord of vertex <i> inside the coordinate set <set>
	virtual const core::vector2df& getTCoords(u32 i, u32 set = 0) const = 0;

	//! returns texture coord of vertex <i> inside the coordinate set <set>
	virtual core::vector2df& getTCoords(u32 i, u32 set = 0) = 0;

	//! Append the vertices and indices to the current buffer
	/** Only works for compatible vertex types.
		\param vertices Pointer to a vertex array.
		\param numVertices Number of vertices in the array.
		\param indices Pointer to index array.
		\param numIndices Number of indices in array. */
	virtual void append(const void* const vertices,
						u32 numVertices,
						const u16* const indices,
						u32 numIndices) = 0;

	//! Append the meshbuffer to the current buffer
	/** Only works for compatible vertex types
		\param other Buffer to append to this one. */
	virtual void append(const IMeshBuffer* const other) = 0;

	//! get the current hardware mapping hint
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const = 0;

	//! get the current hardware mapping hint
	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const = 0;

	//! set the hardware mapping hint, for driver
	virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint,
										E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX ) = 0;

	//! flags the meshbuffer as changed, reloads hardware buffers
	virtual void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;

	//to be spit into vertex and index buffers:
	//! Get the currently used ID for identification of changes.
	/** This shouldn't be used for anything outside the VideoDriver. */
	virtual u32 getChangedID_Vertex() const = 0;

	//! Get the currently used ID for identification of changes.
	/** This shouldn't be used for anything outside the VideoDriver. */
	virtual u32 getChangedID_Index() const = 0;

	//!
	video::IDriverBinding* getDriverBinding() const
	{
		return DriverBinding;
	}

	//!
	void setDriverBinding(video::IDriverBinding* binding)
	{
		if (DriverBinding != binding)
		{
			delete DriverBinding;
			DriverBinding = binding;
		}
	}

	//!
	bool isDynamic() const
	{
		return IsDynamic;
	}

	//!
	void setIsDynamic(bool value)
	{
		IsDynamic = value;
	}

	//!
	bool isVisible() const
	{
		return IsVisible;
	}

	//!
	void setIsVisible(bool value)
	{
		IsVisible = value;
	}

protected:

	IMeshBuffer(bool isDynamic = false)
		: DriverBinding(NULL)
		, IsDynamic(isDynamic)
	{
	}

	mutable video::IDriverBinding* DriverBinding;
	bool IsDynamic;
	bool IsVisible;

	friend class video::IVideoDriver;
};

} // end namespace scene
} // end namespace irr

#endif
