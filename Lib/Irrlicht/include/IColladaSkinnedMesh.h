//-*-c++-*-
#ifndef __I_COLLADA_SKINNED_MESH_H_INCLUDED__
#define __I_COLLADA_SKINNED_MESH_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaMesh.h"

namespace irr
{
namespace video
{

class IVideoDriver;

} // end namespace video

namespace scene
{

class IColladaSkinnedMesh
	: public IColladaMesh
{
private:

	bool IsSkinningEnabled;

protected:

	IColladaSkinnedMesh(const collada::CColladaDatabase& database)
		: IColladaMesh(database)
		, IsSkinningEnabled(true)
	{
	}

public:

	//! \internal
	virtual void skin(u32 buffer) = 0;

	bool isSkinningEnabled() const
	{
		return IsSkinningEnabled;
	}

	/** \note Subclasses overriding this method should call this one in the
		override. */
	virtual void setIsSkinningEnabled(bool value);

	//!
	virtual void releaseProcessBuffer(video::IVideoDriver* driver,
									  u32 buffer);

	// forward
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const = 0;

	//! Returns pointer to a mesh buffer which fits a material
	/** \param material: material to search for
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual IMeshBuffer* getMeshBuffer(const video::SMaterial& material) const;

	//! set user axis aligned bounding box
	/** \param box New bounding box to use for the mesh. */
	virtual void setBoundingBox( const core::aabbox3df& box);

	//! Sets a flag of all contained materials to a new value.
	/** \param flag: Flag to set in all materials.
	\param newvalue: New value to set in all materials. */
	virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

	virtual core::stringc getUserProperty() const;
};

} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
