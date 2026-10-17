///////////////////////////////////////////////////////////////////////
/// StringTable.cpp
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/StringTable.h"
#include "Game/Config/UserProfile.h"

///////////////////////////////////////////////////////////////////////

#define STRING_FOLDER					"Strings/"

#define SWAP_ENDIANS_I16( x )			( x )
#define SWAP_ENDIANS_I32( x )			( x )

///////////////////////////////////////////////////////////////////////

using namespace irr;

///////////////////////////////////////////////////////////////////////
/// STRING_SIDS
///////////////////////////////////////////////////////////////////////
// STRING_SIDS static memeber definition
std::map< stringc, int > STRING_SIDS::s_stringMap;
///////////////////////////////////////////////////////////////////////
/// CStringTable
///////////////////////////////////////////////////////////////////////

CStringTable::CStringTable() :
	m_CurrentLanguage( Language_None ),
	m_Strings( NULL ),
	m_StringsNb( 0 )
{
}
CStringTable::~CStringTable()
{
	FreeLoadedLanguage();
}

///////////////////////////////////////////////////////////////////////

ELanguages CStringTable::GetDefaultLanguage()
{
	ELanguages language = Language_English; //default

	return language;
};

void CStringTable::FreeLoadedLanguage()
{
	SAFE_DEL_ARRAY_ARRAY(m_Strings, m_StringsNb);

	m_CurrentLanguage = Language_None;
}

///////////////////////////////////////////////////////////////////////

core::stringc CStringTable::GetLanguageResString( ELanguages language )
{
	switch( language )
	{
	case Language_English:
		return "FortressDefender_EN.txt";

	case Language_French:
		return "FortressDefender_FR.txt";

	case Language_Spanish:
		return "FortressDefender_ES.txt";

	case Language_Deutsch:
		return "FortressDefender_DE.txt";

	case Language_Italian:
		return "FortressDefender_IT.txt";

	case Language_Japanese:
		return "FortressDefender_JP.txt";

	//case Language_SimplifiedChinese:
	//	return "FortressDefender_SC.txt";
	}

	GX_ASSERT(false); //Language not recognized

	return ""; // will return error when try to load resourse
}

///////////////////////////////////////////////////////////////////////

bool CStringTable::Load( ELanguages language )
{
	irr::io::IFileSystem* fileSystem = __IRR_FILE_SYSTEM();

	core::stringc fileName = STRING_FOLDER;
	fileName = fileName + GetLanguageResString( language );

	/// Language not present
	io::IReadFile* file = fileSystem->createAndOpenFile( fileName.c_str() );
	if( file == NULL )
		return false;

	/// Free old language
	FreeLoadedLanguage();

	/// Load new langugage
	u32 size = file->getSize();
	file->read( &m_StringsNb, 4 );

	m_Strings = (TChar**)(STATIC_NEW TChar*[m_StringsNb]);

	/// Compute strings len
	core::array<s32> strLen;
	u32 curStrLen = 0;
	u32 prevStrLen = 0;

	file->read( &prevStrLen, 4 );

	for( u32 i = 0; i < m_StringsNb; ++i )	
	{
		if (i < m_StringsNb - 1)
			file->read( &curStrLen, 4 );
		else
			curStrLen = size;

		strLen.push_back( (curStrLen - prevStrLen) / 2 );

		prevStrLen = curStrLen;
	}

	/// Load strings
	for( u32 i = 0; i < m_StringsNb; ++i )	
	{
		m_Strings[i] = NEW TChar[strLen[i]];

		for( u32 j = 0; j < strLen[i]; j++ )
		{
			u16 ch;
			file->read( &ch, 2 );

			m_Strings[i][j] = ch;
		}
	}

	file->drop();

	return true;
}

///////////////////////////////////////////////////////////////////////

bool CStringTable::SetLanguage( ELanguages language )
{
	// if the language is not set -> loading english anyway
	if( language == Language_None )
	{
		if( !Load( Language_English ) )
		{
			DBG_ERROR( "Can't change language" );
			return false;
		}
		m_CurrentLanguage = Language_None;
		return true;
	}else
	{
		/// Not needed to change language
		if( m_CurrentLanguage == language )
			return true;

		if( !Load( language ) )
		{
			DBG_ERROR( "Can't change language" );
			return false;
		}
	}
	m_CurrentLanguage = language;

	return true;
}
