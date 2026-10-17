#include "stdafx.h"
#include "VersionManager.h"

VersionManager *VersionManager::s_pVersionManager = NULL;

VersionManager::VersionManager()
{

}

VersionManager::~VersionManager()
{

}

VersionManager *VersionManager::GetInst()
{
	return s_pVersionManager ? s_pVersionManager : ( s_pVersionManager = new VersionManager() );
}

void VersionManager::Release()
{
	SAFE_DELETE( s_pVersionManager );
}

VersionNumber VersionManager::GetVersionNumber( _TCHAR *appFilePath )
{
	DWORD dummyHandle;
	int fileVersionInfoSize = 0;
	void *pBuf = NULL;
	unsigned int len = 0;

	ASSERT( ( fileVersionInfoSize = GetFileVersionInfoSize( appFilePath, &dummyHandle ) ) > 0, return VersionNumber::Invalid() );

	void *pFileVersionInfoData = malloc( fileVersionInfoSize );
	ASSERT( GetFileVersionInfo( appFilePath, dummyHandle, fileVersionInfoSize, pFileVersionInfoData ) > 0, return VersionNumber::Invalid() );

	ASSERT( VerQueryValue( pFileVersionInfoData, _T("\\"), &pBuf, &len ) > 0, return VersionNumber::Invalid() );
	ASSERT( pBuf, return VersionNumber::Invalid() );

	VS_FIXEDFILEINFO *pFileVersionInfo = ( VS_FIXEDFILEINFO * )pBuf;

	VersionNumber verNum( 
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionMS, 16, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionMS, 0, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionLS, 16, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionLS, 0, 0xffff ) 
		);

	
	free( pFileVersionInfoData );
	return verNum;
}

void VersionManager::UpdateVersionNumberDefinitionFile( _TCHAR *versionDefFilePath )
{
	VersionNumber verNum;
	char *wholeFile;
	int fileLength;
	char firstLine[256];
	std::stringstream updatedFileContent;
	FILE *pFile = NULL;

	// read current version file
	pFile = _wfopen( versionDefFilePath, _T("r") );
	if ( !pFile )
	{
		wprintf( _T("Error : Could not read Version Number Definition File \"%s\""), versionDefFilePath );
		return;
	}

	// read whole file
	fseek(pFile, 0, SEEK_END);
	fileLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	wholeFile = new char[fileLength + 1];
	fread(wholeFile, 1, fileLength, pFile);
	fclose(pFile);
	wholeFile[fileLength] = 0;

	// parse the information needed
	int count;
	for ( count = 0; wholeFile[count] != '\r' && wholeFile[count] != '\n' && wholeFile[count] != '\0'; count++ )
	{
		firstLine[count] = wholeFile[count];
	}
	firstLine[count] = '\0';

	SAFE_DELETE_ARRAY( wholeFile );

	// get version number
	char *verString = firstLine + 3;
	verNum = VersionNumber::Get( verString );

	// increment version number
	verNum++;

	// update file content
	updatedFileContent << "// " << verNum.m_major << "." << verNum.m_minor << "." << verNum.m_build << "." << verNum.m_revision << std::endl;
	updatedFileContent << "#define VER_MAJOR " << verNum.m_major << std::endl;
	updatedFileContent << "#define VER_MINOR " << verNum.m_minor << std::endl;
	updatedFileContent << "#define VER_BUILD " << verNum.m_build << std::endl;
	updatedFileContent << "#define VER_REVISION " << verNum.m_revision << std::endl;

	updatedFileContent << std::endl;
	updatedFileContent << "#define FILEVER " << verNum.m_major << "," << verNum.m_minor << "," << verNum.m_build << "," << verNum.m_revision << std::endl;
	updatedFileContent << "#define PRODUCTVER " << verNum.m_major << "," << verNum.m_minor << "," << verNum.m_build << "," << verNum.m_revision << std::endl;
	updatedFileContent << "#define STRFILEVER " << "\"" << verNum.m_major << "," << verNum.m_minor << "," << verNum.m_build << "," << verNum.m_revision << "\"" << std::endl;
	updatedFileContent << "#define STRPRODUCTVER " << "\"" << verNum.m_major << "," << verNum.m_minor << "," << verNum.m_build << "," << verNum.m_revision  << "\"" << std::endl;

	// overwrite version number file
	pFile = _wfopen( versionDefFilePath, _T("w") );
	fputs( updatedFileContent.str().c_str(), pFile );
	fclose( pFile );
}