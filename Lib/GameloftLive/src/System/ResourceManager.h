// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "GLLive_ResFile.h"

#if EXTRA_RES_ENABLED > 0
#define  MAX_RESOURCE_FILE 3
#else
#define  MAX_RESOURCE_FILE 2
#endif

namespace gllive
{
	/// @brief This class manage all resource files in game.
	/// A single resource file can be added/removed from this resource manager.
	/// Resource files downloaded from network can be dynamicly bound to game engine with this class.
	class CGLLiveResourceManager
	{
	public:
		CGLLiveResourceManager();
		virtual ~CGLLiveResourceManager();

		/// @brief Bind a resource file to resource manager.
		/// @attention Don't bind an already bound resource file.
		///	@param[in]	file	pointer to a CGLLiveResFile object.
		void AddResFile(CGLLiveResFile *file);

		/// @brief Remove a bound resource file from resource manager.
		///
		/// Nothing happen if the resource file is not bound to the resource manager.
		///	@param[in]	file	pointer to a CGLLiveResFile object to remove.
		void RemoveResFile(CGLLiveResFile *file);
		
		/// @brief Finds a resource with a given name in all bound resource files.
		/// @attention The name of the resource is case sensitive.
		///	@param[in]	name	name of the desired resource.
		///	@param[in]	memType	the type of memory to hold created CGLLiveStream object. Default value is POOL_BOTTOM.
		CGLLiveStream* GetResourceAsStream(const char *name);

		
		void FindRecord(const char *name, void** pRecord, CGLLiveResFile** pResFile, int& length);


	private:
		CGLLiveResourceManager& operator=(const CGLLiveResourceManager& rhs);
		CGLLiveResFile *m_files[MAX_RESOURCE_FILE];
	};
}
#endif