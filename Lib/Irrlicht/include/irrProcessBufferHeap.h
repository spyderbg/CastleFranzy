//-*-c++-*-
#ifndef __C_PROCESS_BUFFER_HEAP_H_INCLUDED__
#define __C_PROCESS_BUFFER_HEAP_H_INCLUDED__

#include "irrTypes.h"
#include "irros.h"
#include <stddef.h>

namespace irr {

//! Result values for process buffer whole heap allocation.
enum E_PROCESS_BUFFER_HEAP_ALLOC_RESULT
{
	//! Allocation (i.e. setting the size) was successful.
	EPBHAR_SUCCESS,

	//! Allocation failed because the heap is currently in use.
	EPBHAR_IN_USE,

	//! Allocation failed because there is not enough free system memory.
	EPBHAR_OUT_OF_MEMORY
};

//! Reserves the total heap space (in bytes) for process buffers.
/** Changing size will fail if maxTotalSize is too large for the available
	system memory or, more importantly, if the process buffer heap is currently
	in use. */
E_PROCESS_BUFFER_HEAP_ALLOC_RESULT setProcessBufferHeapSize(int size);

//! Returns the available memory space in the process buffer heap.
/** \see releaseProcessBuffer(void*) */
int getProcessBufferHeapAvailableMemory();

#ifdef _DEBUG
void* allocProcessBufferDebug(int size,
							  const char* filename,
							  int line);
#    define allocProcessBuffer(size) \
	allocProcessBufferDebug((size), __FILE__, __LINE__)
#else
//! Allocates a process buffer of a given size (in bytes).
/** This is a O(1) operation. */
void* allocProcessBuffer(int size);
#endif

#ifdef _DEBUG
struct SDebugAllocProcessBufferByType
{
	const char* Filename;
	int Line;

	SDebugAllocProcessBufferByType(const char* filename,
								   int line)
		: Filename(filename),
		  Line(line)
	{
	}

	template < typename T >
	T* doAlloc(int count)
	{
		return reinterpret_cast<T*>(allocProcessBufferDebug(count * sizeof(T),
															Filename,
															Line));
	}
};
#    define allocProcessBufferOfType \
	SDebugAllocProcessBufferByType(__FILE__, __LINE__).doAlloc
#else
//! As allocProcessBuffer(int), but for an array of T of size count.
template < typename T >
inline T* allocProcessBufferOfType(int count)
{
	return reinterpret_cast<T*>(allocProcessBuffer(count * sizeof(T)));
}
#endif

//! Frees the allocated buffer.
/** This is a O(1) operation. Note that it is possible that after releasing a
	buffer, available memory does not increase. It will be the case if the
	buffer is not last allocated. Keep in mind that this heap is intended for
	temporary buffers. */
void releaseProcessBuffer(void* buffer);

//! Uses a logger to dump information on allocated process buffers.
/** Returns the number of allocated process buffers. */
int dumpProcessBufferAllocations(ILogger* dumpLogger,
								 ELOG_LEVEL logLvl = ELL_INFORMATION);

//! Dump information on allocated process buffers. (using the os::Printer's logger).
/** Returns the number of allocated process buffers. */
inline int
dumpProcessBufferAllocations(ELOG_LEVEL logLvl = ELL_INFORMATION)
{
	return dumpProcessBufferAllocations(os::Printer::Logger, logLvl);
}

/**
   Helper for automatic release when allocating a temporary buffer.

   Example:
   \code
   {
       SScopedBuffer buffer(1024);
	   // Alternative for above line:
	   // SScopedBuffer buffer(allocProcessBuffer(1024));

       do_something_with_buffer(buffer.get());
   }
   // here the buffer is released
   \endcode
*/
struct SScopedProcessBuffer
{
public:

	explicit SScopedProcessBuffer(void* buffer = NULL)
		: Buffer(buffer)
	{
	}

	explicit SScopedProcessBuffer(int size)
		: Buffer(allocProcessBuffer(size))
	{
	}

	~SScopedProcessBuffer()
	{
		reset((void*)0);
	}

	void reset(void* buffer)
	{
		if (Buffer)
		{
			releaseProcessBuffer(Buffer);
		}
		Buffer = buffer;
	}

	void reset(int size)
	{
		reset(allocProcessBuffer(size));
	}

	void* get() const
	{
		return Buffer;
	}

private:

	SScopedProcessBuffer(const SScopedProcessBuffer&);
	SScopedProcessBuffer& operator =(const SScopedProcessBuffer&);

	void* Buffer;
};

/**
   As SScopedProcessBuffer, but templated for ease of use with arrays of a
   uniform type (which should cover most use cases).
 */
template < typename T >
struct SScopedProcessArray : private SScopedProcessBuffer
{
public:

	explicit SScopedProcessArray(T* buffer = NULL)
		: SScopedProcessBuffer(buffer)
	{
	}

	explicit SScopedProcessArray(int count)
		: SScopedProcessBuffer(allocProcessBufferOfType<T>(count))
	{
	}

	~SScopedProcessArray()
	{
	}

	void reset(T* buffer = NULL)
	{
		SScopedProcessBuffer::reset(buffer);
	}

	void reset(int count)
	{
		reset(allocProcessBufferOfType<T>(count));
	}

	T* get() const
	{
		return reinterpret_cast<T*>(SScopedProcessBuffer::get());
	}

	T& operator [] (ptrdiff_t i)
	{
		return get()[i];
	}

	const T& operator [] (ptrdiff_t i) const
	{
		return get()[i];
	}
};

/**
 */
template <typename T>
class irrProcessBufferAllocator
{
public:

	T* allocate(size_t cnt)
	{
		return allocProcessBufferOfType<T>(cnt);
	}

	void deallocate(T* ptr)
	{
		if (ptr)
		{
			releaseProcessBuffer(ptr);
		}
	}

	void construct(T* ptr, const T& e)
	{
		new ((void*)ptr) T(e);
	}

	void destruct(T* ptr)
	{
		ptr->~T();
	}
};

} // end namespace irr

#endif // __C_PROCESS_BUFFER_HEAP_H_INCLUDED__
