// Author: Zhong Yan

#include "stdafx.h"
#include "ResFile.h"
#include "Stream.h"

#if(PLATFORM == PLATFORM_MAC_OS_X)&&(!defined(WIN32))
#include <ctype.h>
void strlwr(char * str)
{
	while((*str) != '\0')
	{
		(*str) = tolower(*str);
		str++;
	}
}
#endif
//#include "LzmaDecode.h"
#if PLATFORM != PLATFORM_KDDI
#include "zlib.h"
#endif

CResFile::CResFile()
	:m_recordNumber(0),
	m_records(NULL),
	m_file(NULL)
{
}

CResFile::~CResFile()
{
	Close();
}

bool CResFile::Open(const char *pName)
{
	CFileStream *file = NEW CFileStream();
	
	if(!file->Open(pName, CRawFile::MODE_READ))
	{
		SAFE_DELETE(file);
		return false;
	}
	
	m_file = file;

	//verify magic number
	//if (m_file->ReadIntLE() != 0x4b504c47)
	//{
	//	SAFE_DELETE(m_file);
	//	return false;
	//}

	m_recordNumber = m_file->ReadIntLE();

	m_compressMode = m_file->ReadIntLE();
	
	if(m_file->GetLength() - m_file->GetPosition() < (m_recordNumber + 1) * (int)sizeof(Record))
	{
		SAFE_DELETE(m_file);
		m_recordNumber = 0;
		return false;
	}

	m_records = NEW Record[m_recordNumber + 1];

	for(int i = 0; i < m_recordNumber + 1; i++)
	{
		m_records[i].m_hash = m_file->ReadIntLE();
		m_records[i].m_offset = m_file->ReadIntLE();
	}

	return true;
}

void CResFile::Close()
{
	SAFE_DELETE_ARRAY(m_records);
	SAFE_DELETE(m_file);
	m_recordNumber = 0;
}

void* CResFile::FindRecord(const char *pName)
{
	unsigned long h = ComputeHash(pName);
	int i, u, l;
	l = 0;
	u = m_recordNumber - 1;
	i = m_recordNumber / 2;
	while(m_records[i].m_hash != h)
	{
		if(u <= l)
		{
			//record not found
			return NULL;
		}
		if(m_records[i].m_hash > h)
			u = i - 1;
		else
			l = i + 1;
		i = (u + l) / 2;
	}
	return m_records + i;
}

unsigned long CResFile::ComputeHash(const char *s)
{	
	ASSERT(strlen(s) < 128);

	char szTemp[128];
	strcpy(szTemp, s);
#if PLATFORM == PLATFORM_WINDOWS_MOBILE
	_strlwr(szTemp);
#else
	strlwr(szTemp);
#endif

	unsigned long h=0;
	const char *p;
	for(p = szTemp; *p ; p ++)
		h = ( h << 5 ) - h + (*p == '/' ? '\\' : *p);
	return h;
}

unsigned int CResFile::GetRecordPackedSize(void* hRecord)
{
	Record *p = (Record*)hRecord;

	ASSERT(p >= m_records && p < m_records + m_recordNumber);

	return p[1].m_offset - p[0].m_offset;
}

// Size before compression
unsigned int CResFile::GetRecordSize(void *hRecord)
{
	Record *p = (Record*)hRecord;
	m_file->SetPosition(p->m_offset);
	return m_file->ReadUIntLE();
}

unsigned int CResFile::ReadRecord(void *hRecord, void *pBuf, unsigned int len)
{
	if(m_compressMode == 0)
	{
		unsigned int packedSize = GetRecordPackedSize(hRecord) - 4;

		if(packedSize > len)
		{
			ASSERT(false);
			return 0;
		}

		m_file->SetPosition(((Record*)hRecord)->m_offset);
		m_file->Skip(4);
		m_file->Read(pBuf, packedSize);

		return 0;
	}
	else if(m_compressMode == 1)
	{
		unsigned int packedSize = GetRecordPackedSize(hRecord) - 4;

		if(packedSize > len)
		{
			ASSERT(false);
			return 0;
		}

		m_file->SetPosition(((Record*)hRecord)->m_offset);
	
		unsigned int originalSize = m_file->ReadUIntLE();

		unsigned char *inBuf = NEW unsigned char[packedSize];
		
		m_file->Read(inBuf, packedSize);

#if PLATFORM == PLATFORM_KDDI
		size_t outSize, outSizeProcessed;
		LzmaRamGetUncompressedSize(inBuf , packedSize, &outSize);
		int ret = LzmaRamDecompress(inBuf, packedSize, (unsigned char*)pBuf, outSize, &outSizeProcessed);
#else
		int ret = uncompress((Byte*)pBuf, (unsigned long*)&originalSize, inBuf, packedSize);
#endif
		SAFE_DELETE(inBuf);

		return 0;
	}
	else if(m_compressMode == 2)
	{
		unsigned int originalSize;
		unsigned int packedSize = GetRecordPackedSize(hRecord) - 4;

		if(packedSize > len)
		{
			ASSERT(false);
			return 0;
		}

		m_file->SetPosition(((Record*)hRecord)->m_offset);

		originalSize = m_file->ReadUIntLE();

		u32 inSize = packedSize;
		u32 outSize = originalSize;
		//u32 outSizeProcessed;

		unsigned char *inBuf = NEW unsigned char[inSize];
		m_file->Read(inBuf, inSize);
		u8* outBuf = (u8*)pBuf;

//		if(LzmaRamDecompress(inBuf, (size_t)inSize, outBuf, (size_t)outSize, (size_t*)(&outSizeProcessed)))
		{
			ASSERT(0);
		}

		SAFE_DELETE(inBuf);

		return 0;
	}
	else
	{
		return 0;
	}
}
