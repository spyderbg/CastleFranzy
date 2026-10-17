// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_MEMORY_READ_FILE_H_INCLUDED__
#define __C_MEMORY_READ_FILE_H_INCLUDED__

#include "IReadFile.h"
#include "irrString.h"

namespace irr
{

namespace io
{

	/*!
		Class for reading from memory.
	*/
	class CMemoryReadFile : public IReadFile
	{
	public:

		//! Constructor
		CMemoryReadFile();
		CMemoryReadFile(void* memory, long len, const c8* fileName, bool deleteMemoryWhenDropped);

		//! Destructor
		virtual ~CMemoryReadFile();

		//! set the memory, length and filename
		void set(void* memory, long len, const char* fileName);

		//! returns if the file is valid
		bool isValid() const;


		//! returns how much was read
		virtual s32 read(void* buffer, u32 sizeToRead);

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false);

		//! returns size of file
		virtual long getSize() const;

		//! returns where in the file we are.
		virtual long getPos() const;

		//! returns name of file
		virtual const c8* getFileName() const;

		virtual void* getBuffer(long* size) {
			if(size) *size = Pos;
			return Buffer;
		}

		virtual bool isAllInMemory() const {
			return true;
		}

	private:

		void *Buffer;
		long Len;
		long Pos;
		core::stringc Filename;
		bool deleteMemoryWhenDropped;
	};

} // end namespace io
} // end namespace irr

#endif

