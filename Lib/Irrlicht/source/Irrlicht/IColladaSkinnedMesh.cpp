#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSkinnedMesh.h"
#include "IVideoDriver.h"

namespace irr
{
namespace scene
{

void
IColladaSkinnedMesh::setIsSkinningEnabled(bool value)
{
	IsSkinningEnabled = value;
}

void
IColladaSkinnedMesh::releaseProcessBuffer(video::IVideoDriver* driver,
										  u32 buffer)
{
	// ensure skinning is enabled just in case we are using a process buffer
	bool skinningEnabled = isSkinningEnabled();
	if (!skinningEnabled)
	{
		setIsSkinningEnabled(true);
	}
	IMeshBuffer* mb = getMeshBuffer(buffer);
	video::S3DVertexComponentArrays* components
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			mb->getVertices()
		);
	driver->releaseProcessBuffer(video::EPBT_DYNAMIC,
								 components->Position,
								 mb->getVertexIndexStart(),
								 components->PositionStride);
	components->Position = NULL;
	components->Normal = NULL;
	HasOutputBuffer = false;
	if (!skinningEnabled)
	{
		setIsSkinningEnabled(false);
	}
}

IMeshBuffer*
IColladaSkinnedMesh::getMeshBuffer(const video::SMaterial& material) const
{
	return 0;
}

void
IColladaSkinnedMesh::setBoundingBox(const core::aabbox3df& box)
{
	_IRR_DEBUG_BREAK_IF("UNIMPLEMENTED");
	//m_pSource->setBoundingBox(box);
}

void
IColladaSkinnedMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	_IRR_DEBUG_BREAK_IF("UNIMPLEMENTED");
	//m_pSource->setMaterialFlag(flag, newvalue);
}

core::stringc
IColladaSkinnedMesh::getUserProperty() const
{
	return "";
}

} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
