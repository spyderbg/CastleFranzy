//-*-c++-*-
#ifndef __CCOLLADASKINNEDMESHSCENENODE_H__
#define __CCOLLADASKINNEDMESHSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaMeshSceneNode.h"
#include "ECullingTypes.h"
#include "IColladaSkinnedMesh.h"

namespace irr
{
namespace scene
{
	class IColladaMesh;
}

namespace collada
{

class CSkinnedMeshSceneNode
	: public scene::CColladaMeshSceneNode
{
public:

	CSkinnedMeshSceneNode(scene::IColladaMesh* mesh,
						  IRootSceneNode* root,
						  SNode* node = 0,
						  s32 id = -1,
						  const core::vector3df& position = core::vector3df(0,0,0),
						  const core::quaternion& rotation = core::quaternion(0,0,0,1.0f),
						  const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

	virtual scene::ESCENE_NODE_TYPE getType() const;

	virtual void OnAnimate(u32 timeMs);

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
	virtual const core::aabbox3d<f32>& getTransformedBoundingBox() const;
#else
	virtual const core::aabbox3d<f32> getTransformedBoundingBox() const;
#endif

	virtual void render(void* renderData = 0);

	bool isSkinningEnabled() const
	{
		return static_cast<const scene::IColladaSkinnedMesh*>(Mesh)->isSkinningEnabled();
	}

	void setIsSkinningEnabled(bool value);


	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		scene::CColladaMeshSceneNode::serializeAttributes(out, options);
	}

	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		scene::CColladaMeshSceneNode::deserializeAttributes(in, options);
	}
};

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
#endif
