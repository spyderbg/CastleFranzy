#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CReadFile.h"
#include "CFileSystem.h"
namespace irr
{
namespace io
{


CReadFile::CReadFile(const c8* fileName)
: FileSize(0), Filename(fileName)
#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
, IsOpen(false), Buffer(0), Padding(0), Offset(0)
#else
, File(0)
#endif
{
	#ifdef _DEBUG
	setDebugName("CReadFile");
	#endif

	openFile();
}


CReadFile::~CReadFile()
{
#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
    if (IsOpen)
    {
        DVDClose(&File);
        IsOpen = false;
    }

    if (Buffer)
    {
        delete [] Buffer;
        Buffer  = 0;
    }
    
    Padding = 0;
    Offset  = 0;
#else
	if (File)
		fclose(File);
#endif
}


//! returns how much was read
s32 CReadFile::read(void* buffer, u32 sizeToRead)
{
	if (!isOpen())
		return 0;

#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
    s32 bytesRead = (FileSize - Offset) < sizeToRead ? (FileSize - Offset) : sizeToRead;
    memcpy(buffer, Buffer + Padding + Offset, bytesRead);
    Offset += bytesRead;
    return bytesRead;
#else
	return (s32)fread(buffer, 1, sizeToRead, File);
#endif
}


//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool CReadFile::seek(long finalPos, bool relativeMovement)
{
	if (!isOpen())
		return false;
        
#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
    s32 ofs = relativeMovement ? finalPos : (finalPos - Offset);
    if (Offset + ofs < FileSize)
    {
        Offset += ofs;
        return true;
    }
    return false;
#else
	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
#endif
}


//! returns size of file
long CReadFile::getSize() const
{
	return FileSize;
}


//! returns where in the file we are.
long CReadFile::getPos() const
{
#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
    return Offset;
#else
	return ftell(File);
#endif
}


//! opens the file
void CReadFile::openFile()
{
	if (Filename.size() == 0) // bugfix posted by rt
	{
#if !defined(_IRR_WII_PLATFORM_) || defined(_IRR_WII_VIEWER_)
		File = 0;
#endif
		return;
	}

#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
	s32 entrynum = DVDConvertPathToEntrynum(Filename.c_str());
	if(entrynum == -1)
		return;
    IsOpen = DVDFastOpen(entrynum, &File);
    if (IsOpen)
    {
        FileSize = DVDGetLength(&File);

        // Keep buffer size 32 bytes aligned. 
        // Add 32 bytes to make space for padding if necessary.
        s32 bufSize = OSRoundUp32B(FileSize) + 32;
        Buffer = irrnew u8[bufSize];
        memset(Buffer, 0, bufSize);
        Padding = OSRoundUp32B((u32)Buffer) - ((u32)Buffer);

        // Read the whole file into the buffer.
        s32 err = DVDRead(&File, Buffer + Padding, OSRoundUp32B(FileSize), 0);
        if (err < 0)
        {
            OSReport("CReadFile::openFile() Read file %s error (%d).\n", Filename.c_str(), err);
        }
    }
#else
	//File = fopen(Filename.c_str(), "rb");
	File = irr::io::CFileSystem::open(Filename.c_str(), "rb");	

	if (File)
	{
		// get FileSize
		fseek(File, 0, SEEK_END);
		FileSize = getPos();
        fseek(File, 0, SEEK_SET);
	}
#endif
}


//! returns name of file
const c8* CReadFile::getFileName() const
{
	return Filename.c_str();
}



IReadFile* createReadFile(const c8* fileName)
{
	CReadFile* file = irrnew CReadFile(fileName);
	if (file->isOpen())
		return file;

	file->drop();
	return 0;
}


} // end namespace io
} // end namespace irr

