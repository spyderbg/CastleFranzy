#include "stdafx.h"
#include "IrrlichtDevice.h"

#include "irrProcessBufferHeap.h"

namespace irr
{

namespace
{

u32 ProcessBufferInitCount = 0;

}

IrrlichtDevice::IrrlichtDevice()
{
	if (ProcessBufferInitCount == 0)
	{
		E_PROCESS_BUFFER_HEAP_ALLOC_RESULT result
			= setProcessBufferHeapSize(_IRR_PROCESS_BUFFER_HEAP_SIZE_);
		_IRR_DEBUG_BREAK_IF(result != EPBHAR_SUCCESS);
		++ProcessBufferInitCount;
	}
}

IrrlichtDevice::~IrrlichtDevice()
{
	-- ProcessBufferInitCount;
	if (ProcessBufferInitCount == 0)
	{
		E_PROCESS_BUFFER_HEAP_ALLOC_RESULT result
			= setProcessBufferHeapSize(0);
		if (result == EPBHAR_IN_USE)
		{
			os::Printer::log("Failed to free process buffer heap due to the "
							 "following allocated blocks:",
							 ELL_ERROR);
			dumpProcessBufferAllocations(ELL_ERROR);
		}
	}
}

void
IrrlichtDevice::setScreenCapture(bool activate)
{
}

} // end namespace irr
