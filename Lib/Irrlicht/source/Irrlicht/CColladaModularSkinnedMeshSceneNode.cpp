#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaModularSkinnedMeshSceneNode.h"
#include "CColladaModularSkinnedMesh.h"

using namespace irr;
using namespace collada;


CModularSkinnedMeshSceneNode::CModularSkinnedMeshSceneNode(
	scene::IColladaMesh* mesh,
	IRootSceneNode* root,
	SNode* node,
	s32 id,
	const core::vector3df& position,
	const core::quaternion& rotation,
	const core::vector3df& scale
)
	: CSkinnedMeshSceneNode(mesh, root, node, id, position, rotation, scale)
{
#ifdef _DEBUG
	setDebugName("CModularSkinnedMeshSceneNode");
#endif
}

scene::ESCENE_NODE_TYPE
CModularSkinnedMeshSceneNode::getType() const
{
	return scene::ESNT_COLLADA_MODULAR_SKIN_MESH;
}

void 
CModularSkinnedMeshSceneNode::serializeAttributes(io::IAttributes* out, 
												  io::SAttributeReadWriteOptions* options) const
{
	CSkinnedMeshSceneNode::serializeAttributes(out, options);
	core::array<const char *> temp;

	for(int i = 0; i < getCategoryCount(); i++)
	{
		temp.clear();
		for(int j = 0; j < getCategoryModuleCount(i); j++)
		{
			temp.push_back(getModuleName(i, j));
		}
		temp.push_back("not used (-1)");
		temp.push_back(0);
		out->addEnum(getCategoryName(i), getCurrentModuleId(i), &temp[0]);
	}
}

void 
CModularSkinnedMeshSceneNode::deserializeAttributes(io::IAttributes* in, 
													io::SAttributeReadWriteOptions* options)
{
	CSkinnedMeshSceneNode::deserializeAttributes(in, options);
	for(int i = 0; i < getCategoryCount(); i++)
	{
		setCategoryModule(i, getModuleId(in->getAttributeAsEnumeration(getCategoryName(i))));
	}
}

int 
CModularSkinnedMeshSceneNode::getCategoryId(const char *name) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCategoryId(name);
}

const char * 
CModularSkinnedMeshSceneNode::getCategoryName(int categoryId) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCategoryName(categoryId);
}

int 
CModularSkinnedMeshSceneNode::getModuleId(const char *name) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getModuleId(name);
}

int 
CModularSkinnedMeshSceneNode::getCurrentModuleId(int categoryId) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCurrentModuleId(categoryId);
}

const char * 
CModularSkinnedMeshSceneNode::getCurrentModuleName(int categoryId) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCurrentModuleName(categoryId);
}	

const char * 
CModularSkinnedMeshSceneNode::getModuleName(int categoryId, int moduleId) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getModuleName(categoryId, moduleId);
}

int 
CModularSkinnedMeshSceneNode::getCategoryCount() const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCategoryCount();
}

int 
CModularSkinnedMeshSceneNode::getCategoryModuleCount(int categoryId) const
{
	return ((scene::CColladaModularSkinnedMesh *)Mesh)->getCategoryModuleCount(categoryId);
}

void 
CModularSkinnedMeshSceneNode::setCategoryModule(int categoryId, int moduleId)
{
	((scene::CColladaModularSkinnedMesh *)Mesh)->setCategoryModule(categoryId, moduleId);
	prepareMaterial();
}

void 
CModularSkinnedMeshSceneNode::setCategoryModule(const char *categoryName, const char *moduleName)
{
	((scene::CColladaModularSkinnedMesh *)Mesh)->setCategoryModule(categoryName, moduleName);
	prepareMaterial();
}
#endif