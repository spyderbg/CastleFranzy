#include "stdafx.h"
#include "IVideoDriver.h"

namespace irr
{
namespace video
{

#ifdef _DEBUG
u8* LastProcessBufferAllocationRealStart = NULL;
u8* LastProcessBufferAllocationRealEnd = NULL;
#endif

IVideoDriver::ICompileData::~ICompileData()
{
}

void
IVideoDriver::flush()
{
	// Does nothing by default because some drivers might not implement batching
}

} // end namespace video
} // end namespace irr
