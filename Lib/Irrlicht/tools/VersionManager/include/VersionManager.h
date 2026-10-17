#pragma once

#include "VersionNumber.h"

class VersionManager
{
private:
	static VersionManager *s_pVersionManager;
	VersionManager();
	~VersionManager();
public:
	static VersionManager *GetInst();
	static void Release();

	VersionNumber GetVersionNumber( _TCHAR *appFilePath );
	void VersionManager::UpdateVersionNumberDefinitionFile( _TCHAR *versionDefFilePath );
};