#ifndef __CSKINNEDMESHSCENENODE_H__
#define __CSKINNEDMESHSCENENODE_H__

//! Temporary use AnimatedMeshSceneNode
/** We should define a better interface
*/
#include "CAnimatedMeshSceneNode.h"

namespace irr
{
namespace scene
{

class CSkinnedMeshSceneNode
	: public CAnimatedMeshSceneNode
{
public:
	//! constructor
	CSkinnedMeshSceneNode(IAnimatedMesh* mesh, s32 id = -1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::quaternion& rotation = core::quaternion(0,0,0, 1.0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
		: CAnimatedMeshSceneNode(mesh, id, position, rotation, scale)
	{

	}

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
	virtual const core::aabbox3d<f32>& getTransformedBoundingBox() const
	{
		return getBoundingBox();
	}
#else
	virtual const core::aabbox3d<f32> getTransformedBoundingBox() const
	{
		core::aabbox3d<f32> box = getBoundingBox();
		return box;
	}
#endif

	virtual void render(void* renderData);

private:
	mutable core::matrix4 m_AbsoluteTransformation;
}; // class CSkinnedMeshSceneNode

}; // namespace scene
}; // namespace irr

#endif
