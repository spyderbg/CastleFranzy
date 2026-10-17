////////////////////////////////////////////////////////////////////////////////
// See ..\readme.txt for usage
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionManager.h"

#define USAGE "VersionManager MODE FILEPATH"

#define GET_VERSION_OPTION "/g"
#define UPDATE_VERSION_OPTION "/u"

#define GET_VERSION_DESC "Gets version of supplied file."
#define UPDATE_VERSION_DESC "Updates teh supplied Version Number Definition File.  Increments build number by one."

#define GET_VERSION_ID 0
#define UPDATE_VERSION_ID 1

_TCHAR *g_filePath = NULL;
int g_option = -1;

void PrintUsage()
{
	int indent = 0;

#define PRINTFI( ... ){ for ( int curInd = 0; curInd < indent; curInd++ ){ printf( "\t" ); } printf( __VA_ARGS__ ); }

	PRINTFI( USAGE "\n" );
	PRINTFI( "\n" );
	PRINTFI( "MODE : \n" );

	indent++;
	PRINTFI( GET_VERSION_OPTION " : " GET_VERSION_DESC "\n" );
	PRINTFI( UPDATE_VERSION_OPTION " : " UPDATE_VERSION_DESC "\n" );

#undef PRINTFI
}

bool GetArguments( int argc, _TCHAR* argv[] )
{
	if ( argc < 3 )
	{
		return false;
	}

	if ( !_wcsicmp( argv[1], _T(GET_VERSION_OPTION) ) )
	{
		g_option = GET_VERSION_ID;
	}
	else if ( !_wcsicmp( argv[1], _T(UPDATE_VERSION_OPTION) ) )
	{
		g_option = UPDATE_VERSION_ID;
	}
	else
	{
		return false;
	}

	g_filePath = argv[2];
	return true;
}

void GetFileVersion()
{
	VersionNumber verNum = VersionManager::GetInst()->GetVersionNumber( g_filePath );
	printf( "%d.%d.%d.%d", verNum.m_major, verNum.m_minor, verNum.m_build, verNum.m_revision );
}

void UpdateVersionDefinitionFile()
{
	VersionManager::GetInst()->UpdateVersionNumberDefinitionFile( g_filePath );
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( !GetArguments( argc, argv ) )
	{
		PrintUsage();
		return 1;
	}

	switch ( g_option )
	{
	case GET_VERSION_ID:
		{
			GetFileVersion();
			break;
		}
	case UPDATE_VERSION_ID:
		{
			UpdateVersionDefinitionFile();
			break;
		}
	}

	return 0;
}