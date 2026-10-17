// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_READ_FILE_H_INCLUDED__
#define __C_READ_FILE_H_INCLUDED__

#include "IReadFile.h"
#include "irrString.h"

#ifdef _IRR_WII_PLATFORM_
#include <revolution.h>
#else
#include <stdio.h>
#endif

namespace irr
{

namespace io
{

	/*!
		Class for reading a real file from disk.
	*/
	class CReadFile : public IReadFile
	{
	public:

		CReadFile(const wchar_t* fileName);
		CReadFile(const c8* fileName);

		virtual ~CReadFile();

		//! returns how much was read
		virtual s32 read(void* buffer, u32 sizeToRead);

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false);

		//! returns size of file
		virtual long getSize() const;

		//! returns if file is open
		virtual bool isOpen() const
		{
#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
            return IsOpen;
#else
			return File != 0;
#endif
		}

		//! returns where in the file we are.
		virtual long getPos() const;

		//! returns name of file
		virtual const c8* getFileName() const;

	private:

		//! opens the file
		void openFile();

#if defined(_IRR_WII_PLATFORM_) && !defined(_IRR_WII_VIEWER_)
        /** DVD implementation. 
            DVDRead() has restrictions on buffer and length parameters. We
            read the whole content of specific file into the buffer when 
            openFile and copy data to user in the future read().
        */
        DVDFileInfo File;
        bool IsOpen;
        u8*  Buffer;
        s32  Padding;
        s32  Offset;
#else
		FILE* File;
#endif

		long FileSize;
		core::stringc Filename;
	};

} // end namespace io
} // end namespace irr

#endif

