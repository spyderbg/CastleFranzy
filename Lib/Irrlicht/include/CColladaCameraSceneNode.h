//-*-c++-*-
#ifndef __CCOLLADACAMERASCENENODE_H__
#define __CCOLLADACAMERASCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CCameraSceneNode.h"
#include "SDatabaseCollada.h"
#include "SViewFrustum.h"
#include "IColladaRootSceneNode.h"

namespace irr
{
namespace collada
{

class CCameraSceneNode
	: public scene::CCameraSceneNode
	, public IObject
{
public:

	CCameraSceneNode(const CColladaDatabase& database, SCamera& carema);

	virtual ~CCameraSceneNode();

	virtual void OnRegisterSceneNode();

	void setTarget(ISceneNode* targetNode)
	{
		if (targetNode)
		{
			targetNode->grab();
		}
		if (TargetNode)
		{
			TargetNode->drop();
		}
		TargetNode = targetNode;
	}
	
	void attach(ISceneNode* root)
	{
		if (*Camera.target != '\0')
		{
			setTarget(root->getSceneNodeFromUID(Camera.target + 1));
		}
	}
	
	ISceneNode* getTargetNode()
	{
		return TargetNode;
	}

	const ISceneNode* getTargetNode() const
	{ 
		return TargetNode;
	}

	//! Returns type of the scene node
	virtual scene::ESCENE_NODE_TYPE getType() const;

	virtual const char * getUID() const;

protected:

	ISceneNode* TargetNode;
	SCamera& Camera;
};

} // end namespace collada
} // end namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADACAMERASCENENODE_H__
