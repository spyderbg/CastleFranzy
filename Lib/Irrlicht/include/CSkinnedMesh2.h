//New skinned mesh

#ifndef __C_SKINNED_MESH2_H_INCLUDED__
#define __C_SKINNED_MESH2_H_INCLUDED__


#include <IAnimatedMesh.h>
#include <ISceneNode.h>
#include <IMeshBuffer.h>
#include <aabbox3d.h>

namespace irr
{
namespace scene
{

class CSkin : public virtual IReferenceCounted
{
public:
	core::matrix4				m_bindShapeMtx;
	core::array<core::matrix4>	m_invBindMtx;
	core::array<f32>			m_weights;
	core::array<u8>				m_bonesPerVertex;
	core::array<u8>				m_bonesRefIdPerVertex;
	core::array<core::stringc>  m_bonesName;

};

// For future implementation 
#define CSkeleton ISceneNode


//class CMeshBufferProxy
//	: public IMeshBuffer
//{
//protected:
//	IMeshBuffer * m_pIndicesRef;
//	IMeshBuffer * m_pVerticesRef;
//public:
//	//! Default constructor for empty meshbuffer
//	CMeshBufferProxy():m_pIndicesRef(0),m_pVerticesRef(0)
//	{
//		#ifdef _DEBUG
//		setDebugName("CMeshBufferProxy");
//		#endif
//	}
//
//	void setReferences(IMeshBuffer *pIndices, IMeshBuffer *pVertices)
//	{
//		m_pIndicesRef = pIndices;
//		m_pVerticesRef = pVertices;
//	}
//
//
//	//! Get material of this meshbuffer
//	/** \return Material of this buffer */
//	virtual const video::SMaterial& getMaterial() const
//	{
//		return m_pVerticesRef->getMaterial();
//	}
//
//
//	//! Get material of this meshbuffer
//	/** \return Material of this buffer */
//	virtual video::SMaterial& getMaterial()
//	{
//		return m_pVerticesRef->getMaterial();
//	}
//
//
//	//! Get pointer to vertices
//	/** \return Pointer to vertices. */
//	virtual const void* getVertices() const
//	{
//		return m_pVerticesRef->getVertices();
//	}
//
//
//	//! Get pointer to vertices
//	/** \return Pointer to vertices. */
//	virtual void* getVertices()
//	{
//		return m_pVerticesRef->getVertices();
//	}
//
//
//	//! Get number of vertices
//	/** \return Number of vertices. */
//	virtual u32 getVertexCount() const
//	{
//		return m_pVerticesRef->getVertexCount();
//	}
//
//	//! Get type of index data which is stored in this meshbuffer.
//	/** \return Index type of this buffer. */
//	virtual video::E_INDEX_TYPE getIndexType() const
//	{
//		return m_pIndicesRef->getIndexType();
//	}
//
//	//! Get pointer to indices
//	/** \return Pointer to indices. */
//	virtual const u16* getIndices() const
//	{
//		return m_pIndicesRef->getIndices();
//	}
//
//
//	//! Get pointer to indices
//	/** \return Pointer to indices. */
//	virtual u16* getIndices()
//	{
//		return m_pIndicesRef->getIndices();
//	}
//
//
//	//! Get number of indices
//	/** \return Number of indices. */
//	virtual u32 getIndexCount() const
//	{
//		return m_pIndicesRef->getIndexCount();
//	}
//
//
//	//! Get the axis aligned bounding box
//	/** \return Axis aligned bounding box of this buffer. */
//	virtual const core::aabbox3d<f32>& getBoundingBox() const
//	{
//		return m_pVerticesRef->getBoundingBox();
//	}
//
//
//	//! Set the axis aligned bounding box
//	/** \param box New axis aligned bounding box for this buffer. */
//	//! set user axis aligned bounding box
//	virtual void setBoundingBox(const core::aabbox3df& box)
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//
//	//! Recalculate the bounding box.
//	/** should be called if the mesh changed. */
//	virtual void recalculateBoundingBox()
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//
//	//! Get type of vertex data stored in this buffer.
//	/** \return Type of vertex data. */
//	virtual video::E_VERTEX_TYPE getVertexType() const
//	{
//		return m_pVerticesRef->getVertexType();
//	}
//
//	//! returns position of vertex i
//	virtual const core::vector3df& getPosition(u32 i) const
//	{
//		return m_pVerticesRef->getPosition(i);
//	}
//
//	//! returns position of vertex i
//	virtual core::vector3df& getPosition(u32 i)
//	{
//		return m_pVerticesRef->getPosition(i);
//	}
//
//	//! returns normal of vertex i
//	virtual const core::vector3df& getNormal(u32 i) const
//	{
//		return m_pVerticesRef->getNormal(i);
//	}
//
//	//! returns normal of vertex i
//	virtual core::vector3df& getNormal(u32 i)
//	{
//		return m_pVerticesRef->getNormal(i);
//	}
//
//	//! returns texture coord of vertex i
//	virtual const core::vector2df& getTCoords(u32 i) const
//	{
//		return m_pVerticesRef->getTCoords(i);
//	}
//
//	//! returns texture coord of vertex i
//	virtual core::vector2df& getTCoords(u32 i)
//	{
//		return m_pVerticesRef->getTCoords(i);
//	}
//
//
//	//! Append the vertices and indices to the current buffer
//	/** Only works for compatible types, i.e. either the same type
//	or the main buffer is of standard type. Otherwise, behavior is
//	undefined.
//	*/
//	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices)
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//
//	//! Append the meshbuffer to the current buffer
//	/** Only works for compatible types, i.e. either the same type
//	or the main buffer is of standard type. Otherwise, behavior is
//	undefined.
//	\param other Meshbuffer to be appended to this one.
//	*/
//	virtual void append(const IMeshBuffer* const other)
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//
//	//! get the current hardware mapping hint
//	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const
//	{
//		return m_pVerticesRef->getHardwareMappingHint_Vertex();
//	}
//
//	//! get the current hardware mapping hint
//	virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const
//	{
//		return m_pVerticesRef->getHardwareMappingHint_Index();
//	}
//
//	//! set the hardware mapping hint, for driver
//	virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint, E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX )
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//
//	//! flags the mesh as changed, reloads hardware buffers
//	virtual void setDirty(E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX)
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//	}
//
//	//! Get the currently used ID for identification of changes.
//	/** This shouldn't be used for anything outside the VideoDriver. */
//	virtual u32 getChangedID_Vertex() const 
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//		return 0;
//	}
//
//	//! Get the currently used ID for identification of changes.
//	/** This shouldn't be used for anything outside the VideoDriver. */
//	virtual u32 getChangedID_Index() const 
//	{
//		_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
//		return 0;
//	}
//	
//	
//};


class CSkinnedMesh2 
	: public IAnimatedMesh
{
protected:
	const IMesh *m_pSource;
	const CSkin *m_pSkin;
	const CSkeleton *m_pSkeleton;
	bool m_bSkeletonMtxPtrChacheIsDirty;
	bool m_bSkeletonMtxChacheIsDirty;
	
	void prepareSkeletonMtxCache();
	void prepareSkeletonMtxPtrCache();

public:
	
	CSkinnedMesh2();
	void skin();

	const IMesh *			setSource(const IMesh *);
	const CSkin *			setSkin(const CSkin *);
	const CSkeleton *		setSkeleton(const CSkeleton *);

	//------------------------
	// Base class requirements
	//------------------------

	//! Returns the amount of mesh buffers.
	/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
	virtual u32 getMeshBufferCount() const { return 1; };

	//! Returns pointer to a mesh buffer.
	/** \param nr: Zero based index of the mesh buffer. The maximum value is
	getMeshBufferCount() - 1;
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const { return const_cast<IMeshBuffer *> ( m_pWorkingMeshBuffer ); };

	//! Returns pointer to a mesh buffer which fits a material
	/** \param material: material to search for
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const { return m_pWorkingMeshBuffer; };

	//! Returns an axis aligned bounding box of the mesh.
	/** \return A bounding box of this mesh is returned. */
	virtual const core::aabbox3d<f32>& getBoundingBox() const 
	{ 
		return m_box;
	};

	//! set user axis aligned bounding box
	/** \param box New bounding box to use for the mesh. */
	virtual void setBoundingBox( const core::aabbox3df& box) 
	{
		return;
	};

	//! Sets a flag of all contained materials to a new value.
	/** \param flag: Flag to set in all materials.
	\param newvalue: New value to set in all materials. */
	virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) {return;};

	//! OnAnimate() is called just before rendering the whole scene.
	virtual void OnAnimate(u32 timeMs);

	//! Gets the frame count of the animated mesh.
	/** \return Returns the amount of frames. If the amount is 1,
	it is a static, non animated mesh. */
	u32 getFrameCount() const { return 1; }

	//! Returns the IMesh interface for a frame.
	/** \param frame: Frame number as zero based index. The maximum
	frame number is getFrameCount() - 1;
	\param detailLevel: Level of detail. 0 is the lowest, 255 the
	highest level of detail. Most meshes will ignore the detail level.
	\param startFrameLoop: Because some animated meshes (.MD2) are
	blended between 2 static frames, and maybe animated in a loop,
	the startFrameLoop and the endFrameLoop have to be defined, to
	prevent the animation to be blended between frames which are
	outside of this loop.
	If startFrameLoop and endFrameLoop are both -1, they are ignored.
	\param endFrameLoop: see startFrameLoop.
	\return Returns the animated mesh based on a detail level. */
	virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1)
	{
		skin();
		return this;
	}
private:
	//! Variable that track the translation generated by the skinning.
	/** This value is based on the first vertex and isn't a 
		average of all the translation of all vertex */
	mutable core::aabbox3d<f32> m_box;

	//! Variable that track the translation generated by the skinning.
	/** This value is based on the first vertex and isn't a 
		average of all the translation of all vertex */
	core::vector3df m_SkinningTranslation;

	//! Set of pointer to the Absolute Transformation of each nodes.
	core::array<const core::matrix4 *> m_SkeletonMtxPtrCache;

	//! Variable that store the computation of the Bones Inv Matrix with his Global Matrix .
	core::array<core::matrix4> m_SkeletonMtxCache;


	IMeshBuffer* m_pWorkingMeshBuffer;
	//core::array<CMeshBufferProxy *> m_VertexBuffers;
};

}
}

#endif
