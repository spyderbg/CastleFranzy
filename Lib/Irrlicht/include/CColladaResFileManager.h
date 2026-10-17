#ifndef __CCOLLADARESFILEMANAGER_H__
#define __CCOLLADARESFILEMANAGER_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include <SDatabaseCollada.h>
#include "IrrlichtDevice.h"
#include "irrMap.h"
#include "irrString.h"
#include "resFile.h"
#include "IReadFile.h"

namespace irr
{
namespace collada
{

class CResFileReader 
	: public irr::res::FileReader
{
public:
	CResFileReader(irr::io::IReadFile *pFile)
		: LocalFileReader(pFile) {}
	
	virtual int read( void* destination, unsigned int size){return LocalFileReader->read(destination,size);}
	virtual long seek(long pos, bool relativeMovement = false){return LocalFileReader->seek(pos,relativeMovement);}
	virtual long getSize(){return LocalFileReader->getSize();}
private:
	irr::io::IReadFile *LocalFileReader;
};

class CResFile
	: public IReferenceCounted
{
protected:
	void releaseTextures();

public:
	CResFile(const char *filename, io::IReadFile *pReader, bool refData = false);

	virtual ~CResFile();
	
	void *getDataPtr() 
	{
		return resFile->data.ptr();
	}

	bool isValid()
	{
		return resFile.isValid();
	}

	io::IReadFile			*memFile;
	core::stringc			absoluteFilename;
	res::File				resFile;
};

//! Return results for CResFileManager::unload
enum E_RESFILE_UNLOAD_RESULT
{
	//! Unloaded successfully.
	ERUR_SUCCESS = 0,

	//! Resource file was still referenced but unloaded as requested.
	ERUR_FORCED,

	//! Failed to unload because there are dangling references.
	ERUR_STILL_REFERENCED,

	//! Resource name not found among loaded resources.
	ERUR_NOT_FOUND
};

class CResFileManager
	: public IReferenceCounted 
{
public:
	CResFileManager(IrrlichtDevice *pFileSystem);
	virtual ~CResFileManager();

	CResFile * get(CResFile *pCaller, const char *pFilename, bool load = true);	
	CResFile * get(const char *pFilename, bool load = true);
	CResFile * load(const char *pFilename, bool threaded = false, 
		void (*FxThreadCallback)(const char *, const collada::SCollada *) = 0);

	E_RESFILE_UNLOAD_RESULT unload(const char *pFilename, bool forced = false);
	E_RESFILE_UNLOAD_RESULT unload(const collada::SCollada *pFilename, bool forced = false);

	//! Unloads all resources only referenced by the manager.
	/*! \return The number of successfully unloaded resources. */
	int unloadAll();

	//! Returns the number of loaded resources.
	int getLoadedCount() const
	{
		return m_pFileMap.size();
	}
	
	CResFile * get(io::IReadFile *pReader, bool load = true, bool refData = false);
	CResFile * load(io::IReadFile *pReader, bool threaded = false, 
		void (*FxThreadCallback)(const char *, const collada::SCollada *) = 0, bool refData = false);

	//! Indicates whether resource files are automatically unloaded when their
	//! reference count drops to 1.
	bool isAutoUnload() const
	{
		return m_bAutoUnload;
	}

	//! Sets whether resource files are automatically unloaded when their
	//! reference count drops to 1.
	void setIsAutoUnload(bool value)
	{
		m_bAutoUnload = value;
	}

	IrrlichtDevice* getDevice() const
	{
		return m_pDevice;
	}

	static CResFileManager *getInst() { return s_Inst; }

protected:

	int postLoadProcess(CResFile* resFile, irr::io::IReadFile* pReader);
	void updateExternalResources(CResFile* resFile, irr::io::IReadFile* pReader);

	E_RESFILE_UNLOAD_RESULT unload(core::map<core::stringc, CResFile*>::Node* n, bool forced = false);

	core::map<core::stringc, CResFile*>		m_pFileMap;
	IrrlichtDevice* 						m_pDevice;
	bool 						 			m_bAutoUnload;
	static CResFileManager* s_Inst;
};

//! Helper for temporarily disabling auto unloading
struct SAutoUnloadOff
{
	bool AutoUnloadValue;

	SAutoUnloadOff()
		: AutoUnloadValue(CResFileManager::getInst()->isAutoUnload())
	{
		CResFileManager::getInst()->setIsAutoUnload(false);
	}

	~SAutoUnloadOff()
	{
		CResFileManager::getInst()->setIsAutoUnload(AutoUnloadValue);
	}
};

};	// namespace collada
};  // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
