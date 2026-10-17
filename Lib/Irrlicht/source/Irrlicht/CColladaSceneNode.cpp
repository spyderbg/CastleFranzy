#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNode.h"

namespace irr
{
namespace collada
{

CSceneNode::CSceneNode(const CColladaDatabase& database, SNode* node)
	: IObject(database)
	, Node(node)
{
	if (node)
	{
		// Has initial state
		setName(Node->name);
		setPosition(Node->position);
		setRotation(Node->rotation);
		setScale(Node->scale);
	}
}

void
CSceneNode::resetTransform(bool resetChildren)
{
	if (Node)
	{
		// Has initial state
		setPosition(Node->position);
		setRotation(Node->rotation);
		setScale(Node->scale);
	}

	if (resetChildren)
	{
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->resetTransform(true);
	}
}

scene::ESCENE_NODE_TYPE
CSceneNode::getType() const
{
	return scene::ESNT_COLLADA_NODE;
}

bool
CSceneNode::computeBoundingBox(core::aabbox3d<f32>& bbox) const
{
	bool foundMesh = false;
	const irr::core::list<irr::scene::ISceneNode*>& list = getChildren();
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it;
		
	for (it = list.begin(); it != list.end(); ++it)
	{
		if ((*it)->getType() == scene::ESNT_COLLADA_MESH)
		{
			if (foundMesh)
			{
				bbox = (*it)->getTransformedBoundingBox();
				foundMesh = true;
			}
			else
			{
				bbox.addInternalBox((*it)->getTransformedBoundingBox());
			}
		}
		else if ((*it)->getType() == scene::ESNT_COLLADA_NODE)
		{
			core::aabbox3d<f32> newbbox;
			if (((const CSceneNode*)(*it))->computeBoundingBox(newbbox))
			{
				if (!foundMesh)
				{
					bbox = newbbox;
					foundMesh = true;
				}
				else
				{
					bbox.addInternalBox(newbbox);
				}
			}
		}
	}

	return foundMesh;
}

const char*
CSceneNode::getUID() const
{
	if (Node)
	{
		return Node->id;
	}
	else
	{
		return "";
	}
}

const char*
CSceneNode::getScopeID() const
{
	if (Node)
	{
			return Node->sid;
	}
	else
	{
		return "";
	}
}

const void*
CSceneNode::getUserProperty() const
{
	if (Node)
	{
		return Node->pUserProperty;
	}
	else
	{
		return 0;
	}
}

const void*
CSceneNode::getUserPropertyStr() const
{
	if (Node && Node->pUserPropertyStr)
	{
		return *Node->pUserPropertyStr;
	}
	else
	{
		return 0;
	}
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
