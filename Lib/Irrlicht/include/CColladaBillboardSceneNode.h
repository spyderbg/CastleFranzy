//-*-c++-*-
#ifndef __CCOLLADABILLBOARDSCENENODE_H__
#define __CCOLLADABILLBOARDSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNode.h"

namespace irr
{
namespace collada
{

class CBillboardSceneNode
	: public CSceneNode
{
public:

	CBillboardSceneNode(const CColladaDatabase& database, SNode* node = 0)
		: CSceneNode(database, node)
	{
		_IRR_DEBUG_BREAK_IF(Node->pBillboard == 0);
	}

	virtual void updateAbsolutePosition(bool updateChildren);
};

} // namespace collada
} // namespace irr
#endif
#endif // __CCOLLADABILLBOARDSCENENODE_H__
