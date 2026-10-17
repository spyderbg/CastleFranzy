#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

//#include "ISceneNodeAnimatorChannelLibrary.h"
//#include "CSceneNodeAnimatorChannelLibrary.h"
#include "CColladaBinaryFileLoader.h"
#include <SDatabaseCollada.h>
#include "IReadFile.h"
#include "resFile.h"
#include "irrTypes.h"
#include "CEmptySceneNode.h"
#include "CColladaMesh.h"
#include "CColladaMeshSceneNode.h"
#include "CColladaResFileManager.h"
#include "CColladaSkinnedMesh.h"
#include "CColladaDatabase.h"
#include "CColladaSceneNodeAnimator.h"

namespace irr
{

using namespace collada;

namespace scene
{

CColladaMesh *
instanciateGeometry(const SCollada &dat, const SGeometry &geometry);

ISceneNode *
instanciateController(const SCollada &dat
					  , const SController &controller, const SBindMaterial &bindMaterial);

//! Constructor
CColladaBinaryFileLoader::CColladaBinaryFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs)
: m_pSceneManager(smgr)
, m_pFileSystem(fs)
{
	setDebugName("CColladaFileLoader");
}

//! destructor 
CColladaBinaryFileLoader::~CColladaBinaryFileLoader()
{

}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".cob")
bool 
CColladaBinaryFileLoader::isALoadableFileExtension(const c8* fileName) const
{
	return strstr(fileName, ".bdae") != NULL;
}

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* 
CColladaBinaryFileLoader::createMesh(io::IReadFile* file)
{
	collada::CResFile * dat = collada::CResFileManager::getInst()->load(file);
	collada::CColladaDatabase colladadb(dat);
	ISceneNode* pScene = colladadb.constructScene();
	CSceneNodeAnimator* pAnimator = colladadb.constructAnimator();
	pScene->addAnimator(pAnimator);
	m_pSceneManager->getRootSceneNode()->addChild(pScene);

	return 0;
}

ISceneNode* 
CColladaBinaryFileLoader::createScene(io::IReadFile* file)
{
	return 0;
}

} // namespace scene
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_LOADER_

