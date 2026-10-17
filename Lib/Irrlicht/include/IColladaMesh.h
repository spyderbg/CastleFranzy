//-*-c++-*-
#ifndef __ICOLLADAMESH_H__
#define __ICOLLADAMESH_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IMesh.h"
#include "IColladaObject.h"
#include "S3DVertex.h"
#include "EDriverAllocationResult.h"

namespace irr
{
namespace collada
{

struct SMaterial;

}

namespace video
{

class IVideoDriver;

}

namespace scene
{

class IColladaMesh
	: public IMesh
	, public collada::IObject
{
public:

	//!
	enum E_TYPE
	{
		ET_MESH,
		ET_MORPHING_MESH,
		ET_SKINNED_MESH,
		ET_MODULAR_SKINNED_MESH
	};

	//!
	enum E_PREPARE_BUFFER_STEP
	{
		EPBS_REGISTRATION,
		EPBS_RENDERING,
	};

	//!
	virtual E_TYPE getType() const = 0;

	//!
	virtual void onAnimate(u32 timeMs);

	//! If the mesh requires any processing before rendering, this method
	//! executes it.
	/**
	   \param step Indicates the time when processing is occuring
	   \param buffer Indicates which mesh buffer to process, or all of them if
	   u32(-1) is passed
	 */
	virtual video::E_DRIVER_ALLOCATION_RESULT onPrepareBufferForRendering(
		E_PREPARE_BUFFER_STEP step,
		video::IVideoDriver* driver,
		u32 buffer
	);

	//!
	virtual void releaseProcessBuffer(video::IVideoDriver* driver,
									  u32 buffer);


	//! Indicates if the mesh has its own working buffer.
	/** \note Should we change the name to something better? */
	bool hasOutputBuffer() const
	{
		return HasOutputBuffer;
	}

	// Should we cache here the values that the two methods below would return?

	//! Convenience function to test if the mesh has normals.
	bool hasNormals() const
	{
		return reinterpret_cast<const video::SS3DVertexComponentArrays*>(
			getMeshBuffer(0)->getVertices()
		)->Normal != NULL;
	}

	//! Convenience function to return the number of vertices in the mesh.
	u32 getVertexCount() const
	{
		return getMeshBuffer(0)->getVertexCount();
	}

	//! Sets a material and effect for a given meshBuffer
	/** \param idx: a meshBuffer index
	\param material: a Collada Maretial.*/
	void setMaterial(int idx, const collada::SMaterial& material)
	{
		Materials[idx] = &material;
	}

	//!
	const collada::SMaterial* getMaterial(int idx) const
	{
		return Materials[idx];
	}

protected:

	IColladaMesh(const collada::CColladaDatabase& database)
		: collada::IObject(database)
		, HasOutputBuffer(false)
	{
	}

	core::array<const collada::SMaterial*> Materials;
	bool HasOutputBuffer;
};

} // end namespace scene
} // end namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
