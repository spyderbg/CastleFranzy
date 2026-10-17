#include "stdafx.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

void IMaterialRenderer::onSetMaterial(const SMaterial& material,
									  const SMaterial& lastMaterial,
									  bool resetAllRenderstates,
									  IMaterialRendererServices* services)
{
}

bool IMaterialRenderer::onRender(IMaterialRendererServices* service,
								 E_VERTEX_TYPE vtxtype)
{
	return true;
}

void IMaterialRenderer::onUnsetMaterial()
{
}

bool IMaterialRenderer::isTransparent() const
{
	return false;
}

s32 IMaterialRenderer::getRenderCapability() const
{
	return 0;
}

//------------------------------------------------------------------------------

u32 CNullMaterialRenderer::getVertexAttributeMask() const
{
	return 0;
}

} // end namespace video
} // end namespace irr
