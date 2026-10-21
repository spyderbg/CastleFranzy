#ifndef __ICOLLADAOBJECT_H__
#define __ICOLLADAOBJECT_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaDatabase.h"
#include "SDatabaseCollada.h"
 

namespace irr
{
namespace collada
{
	class IColladaMesh;
	class IMorphingMesh;
	class ISkinnedMesh;
	class IMaterial;
	class CColladaDatabase;

class IObject
{
public:

	IObject(const CColladaDatabase &database)
		: Collada(database)
	{
		UID = res::String::Empty();
	}

	// Same as ISceneNode uid ... should be generalized later
	const res::String& getUID() const
	{
		return UID;
	}

	const CColladaDatabase& getCollada() const
	{
		return Collada;
	}

protected:

	void setUID(const res::String& uid)
	{
		UID = uid;
	}

private:

	res::String UID;
	CColladaDatabase Collada;
};

}; // namespace collada
}; // namespace irr

#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
