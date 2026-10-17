#ifndef __CCOLLADABINARYFILELOADER_H__
#define __CCOLLADABINARYFILELOADER_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{


//! Meshloader capable of loading COLLADA meshes and scene descriptions into Irrlicht.
class CColladaBinaryFileLoader : public IMeshLoader
{
public:
	//! Constructor
	CColladaBinaryFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs);

	//! destructor
	virtual ~CColladaBinaryFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".cob")
	virtual bool isALoadableFileExtension(const c8* fileName) const;

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file);
	virtual ISceneNode* createScene(io::IReadFile* file);

private:
	scene::ISceneManager* m_pSceneManager;
	io::IFileSystem* m_pFileSystem;
};




} // end namespace scene
} // end namespace irr




#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADABINARYFILELOADER_H__

