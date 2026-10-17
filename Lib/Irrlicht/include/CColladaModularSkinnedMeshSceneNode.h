#ifndef __CCOLLADAMODULARSKINMESHSCENENODE_H__
#define __CCOLLADAMODULARSKINMESHSCENENODE_H__

#include "CColladaSkinnedMeshSceneNode.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
namespace irr
{

namespace collada
{

class CModularSkinnedMeshSceneNode
	: public CSkinnedMeshSceneNode
{
public:

	CModularSkinnedMeshSceneNode(scene::IColladaMesh* mesh,
						  IRootSceneNode* root,
						  SNode* node = 0,
						  s32 id = -1,
						  const core::vector3df& position = core::vector3df(0,0,0),
						  const core::quaternion& rotation = core::quaternion(0,0,0,1.0f),
						  const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

	virtual scene::ESCENE_NODE_TYPE getType() const;

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	int getCategoryId(const char *name) const;

	const char * getCategoryName(int categoryId) const;

	int getModuleId(const char *name) const;

	int getCurrentModuleId(int categoryId) const;

	const char * getCurrentModuleName(int categoryId) const;

	const char * getModuleName(int categoryId, int moduleId) const;

	int getCategoryCount() const;

	int getCategoryModuleCount(int categoryId) const;

	void setCategoryModule(int categoryId, int moduleId = -1);

	void setCategoryModule(const char *categoryName, const char *moduleName = 0);
};

} // namespace collada
} // namespace irr
#endif
#endif