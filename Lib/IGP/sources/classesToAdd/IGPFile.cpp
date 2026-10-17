// File.cpp: implementation of the IFileReadI class.
////////////////////////////////////////////////////////////////////////////////////////////////////

//#if defined(OS_WIN32)
	#import "IGPFile.h"
//#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void IGPIResourceReadI::Release()
{
	GX_MEMSET(m_ResourceProps, 0, RESOURCE_PROPS_BUFFER);
	m_ResourceOffset	= 0;
	m_ResourceSize		= 0;

	m_BufferStart		= NULL;
	m_BufferEnd			= NULL;
	m_BufferCrt			= NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void IGPIResourceReadI::Close( )
{
	Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef OS_IPHONE
char GLOBAL_FILE_PATH[256];
//#endif
IGPIResourceReadI* IGPIFileReadI::Open(void *in_pResource, int in_cache, int in_nOffset)
{
	IGPIFileReadI* pFile = NEW IGPIFileReadI();

	if (!pFile)
		return NULL;

	char ___tmp[1024];
	memset(___tmp, '\0', 1024);
	strcpy(___tmp, GLOBAL_FILE_PATH);
	strcat(___tmp, (TFileName)in_pResource);
	//printf("%s\n",___tmp);
	pFile->_fileSys = ::fopen( ___tmp, "rb" );


	if( pFile->_fileSys == NULL ) 
	{
		printf("file open err");
		return NULL;
	}
	return pFile;

}
int IGPIFileReadI::GetSize(void *in_pResource, int in_cache, int in_nOffset)
{
	IGPIFileReadI* pFile = NEW IGPIFileReadI();

	if (!pFile)
		return 0;


  //  #if defined( OS_NITRO )

		//{/*
		//	FS_InitFile( &pFile->_fileSys );

		//	bool bSuccess = FS_OpenFile( &pFile->_fileSys, (TFileName)in_pResource );

		//	CheckCardRemoval();

		//	return ( bSuccess ) ? pFile : NULL;;*/
		//}

  //  #elif defined( OS_WIN32 )

		{
			pFile->_fileSys = ::fopen( (TFileName)in_pResource, "rb" );
			FILE*  file = pFile->_fileSys;
			int len = 0;
			if( pFile->_fileSys == NULL ) 
			{
				return 0;
			}
			/*while(eof(file))
			{
				fseek( file, sizeof(unsigned char), SEEK_CUR );
				len++;
			}*/
			fseek(file, 0L, SEEK_END);
			len = ftell(file);
			return len;
		}

 //   #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void IGPIFileReadI::Close( )
{
//#if defined(OS_NITRO)
//
//		FS_CloseFile( &_fileSys );
//
//#elif defined( OS_WIN32 )

		fclose( _fileSys );

//#endif

	IGPIResourceReadI::Close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int IGPIFileReadI::Skip(s32 value)
{
	if (value <= 0)
		return 0;

  //  #if defined( OS_NITRO )

		//GX_VBlankIntr( FALSE );
		//FS_SeekFile( &_fileSys, value, FS_SEEK_CUR );
		//GX_VBlankIntr( TRUE );
		//CheckCardRemoval();

  //  #elif defined( OS_WIN32 )

        fseek( _fileSys, value, SEEK_CUR );
        int off = ftell( _fileSys );
        off = off;

//    #endif
        
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int IGPIFileReadI::Read( void* out_pData, u32 in_nSize )
{	
  //  #if defined( OS_NITRO )

		//GX_VBlankIntr( FALSE );
  //      FS_ReadFile( &_fileSys, out_pData, in_nSize );
		//GX_VBlankIntr( TRUE );

		//CheckCardRemoval();

  //  #elif defined( OS_WIN32 )

        fread( out_pData, 1, in_nSize, _fileSys );

 //   #endif

	return in_nSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IGPIResourceReadI* IGPIBufferReadI::Open(void *in_pResource, int in_cache, int in_nOffset)
{
	IGPIBufferReadI* pFile = NEW IGPIBufferReadI();

	if (!pFile)
		return NULL;

	if (pFile->OpenResource(in_pResource, in_cache, in_nOffset) == NULL)
		SAFE_DELETE(pFile);

	return pFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IGPIResourceReadI* IGPIBufferReadI::OpenResource(void *in_pResource, int in_cache, int in_nOffset)
{
	// the offset is always 0
	// in_cache is the size of the input buffer

	//Open the file
	if (in_pResource == NULL)
		return NULL;

	// keep the name
	GX_MEMSET(m_ResourceProps, 0, RESOURCE_PROPS_BUFFER);

	m_ResourceOffset	= 0;
	m_ResourceSize		= in_cache;

	m_BufferStart		= (u8*)(in_pResource);
	m_BufferCrt			= m_BufferStart;
	m_BufferEnd			= m_BufferStart + in_cache;

	return this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int IGPIBufferReadI::Skip(s32 value )
{
	if (value <= 0)
		return 0;

	int availableBytes = (m_BufferEnd - m_BufferCrt);

	// if cache big enough, ok, otherwise force an update on the first read
	if (value <= availableBytes)
		m_BufferCrt += value;
	else
	{
		value -= availableBytes;
		if (value >= m_ResourceSize)
		{
			// to the end...
			m_ResourceSize = 0;
			m_BufferCrt = m_BufferEnd;
			return 0;
		}

		m_BufferCrt = m_BufferEnd;

		m_ResourceOffset	+= value;
		m_ResourceSize		-= value;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int IGPIBufferReadI::Read(void* out_pData, u32 in_nSize)
{	
	if (in_nSize <= 0)
		return 0;

	u8 *dest = (u8*)out_pData;

	int availableBytes = (m_BufferEnd - m_BufferCrt);
	int readSize = in_nSize;

	if (readSize > availableBytes)
		readSize = availableBytes;

	if (readSize <= 16)
	{
		switch (16 - readSize)
		{
			case  0: *dest++ = *m_BufferCrt++;
			case  1: *dest++ = *m_BufferCrt++;
			case  2: *dest++ = *m_BufferCrt++;
			case  3: *dest++ = *m_BufferCrt++;
			case  4: *dest++ = *m_BufferCrt++;
			case  5: *dest++ = *m_BufferCrt++;
			case  6: *dest++ = *m_BufferCrt++;
			case  7: *dest++ = *m_BufferCrt++;
			case  8: *dest++ = *m_BufferCrt++;
			case  9: *dest++ = *m_BufferCrt++;
			case 10: *dest++ = *m_BufferCrt++;
			case 11: *dest++ = *m_BufferCrt++;
			case 12: *dest++ = *m_BufferCrt++;
			case 13: *dest++ = *m_BufferCrt++;
			case 14: *dest++ = *m_BufferCrt++;
			case 15: *dest++ = *m_BufferCrt++;
		}
	}
	else
	{
		GX_MEMCPY(dest, m_BufferCrt, readSize);
		m_BufferCrt += readSize;
	}

	return readSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
