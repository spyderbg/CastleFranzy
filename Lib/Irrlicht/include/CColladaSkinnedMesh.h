//-*-c++-*-
#ifndef __CCOLLADASKINNEDMESH_H__
#define __CCOLLADASKINNEDMESH_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSkinnedMesh.h"
#include <SDatabaseCollada.h>
#include "CColladaDatabase.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

class CColladaSkinnedMesh
	: public IColladaSkinnedMesh
{
private:

	bool BindShapeMatrixIsIdentity;
	bool SelfAttach;
	mutable bool SkeletonMtxChacheIsDirty;
	bool SkeletonMtxPtrChacheIsDirty;
	bool ReleaseSourceProcessBuffer;

	struct SMatrixCacheEntry
	{
		core::matrix4 MatrixCache;
		const core::matrix4* SkeletonMatrix;

		SMatrixCacheEntry()
			: MatrixCache(core::matrix4::EM4CONST_NOTHING),
			  SkeletonMatrix(NULL)
		{
		}
	};
	
	core::array<SMatrixCacheEntry> SkeletonMatrixCache;

	mutable core::aabbox3d<f32> Box;

protected:

	struct SBuffer
	{
		IMeshBuffer* MeshBuffer;
		u32 BoneStart;

		explicit SBuffer(IMeshBuffer* meshBuffer = NULL)
			: MeshBuffer(meshBuffer)
			, BoneStart(0)
		{
			if (meshBuffer)
			{
				meshBuffer->grab();
			}
		}

		SBuffer(const SBuffer& other)
			: MeshBuffer(other.MeshBuffer)
			, BoneStart(other.BoneStart)
		{
			if (MeshBuffer)
			{
				MeshBuffer->grab();
			}
		}

		~SBuffer()
		{
			if (MeshBuffer)
			{
				MeshBuffer->drop();
			}
		}

		SBuffer& operator = (const SBuffer& other)
		{
			if (other.MeshBuffer)
			{
				other.MeshBuffer->grab();
			}
			if (MeshBuffer)
			{
				MeshBuffer->drop();
			}
			MeshBuffer = other.MeshBuffer;
			return *this;
		}
	};

	core::array<SBuffer> VertexBuffers;
	IColladaMesh* Source;

	collada::SSkin& Skin;
	res::vector<res::String>* Skeleton;
	collada::SBindMaterial* BindMaterial;
	scene::ISceneNode* SkeletonNode;

private:

	int instanciateMesh(collada::IRootSceneNode* root);
	void prepareSkinBuffers(bool useProcessBuffer,
							video::IVideoDriver* driver);
	void computeBoundingBox();
	void prepareSkeletonMtxCache();
	void prepareSkeletonMtxPtrCache();
	void selfAttach();

public:

	virtual E_TYPE getType() const;

	CColladaSkinnedMesh(
		const collada::CColladaDatabase& collada, 
		collada::SController& controller,
		res::vector<res::String>* skeletons = 0,
		collada::IRootSceneNode* root = 0,
		bool useProcessBuffer = true,
		video::IVideoDriver* driver = NULL
	);

	~CColladaSkinnedMesh();

	//! \internal
	virtual void skin(u32 buffer);

	void attach(ISceneNode*);
	void detach();

	bool isBindShapeMatrixIdentity() const
	{
		return BindShapeMatrixIsIdentity;
	}

	collada::SMatrix& getBindShapeMatrix() const
	{
		return Skin.bindShapeMatrix;
	}

	IColladaMesh* getSource() const
	{
		return Source;
	}

	// IColladaMesh - virtual functions

	virtual void onAnimate(u32 timeMs);
	virtual video::E_DRIVER_ALLOCATION_RESULT onPrepareBufferForRendering(
		E_PREPARE_BUFFER_STEP step,
		video::IVideoDriver* driver,
		u32 buffer
	);

	//!
	virtual void releaseProcessBuffer(video::IVideoDriver* driver,
									  u32 buffer);

	// IColladaSkinnedMesh - virtual functions

	virtual void setIsSkinningEnabled(bool value);

	// IMesh - virtual function

	//! Returns the amount of mesh buffers.
	/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
	virtual u32 getMeshBufferCount() const;

	//! Returns pointer to a mesh buffer.
	/** \param nr: Zero based index of the mesh buffer. The maximum value is
	getMeshBufferCount() - 1;
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const;

	//! Returns an axis aligned bounding box of the mesh.
	/** \return A bounding box of this mesh is returned. */
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

}; // class CColladaSkinnedMesh

} // end namespace collada
} // end namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
