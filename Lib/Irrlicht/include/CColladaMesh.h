//-*-c++-*-
#ifndef __CCOLLADAMESH_H__
#define __CCOLLADAMESH_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include <resFile.h>
#include "CColladaMeshBuffer.h"
#include "IColladaMesh.h"

namespace irr
{

namespace collada
{
	struct SGeometry;
};

namespace scene
{

class CColladaMeshBuffer;

class CColladaMesh
	: public IColladaMesh
{
public:

	CColladaMesh(const collada::CColladaDatabase& database, const collada::SGeometry&);
	~CColladaMesh();
	
	virtual E_TYPE getType() const;

	//! Returns the amount of mesh buffers.
	/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
	virtual u32 getMeshBufferCount() const;

	//! Returns pointer to a mesh buffer.
	/** \param nr: Zero based index of the mesh buffer. The maximum value is
	getMeshBufferCount() - 1;
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const;

	//! Returns pointer to a mesh buffer which fits a material
	/** \param material: material to search for
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const;

	//! Returns an axis aligned bounding box of the mesh.
	/** \return A bounding box of this mesh is returned. */
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! Set user axis aligned bounding box
	/** Not supported */
	virtual void setBoundingBox( const core::aabbox3df& box);

	//! Sets a flag of all contained materials to a new value.
	/** \param flag: Flag to set in all materials.
	\param newvalue: New value to set in all materials. */
	virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

	//! returns the user properties of the mesh
	virtual core::stringc getUserProperty() const;

public:

	const collada::SGeometry&				Geometry;

	core::aabbox3d<f32>						BoundingBox;
	core::array<CColladaMeshBuffer*>		MeshBuffers;

}; // class CColladaMesh

} // namespace scene
} // namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADAMESH_H__
