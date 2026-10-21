#include "stdafx.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaResFileManager.h"
#include "CColladaDatabase.h"
#include "irrTypes.h"
#include "IReadFile.h"
#include "IFileSystem.h"
#include "IrrlichtDevice.h"
#include "IVideoDriver.h"
#include "CMemoryReadFile.h"
#include "irros.h"
#include "ILogger.h"

//#define TEMP_HACK_FOR_DEMO

namespace irr
{
namespace collada
{
//******************
// class CResFile	
//******************
CResFile::CResFile(const char *filename, io::IReadFile *pFile, bool refData)
	: memFile(0)
	, absoluteFilename(filename)
	, resFile(0)
{
	if (refData && pFile->isAllInMemory()) {
		// Increase the ref count on the memory file and don't copy the data
		memFile = pFile;
		memFile->grab();
		resFile.Init(memFile->getBuffer(0));
	}
	else 
	{
		CResFileReader fileReader(pFile);
		resFile.Init( &fileReader );
	}
}

CResFile::~CResFile()
{
	releaseTextures();
	if (memFile) {
		memFile->drop();
		memFile = 0;
	} else {
		delete[] reinterpret_cast<char*>(resFile.ptr());
	}
}

void
CResFile::releaseTextures()
{
	if (!isValid()) return;
	collada::SCollada &refCollada = *(collada::SCollada *)getDataPtr();
	collada::CColladaDatabase colladaDB(this);

	int iImagesCount = refCollada.libraryImages.images.size();

	for(int i = 0; i < iImagesCount; i++)
	{
		collada::SImage &image = refCollada.libraryImages.images[i];
		if(image.type == collada::SImage::CIT_SINGLE)
		{
			if(image.pTexture)
			{
				image.pTexture->drop();
				if (CResFileManager::getInst()->isAutoUnload()
					&& image.pTexture->getReferenceCount() == 1)
				{
					CResFileManager::getInst()->getDevice()->getVideoDriver()->removeTexture(image.pTexture);
				}
				//image.pTexture = 0;
			}
		}
		else 
		{
			_IRR_DEBUG_BREAK_IF(image.type != collada::SImage::CIT_IFL);
			collada::SImageIFL &imageIFL = *image.pIFL;
			int iCount = imageIFL.getCount();
			for(int i = 0; i < iCount; i++)
			{
				if(imageIFL.getTexture(i))
				{
					imageIFL.getTexture(i)->drop();
					if (CResFileManager::getInst()->isAutoUnload()
						&& imageIFL.getTexture(i)->getReferenceCount() == 1)
					{
						CResFileManager::getInst()->getDevice()->getVideoDriver()->removeTexture(imageIFL.getTexture(i));
					}
					//imageIFL.getTexture(i) = 0;
				}
			}
		}
	}
}

//******************
// class CResFileManager	
//******************
CResFileManager * CResFileManager::s_Inst = 0;

CResFileManager::CResFileManager(IrrlichtDevice *pDevice)
  : m_pDevice(pDevice)
  , m_bAutoUnload(true)
{
	_IRR_DEBUG_BREAK_IF(m_pDevice == 0);
#ifndef TEMP_HACK_FOR_DEMO
	_IRR_DEBUG_BREAK_IF(s_Inst != 0);
#endif
	s_Inst = this;
}

CResFileManager::~CResFileManager()
{
	core::map<core::stringc, CResFile *>::ParentLastIterator it = m_pFileMap.getParentLastIterator();
	for(; !it.atEnd(); it++)
	{
		it->getValue()->drop();
	}
}


void
CResFileManager::updateExternalResources(CResFile* resFile, irr::io::IReadFile* pReader)
{
	collada::SCollada &refCollada = *(collada::SCollada *)resFile->getDataPtr();

	int iImagesCount = refCollada.libraryImages.images.size();
	core::stringc fileDir;
	fileDir = m_pDevice->getFileSystem()->getFileDir(resFile->absoluteFilename);

	for(int i = 0; i < iImagesCount; i++)
	{
		collada::SImage &image = refCollada.libraryImages.images[i];
		if(image.type == collada::SImage::CIT_SINGLE)
		{
			irr::ELOG_LEVEL ll = os::Printer::getLogLevel();
			irr::os::Printer::setLogLevel(irr::ELL_ERROR);
			image.pTexture = m_pDevice->getVideoDriver()->getTexture(fileDir + "/" + image.path.ptr.ptr());
			if(image.pTexture == 0)
				image.pTexture = m_pDevice->getVideoDriver()->getTexture(image.path.ptr);
			irr::os::Printer::setLogLevel(ll);
			if(image.pTexture == 0 && pReader)
			{
				long size;
				void* buffer = pReader->getExternalBuffer(image.path.ptr, &size);
				if(buffer) {
					core::stringc fullpath = fileDir + "/" + image.path.ptr.ptr();
					irr::io::CMemoryReadFile mrf(buffer, size, fullpath.c_str(), false);
					image.pTexture = m_pDevice->getVideoDriver()->getTexture(&mrf);
				}
			}
			if(image.pTexture)
				image.pTexture->grab();
		}
		else 
		{
			_IRR_DEBUG_BREAK_IF(image.type != collada::SImage::CIT_IFL);
			collada::SImageIFL &imageIFL = *image.pIFL;

			int iCount = imageIFL.getCount();
			for(int i = 0; i < iCount; i++)
			{
				const char* filename = imageIFL.getFilename(i).ptr;

				irr::ELOG_LEVEL ll = os::Printer::getLogLevel();
				irr::os::Printer::setLogLevel(irr::ELL_ERROR);
				irr::video::ITexture* tex = m_pDevice->getVideoDriver()->getTexture(fileDir + "/" + filename);
				if(tex == 0)
					tex = m_pDevice->getVideoDriver()->getTexture(filename);
				irr::os::Printer::setLogLevel(ll);
				
				if(tex == 0 && pReader) {
					long size;
					void* buffer = pReader->getExternalBuffer(filename, &size);
					if(buffer) {
						core::stringc fullpath = fileDir + "/" + filename;
						irr::io::CMemoryReadFile mrf(buffer, size, fullpath.c_str(), false);
						tex = m_pDevice->getVideoDriver()->getTexture(&mrf);
					}
				}

				imageIFL.getTexture(i) =  tex;
				_IRR_DEBUG_BREAK_IF(imageIFL.getTexture(i) == 0);
				if(imageIFL.getTexture(i))
				{
					imageIFL.getTexture(i)->grab();
				}
			}
		}
	}
}

int 
CResFileManager::postLoadProcess(CResFile* resFile, irr::io::IReadFile* pReader)
{
	collada::SCollada &refCollada = *(collada::SCollada *)resFile->getDataPtr();
	collada::CColladaDatabase colladaDB(resFile);

	if (refCollada.configurations.bPostLoadingDone)
	{
		updateExternalResources(resFile, pReader);
		return 0;
	}

	if(strcmp(colladaDB.getVersion(), STRFILEVER))
	{
		os::Printer::log("- Error - Collada binary version check failed -----", ELL_WARNING);
		os::Printer::log("- Expected versions : %s", STRFILEVER, ELL_WARNING);
		os::Printer::log(resFile->absoluteFilename.c_str(), ELL_WARNING);
		os::Printer::log(colladaDB.getVersion(), ELL_WARNING);
		if(CColladaDatabase::getVersionCheckBehavior() == CColladaDatabase::EVCB_DONT_LOAD)
		{
			os::Printer::log("- The file won't be loaded							 -", ELL_ERROR);
			os::Printer::log("- see CColladaDatabase::setVersionCheckBehavior(...) -", ELL_ERROR);
			os::Printer::log("------------------------------------------------------", ELL_ERROR);
			return -1;
		}
		else
		{
			os::Printer::log("- The file will be loaded							 -", ELL_WARNING);
			os::Printer::log("- see CColladaDatabase::setVersionCheckBehavior(...) -", ELL_WARNING);
			os::Printer::log("------------------------------------------------------", ELL_WARNING);
		}
	}

	for(int iAnimation = 0; iAnimation < colladaDB.getAnimationCount(); iAnimation++)
	{
		collada::SAnimation *pAnim = colladaDB.getAnimation(iAnimation);

		pAnim->pAnimator = (void *)colladaDB.getAnimationTrackEx(pAnim);

		for(int iSampler = 0; iSampler < pAnim->samplers.size(); iSampler++)
		{
			collada::SSampler &sampler = pAnim->samplers[iSampler];
			
			_IRR_DEBUG_BREAK_IF(sampler.iInput == -1);
			sampler.pInput  = &pAnim->sources[sampler.iInput];
			
			_IRR_DEBUG_BREAK_IF(sampler.iOutput == -1);
			sampler.pOutput  = &pAnim->sources[sampler.iOutput];
			
			if(sampler.iInTangent != -1)
			{
				sampler.pInTangent  = &pAnim->sources[sampler.iInTangent];
			}
			else
			{
				sampler.pInTangent = 0;
			}

			if(sampler.iOutTangent != -1)
			{
				sampler.pOutTangent  = &pAnim->sources[sampler.iOutTangent];
			}
			else
			{
				sampler.pOutTangent = 0;
			}

		}
	}


	int iImagesCount = refCollada.libraryImages.images.size();
	core::stringc fileDir;
	fileDir = m_pDevice->getFileSystem()->getFileDir(resFile->absoluteFilename);

	for(int i = 0; i < iImagesCount; i++)
	{
		collada::SImage &image = refCollada.libraryImages.images[i];
		if(image.type == collada::SImage::CIT_SINGLE)
		{
			_IRR_DEBUG_BREAK_IF(image.pTexture != 0);
			irr::ELOG_LEVEL ll = os::Printer::getLogLevel();
			irr::os::Printer::setLogLevel(irr::ELL_ERROR);
			image.pTexture = m_pDevice->getVideoDriver()->getTexture(fileDir + "/" + image.path.ptr.ptr());
			if(image.pTexture == 0)
				image.pTexture = m_pDevice->getVideoDriver()->getTexture(image.path.ptr);
			irr::os::Printer::setLogLevel(ll);
			//_IRR_DEBUG_BREAK_IF(image.pTexture == 0);
			if(image.pTexture == 0 && pReader)
			{
				long size;
				void* buffer = pReader->getExternalBuffer(image.path.ptr, &size);
				if(buffer) {
					core::stringc fullpath = fileDir + "/" + image.path.ptr.ptr();
					irr::io::CMemoryReadFile mrf(buffer, size, fullpath.c_str(), false);
					image.pTexture = m_pDevice->getVideoDriver()->getTexture(&mrf);
				}
			}

			if(image.pTexture)
				image.pTexture->grab();
		}
		else 
		{
			_IRR_DEBUG_BREAK_IF(image.type != collada::SImage::CIT_IFL);
			collada::SImageIFL &imageIFL = *image.pIFL;

			int iCount = imageIFL.getCount();
			imageIFL.animation.samplers[0].pInput = &imageIFL.animation.sources[imageIFL.animation.samplers[0].iInput];
			imageIFL.animation.samplers[0].pOutput = &imageIFL.animation.sources[imageIFL.animation.samplers[0].iOutput];

			for(int i = 0; i < iCount; i++)
			{
				//_IRR_DEBUG_BREAK_IF(imageIFL.getTexture(i) != 0);
				const char* filename = imageIFL.getFilename(i).ptr;

				irr::ELOG_LEVEL ll = os::Printer::getLogLevel();
				irr::os::Printer::setLogLevel(irr::ELL_ERROR);
				irr::video::ITexture* tex = m_pDevice->getVideoDriver()->getTexture(fileDir + "/" + filename);
				if(tex == 0)
					tex = m_pDevice->getVideoDriver()->getTexture(filename);
				irr::os::Printer::setLogLevel(ll);
				
				if(tex == 0 && pReader) {
					long size;
					void* buffer = pReader->getExternalBuffer(filename, &size);
					if(buffer) {
						core::stringc fullpath = fileDir + "/" + filename;
						irr::io::CMemoryReadFile mrf(buffer, size, fullpath.c_str(), false);
						tex = m_pDevice->getVideoDriver()->getTexture(&mrf);
					}
				}

				imageIFL.getTexture(i) =  tex;
				_IRR_DEBUG_BREAK_IF(imageIFL.getTexture(i) == 0);
				if(imageIFL.getTexture(i))
				{
					imageIFL.getTexture(i)->grab();
				}
			}
		}
	}

	int iMaterialCount = refCollada.libraryMaterials.materials.size();
	for(int materialId = 0; materialId < iMaterialCount; materialId++)
	{
		collada::SMaterial &material = refCollada.libraryMaterials.materials[materialId];
		//_IRR_DEBUG_BREAK_IF(material.instanceEffect.pEffect != 0);
		material.instanceEffect.pEffect = &refCollada.libraryEffects.effects[material.instanceEffect.effectId];
		if(material.iDiffuseSurface != -1)
		{
			material.pDiffuseSurface = &refCollada.libraryImages.images[material.iDiffuseSurface];
		}
		else
		{
			material.pDiffuseSurface = 0;
		}

		if(material.iEnvmapSurface != -1)
		{
			material.pEnvmapSurface = &refCollada.libraryImages.images[material.iEnvmapSurface];
		}
		else
		{
			material.pEnvmapSurface = 0;
		}
		_IRR_DEBUG_BREAK_IF(material.instanceEffect.pEffect == 0);
	}

	int iEffectCount = refCollada.libraryEffects.effects.size();
	for(int effectId = 0; effectId < iEffectCount; effectId++)
	{
		collada::SEffect &effect = refCollada.libraryEffects.effects[effectId];

		// Resolve sampler ptr
		for(int iSamplerId = 0; iSamplerId < effect.samplers.size(); iSamplerId++)
		{
			effect.samplers[iSamplerId].pSurface = &effect.surfaces[effect.samplers[iSamplerId].surface];
		}

		// Resolve surface image
		for(int iSurfacerId = 0; iSurfacerId < effect.surfaces.size(); iSurfacerId++)
		{
			effect.surfaces[iSurfacerId].pImage = &refCollada.libraryImages.images[effect.surfaces[iSurfacerId].image];
		}

		// Resolve texture sampler
		if(effect.ambient.eType == SCommonColorOrTexture::ETEXTURE)
		{
			for(int i = 0; i < effect.ambient.pTexture->size(); i++)
			{
				STexture &texture = (*effect.ambient.pTexture)[i];
				texture.sampler = &effect.samplers[texture.texture];
			}
		}

		// Resolve texture sampler
		if(effect.diffuse.eType == SCommonColorOrTexture::ETEXTURE)
		{
			for(int i = 0; i < effect.diffuse.pTexture->size(); i++)
			{
				STexture &texture = (*effect.diffuse.pTexture)[i];
				texture.sampler = &effect.samplers[texture.texture];
			}
		}

		// Resolve texture sampler
		if(effect.reflective.eType == SCommonColorOrTexture::ETEXTURE)
		{
			for(int i = 0; i < effect.reflective.pTexture->size(); i++)
			{
				STexture &texture = (*effect.reflective.pTexture)[i];
				texture.sampler = &effect.samplers[texture.texture];
			}
		}

		// Resolve texture sampler
		if(effect.specular.eType == SCommonColorOrTexture::ETEXTURE)
		{
			for(int i = 0; i < effect.specular.pTexture->size(); i++)
			{
				STexture &texture = (*effect.specular.pTexture)[i];
				texture.sampler = &effect.samplers[texture.texture];
			}
		}

		// Resolve texture sampler
		if(effect.transparent.eType == SCommonColorOrTexture::ETEXTURE)
		{
			for(int i = 0; i < effect.transparent.pTexture->size(); i++)
			{
				STexture &texture = (*effect.transparent.pTexture)[i];
				texture.sampler = &effect.samplers[texture.texture];
			}
		}
	}

	int iControllerCount = colladaDB.getControllerCount();
	for(int controllerId = 0; controllerId < iControllerCount; controllerId++)
	{
		collada::SController *pController = colladaDB.getController(controllerId);
		if(pController->type == SController::EMORPH)
		{
			SMorph *pMorph = pController->pMorph;
			int iTargets = pMorph->iTargets.size();
			for(int i = 0; i < iTargets; i++)
			{
				pMorph->pTargets[i] = colladaDB.getGeometry(pMorph->iTargets[i]);
			}
		}
	}

	//Post load process done, flag it
	refCollada.configurations.bPostLoadingDone = 1;

	return 0;
}

CResFile * 
CResFileManager::get(CResFile *pCaller, const char *pFilename, bool load)
{
	core::stringc oldDir = m_pDevice->getFileSystem()->getWorkingDirectory();
	core::stringc newdir = m_pDevice->getFileSystem()->getFileDir(pCaller->absoluteFilename);
	m_pDevice->getFileSystem()->changeWorkingDirectoryTo(newdir.c_str());

	CResFile *ret = get(pFilename, load);
	m_pDevice->getFileSystem()->changeWorkingDirectoryTo(oldDir.c_str());

	return ret;
}

CResFile * 
CResFileManager::get(const char *pFilename, bool load)
{
	SAutoUnloadOff autoUnloadOff;
	CResFile *pResFile = 0;

	core::stringc absPath = m_pDevice->getFileSystem()->getAbsolutePath(pFilename);

	if(!m_pFileMap.find(absPath.c_str()))
	{
		if(load)
		{
			// Get File Accessor
			io::IReadFile *pFile = m_pDevice->getFileSystem()->createAndOpenFile(pFilename);

			if(!pFile)
			{
				os::Printer::print("- Error - File not found   -");
				os::Printer::print(pFilename);
				os::Printer::print("----------------------------");
				return 0;
			}
			
			// Allocate the res::File
			pResFile = irrnew CResFile(absPath.c_str(), pFile);
			// Free the accessor
			pFile->drop();
			if (pResFile && !pResFile->isValid()) { pResFile->drop(); return 0; }
			if (pResFile)
			{
				// Store the res::File
				m_pFileMap[absPath.c_str()] = pResFile;
				// Post load process
				if(postLoadProcess(pResFile, 0))
				{
					unload(absPath.c_str());
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		pResFile = m_pFileMap[absPath.c_str()];
	}

	return pResFile;
}

CResFile * 
CResFileManager::get(io::IReadFile *pReader, bool load, bool refData)
{
	SAutoUnloadOff autoUnloadOff;
	CResFile *pResFile = 0;

	//core::stringc absPath = m_pDevice->getFileSystem()->getAbsolutePath(pReader->getFileName());
	core::stringc absPath = pReader->getFileName();

	if(!m_pFileMap.find(absPath))
	{
		if(load)
		{
			// Allocate the res::File
			pResFile = irrnew CResFile(absPath.c_str(), pReader, refData);
			if (!pResFile->isValid()) { pResFile->drop(); return 0; }
			// Store the res::File
			m_pFileMap[absPath.c_str()] = pResFile;
			// Post load process
			if(postLoadProcess(pResFile, pReader))
			{
				// Unload has to be done
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		pResFile = m_pFileMap[absPath];
	}
	return pResFile;
}

CResFile * 
CResFileManager::load(const char *pFilename, bool threaded, 
	void (*FxThreadCallback)(const char *, const collada::SCollada *))
{
	if(threaded)
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
		return 0;
	}
	else
	{
		return get(pFilename);
	}
}

CResFile * 
CResFileManager::load(io::IReadFile *pReader, bool threaded, 
	void (*FxThreadCallback)(const char *, const collada::SCollada *), bool refData)
{
	if(threaded)
	{
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
		return 0;
	}
	else
	{
		return get(pReader, true, refData);
	}
}

E_RESFILE_UNLOAD_RESULT
CResFileManager::unload(const char *pFilename, bool forced)
{
	return unload(
		m_pFileMap.find(m_pDevice->getFileSystem()->getAbsolutePath(pFilename)),
		forced
	);
}

E_RESFILE_UNLOAD_RESULT
CResFileManager::unload(const collada::SCollada *pFilename, bool forced)
{
	// Seems like the only way to find the corresponding entry in the file map
	// is to linearly search it all
	for (core::map<core::stringc, CResFile*>::Iterator it = m_pFileMap.getIterator();
		 !it.atEnd();
		 it++)
	{
		if (it->getValue()->getDataPtr() == (void*)pFilename)
		{
			return unload(&(*it), forced);
		}
	}
	return ERUR_NOT_FOUND;
}

E_RESFILE_UNLOAD_RESULT
CResFileManager::unload(core::map<core::stringc, CResFile*>::Node* n, bool forced)
{
	if (n)
	{
		CResFile* resFile = n->getValue();
		E_RESFILE_UNLOAD_RESULT result = ERUR_SUCCESS;
		if (resFile->getReferenceCount() > 1)
		{
			if (forced)
			{
				result = ERUR_FORCED;
			}
			else
			{
				return ERUR_STILL_REFERENCED;
			}
		}
		resFile->drop();
		m_pFileMap.remove(n);
		return result;
	}
	return ERUR_NOT_FOUND;
}

int
CResFileManager::unloadAll()
{
	int successCount = 0;
	for (core::map<core::stringc, CResFile*>::Iterator it = m_pFileMap.getIterator();
		 !it.atEnd();
		 it++)
	{
		if (unload(it.getNode()) == ERUR_SUCCESS)
		{
			++successCount;
		}
	}
	return successCount;
}

}; // namespace collada
}; // namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
