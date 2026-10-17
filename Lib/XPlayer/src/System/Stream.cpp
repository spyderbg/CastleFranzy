// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#include "stdafx.h"
#include "Stream.h"
#if(PLATFORM != PLATFORM_MAC_OS_X)
#include "ResFile.h"
#endif
#if PLATFORM == PLATFORM_NGAGE
#include <stdlib.h>
#include <wchar.h>              // libc (wcsrchr, wcslen, wmemcpy)
#include <runtime.h>            // ngi
#include <errorcodes.h>         // ngi
#include <fileaccess.h>         // ngi
#include <filemanagerfactory.h> // ngi
using namespace ngi;
#elif PLATFORM == PLATFORM_SYMBIAN
#include <e32svr.h>

#if DEVICE_SPEC == DEVICE_Z8
#include <EIKENV.H>
#include <EIKAPPUI.H>
#else
#include <aknutils.h>
#endif

#include <eikapp.h>
#include "RFSymbianApplication.h"

#elif PLATFORM == PLATFORM_KDDI
#include <AEEFile.h>
#include "Global.h"
#include "RealFootball2009.h"
#endif
#if NGAGE
#if (PLATFORM == PLATFORM_NGAGE)
#include "RealFootball09.h"
#endif
#include "Game.h"
#include "Global.h"
#endif

#if PLATFORM == PLATFORM_MAC_OS_X
extern const char* RECORD_SAVE_FILE_PATH;
#endif

CRawFile::CRawFile()
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X) || (PLATFORM == PLATFORM_KDDI)
	m_file = NULL;
#endif
}

CRawFile::~CRawFile()
{
	Close();
}

#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X) || (PLATFORM == PLATFORM_KDDI)
extern char	g_moudlePath[512];
#endif

bool CRawFile::Open(const char *fn, int mode)
{

#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE)
	char m[3] = "rb";
	
	if((mode & MODE_CREATE) || (mode & MODE_WRITE))
		m[0] = 'w';
	else if(mode & MODE_APPEND)
		m[0] = 'a';
	char filename[256];
	strcpy(filename, g_moudlePath);
	strcat(filename, "Data\\");
	strcat(filename, fn);

	m_file = fopen(filename, m);

	return m_file != NULL;
#elif (PLATFORM == PLATFORM_KDDI)
	if (GetApp()->m_pFileMgr)
	{
		m_file = IFILEMGR_OpenFile((IFileMgr*)(GetApp()->m_pFileMgr), fn, mode);
		if(NULL == m_file)
			return false;
	}
	return true;

#elif PLATFORM == PLATFORM_NGAGE
	char16 fullName[MAX_PATH_LENGTH] = {0};
	char16 aFilename[MAX_PATH_LENGTH] = {0};

	mbstowcs(aFilename, fn, MAX_PATH_LENGTH);

	IFileManager* fileManager = NULL;
	fileManager = GameApp().m_pAppRF09->GetFileManager();
	wcscpy( fullName, fileManager->GetWorkingDirectory());

	wcscat( fullName, L"\\");
	wcscat( fullName, aFilename);

	ReturnCode err = OK;
	err = fileManager->AccessFile(fullName, m_file);
	if (err != OK)
		return false;

	FileMode mode2 = ((mode & MODE_READ) == 1)? FILE_MODE_READ : FILE_MODE_WRITE;
	err = m_file->Open(mode2);

	return err == OK;
#elif (PLATFORM == PLATFORM_SYMBIAN)
	TInt err = 0 ;

	unsigned short uiName[MAX_PATH_LENGTH] = {0};

	mbstowcs((wchar_t *)uiName, fn, strlen(fn));

	TFileName filename(uiName);
	TFileName iPath;			/// Application path

	//CompleteWithAppPath(filename);
	if(filename.Length() > 0) 
	{
		User::LeaveIfError(iFs.Connect());
#if SYMBIAN_OS == SYMBIAN_OS_S60_3RD
		iFs.PrivatePath(iPath);
#ifdef __WINS__
		iPath.Insert(0, _L("c:"));
#else
		TFileName appFullName = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
		TParse parse;
		parse.Set( appFullName, NULL, NULL);
		iPath.Insert(0, parse.Drive());
#endif
		filename.Insert(0,iPath);
#elif SYMBIAN_OS == SYMBIAN_OS_S60_2ND
		CompleteWithAppPath(filename);
#endif
//		if(openType == F_WRITE) {
//			User:eaveIfError(iFile.Replace(iFs, iFileName, EFileWrite));
//			iFws.Attach(iFile) ;
//		} else {
			// get the file size first
			err = m_file.Open(iFs, filename, EFileStream | ((mode & MODE_READ)?EFileRead:EFileWrite));
			if(err != KErrNone)
			{
				m_file.Close();
				m_file.Create(iFs, filename, EFileStream | ((mode & MODE_READ)?EFileRead:EFileWrite));
				m_file.Close();
				err = m_file.Open(iFs, filename, EFileStream | ((mode & MODE_READ)?EFileRead:EFileWrite));
			}
			User::LeaveIfError(err) ;
			TInt iSize = 0;
			err = m_file.Size(iSize) ;
			if(err != KErrNone)
			{
				iSize = -1 ;
				return false;
			}
//		}
		return true;
	}
	return false;
#elif (PLATFORM == PLATFORM_MAC_OS_X)
	char m[3] = "rb";

	if((mode & MODE_CREATE) || (mode & MODE_WRITE))
		m[0] = 'w';
	else if(mode & MODE_APPEND)
		m[0] = 'a';
	char filename[256];
	strcpy(filename, g_moudlePath);
#ifdef WIN32
	strcat(filename, "Data\\");
#endif
	strcat(filename, fn);

	m_file = fopen(filename, m);

	return m_file != NULL;
#endif
}

bool CRawFile::OpenMacOtherPath(const char *fn, int mode)
{
	char m[3] = "rb";

	if((mode & MODE_CREATE) || (mode & MODE_WRITE))
		m[0] = 'w';
	else if(mode & MODE_APPEND)
		m[0] = 'a';
	char filename[512] = {0};
#if PLATFORM == PLATFORM_MAC_OS_X
	strcpy(filename, RECORD_SAVE_FILE_PATH);
#elif PLATFORM == PLATFORM_WIN32
	strcpy(filename, g_moudlePath);
	strcat(filename, "Data\\");
#endif
	strcat(filename, fn);
	m_file = fopen(filename, m);
	return m_file != NULL;

}


void CRawFile::Close()
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	if(m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
#elif PLATFORM == PLATFORM_KDDI
	if (m_file)
	{
		if (IFILE_Release((IFile*)m_file) == 0)
			m_file = NULL;
	}
#elif PLATFORM == PLATFORM_NGAGE
	if(m_file)
	{
		m_file->CancelAll();
		m_file->SetObserver(NULL);
		m_file->Close();
		m_file->Release();
		m_file = NULL;
	}
#elif PLATFORM == PLATFORM_SYMBIAN
	m_file.Close();
	iFs.Close();
#endif

}

bool CRawFile::Seek(int moveDistance, int seekMode)
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	int o;
	if(seekMode == FSEEK_BEGIN)
		o = SEEK_SET ;
	else if(seekMode == FSEEK_CURRENT)
		o = SEEK_CUR;
	else if(seekMode == FSEEK_END)
		o = SEEK_END;
	else
	{
		ASSERT(false);
		return false;
	}

	return fseek(m_file, moveDistance, o) == 0;
#elif PLATFORM == PLATFORM_KDDI
	int o;
	if (seekMode == FSEEK_BEGIN)
		o = _SEEK_START;
	else if (seekMode == FSEEK_CURRENT)
		o = _SEEK_CURRENT;
	else if (seekMode == FSEEK_END)
		o = _SEEK_END;
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	IFILE_Seek((IFile*)m_file, o, moveDistance); // the function return SUCCESS or EFAILED
	return TRUE;

#elif PLATFORM == PLATFORM_NGAGE
	if (!m_file)
		return -1;

	ngi::SeekMode mode = SEEK_MODE_START;
	if (seekMode == FSEEK_CURRENT)
		mode = SEEK_MODE_CURRENT;
	else if (seekMode == FSEEK_END)
		mode = SEEK_MODE_END;

	uint32 newOffset = m_file->Seek(moveDistance, mode);
	return true;

#elif PLATFORM == PLATFORM_SYMBIAN
	TSeek o;
	if(seekMode == FSEEK_BEGIN)
		o = ESeekStart ;
	else if(seekMode == FSEEK_CURRENT)
		o = ESeekCurrent;
	else if(seekMode == FSEEK_END)
		o = ESeekEnd;
	else
	{
		ASSERT(false);
		return false;
	}
	m_file.Seek(o, moveDistance);
	return true;
#else
	return false;
#endif
}

unsigned int CRawFile::GetLength()
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	if(m_file == NULL)
	{
		return 0;
	}
	long orgPos = ftell(m_file);
	fseek(m_file, 0, SEEK_END);
	long fl = ftell(m_file);
	fseek(m_file, orgPos, SEEK_SET);
	return fl;
#elif PLATFORM == PLATFORM_KDDI
	FileInfo fileinfo;
	if (m_file == NULL)
	{
		return 0;
	}
	if (IFILE_GetInfo((IFile*)m_file, &fileinfo) == SUCCESS)
		return fileinfo.dwSize;
	else
		return 0;
#elif PLATFORM == PLATFORM_NGAGE
	uint32 orgPos = m_file->Seek(0, SEEK_MODE_CURRENT);
	uint32 startPos = m_file->Seek(0, SEEK_MODE_START);
	uint32 endPos = m_file->Seek(0, SEEK_MODE_END);
	m_file->Seek(orgPos, SEEK_MODE_START);
	return endPos - startPos;
#elif PLATFORM == PLATFORM_SYMBIAN
	TInt iSize = 0;
	TInt err = m_file.Size(iSize) ;
	return err == KErrNone ? iSize : -1;
#else
	return 0;
#endif
}

unsigned int CRawFile::GetPosition()
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	return ftell(m_file);
#elif PLATFORM == PLATFORM_KDDI
	if (m_file == NULL)
		return 0;
	uint32 orgPos = IFILE_Seek((IFile*)m_file, _SEEK_CURRENT, 0);
	return orgPos;
#elif PLATFORM == PLATFORM_NGAGE
	if(m_file == NULL) return 0;
	uint32 orgPos = m_file->Seek(0, SEEK_MODE_CURRENT);
	return orgPos;
#elif PLATFORM == PLATFORM_SYMBIAN
	TInt Pos = 0;
	uint32 orgPos = m_file.Seek(ESeekCurrent, Pos);
	return orgPos;
#else
	return 0;
#endif
}

unsigned int CRawFile::Write(const void *buf, unsigned int size)
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	return (u32)fwrite(buf, 1, size, m_file);
#elif PLATFORM == PLATFORM_KDDI
	if (m_file == NULL)
		return 0;
	return IFILE_Write((IFile*)m_file, buf, size);
#elif PLATFORM == PLATFORM_NGAGE
	if(m_file == NULL) return 0;
	ReturnCode err = m_file->WriteSynchronous((uint8*)buf, size);
	return err == OK?size:0;
#elif PLATFORM == PLATFORM_SYMBIAN
	TPtrC8 pBuf((const TUint8 *)buf, (TInt)size);
	TInt err = m_file.Write(pBuf, (TInt)size);
	return err == KErrNone?size:0;
#else
	return 0;
#endif
}

unsigned int CRawFile::Read(void *buf, unsigned int size)
{
#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
	return (u32)fread(buf, 1, size, m_file);
#elif PLATFORM == PLATFORM_KDDI
	if (m_file == NULL)
		return 0;
	return IFILE_Read((IFile*)m_file, buf, size);
#elif PLATFORM == PLATFORM_NGAGE
	if(m_file == NULL) return 0;
	uint32 ss = size;
	ReturnCode err = m_file->ReadSynchronous((uint8*)buf, ss);
	return (unsigned int)ss;
#elif PLATFORM == PLATFORM_SYMBIAN
	TPtr8 ptr( (TUint8*)buf, size );
	//User::LeaveIfError(m_file.Read(ptr));
	TInt err = m_file.Read(ptr);
	return err == KErrNone?size:0;
#else
	return 0;
#endif
}

CFileStream::CFileStream()
{
}

CFileStream::~CFileStream()
{
}

bool CFileStream::Open(const char* pName, int mode)
{
	m_mode = mode;
	if(!m_file.Open(pName, mode))
		return false;
	return true;
}

bool CFileStream::OpenMacOtherPath(const char *pName, int mode)
{
	m_mode = mode;
	if(!m_file.OpenMacOtherPath(pName, mode))
		return false;
	return true;
}

int CFileStream::GetLength()
{
	return m_file.GetLength();
}

int CFileStream::GetPosition()
{
	return m_file.GetPosition();
}

bool CFileStream::SetPosition(int pos)
{
	return m_file.Seek(pos, CRawFile::FSEEK_BEGIN);
}

bool CFileStream::Skip(int pos)
{
	return m_file.Seek(pos, CRawFile::FSEEK_CURRENT);
}

bool CFileStream::IsReadOnly()
{
	return m_mode == CRawFile::MODE_READ;
}

int CFileStream::Read(void* pBuffer, int length)
{
	return m_file.Read(pBuffer, length);
}

signed char CFileStream::ReadByte()
{
	signed char c;
	int ret = m_file.Read(&c, 1);
	ASSERT(ret == 1);
	return c;
}

unsigned char CFileStream::ReadUByte()
{
	unsigned char c;
	int ret = m_file.Read(&c, 1);
	ASSERT(ret == 1);
	return c;
}

short CFileStream::ReadShortLE()
{
	unsigned char c[2];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (short)((c[1] << 8) | c[0]);
}

unsigned short CFileStream::ReadUShortLE()
{
	unsigned char c[2];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (unsigned short)((c[1] << 8) | c[0]);
}

int CFileStream::ReadIntLE()
{
	unsigned char c[4];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
}

unsigned int CFileStream::ReadUIntLE()
{
	unsigned char c[4];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0];
}

short CFileStream::ReadShortBE()
{
	unsigned char c[2];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (short)((c[0] << 8) | c[1]);
}

unsigned short CFileStream::ReadUShortBE()
{
	unsigned char c[2];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (unsigned short)((c[0] << 8) | c[1]);
}

int CFileStream::ReadIntBE()
{
	unsigned char c[4];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

unsigned int CFileStream::ReadUIntBE()
{
	unsigned char c[4];
	int ret = m_file.Read(&c, sizeof(c));
	ASSERT(ret == sizeof(c));
	return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

int CFileStream::Write(const void* pBuffer, int length)
{
	return m_file.Write(pBuffer, length);
}

void CFileStream::WriteByte(int v)
{
	unsigned char c = v;
	int ret = m_file.Write(&c, 1);
	ASSERT(ret == 1);
}

void CFileStream::WriteShortLE(int v)
{
	unsigned char c[2];
	c[1] = v >> 8;
	c[0] = v;
	int ret = m_file.Write(&c, 2);
	ASSERT(ret == 2);
}

void CFileStream::WriteIntLE(int v)
{
	unsigned char c[4];
	c[3] = v >> 24;
	c[2] = v >> 16;
	c[1] = v >> 8;
	c[0] = v;
	int ret = m_file.Write(&c, 4);
	ASSERT(ret == 4);
}

void CFileStream::WriteShortBE(int v)
{
	unsigned char c[2];
	c[0] = v >> 8;
	c[1] = v;
	int ret = m_file.Write(&c, 2);
	ASSERT(ret == 2);
}

void CFileStream::WriteIntBE(int v)
{
	unsigned char c[4];
	c[0] = v >> 24;
	c[1] = v >> 16;
	c[2] = v >> 8;
	c[3] = v;
	int ret = m_file.Write(&c, 4);
	ASSERT(ret == 4);
}

void CFileStream::Close()
{
	m_file.Close();
}

CMemoryStream::CMemoryStream(void *buf, int length)
{
	m_data = (unsigned char*)buf;
	m_current = m_data;
	m_length = length;
	m_needFreeBuffer = false;
}

CMemoryStream::CMemoryStream(int length/*, MemoryType memType*/)
{
	m_data = NEW unsigned char[length];
	m_current = m_data;
	m_length = length;
	m_needFreeBuffer = true;
}

#include "Game.h"
#include "ResFile.h"
#include "ResourceManager.h"

CMemoryStream::CMemoryStream(const char* fileName)
{
	void*		pRecord = NULL;
	CResFile*	pResFile = NULL;
	int		length = 0;

	GetResourceManager().FindRecord(fileName, &pRecord, &pResFile, length);

	if(pRecord != NULL)
	{
		m_data = NEW unsigned char[length];
		m_current = m_data;
		m_length = length;
		m_needFreeBuffer = true;
		
		pResFile->ReadRecord(pRecord, m_data, length);
	}
	else
	{
		CFileStream fs;

		if(!fs.Open(fileName))
		{
			ASSERT(0);//file does not exist in either package or direct file
		}

		length = fs.GetLength();
		m_data = NEW unsigned char[length];
		m_current = m_data;
		m_length = length;
		m_needFreeBuffer = true;
		fs.Read(m_data, length);
		fs.Close();
	} 
}

CMemoryStream::~CMemoryStream()
{
	Close();
}

int CMemoryStream::GetLength()
{
	return m_length;
}

int CMemoryStream::GetPosition()
{
	return (int)(m_current - m_data);
}

bool CMemoryStream::IsReadOnly()
{
	//always read-write
	return false;
}

int CMemoryStream::Write(const void* pBuffer, int length)
{
	ASSERT(m_current + length <= m_data + m_length);
	MEMCPY(m_current, pBuffer, length);
	m_current += length;
	return length;
}

void CMemoryStream::WriteByte(int v)
{
	ASSERT(m_current < m_data + m_length);
	*m_current++ = v;
}

void CMemoryStream::WriteShortLE(int v)
{
	ASSERT(m_current + 2 <= m_data + m_length);
	*m_current++ = v;
	*m_current++ = v >> 8;
}

void CMemoryStream::WriteIntLE(int v)
{
	ASSERT(m_current + 4 <= m_data + m_length);
	*m_current++ = v;
	*m_current++ = v >> 8;
	*m_current++ = v >> 16;
	*m_current++ = v >> 24;
}

void CMemoryStream::WriteShortBE(int v)
{
	ASSERT(m_current + 2 <= m_data + m_length);
	*m_current++ = v >> 8;
	*m_current++ = v;
}

void CMemoryStream::WriteIntBE(int v)
{
	ASSERT(m_current + 4 <= m_data + m_length);
	*m_current++ = v >> 24;
	*m_current++ = v >> 16;
	*m_current++ = v >> 8;
	*m_current++ = v;
}

void CMemoryStream::Close()
{
	if(m_needFreeBuffer)
	{
		SAFE_DELETE_ARRAY(m_data);
	}
	m_data = NULL;
}

bool CMemoryStream::Skip(int len)
{
	m_current += len;
	ASSERT(m_current >= m_data && m_current <= m_data + m_length);
	return true;
}

bool CMemoryStream::SetPosition(int pos)
{
	ASSERT(pos < m_length);
	m_current = m_data + pos;
	return true;
}

int CMemoryStream::Read(void* pBuffer, int length)
{
	ASSERT(m_current + length <= m_data + m_length);
	MEMCPY(pBuffer, m_current, length);
	m_current += length;
	return length;
}

signed char CMemoryStream::ReadByte()
{
	ASSERT(m_current < m_data + m_length);
	return (signed char)*m_current++;
}

unsigned char CMemoryStream::ReadUByte()
{
	ASSERT(m_current < m_data + m_length);
	return (unsigned char)*m_current++;
}

short CMemoryStream::ReadShortLE()
{
	ASSERT(m_current + 2 <= m_data + m_length);
	short r = (short)((m_current[1] << 8) | m_current[0]);
	m_current += 2;
	return r;
}

unsigned short CMemoryStream::ReadUShortLE()
{
	ASSERT(m_current + 2 <= m_data + m_length);
	unsigned short r = (unsigned short)((m_current[1] << 8) | m_current[0]);
	m_current += 2;
	return r;
}

int CMemoryStream::ReadIntLE()
{
	ASSERT(m_current + 4 <= m_data + m_length);
	int r = (m_current[3] << 24) | (m_current[2] << 16) | (m_current[1] << 8) | m_current[0];
	m_current += 4;
	return r;
}

unsigned int CMemoryStream::ReadUIntLE()
{
	ASSERT(m_current + 4 <= m_data + m_length);
	unsigned int r = (m_current[3] << 24) | (m_current[2] << 16) | (m_current[1] << 8) | m_current[0];
	m_current += 4;
	return r;
}

short CMemoryStream::ReadShortBE()
{		
	ASSERT(m_current + 2 <= m_data + m_length);
	short r = (short)((m_current[0] << 8) | m_current[1]);
	m_current += 2;
	return r;
}

unsigned short CMemoryStream::ReadUShortBE()
{
	ASSERT(m_current + 2 <= m_data + m_length);
	unsigned short r = (unsigned short)((m_current[0] << 8) | m_current[1]);
	m_current += 2;
	return r;
}

int CMemoryStream::ReadIntBE()
{
	ASSERT(m_current + 4 <= m_data + m_length);
	int r = (m_current[0] << 24) | (m_current[1] << 16) | (m_current[2] << 8) | m_current[3];
	m_current += 4;
	return r;
}

unsigned int CMemoryStream::ReadUIntBE()
{
	ASSERT(m_current + 4 <= m_data + m_length);
	unsigned int r = (m_current[0] << 24) | (m_current[1] << 16) | (m_current[2] << 8) | m_current[3];
	m_current += 4;
	return r;
}

//char* CMemoryStream::readUTF(MemoryType memType)
//{
//	unsigned short len = ReadUShortBE();
//	char* str = NEW(memType) char[len + 1];
//	str[Read(str, len)] = 0;
//	return str;
//}
