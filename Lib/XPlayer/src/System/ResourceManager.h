// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "resfile.h"

#if EXTRA_RES_ENABLED > 0
#define  MAX_RESOURCE_FILE 3
#else
#define  MAX_RESOURCE_FILE 1
#endif

/// @brief This class manage all resource files in game.
/// A single resource file can be added/removed from this resource manager.
/// Resource files downloaded from network can be dynamicly bound to game engine with this class.
class CResourceManager
{
public:
	CResourceManager();
	virtual ~CResourceManager();

	/// @brief Bind a resource file to resource manager.
	/// @attention Don't bind an already bound resource file.
	///	@param[in]	file	pointer to a CResFile object.
	void AddResFile(CResFile *file);

	/// @brief Remove a bound resource file from resource manager.
	///
	/// Nothing happen if the resource file is not bound to the resource manager.
	///	@param[in]	file	pointer to a CResFile object to remove.
	void RemoveResFile(CResFile *file);
	
	/// @brief Finds a resource with a given name in all bound resource files.
	/// @attention The name of the resource is case sensitive.
	///	@param[in]	name	name of the desired resource.
	///	@param[in]	memType	the type of memory to hold created CStream object. Default value is POOL_BOTTOM.
	CStream* GetResourceAsStream(const char *name);

	
	void FindRecord(const char *name, void** pRecord, CResFile** pResFile, int& length);


private:
	CResourceManager& operator=(const CResourceManager& rhs);
	CResFile *m_files[MAX_RESOURCE_FILE];
};
#endif