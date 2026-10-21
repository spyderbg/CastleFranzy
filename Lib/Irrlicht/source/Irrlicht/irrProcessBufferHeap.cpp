#include "stdafx.h"
#include "irrProcessBufferHeap.h"

#ifdef _DEBUG
#    include <set>
#    include "irrAllocator.h"
#endif

namespace irr {

// Implementation note
//
// The class below is now burried here to hide singleton implementation because
// it was though that it is better to write
//
// allocProcessBuffer(size)
//
// instead of something like
//
// CProcessBufferHeap::getInstance().alloc(size)

/**
   This class provides a simple heap designed to be used for temporary process
   buffers. Each allocation and deallocation takes O(1) time.

   \todo Allocation hints.
 */
class CProcessBufferHeap
{	
public:
	
	explicit CProcessBufferHeap(int initSize = 0);
	~CProcessBufferHeap();

	//! Returns the maximum number of bytes allocatable.
	int getSize() const;

	//! Sets the total available memory for process buffers.
	/** Changing size will fail if maxTotalSize is too large for the available
		system memory or, more importantly, if the process buffer heap is
		currently in use. */
	E_PROCESS_BUFFER_HEAP_ALLOC_RESULT setSize(int maxTotalSize);

	//! Returns the number of bytes still available.
	int getAvailable() const;

	//! Returns a buffer of given size (in bytes).
	void* alloc(int size
#ifdef _DEBUG
				, const char* filename = 0, int line = 0
#endif
	);

	//! Frees the buffer, which must have previously been allocated with alloc(int).
	void release(void* buffer);

	//! Uses a logger to dump information on allocated process buffers.
	/** Returns the number of allocated process buffers. */
	int dump(ILogger* dumpLogger, ELOG_LEVEL logLvl = ELL_INFORMATION) const;

	//! Dump information on allocated process buffers (using the os::Printer's
	//! logger).
	/** Returns the number of allocated process buffers. */
	int dump(ELOG_LEVEL logLvl = ELL_INFORMATION) const;

private:

	s32* BufferStart;
	s32* BufferEnd;
	s32* NextBuffer;
	int RequestedSize;

#ifdef _DEBUG
	typedef std::set<u8*, std::less<u8*>, core::irrAllocator<u8*> > SystemAllocationSet;
	mutable SystemAllocationSet* SystemAllocations;

	SystemAllocationSet& getSystemAllocations()
	{
		if (SystemAllocations == NULL)
		{
			SystemAllocations = irrnew SystemAllocationSet;
		}
		return *SystemAllocations;
	}
#endif
};

#ifdef _DEBUG
// extra info in debug for allocation location; this should make the code
// 64-bit-friendly
#    define IRR_DEBUG_INFO_SIZE (1 + (sizeof(const char*) + sizeof(s32) - 1) / sizeof(s32))
#else
#    define IRR_DEBUG_INFO_SIZE 0
#endif

inline
int
CProcessBufferHeap::getSize() const
{
	return (reinterpret_cast<u8*>(BufferEnd)
			- reinterpret_cast<u8*>(BufferStart));
}

inline
int
CProcessBufferHeap::getAvailable() const
{
	return (BufferEnd - NextBuffer - (2 + IRR_DEBUG_INFO_SIZE)) * sizeof(s32);
}

inline
int
CProcessBufferHeap::dump(ELOG_LEVEL logLvl) const
{
	return dump(os::Printer::Logger, logLvl);
}

#undef IRR_DEBUG_INFO_SIZE

////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
// extra info in debug for allocation location; this should make the code
// 64-bit-friendly
#    define IRR_DEBUG_INFO_SIZE (1 + (sizeof(const char*) + sizeof(s32) - 1) / sizeof(s32))
#else
#    define IRR_DEBUG_INFO_SIZE 0
#endif

CProcessBufferHeap::CProcessBufferHeap(int initSize)
	: BufferStart(NULL)
	, BufferEnd(NULL)
	, NextBuffer(NULL)
#ifdef _DEBUG
	, SystemAllocations(NULL)
#endif
{
	RequestedSize = initSize;
	//setSize(initSize);
}

CProcessBufferHeap::~CProcessBufferHeap()
{
	if (setSize(0) == EPBHAR_IN_USE)
	{
		_IRR_DEBUG_BREAK_IF(BufferStart == NULL);
		os::Printer::log("Destroying process buffer heap with allocated blocks:",
						 ELL_ERROR);
		dump(ELL_ERROR);
		// Force deallocation, we still do not want real memory leaks
		delete[] (BufferStart - 1); // see comments in setSize for that -1
	}
#ifdef _DEBUG
	if (SystemAllocations)
	{
		delete SystemAllocations;
		SystemAllocations = NULL;
	}
#endif
}

E_PROCESS_BUFFER_HEAP_ALLOC_RESULT
CProcessBufferHeap::setSize(int maxTotalSize)
{
	maxTotalSize = (maxTotalSize + (sizeof(s32) - 1)) / sizeof(s32);

	if (BufferStart)
	{
		// already the right size?
		if (BufferEnd - BufferStart == maxTotalSize)
		{
			return EPBHAR_SUCCESS;
		}
		// are there any allocations?
		else if (NextBuffer > BufferStart)
		{
			return EPBHAR_IN_USE;
		}
		else
		{
			// see below
			delete[] (BufferStart - 1);
			BufferStart = BufferEnd = NextBuffer = 0;
		}
	}

	if (maxTotalSize > 0)
	{
		// add 1 to size because we sacrifice a dummy s32 block to reduce the number
		// of ifs
		BufferStart = irrnew s32 [maxTotalSize + 1];
		if (BufferStart)
		{
			BufferStart[0] = 0;
			++BufferStart;
			NextBuffer = BufferStart;
			BufferEnd = BufferStart + maxTotalSize;
		}
		else
		{
			return EPBHAR_OUT_OF_MEMORY;
		}
	}
	return EPBHAR_SUCCESS;
}

void*
CProcessBufferHeap::alloc(int size
#ifdef _DEBUG
						  , const char* filename
						  , int line
#endif
)
{
	if(BufferStart == 0)
		setSize(RequestedSize);

	_IRR_DEBUG_BREAK_IF(BufferStart == NULL);
	_IRR_DEBUG_BREAK_IF(size <= 0);

	// add 2 (for info at start and end) and round up to higher multiple of
	// sizeof(s32)
	int heapsize = ((size + (sizeof(s32) - 1)) / sizeof(s32)
					+ 2 + IRR_DEBUG_INFO_SIZE);

	// is there enough size?
	s32* buffer = NULL;
	if (heapsize <= BufferEnd - NextBuffer)
	{
		s32* buffer = NextBuffer;
		buffer[0] = heapsize;
#ifdef _DEBUG
		buffer[1] = line;
		reinterpret_cast<const char**>(&buffer[2])[0] = filename;
#endif
		NextBuffer += heapsize;
		NextBuffer[-1] = heapsize;
		return buffer + (1 + IRR_DEBUG_INFO_SIZE);
	}

#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	// if allocation failed in the process heap, then allocate it in the system
	// heap
	u8* sysbuf = irrnew u8 [size];
#    ifdef _DEBUG
	char msg[2048];
	sprintf(msg,
			"process buffer heap exceeded at %s:%d "
			"(request: %d, left: %d, max: %d); using irrnew",
			filename,
			line,
			size,
			(BufferEnd - NextBuffer) * sizeof(s32),
			(BufferEnd - BufferStart) * sizeof(s32));
	os::Printer::log(msg, "Warning:", ELL_WARNING);
	std::pair<SystemAllocationSet::iterator, bool>
		i = getSystemAllocations().insert(sysbuf);
	_IRR_DEBUG_BREAK_IF(!i.second);
#    endif
	return sysbuf;
#else
	return NULL;
#endif
}

void
CProcessBufferHeap::release(void* bufferToRelease)
{
#ifdef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	// if the buffer was allocated outside the range of the process heap, we
	// then delete it in the system heap
	if (reinterpret_cast<s32*>(bufferToRelease) < BufferStart
		|| reinterpret_cast<s32*>(bufferToRelease) >= BufferEnd)
	{
#    ifdef _DEBUG
		SystemAllocationSet::iterator i = getSystemAllocations().find(
			reinterpret_cast<u8*>(bufferToRelease)
		);
		_IRR_DEBUG_BREAK_IF(i == getSystemAllocations().end());
		getSystemAllocations().erase(i);
#    endif
		delete[] reinterpret_cast<u8*>(bufferToRelease);
		return;
	}
#endif

	_IRR_DEBUG_BREAK_IF(bufferToRelease == NULL);
	_IRR_DEBUG_BREAK_IF(NextBuffer == BufferStart);

	s32* buffer = reinterpret_cast<s32*>(bufferToRelease) - 1 - IRR_DEBUG_INFO_SIZE;

	// validate address
	_IRR_DEBUG_BREAK_IF(buffer < BufferStart || buffer >= BufferEnd);

	// mark as free
	s32 size = -buffer[0];
	buffer[0] = size;
	buffer[-1 - size] = size;

	// can we pop (i.e. liberate)?
	s32 size2 = NextBuffer[-1];
	if (size2 < 0)
	{
		NextBuffer += size2;
		size2 = NextBuffer[-1];
		// check if previous block is free as well
		if (size2 < 0)
		{
			NextBuffer += size2;
			_IRR_DEBUG_BREAK_IF(NextBuffer[-1] < 0);
		}
	}
	else
	{
		// note for below: all size values are negative

		// check if we can merge with a previous free block
		size2 = buffer[-1];
		if (size2 < 0)
		{
			size += size2;
			buffer += size2;
			buffer[0] = size;
			buffer[-1 - size] = size;
		}
		// we can check to merge with the next free block
		_IRR_DEBUG_BREAK_IF(buffer - size >= NextBuffer);
		size2 = buffer[-size];
		if (size2 < 0)
		{
			size += size2;
			buffer[0] = size;
			buffer[-1 - size] = size;
		}
	}
}

int
CProcessBufferHeap::dump(ILogger* dumpLogger, ELOG_LEVEL logLvl) const
{
	c8 tmp[1024];
	s32* buffer = BufferStart;
	int allocCount = 0;
	while (buffer != NextBuffer)
	{
		s32 size = buffer[0];
		if (size > 0)
		{
#ifdef _DEBUG
			sprintf(tmp,
					"%p: %8zu @ %s:%d",
					static_cast<void*>(buffer + 1 + IRR_DEBUG_INFO_SIZE),
					(size - 2 - IRR_DEBUG_INFO_SIZE) * sizeof(s32),
					reinterpret_cast<const char**>(&buffer[2])[0],
					buffer[1]
			);
#else
			sprintf(tmp,
					"%p: %8zu",
					static_cast<void*>(buffer + 1),
					(size - 2) * sizeof(s32)
			);
#endif
			dumpLogger->log(tmp, logLvl);
			++allocCount;
		}
		else
		{
			size = -size;
		}
		buffer += size;
	}
	return allocCount;
}

#ifdef _DEBUG
#    undef IRR_DEBUG_INFO_SIZE
#endif

// end of CProcessBufferHeap implementation ------------------------------------

////////////////////////////////////////////////////////////////////////////////

namespace {

CProcessBufferHeap ProcessBufferHeap;

}

E_PROCESS_BUFFER_HEAP_ALLOC_RESULT
setProcessBufferHeapSize(int size)
{
	return ProcessBufferHeap.setSize(size);
}

int
getProcessBufferHeapAvailableMemory()
{
	return ProcessBufferHeap.getAvailable();
}

#ifdef _DEBUG
void*
allocProcessBufferDebug(int size, const char* filename, int line)
{
	return ProcessBufferHeap.alloc(size, filename, line);
}
#else
void*
allocProcessBuffer(int size)
{
	return ProcessBufferHeap.alloc(size);
}
#endif

void
releaseProcessBuffer(void* buffer)
{
	ProcessBufferHeap.release(buffer);
}

int
dumpProcessBufferAllocations(ILogger* dumpLogger, ELOG_LEVEL logLvl)
{
	return ProcessBufferHeap.dump(dumpLogger, logLvl);
}

} // end namespace irr
