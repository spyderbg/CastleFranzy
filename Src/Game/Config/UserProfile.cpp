////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////

#include "Core/Application.h"

#include "Game/Config/GameConfig.h"
#include "Game/Config/UserProfile.h"

////////////////////////////////////////////////////////

#define USE_PROFILE_DEBUG_OUT	0

#if USE_PROFILE_DEBUG_OUT == 1 

	#define PROFILE_DBG( ... )												\
			DBG_OUT( "****************** PROFILE ******************\n" );	\
			DBG_OUT( __VA_ARGS__ );											\
			DBG_OUT( "\n" );												\
			DBG_OUT( "*********************************************\n" );	
#else

	#define PROFILE_DBG( ... )					

#endif

#define PROFILES_PATH						"config/Profiles/"
#define PROFILES_EXT						".xml"
#define DEFAULT_PROFILE_FILE_NAME			"default"
#define DEFAULT_PROFILE_NAME				L"Profile"
#define DEFAULT_PROFILE_TYPE				L"ProfileDefault"
#define USER_PROFILE_TYPE					L"ProfileUser"


////////////////////////////////////////////////////////
/// Global methods
////////////////////////////////////////////////////////

stringc GenerateProfilePath( const stringc& fileName )
{
	stringc path( __APP()->getFileSavePath() ); 

#ifdef OS_WIN32
	path += PROFILES_PATH;
#endif

	path += fileName;
	path += PROFILES_EXT;

	return path;
}


////////////////////////////////////////////////////////
/// CUserProfile
////////////////////////////////////////////////////////

CUserProfile::CUserProfile( const stringw& name ) :
	m_Name( name ),
	m_Buffer( NULL )
{
	ResetToDefault();
}

CUserProfile::CUserProfile( const stringw& name, const stringc& fileName ) :
	m_Name( name ),
	m_Buffer( NULL )
{
	ResetToDefault();

	SetFileName( fileName );
}

CUserProfile::CUserProfile( const CUserProfile& profile )
{
	CopyAllFields( profile );
}

CUserProfile::~CUserProfile()
{
//	SAFE_DEL( m_Buffer );
	Save();
}

void CUserProfile::Release()
{
}

////////////////////////////////////////////////////////

CUserProfile& CUserProfile::operator=( CUserProfile& profile )
{
	if( this != &profile )
		CopyAllFields( profile );

	return *this;
}

////////////////////////////////////////////////////////

void CUserProfile::CopyAllFields( const CUserProfile& profile )
{
	m_Name		= profile.GetName();
	m_FileName	= profile.GetFileName();
}

////////////////////////////////////////////////////////

void CUserProfile::DefProfile_Sync( bool overwrite )
{
	PROFILE_DBG( "DefProfile_Sync" );

	CGameConfig* gc = __GAME_CONFIG();

	if( overwrite )
		gc->clear( USER_PROFILE_TYPE, GetName() );

	IAttributes* attrListDef = gc->getAttributes( DEFAULT_PROFILE_TYPE, DEFAULT_PROFILE_NAME );		
	IAttributes* attrList = gc->getAttributes( USER_PROFILE_TYPE, GetName() );	

	if( attrListDef == NULL )
	{
		DBG_ERROR( "Default profile not loaded" );
		return;
	}

	if( attrList != NULL )
	{
		if( attrList->getAttributeAsInt( "_version" ) == attrListDef->getAttributeAsInt( "_version" ) )
		{
			gc->copyAttributes( attrList, attrListDef, overwrite );
			return;
		}

		gc->clear( USER_PROFILE_TYPE, GetName() );
	}

	attrList = __IRR_FILE_SYSTEM()->createEmptyAttributes();
	gc->copyAttributes( attrList, attrListDef, overwrite );
	gc->setAttributes( USER_PROFILE_TYPE, GetName(), attrList );
}

bool CUserProfile::DefProfile_LoadOrCreate()
{
	PROFILE_DBG( "DefProfile_LoadOrCreate" );

	bool ret = true;

	if( !LoadFile( DEFAULT_PROFILE_FILE_NAME ) )
	{
		TAttributesMap attrMap;		
		IAttributes* attrList = __IRR_FILE_SYSTEM()->createEmptyAttributes();

		attrList->addInt( "_version", 1 );

		attrMap[DEFAULT_PROFILE_NAME] = attrList;

		__GAME_CONFIG()->setAttributes( DEFAULT_PROFILE_TYPE, DEFAULT_PROFILE_NAME, attrList );

		ret = DefProfile_Save();
	}

	if( !ret )
		DBG_WARNING( "Could not save default profile !!!" );

	return ret;
}

bool CUserProfile::DefProfile_Save()
{
	PROFILE_DBG( "DefProfile_Save" );

	return Save( true );		
}

bool CUserProfile::DefProfile_AddNewAttribute( const stringc& var )
{
	PROFILE_DBG( "DefProfile_AddNewAttribute" );

	CGameConfig* gc = __GAME_CONFIG();

	/// Load or create default profile
	DefProfile_LoadOrCreate();

	/// Add 
	IAttributes* attrList = gc->getAttributes( DEFAULT_PROFILE_TYPE, DEFAULT_PROFILE_NAME );	

	if( attrList->findAttribute( var.c_str() ) >= 0 )
		return true;

	//attrList->addFloat( var.c_str(), 0.0f );
	attrList->addInt( var.c_str(), 0 );

	/// Save
	return DefProfile_Save();
}

////////////////////////////////////////////////////////

bool CUserProfile::IsAcceptableFileName( const stringc& fileName )
{
	return (fileName.size() > 0) && (fileName != DEFAULT_PROFILE_FILE_NAME);	
}

bool CUserProfile::LoadFile( const stringc& name )
{
	stringc fileName = GenerateProfilePath( name );

	if( __IRR_FILE_SYSTEM()->existFile( fileName ) )
	{
		return __GAME_CONFIG()->importFile( fileName.c_str() );
	}

	return false;
}

bool CUserProfile::Load( const stringc& fileName )
{
	PROFILE_DBG( "Load" );

	CGameConfig* gc = __GAME_CONFIG();

	/// Check name
	if( !IsAcceptableFileName( fileName ) )
	{
		DBG_ERROR( "Invalid profile file name" );
		return false;
	}
	m_FileName = fileName; 

	/// Load or create default profile
	DefProfile_LoadOrCreate();

	/// Load or create current file
	if( !LoadFile( m_FileName ) )
	{
		DBG_WARNING( "Create new user profile file \"%s\" !!!", m_FileName.c_str() );

		IAttributes* attrListDef = gc->getAttributes( DEFAULT_PROFILE_TYPE, DEFAULT_PROFILE_NAME );
		IAttributes* attrList = __IRR_FILE_SYSTEM()->createEmptyAttributes();

		gc->copyAttributes( attrList, attrListDef, true );
		gc->setAttributes( USER_PROFILE_TYPE, GetName(), attrList );

		TAttributesMap& attrMap = gc->getAttributesMap( USER_PROFILE_TYPE );

		if( !gc->exportObjects( GenerateProfilePath( m_FileName ).c_str(), attrMap, USER_PROFILE_TYPE ) )
		{
			DBG_WARNING( "Could not save %s profile file !!!", m_FileName.c_str() );
		}
	}
	else
	{
		DefProfile_Sync( false );
	}

	return true;
}

bool CUserProfile::Save( bool defaultProfile )
{
	PROFILE_DBG( "Save" );

	CGameConfig* gc = __GAME_CONFIG();

	stringc fileName;
	stringw profileType;
	stringw profileName;

	if( defaultProfile )
	{
		fileName = GenerateProfilePath( DEFAULT_PROFILE_FILE_NAME );
		profileType = DEFAULT_PROFILE_TYPE;
		profileName = DEFAULT_PROFILE_NAME;
	}
	else
	{
		if( IsAcceptableFileName( m_FileName ) )
		{
			fileName = GenerateProfilePath( m_FileName );
			profileType = USER_PROFILE_TYPE;
			profileName = GetName();
		}
		else
		{
			DBG_ERROR( "Could not save file: \"%s\"", m_FileName );
			return false;
		}
	}
		
	IAttributes* srcAttr = gc->getAttributes( profileType, profileName );
	if( srcAttr == NULL )
		return false;
	
	TAttributesMap attrMap;
	gc->importObjects( fileName.c_str(), attrMap, profileType.c_str() );

	TAttributesMap::Node* node = NULL;
	if( !attrMap.isEmpty() && ( (node = attrMap.find( profileName.c_str())) != NULL ) )
	{
		IAttributes* dstAttr = node->getValue();
		gc->copyAttributes( dstAttr, srcAttr, true );
	}
	else
	{
		// The temporary export map drops its entries; retain the config-owned value.
		srcAttr->grab();
		attrMap[profileName] = srcAttr;
	}

	bool ret = gc->exportObjects( fileName.c_str(), attrMap, profileType.c_str() );

	/// Cleanup
	for (TAttributesMap::Iterator it = attrMap.getIterator(); !it.atEnd(); it++)
	{
		it->getValue()->drop();
	}

	return ret;
}

////////////////////////////////////////////////////////

void CUserProfile::ResetToDefault()
{
	PROFILE_DBG( "ResetToDefault" );

	DefProfile_LoadOrCreate();

	DefProfile_Sync( true );
}

////////////////////////////////////////////////////////

const stringc& CUserProfile::GetFileName() const
{
	return m_FileName;
}

const stringw& CUserProfile::GetName() const
{
	return m_Name;
}

////////////////////////////////////////////////////////

void CUserProfile::SetFileName( const stringc& file_name )
{
	m_FileName = file_name;

	Load( m_FileName );
}

////////////////////////////////////////////////////////

const bool CUserProfile::IsActive() const
{
	return true;
}

void CUserProfile::SetAsActive() const
{
}

////////////////////////////////////////////////////////

bool CUserProfile::profileField( stringc var, s32 & f, bool set )
{
	CGameConfig* gc = __GAME_CONFIG();

	/// Try to load from current user
	IAttributes* attr = gc->getAttributes( USER_PROFILE_TYPE, GetName() );
	if( attr == NULL )
	{
		DBG_ERROR( "User \"%s\" profile not loaded.", m_Name.c_str() );

		return false;
	}

	/// Update var
	s32 idx = attr->findAttribute( var.c_str() );
	if( idx >= 0 )
	{
		if( set )
			attr->setAttribute( idx, f );
		else
			f = attr->getAttributeAsInt( idx );

		return true;
	}
	else
	{
		DefProfile_AddNewAttribute( var );

		DefProfile_Sync( false );

		idx = attr->findAttribute( var.c_str() );
		if( idx >= 0 )
		{
			if( set )
				attr->setAttribute( idx, f );
			else
				f = attr->getAttributeAsInt( idx );

			return true;			
		}
		else
		{
			GX_ASSERT( !"Var not added" );
		}
	}

	return false;
}

bool CUserProfile::profileField( stringc var, f32 & f, bool set )
{
	CGameConfig* gc = __GAME_CONFIG();

	/// Try to load from current user
	IAttributes* attr = gc->getAttributes( USER_PROFILE_TYPE, GetName() );
	if( attr == NULL )
	{
		DBG_ERROR( "User \"%s\" profile not loaded.", m_Name.c_str() );

		return false;
	}

	/// Update var
	s32 idx = attr->findAttribute( var.c_str() );
	if( idx >= 0 )
	{
		if( set )
			attr->setAttribute( idx, f );
		else
			f = attr->getAttributeAsFloat( idx );

		return true;
	}
	else
	{
		DefProfile_AddNewAttribute( var );

		DefProfile_Sync( false );

		idx = attr->findAttribute( var.c_str() );
		if( idx >= 0 )
		{
			if( set )
				attr->setAttribute( idx, f );
			else
				f = attr->getAttributeAsFloat( idx );

			return true;			
		}
		else
		{
			GX_ASSERT( !"Var not added" );
		}
	}

	return false;
}

bool CUserProfile::profileField( stringc var, stringc & f, bool set )
{
	CGameConfig* gc = __GAME_CONFIG();

	/// Try to load from current user
	IAttributes* attr = gc->getAttributes( USER_PROFILE_TYPE, GetName() );
	if( attr == NULL )
	{
		DBG_ERROR( "User \"%s\" profile not loaded.", m_Name.c_str() );

		return false;
	}

	/// Update var
	s32 idx = attr->findAttribute( var.c_str() );
	if( idx >= 0 )
	{
		if( set )
			attr->setAttribute( idx, f.c_str() );
		else
			f = attr->getAttributeAsString( idx );

		return true;
	}
	else
	{
		DefProfile_AddNewAttribute( var );

		DefProfile_Sync( false );

		idx = attr->findAttribute( var.c_str() );
		if( idx >= 0 )
		{
			if( set )
				attr->setAttribute( idx, f.c_str() );
			else
				f = attr->getAttributeAsString( idx );

			return true;			
		}
		else
		{
			GX_ASSERT( !"Var not added" );
		}
	}

	return false;
}

////////////////////////////////////////////////////////
/// CUserProfileManager
////////////////////////////////////////////////////////

CUserProfileManager::CUserProfileManager()
{
	m_ActiveProfileName = stringw( L"" );
}

CUserProfileManager::~CUserProfileManager()
{
}

////////////////////////////////////////////////////////

void CUserProfileManager::Init()
{
#if defined(OS_IPHONE) || defined(OS_LINUX)

	/// Copy default profile file from resource folder to profile folder
	IFileSystem * fs = __IRR_FILE_SYSTEM();

	stringc defaultFilePath = GenerateProfilePath( DEFAULT_PROFILE_FILE_NAME );
//	if( !fs->existFile( defaultFilePath ) )
	{
		IWriteFile* fileOut = fs->createAndWriteFile( defaultFilePath.c_str() );
		if( fileOut == NULL )
		{
			DBG_ERROR( "Default save file \"%s\" not created.", defaultFilePath.c_str() );
		}
		else
		{
			stringc resProfilePath;
			resProfilePath += PROFILES_PATH;
			resProfilePath += DEFAULT_PROFILE_FILE_NAME;
			resProfilePath += PROFILES_EXT;

			IReadFile* fileIn = fs->createAndOpenFile( resProfilePath.c_str() );
			GX_ASSERT( fileIn != NULL );

			s32 fileSize = fileIn->getSize();
			u8* buffer = NEW u8[fileSize];
			fileIn->read( buffer, fileSize );
			fileOut->write( buffer, fileSize );

			SAFE_DEL_ARRAY( buffer );
			
			fileIn->drop();
			fileOut->drop();
		}
	}

#endif
}

////////////////////////////////////////////////////////

bool CUserProfileManager::IsActiveProfileNameValid()
{
	return true;
}

////////////////////////////////////////////////////////
// 
// Get already exist profile or create new and set it to default
//
const CUserProfile CUserProfileManager::GetCreateProfile( const stringw& name )
{
	GX_ASSERT( name.size() > 0 );

	CUserProfile profile( name );

	return profile;
}

const CUserProfile CUserProfileManager::GetCreateProfile( const stringw& name, const stringc& fileName )
{
	GX_ASSERT( name.size() > 0 );
	GX_ASSERT( fileName.size() > 0 );

	CUserProfile profile( name, fileName );

	return profile;
}

////////////////////////////////////////////////////////