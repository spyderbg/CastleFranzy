////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IGPResLib.h"


int IGPResLibHeader::LoadHeader(IGPLZMAFile *pResource, u32 baseOffset)
{
	if (pResource == NULL)
		return -1;

	int filesNumber = pResource->readShort();
	if ((filesNumber <= 0) /*|| (filesNumber >= MAX_FILES_ALLOWED)*/)
		return -2;

	m_offsetsArray = NEW u32[ filesNumber ];

	baseOffset += (2 + (filesNumber << 2));
	for (int i = 0; i < filesNumber; i++)
		m_offsetsArray[i] = pResource->readInt() + baseOffset;

	m_filesNumber = filesNumber /*- 1*/;

	return baseOffset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IGPResLib::IGPResLib()
{
	m_pFile			=	NULL;
	m_nFileOffset	=	0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IGPResLib::~IGPResLib()
{
	IGPLZMAFile::Close( m_pFile );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IGPResLib* IGPResLib::Open(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA )
{
	//Create the lib
	IGPResLib* lib = NEW IGPResLib();
	if ( !lib )
		return NULL;

	//Open the file
	lib->m_pFile = IGPLZMAFile::Open(fileName, i_cache, i_offset, bUseLZMA);
	if (!lib->m_pFile)
	{
		Close( lib );
		return NULL;
	}

	if( lib->m_LibHeader.LoadHeader( lib->m_pFile ) < 0 )
	{
		Close( lib );
		return NULL;
	}

	lib->m_nFileOffset = 0;

	//Return the created lib.
	return lib;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void IGPResLib::Close(IGPResLib*& lib)
{
	SAFE_DELETE( lib );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool IGPResLib::ReadFile( char*& buffer, int nFileID )
{
	if( buffer == NULL )
	{
		buffer = NEW char[ GetFileSize( nFileID ) ];

		if( buffer == NULL )
			return false;
	}

	if( nFileID > m_nFileOffset )
	{
		m_pFile->skip( GetFileOffset( nFileID ) - GetFileOffset( m_nFileOffset ) );
	}
	else if( nFileID < m_nFileOffset )
	{
		//TODO; reset file
	}

	m_pFile->read( buffer, GetFileSize( nFileID ) );
	m_nFileOffset = nFileID + 1;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
