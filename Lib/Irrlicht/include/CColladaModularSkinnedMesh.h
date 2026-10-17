//-*-c++-*-
#ifndef __C_COLLADA_MODULAR_SKINNED_MESH_H_INCLUDED__
#define __C_COLLADA_MODULAR_SKINNED_MESH_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSkinnedMesh.h"
#ifdef _IRR_COMPILE_WITH_EDITOR_
#include "IAttributeExchangingObject.h"
#endif

namespace irr
{

namespace collada
{
class CModularSkinnedMeshSceneNode;
}

namespace scene
{



class CColladaModularSkinnedMesh 
	: public IColladaSkinnedMesh
{
public:
	friend class collada::CModularSkinnedMeshSceneNode;
	enum E_ALLOCATION_POLICY
	{
		EAP_GROWING,
		EAP_TRIMMED,
		EAP_FIXED
	};

	enum E_MODULE_UPDATE_RESULT
	{
		EMUR_SUCCESS,
		EMUR_OUT_OF_MEMORY,
		EMUR_MAX_BUFFER_SIZE_EXCEEDED
	};

protected:
	collada::SInstanceModularSkin &		ModularSkin;
	collada::IRootSceneNode*			RootSceneNode;

	core::array<int>					CurrentModulesId;
	core::array<IColladaSkinnedMesh*>	Modules;

	struct SModularMeshBuffer
	{
		SModularMeshBuffer();
		~SModularMeshBuffer();		
		E_MODULE_UPDATE_RESULT realloc(u32 size);

		IMeshBuffer *					MeshBuffer;
		video::SMaterial				Material;
		core::array<u32>				Modules;
		u8*								Data;
		u32								DataSize;
	};

	core::array<SModularMeshBuffer>		ModularMeshBuffers;

	E_ALLOCATION_POLICY					AllocPolicy;
	core::aabbox3df						Box;
	mutable bool						IsBoxDirty;

	void computeBoundingBox();
	E_MODULE_UPDATE_RESULT updateBuffer(bool useProcessBuffer);
	E_MODULE_UPDATE_RESULT setModuleCount(u32 count, bool withUpdate);
	E_MODULE_UPDATE_RESULT realloc(u32 size);

	u32 getModuleCount() const;
	E_MODULE_UPDATE_RESULT setModuleCount(u32 count);

	//! Returns the mesh module of given id.
	/** \warning If you get a module and later replace it, do not forget to grab() it! */
	IColladaSkinnedMesh* getModule(int id) const;

	//! Change mesh module of given id.
	/** \warning If you want to give owner ship to this modular skin mesh, do
		not forget to drop() module. */
	E_MODULE_UPDATE_RESULT setModule(u32 id, IColladaSkinnedMesh* module);

	//! Changes all modules.
	/** If count == 0, then it is assumed that the provided number of modules is
		the same as getModuleCount(). */
	E_MODULE_UPDATE_RESULT setModules(IColladaSkinnedMesh** modules,
									  u32 count = 0);

	//! Get the Category ID from Name
	int getCategoryId(const char *name) const;

	//! Get the Category Name from ID
	const char * getCategoryName(int categoryId) const;

	//! Get the Module ID from Name
	int getModuleId(const char *name) const;

	//! Get current module ID for a given category ID
	int getCurrentModuleId(int categoryId) const;

	//! Get current module Name for a given category ID
	const char * getCurrentModuleName(int categoryId) const;

	//! Get Module Name for a given {Category ID,Module ID}
	const char * getModuleName(int categoryId, int moduleId) const;

	//! Get the Categories count
	int getCategoryCount() const;

	//! Get the modules count for a given category ID
	int getCategoryModuleCount(int categoryId) const;

	//! Set the module ID for a given category ID
	void setCategoryModule(int categoryId, int moduleId = -1);

	//! Set the module for a given category using their names.
	void setCategoryModule(const char *categoryName, const char *moduleName = 0);

public:

	CColladaModularSkinnedMesh(const collada::CColladaDatabase& collada,
							   collada::SInstanceModularSkin* controller,
							   collada::IRootSceneNode* rootSceneNode,
							   int maxBufferSize = -1,
							   bool useProcessBuffer = true,
							   video::IVideoDriver* driver = NULL);
	/*CColladaModularSkinnedMesh(const collada::CColladaDatabase& collada,
							   int moduleCount,
							   IColladaSkinnedMesh** modules,
							   int maxBufferSize = -1,
							   bool useProcessBuffer = true,
							   video::IVideoDriver* driver = NULL);*/
	virtual ~CColladaModularSkinnedMesh();



	// IColladaSkinnedMesh interface

	//! \internal
	virtual void skin(u32 buffer);

	virtual void setIsSkinningEnabled(bool value);

	// IColladaMesh interface --------------------------------------------------

	virtual E_TYPE getType() const;

	virtual void onAnimate(u32 timeMs);
	virtual video::E_DRIVER_ALLOCATION_RESULT onPrepareBufferForRendering(
		E_PREPARE_BUFFER_STEP step,
		video::IVideoDriver* driver,
		u32 buffer
	);

	// IMesh interface ---------------------------------------------------------

	virtual u32 getMeshBufferCount() const;
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const;
	
	virtual const core::aabbox3d<f32>& getBoundingBox() const;
};

inline u32 CColladaModularSkinnedMesh::getModuleCount() const
{
	return Modules.size();
}

inline IColladaSkinnedMesh* CColladaModularSkinnedMesh::getModule(int id) const
{
	return Modules[id];
}

inline CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT CColladaModularSkinnedMesh::setModuleCount(u32 count)
{
	return setModuleCount(count, true);
}

} // end namespace scene
} // end namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
