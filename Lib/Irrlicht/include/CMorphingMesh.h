#ifndef __IRR_MORPHING_MESH_H__
#define __IRR_MORPHING_MESH_H__

#include "IMorphingMesh.h"
#include "irrArray.h"

namespace irr
{
namespace scene
{
class CMorphingMesh : public IMorphingMesh
{
public:
		//! Constructor
		CMorphingMesh() : m_bIsInDirtyState(true), m_WorkingBuffer(0), m_Source(0) {}
		
		//! Destructor
		virtual ~CMorphingMesh()  { }

		//! Returns the amount of mesh buffers.
		/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
		virtual u32 getMeshBufferCount() const 
		{ 
			return 1; 
		}

		//! Returns pointer to a mesh buffer.
		/** \param nr: Zero based index of the mesh buffer. The maximum value is
		getMeshBufferCount() - 1;
		\return Returns the pointer to the mesh buffer or
		NULL if there is no such mesh buffer. */
		virtual IMeshBuffer* getMeshBuffer(u32 nr) const 
		{ 
			return m_WorkingBuffer; 
		}

		//! Returns an axis aligned bounding box of the mesh.
		/** \return A bounding box of this mesh is returned. */
		virtual const core::aabbox3d<f32>& getBoundingBox() const 
		{ 
			return m_Source->getBoundingBox();
		};

		//! set user axis aligned bounding box
		/** \param box New bounding box to use for the mesh. */
		virtual void setBoundingBox( const core::aabbox3df& box) {};

		//! Sets a flag of all contained materials to a new value.
		/** \param flag: Flag to set in all materials.
		\param newvalue: New value to set in all materials. */
		virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) {};

		//! Returns pointer to a mesh buffer which fits a material
		/** \param material: material to search for
		\return Returns the pointer to the mesh buffer or
		NULL if there is no such mesh buffer. */
		virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const { return m_WorkingBuffer; }

		//! Gets the frame count of the animated mesh.
		/** \return Returns the amount of frames. If the amount is 1,
		it is a static, non animated mesh. */
		virtual u32 getFrameCount() const { return -1; };

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
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1) { return this; };

		//! Returns the type of the animated mesh.
		/** In most cases it is not neccessary to use this method.
		This is useful for making a safe downcast. For example,
		if getMeshType() returns EAMT_MD2 it's safe to cast the
		IAnimatedMesh to IAnimatedMeshMD2.
		\returns Type of the mesh. */
		virtual E_ANIMATED_MESH_TYPE getMeshType() const
		{
			return EAMT_MORPHING;
		}

		void setSource(IMesh*);
		void addTargets(IMesh*, f32 weight);
		void reserveTargets(u32 count);
		
		void init();
		void prepare();
protected:
	bool					  m_bIsInDirtyState;
	IMeshBuffer*			  m_WorkingBuffer;
	const IMesh*			  m_Source;
	core::array<const IMesh*> m_Targets;
	core::array<f32>		  m_Weights;
};
}; // namespace scene
}; // namespace irr

#endif