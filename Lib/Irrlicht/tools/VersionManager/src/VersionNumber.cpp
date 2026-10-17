#include "stdafx.h"
#include "VersionNumber.h"
//#include <sstream>

VersionNumber::VersionNumber( int major, int minor, int build, int revision )
: m_major( major )
, m_minor( minor )
, m_build( build )
, m_revision( revision )
{}

VersionNumber VersionNumber::Invalid()
{
	return VersionNumber( INV_MAJOR, INV_MINOR, INV_BUILD, INV_REV );
}

VersionNumber &VersionNumber::operator++()
{
	return Increment();
}

VersionNumber VersionNumber::operator++(int)
{
	VersionNumber num = *this;
	Increment();
	return num;
}

VersionNumber &VersionNumber::Increment()
{
#define INCREMENT_VERNUM_COMPONENT( comp ) \
	if ( bOverflow && ( bOverflow = ( ++(comp) > MAX_VERNUM_COMPONENT ))) \
	{ \
		comp = 0; \
	}

	bool bOverflow = true;

	INCREMENT_VERNUM_COMPONENT( m_revision );
	INCREMENT_VERNUM_COMPONENT( m_build );
	INCREMENT_VERNUM_COMPONENT( m_minor );
	INCREMENT_VERNUM_COMPONENT( m_major );

	return *this;

#undef INCREMENT_VERNUM_COMPONENT
}

bool VersionNumber::operator==(VersionNumber verNum) const
{
	return m_major == verNum.m_major
		&& m_minor == verNum.m_minor
		&& m_build == verNum.m_build
		&& m_revision == verNum.m_revision;
}

bool VersionNumber::operator!=(VersionNumber verNum) const
{
	return m_major != verNum.m_major
		|| m_minor != verNum.m_minor
		|| m_build != verNum.m_build
		|| m_revision != verNum.m_revision;
}

#define COMPARE_VERNUM_COMPONENTS( comp, op, invOp ) \
	if ( comp op verNum.##comp ) \
	{ \
		return true; \
	} \
	else if ( comp invOp verNum.##comp ) \
	{ \
		return false; \
	}

bool VersionNumber::operator>(VersionNumber verNum) const
{
	COMPARE_VERNUM_COMPONENTS( m_major, >, < );
	COMPARE_VERNUM_COMPONENTS( m_minor, >, < );
	COMPARE_VERNUM_COMPONENTS( m_build, >, < );
	COMPARE_VERNUM_COMPONENTS( m_revision, >, < );

	return false;
}

bool VersionNumber::operator<(VersionNumber verNum) const
{
	COMPARE_VERNUM_COMPONENTS( m_major, <, > );
	COMPARE_VERNUM_COMPONENTS( m_minor, <, > );
	COMPARE_VERNUM_COMPONENTS( m_build, <, > );
	COMPARE_VERNUM_COMPONENTS( m_revision, <, > );

	return false;
}

bool VersionNumber::operator>=(VersionNumber verNum) const
{
	COMPARE_VERNUM_COMPONENTS( m_major, >, < );
	COMPARE_VERNUM_COMPONENTS( m_minor, >, < );
	COMPARE_VERNUM_COMPONENTS( m_build, >, < );
	COMPARE_VERNUM_COMPONENTS( m_revision, >, < );

	return true;
}

bool VersionNumber::operator<=(VersionNumber verNum) const
{
	COMPARE_VERNUM_COMPONENTS( m_major, <, > );
	COMPARE_VERNUM_COMPONENTS( m_minor, <, > );
	COMPARE_VERNUM_COMPONENTS( m_build, <, > );
	COMPARE_VERNUM_COMPONENTS( m_revision, <, > );

	return true;
}

#undef COMPARE_VERNUM_COMPONENTS
 
VersionNumber VersionNumber::Get( char *verStr )
{
	return Get( ( const char * )verStr );
}

char VersionNumber::GetSeparator( const char *verStr )
{
	char separator = '\0';
	while ( verStr != NULL && *verStr != '\0' )
	{
		if ( *verStr == '.' || *verStr == ',' )
		{
			separator = *verStr;
		}

		++verStr;
	}

	return separator;
}

VersionNumber VersionNumber::Get( const char *verStr )
{
	VersionNumber verNum;
	int strLength;
	const char *pSeparator;
	const char *pCur;
	char verNumCompStr[6];
	char separator[2];
	int verNumCompStrLen;
	int *verNumCompPointers[] = { &verNum.m_major, &verNum.m_minor, &verNum.m_build, &verNum.m_revision };
	int verNumCompPointerCount = 0;
	
	if ( verStr == NULL )
	{
		return VersionNumber::Invalid();
	}
	
	strLength = strlen( verStr );
	if ( strLength < 1 )
	{
		return VersionNumber::Invalid();
	}

	separator[0] = GetSeparator( verStr );
	separator[1] = '\0';

	if ( separator[0] == '\0' )
	{
		return VersionNumber::Invalid();
	}

	pCur = verStr;
	pSeparator = strstr( pCur, separator );
	while ( verNumCompPointerCount < NUM_VERNUM_COMPONENTS )
	{
		for ( verNumCompStrLen = 0; pCur != pSeparator; verNumCompStrLen++, pCur++ )
		{
			verNumCompStr[verNumCompStrLen] = *pCur;
		}
		verNumCompStr[verNumCompStrLen] = '\0';

		*(verNumCompPointers[verNumCompPointerCount++]) = ( int )strtol( verNumCompStr, NULL, 0 );

		if ( verNumCompPointerCount < NUM_VERNUM_COMPONENTS )
		{
			pCur = pSeparator + 1;
			pSeparator = strstr( pCur, separator );
			pSeparator = pSeparator == NULL ? &verStr[strLength] : pSeparator;
		}
	}

	return verNum;
}

//std::string VersionNumber::ToString() const
//{
//	std::stringstream stream;
//	stream << m_major << '.' << m_minor << '.' << m_build << '.' << m_revision;
//	return stream.str();
//}