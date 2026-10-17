// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __GLLIVE_STREAM_H__
#define __GLLIVE_STREAM_H__

#if PLATFORM == PLATFORM_NGAGE
#include <fileaccess.h>			// ngi
#elif PLATFORM == PLATFORM_SYMBIAN
#include <s32file.h>
#include <e32std.h>
#include <e32def.h>

#define MAX_PATH_LENGTH 256
#endif

namespace gllive
{
	class CGLLiveRawFile
	#if PLATFORM == PLATFORM_SYMBIAN
	: public CBase
	#endif
	{
	public:
		enum AccessMode{
			MODE_READ	= 0x01,
			MODE_WRITE	= 0x02,
			MODE_CREATE	= 0x04,
			MODE_APPEND	= 0x08
		};

		enum SeekMode{
			FSEEK_BEGIN	= 0,
			FSEEK_CURRENT,
			FSEEK_END
		};

		CGLLiveRawFile();
		virtual ~CGLLiveRawFile();

		bool Open(const char *fn, int mode);
		bool OpenMacOtherPath(const char* fn, int mode);
		void Close();
		bool Seek(int moveDistance, int seekMode);
		unsigned int GetLength();
		unsigned int GetPosition();
		unsigned int Write(const void *buf, unsigned int size);
		unsigned int Read(void *buf, unsigned int size);

	protected:

	#if PLATFORM == PLATFORM_NGAGE
		ngi::IFile* m_file;
	#elif (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)
		FILE* m_file;
	#elif PLATFORM == PLATFORM_SYMBIAN
		RFs iFs ;
		RFile m_file;
	#else
		void * m_file;
	#endif
	};

	//The following defines are used to control the behaviour of ReadShort, ReadInt..., NOT FOR CPU ENDIAN!
	#define DEFAULT_BIG_ENDIAN_DATA	//data in stream is little endian
	//#define DEFAULT_LITTLE_ENDIAN_DATA	//data in stream is big endian

	/// @brief This abstract class is the superclass of all classes representing an input stream of bytes. 
	///
	/// This class supports both big endian and little endian byte order. The functions with LE postfix assume
	/// the byte stream is little endian, and the functions with BE postfix assume the byte stream is big endian.
	/// The other functions that have no postfix will assume a byte order given by a define of 
	/// DEFAULT_BIG_ENDIAN or DEFAULT_LITTLE_ENDIAN. <br>
	/// Random access is possible by using the #SetPosition function.
	/// @attention The functions for reading number like ReadShortLE, ReadUShortLE, ReadUIntLE are independent of
	/// CPU's byte order, so they work perfectly on Nokia 3589i. Never rewrite the or replace them with ReadXXXBE
	/// on Nokia 3589i version. 
	class CGLLiveStream {
	public:
		virtual ~CGLLiveStream(){}
		/// @brief Get the total length of the input stream.
		/// @return The total length of the input stream.
		virtual int GetLength() = 0;
		/// @brief Get the current position of the input stream.
		/// @return The current position of the input stream.
		virtual int GetPosition() = 0;
		/// @brief Set the current position in stream. Subsequence read/write operation will start from this position.
		///	@param[in]	pos	The position to set in byte.
		/// @return true if success, false if failed.
		virtual bool SetPosition(int pos) = 0;
		/// @brief Add n bytes to the current position of this stream.
		///	@param[in]	n	Number of byte to skip.
		/// @return true if success, false if failed.
		virtual bool Skip(int n) = 0;
		/// @brief Return the read/write attribute of this stream.
		/// @return true if this stream is read-only, false if this stream is ok for read/write.
		virtual	bool IsReadOnly() = 0;
		/// @brief Read data from this stream at the current position.
		///	@param[in]	pBuffer	Pointer to the user-supplied buffer that is to receive the data read from the stream.
		///	@param[in]	length	The maximum number of bytes to be read from the stream.
		/// @return The number of bytes transferred to the buffer.
		/// Note that the return value may be less than length if the end of stream was reached. 
		virtual int Read(void* pBuffer, int length) = 0;
		/// @brief Read the next byte of data from the stream.
		/// @return The next byte of this input stream as a signed 8-bit byte.
		virtual signed char ReadByte() = 0;
		/// @brief Read the next byte of data from the stream.
		/// @return The next byte of this input stream as a unsigned 8-bit byte.
		virtual unsigned char ReadUByte() = 0;

		/// @brief Read two input bytes in little endian order and returns a short value.
		///
		/// Let a be the first byte read and b be the second byte. The value returned is: 
		/// \code (short)((b << 8) | (a & 0xff)) \endcode
		/// This method is suitable for reading the bytes written by the WriteShortLE function. 
		/// @return The the 16-bit value read.
		virtual short ReadShortLE() = 0;
		/// @brief Read two input bytes in little endian order and returns an unsigned short value.
		///
		/// Let a be the first byte read and b be the second byte. The value returned is: 
		/// \code (short)((b << 8) | (a & 0xff)) \endcode
		/// This method is suitable for reading the bytes written by the WriteShortLE function. 
		/// @return The the 16-bit value read.
		virtual unsigned short ReadUShortLE() = 0;
		/// @brief Read four input bytes in little endian order and returns an int value.
		///
		/// Let a be the first byte read, b be the second byte, c be the third byte,
		/// and d be the fourth byte. The value returned is:
		/// \code (((d & 0xff) << 24) | ((c & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff)) \endcode
		/// This method is suitable for reading the bytes written by the WriteIntLE function. 
		/// @return The int value read.
		virtual int ReadIntLE() = 0;
		/// @brief Read four input bytes in little endian order and returns an unsigned int value.
		///
		/// Let a be the first byte read, b be the second byte, c be the third byte,
		/// and d be the fourth byte. The value returned is:
		/// \code (((d & 0xff) << 24) | ((c & 0xff) << 16) | ((b & 0xff) << 8) | (a & 0xff)) \endcode
		/// This method is suitable for reading the bytes written by the WriteIntLE function. 
		/// @return The unsigned int value read.
		virtual unsigned int ReadUIntLE() = 0;

		/// @brief Read two input bytes in big endian order and returns a short value.
		///
		/// Let a be the first byte read and b be the second byte. The value returned is: 
		/// \code (short)((a << 8) | (b & 0xff)) \endcode
		/// This method is suitable for reading the bytes with big endian byte order. 
		/// @return The the 16-bit value read.
		virtual short ReadShortBE() = 0;
		/// @brief Read two input bytes in big endian and returns an unsigned short value.
		///
		/// Let a be the first byte read and b be the second byte. The value returned is: 
		/// \code (short)((a << 8) | (b & 0xff)) \endcode
		/// This method is suitable for reading the bytes with big endian byte order. 
		/// @return The the 16-bit value read.
		virtual unsigned short ReadUShortBE() = 0;
		/// @brief Read four input bytes in big endian and returns an int value.
		///
		/// Let a be the first byte read, b be the second byte, c be the third byte,
		/// and d be the fourth byte. The value returned is:
		/// \code (((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8) | (d & 0xff)) \endcode
		/// This method is suitable for reading the bytes with big endian byte order. 
		/// @return The int value read.
		virtual int ReadIntBE() = 0;
		/// @brief Read four input bytes in big endian and returns an unsigned int value.
		///
		/// Let a be the first byte read, b be the second byte, c be the third byte,
		/// and d be the fourth byte. The value returned is:
		/// \code (((a & 0xff) << 24) | ((b & 0xff) << 16) | ((c & 0xff) << 8) | (d & 0xff)) \endcode
		/// This method is suitable for reading the bytes with big endian byte order. 
		/// @return The int value read.
		virtual unsigned int ReadUIntBE() = 0;

		/// @brief Write data from a buffer to the stream.
		///	@param[in]	pBuffer	A pointer to the user-supplied buffer that contains the data to be written to the stream.
		///	@param[in]	length	The number of bytes to be transferred from the stream.
		/// @return The number of bytes written to the stream.
		/// Note that the return value may be less than length if no more storage space is available. 
		virtual int Write(const void* pBuffer, int length) = 0;

		/// @brief Write to the stream the eight low-order bits of the argument v.
		/// The 24 high-order bits of v are ignored.
		///	@param[in]	v	The byte value to be written.
		virtual void WriteByte(int v) = 0;
		/// @brief Write two bytes to the stream in little endian order.
		/// The 16 high-order bits of v are ignored.
		///	@param[in]	v	The short value to be written.
		virtual void WriteShortLE(int v) = 0;
		/// @brief Write an int value, which is comprised of four bytes, to the stream in little endian order.
		///	@param[in]	v	The int value to be written.
		virtual void WriteIntLE(int v) = 0;
		/// @brief Write two bytes to the stream in big endian order.
		/// The 16 high-order bits of v are ignored.
		///	@param[in]	v	The short value to be written.
		virtual void WriteShortBE(int v) = 0;		//added by Lu Yan
		/// @brief Write an int value, which is comprised of four bytes, to the stream in big endian order.
		///	@param[in]	v	The int value to be written.
		virtual void WriteIntBE(int v) = 0;			//added by Lu Yan

		/// @brief Closes this stream and releases any system resources associated with it.
		virtual void Close() = 0;

	#ifdef DEFAULT_LITTLE_ENDIAN_DATA
		short ReadShort() {	return ReadUIntLE(); }
		unsigned short ReadUShort() { return ReadUShortLE(); }
		int ReadInt() {	return ReadIntLE(); }
		unsigned int ReadUInt() { return ReadUIntLE(); }
		void WriteShort(int v) { WriteShortLE(v); }		//added by Lu Yan
		void WriteInt(int v) { WriteIntLE(v); }			//added by Lu Yan
	#endif	//#ifdef DEFAULT_LITTLE_ENDIAN_DATA
	#ifdef DEFAULT_BIG_ENDIAN_DATA
		/// @brief Read two input bytes in default byte order and returns a short value.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		/// @return The 16-bit value read.
		short ReadShort() {	return ReadUIntBE(); }
		/// @brief Read two input bytes in default byte order and returns a unsigned short value.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		/// @return The 16-bit value read.
		unsigned short ReadUShort() { return ReadUShortBE(); }
		/// @brief Read four input bytes in default byte order and returns an int value.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		/// @return The int value read.
		int ReadInt() {	return ReadIntBE(); }
		/// @brief Read four input bytes in default byte order and returns an unsigned int value.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		/// @return The int value read.
		unsigned int ReadUInt() { return ReadUIntBE(); }
		/// @brief Write two bytes to the stream in default byte order.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		///	@param[in]	v	The short value to be written.
		void WriteShort(int v) { WriteShortBE(v); }		//added by Lu Yan
		/// @brief Write an int value, which is comprised of four bytes, to the stream in default byte order.
		///
		/// If DEFAULT_BIG_ENDIAN is defined, the default byte order is big endian.
		/// If DEFAULT_LITTLE_ENDIAN is defined, the default byte order is little endian.
		///	@param[in]	v	The int value to be written.
		void WriteInt(int v) { WriteIntBE(v); }			//added by Lu Yan
	#endif	//#ifdef DEFAULT_BIG_ENDIAN_DATA
	};

	/// @brief Provide file access as CGLLiveStream interface.
	///
	/// This CGLLiveFileStream class is the implementation of CGLLiveStream interface for accessing files.
	class CGLLiveFileStream : public CGLLiveStream{
	public:
		CGLLiveFileStream();
		virtual ~CGLLiveFileStream();

		/// @brief Opens a file with specified file name and mode.
		///	@param[in]	pName	The system-dependent file name.
		///	@param[in]	mode	Flag of desired access mode defined in CGLLiveRawFile::AccessMode.  
		/// @return true if success, else false.
		bool Open(const char* pName, int mode = CGLLiveRawFile::MODE_READ);
		bool OpenMacOtherPath(const char* pName, int mode= CGLLiveRawFile::MODE_READ);
		virtual int GetLength();
		virtual int GetPosition();
		virtual bool SetPosition(int pos);
		virtual bool Skip(int n);
		virtual	bool IsReadOnly();

		virtual int Read(void* pBuffer, int length);
		virtual signed char ReadByte();
		virtual unsigned char ReadUByte();
		virtual short ReadShortLE();
		virtual unsigned short ReadUShortLE();
		virtual int ReadIntLE();
		virtual unsigned int ReadUIntLE();

		//functions for reading big endian data
		virtual short ReadShortBE();
		virtual unsigned short ReadUShortBE();
		virtual int ReadIntBE();
		virtual unsigned int ReadUIntBE();

		virtual int Write(const void* pBuffer, int length);
		virtual void WriteByte(int v);
		virtual void WriteShortLE(int v);
		virtual void WriteIntLE(int v);
		virtual void WriteShortBE(int v);	//added by Lu Yan
		virtual void WriteIntBE(int v);	//added by Lu Yan

		virtual void Close();

	private:
		CGLLiveFileStream& operator=(const CGLLiveFileStream& rhs);
		//embed CGLLiveRawFile here instead of pointer to CGLLiveRawFile to avoid dynamic new
		CGLLiveRawFile m_file;
		int m_mode;
	};

	/// @brief Provide memory buffer access as CGLLiveStream interface.
	///
	/// This CGLLiveMemoryStream class is very similar to the CGLLiveFileStream class,
	/// only it's designed specifically for accessing data in memory.
	class CGLLiveMemoryStream : public CGLLiveStream
	{
	public:
		/// @brief Create a CGLLiveMemoryStream so that it uses buf as its buffer array.
		///
		/// The buffer array is not copied. The initial value of position is 0 and the buffer length is length.
		///	@param[in]	buf	The memory buffer.
		///	@param[in]	length	The size of the memory buffer.
		CGLLiveMemoryStream(void *buf, int length);
		/// @brief Create a CGLLiveMemoryStream and allocate a block of memory as its buffer array.
		///
		/// The buffer array is freed when this CGLLiveMemoryStream object is closed or deleted.
		///	@param[in]	length	The size of the memory buffer.
		///	@param[in]	memType	The memory type of the memory buffer.
		CGLLiveMemoryStream(int length/*, MemoryType memType*/);
		CGLLiveMemoryStream(const char* fileName);
		virtual ~CGLLiveMemoryStream();

		/// @brief Get a pointer to the buffer array.
		///
		/// @return Pointer to the buffer array.
		void* GetBuffer() const { return m_data; }

		virtual int GetLength();
		virtual int GetPosition();
		virtual bool SetPosition(int pos);
		virtual bool Skip(int n);
		virtual	bool IsReadOnly();

		virtual int Read(void* pBuffer, int length);
		virtual signed char ReadByte();
		virtual unsigned char ReadUByte();
		virtual short ReadShortLE();
		virtual unsigned short ReadUShortLE();
		virtual int ReadIntLE();
		virtual unsigned int ReadUIntLE();

		//functions for reading big endian data
		virtual short ReadShortBE();
		virtual unsigned short ReadUShortBE();
		virtual int ReadIntBE();
		virtual unsigned int ReadUIntBE();
		
		//virtual char* readUTF(MemoryType memType = POOL_BOTTOM);

		virtual int Write(const void* pBuffer, int length);
		virtual void WriteByte(int v);
		virtual void WriteShortLE(int v);
		virtual void WriteIntLE(int v);
		virtual void WriteShortBE(int v);	//added by Lu Yan
		virtual void WriteIntBE(int v);	//added by Lu Yan

		virtual void Close();

		//virtual bool FreeBottom();

	protected:
		unsigned char *m_data;
		unsigned char *m_current;	
		int m_length;
		bool m_needFreeBuffer;

	private:
		CGLLiveMemoryStream& operator=(const CGLLiveMemoryStream& rhs);
	};

	typedef CGLLiveMemoryStream CResourceStream;
}
#endif
