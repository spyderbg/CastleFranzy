//-*-c++-*-
#ifndef __CCOLLADAMORPHINGMESH_H__
#define __CCOLLADAMORPHINGMESH_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaMesh.h"
#include "S3DVertexComponentArrays.h"

namespace irr
{
namespace collada
{

class CColladaMorphingMesh
	: public scene::IColladaMesh
{
private:

	int instanciateMesh();
	void prepareMorphBuffers(bool useProcessBuffer,
							 video::IVideoDriver* driver);

protected:

	void morph(u32 buffer);

	bool isRelative() const
	{
		return Morph.method != 0;
	}

	core::array<scene::IMeshBuffer*> VertexBuffers;

	struct STarget
	{
		scene::IMesh* Mesh;
		float Weight;

		explicit STarget(scene::IMesh* mesh = NULL,
						 float weight = 0.0f)
			: Mesh(mesh)
			, Weight(weight)
		{
			if (mesh)
			{
				mesh->grab();
			}
		}

		STarget(const STarget& other)
			: Mesh(other.Mesh)
			, Weight(other.Weight)
		{
			if (Mesh)
			{
				Mesh->grab();
			}
		}

		~STarget()
		{
			if (Mesh)
			{
				Mesh->drop();
			}
		}

		STarget& operator = (const STarget& other)
		{
			if (other.Mesh)
			{
				other.Mesh->grab();
			}
			if (Mesh)
			{
				Mesh->drop();
			}
			Mesh = other.Mesh;
			Weight = other.Weight;
			return *this;
		}
	};

	core::array<STarget> Targets;
	collada::SMorph& Morph;
	collada::SBindMaterial* BindMaterial;

	scene::IMesh* getMesh(u32 index) const
	{
		return Targets[index].Mesh;
	}

	scene::IMesh* getSource() const
	{
		return getMesh(0);
	}

public:

	virtual E_TYPE getType() const;

	CColladaMorphingMesh(const collada::CColladaDatabase& collada, 
						 collada::SController& controller,
						 bool useProcessBuffer = true,
						 video::IVideoDriver* driver = NULL);
	~CColladaMorphingMesh();

	u32 getTargetCount() const
	{
		return Targets.size();
	}

	f32& getWeightRef(int index)
	{
		return Targets[index].Weight;
	}

	f32 getWeight(u32 index) const
	{
		return Targets[index].Weight;
	}

	void setWeight(u32 index, f32 weight)
	{
		Targets[index].Weight = weight;
	}

	f32 getWeight(const char *meshName) const
	{
		int i = getMeshNameID(meshName);
		if(i != -1)
		{
			return getWeight(getMeshNameID(meshName));
		}
		else
		{
			return 0.0f;
		}
	}

	int getMeshNameID(const char *meshName) const
	{
		for(int i = 0, sz = Morph.targets.size(); i < sz; ++i)
		{
			if(Morph.targets[i] == meshName)
			{
				return i;
			}
		}
		return -1;
	}

	// IColladaMesh - virtual function

	virtual video::E_DRIVER_ALLOCATION_RESULT onPrepareBufferForRendering(
		E_PREPARE_BUFFER_STEP step,
		video::IVideoDriver* driver,
		u32 buffer
	);

	//!
	virtual void releaseProcessBuffer(video::IVideoDriver* driver,
									  u32 buffer);

	// IMesh - virtual function

	//! Returns the amount of mesh buffers.
	/** \return Returns the amount of mesh buffers (IMeshBuffer) in this mesh. */
	virtual u32 getMeshBufferCount() const;

	//! Returns pointer to a mesh buffer.
	/** \param nr: Zero based index of the mesh buffer. The maximum value is
	getMeshBufferCount() - 1;
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual scene::IMeshBuffer* getMeshBuffer(u32 nr) const;

	//! Returns pointer to a mesh buffer which fits a material
	/** \param material: material to search for
	\return Returns the pointer to the mesh buffer or
	NULL if there is no such mesh buffer. */
	virtual scene::IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const;

	//! Returns an axis aligned bounding box of the mesh.
	/** \return A bounding box of this mesh is returned. */
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! Set user axis aligned bounding box
	/** \param box New bounding box to use for the mesh. */
	virtual void setBoundingBox(const core::aabbox3df& box);

	//! Sets a flag of all contained materials to a new value.
	/** \param flag: Flag to set in all materials.
	\param newvalue: New value to set in all materials. */
	virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

	//! returns the user properties of the mesh
	virtual core::stringc getUserProperty() const;

};

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
#endif
