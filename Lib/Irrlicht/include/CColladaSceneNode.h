//-*-c++-*-
#ifndef __C_COLLADA_SCENE_NODE_H_INCLUDED__
#define __C_COLLADA_SCENE_NODE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CEmptySceneNode.h"
#include "SDatabaseCollada.h"
#include "CColladaDatabase.h"
#include "IColladaObject.h"
#include "ESceneNodeTypes.h"

namespace irr
{
namespace scene
{

class CEmptySceneNode;

}

namespace collada
{

class CSceneNode
	: public scene::CEmptySceneNode
	, public IObject
{
protected:

	SNode* Node;

public:

	CSceneNode(const CColladaDatabase& database, SNode* node = 0);

	virtual scene::ESCENE_NODE_TYPE getType() const;

	bool computeBoundingBox(core::aabbox3d<f32>& bbox) const;

	virtual const char* getUID() const;
	virtual const char* getScopeID() const;
	virtual const void* getUserProperty() const;
	virtual const void* getUserPropertyStr() const;

	SNode* getNode() const
	{
		return Node;
	}

	void resetTransform(bool resetChildren);
};

} // end namespace collada
} // end namespace irr

#endif  //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
