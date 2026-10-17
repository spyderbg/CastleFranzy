#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaMesh.h"

namespace irr
{
namespace scene
{

void
IColladaMesh::onAnimate(u32 timeMs)
{
}

video::E_DRIVER_ALLOCATION_RESULT
IColladaMesh::onPrepareBufferForRendering(E_PREPARE_BUFFER_STEP step,
										  video::IVideoDriver* driver,
										  u32 buffer)
{
	return video::EDAR_NOT_FOUND;
}

void
IColladaMesh::releaseProcessBuffer(video::IVideoDriver* driver,
								   u32 buffer)
{
}

} // end namespace scene
} // end namespace irr

#endif