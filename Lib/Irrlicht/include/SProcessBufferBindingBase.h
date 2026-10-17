//-*-c++-*-
#ifndef __S_VIDEO_PROCESS_BUFFER_BINDING_BASE_H_INCLUDED__
#define __S_VIDEO_PROCESS_BUFFER_BINDING_BASE_H_INCLUDED__

#include "IVideoDriver.h"
#include "irrProcessBufferHeap.h"

namespace irr
{
namespace video
{

#ifdef _DEBUG
extern u8* LastProcessBufferAllocationRealStart;
extern u8* LastProcessBufferAllocationRealEnd;
#endif

namespace detail
{

u32 getStrides(u32 attributes,
			   S3DVertexComponentArrays& components);

void assignPointers(u8* buffer,
					u32 stride,
					u32 attributes,
					S3DVertexComponentArrays& components);

} // end namespace detail

template <typename Allocator>
inline
u8*
allocateProcessBuffer(u32 vertexStart,
					  u32 vertexEnd,
					  u32 attributes,
					  S3DVertexComponentArrays& components,
					  Allocator allocator,
					  u32& stride)
{
	if (attributes == 0 || vertexEnd == vertexStart)
	{
		return NULL;
	}
	// Compute stride and offsets
	stride = detail::getStrides(attributes, components);

	// Allocate the buffer
	u8* allocatedBuffer = reinterpret_cast<u8*>(
		allocator((vertexEnd - vertexStart) * stride)
	);
#ifdef _DEBUG
	LastProcessBufferAllocationRealStart
		= allocatedBuffer;
	LastProcessBufferAllocationRealEnd
		= allocatedBuffer + ((vertexEnd - vertexStart) * stride);
#endif
	u8* buffer = allocatedBuffer - (vertexStart * stride);

	// Assign addresses and stride
	if (buffer)
	{
		detail::assignPointers(buffer, stride, attributes, components);
	}
	return buffer;
}

//!
template <typename Derived,
		  typename Base>
struct SProcessBufferBindingBase
	: Base
{
protected:

	Derived* this_()
	{
		return static_cast<Derived*>(this);
	}

	const Derived* this_() const
	{
		return static_cast<const Derived*>(this);
	}

	SProcessBufferBindingBase()
		: ProcessBuffer(NULL)
		, ProcessBufferStart(0)
		, ProcessBufferEnd(0)
		, ProcessBufferAttributes(0)
		, ProcessBufferStride(0)
	{
	}

public:

	u8* ProcessBuffer;
	u32 ProcessBufferStart;
	u32 ProcessBufferEnd;
	u32 ProcessBufferAttributes;
	u32 ProcessBufferStride;

	struct SNewAllocator
	{
		void* operator () (u32 size) const
		{
			return irrnew u8 [size];
		}
	};
 
	struct SProcessBufferHeapAllocator
	{
		void* operator () (u32 size) const
		{
			return allocProcessBuffer(size);
		}
	};

	struct SReuseAllocator
	{
		void* Buffer;

		SReuseAllocator(void* buffer)
			: Buffer(buffer)
		{
		}

		void* operator () (u32 /*size*/) const
		{
			return Buffer;
		}
	};

	~SProcessBufferBindingBase()
	{
		if (ProcessBuffer)
		{
			this_()->getDriver()->releaseProcessBuffer(EPBT_STATIC, this_());
		}
	}
	
	void clearProcessBuffer()
	{
		if (ProcessBuffer != NULL)
		{
			delete (ProcessBuffer + ProcessBufferStart * ProcessBufferStride);
			ProcessBuffer = NULL;
			ProcessBufferStart = 0;
			ProcessBufferEnd = 0;
			ProcessBufferAttributes = 0;
			ProcessBufferStride = 0;
		}
	}

	bool processBufferMatches(u32 vertexStart,
							  u32 vertexEnd,
							  u32 attributes) const
	{
		return (vertexStart == ProcessBufferStart
				&& vertexEnd == ProcessBufferEnd
				&& attributes == ProcessBufferAttributes);
	}

	static SProcessBufferHeapAllocator getProcessBufferHeapAllocator()
	{
		return SProcessBufferHeapAllocator();
	}

	static SNewAllocator getNewAllocator()
	{
		return SNewAllocator();
	}

	SReuseAllocator getReuseAllocator() const
	{
		return SReuseAllocator(
			ProcessBuffer + ProcessBufferStart * ProcessBufferStride
		);
	}

	E_DRIVER_ALLOCATION_RESULT getProcessBuffer(u32 vertexStart,
												u32 vertexEnd,
												u32 attributes,
												S3DVertexComponentArrays& components,
												bool allocate = false)
	{
		E_DRIVER_ALLOCATION_RESULT result = EDAR_SUCCESS;
		bool configMatches = processBufferMatches(vertexStart,
												  vertexEnd,
												  attributes);
		if (ProcessBuffer == NULL || !configMatches)
		{
			if (allocate)
			{
				ProcessBuffer = irr::video::allocateProcessBuffer(
					vertexStart,
					vertexEnd,
					attributes,
					components,
					getNewAllocator(),
					ProcessBufferStride
				);
				if (ProcessBuffer == NULL)
				{
					result = EDAR_FAILED;
				}
				ProcessBufferStart = vertexStart;
				ProcessBufferEnd = vertexEnd;
				ProcessBufferAttributes = attributes;
			}
			else if (ProcessBuffer == NULL)
			{
				result = EDAR_NOT_FOUND;
			}
			else
			{
				_IRR_DEBUG_BREAK_IF(configMatches);
				result = EDAR_MISMATCH;
			}
		}
		else
		{
			irr::video::allocateProcessBuffer(
				vertexStart,
				vertexEnd,
				attributes,
				components,
				getReuseAllocator(),
				ProcessBufferStride
			);
		}
		return result;
	}
};

} // end namespace video
} // end namespace irr

#endif
