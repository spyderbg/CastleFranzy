#ifndef __CCOLLADALIGHTSCENENODE_H__
#define __CCOLLADALIGHTSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CLightSceneNode.h"
#include "IColladaObject.h"
#include "resFile.h"

namespace irr
{
namespace collada
{
struct SLight;

class CLightSceneNode
	: public scene::CLightSceneNode
	, public IObject
{
public:
	CLightSceneNode(const CColladaDatabase& database, SLight &light);

	virtual const char * getUID() const;
	
	res::String	&getLightID() const;
protected:
	SLight &m_Light;
}; // class CLightSceneNode
}; // namespace collada
}; // namespace irr
#endif
#endif // __CCOLLADALIGHTSCENENODE_H__