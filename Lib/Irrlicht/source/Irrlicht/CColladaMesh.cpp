#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaMesh.h"
#include "CColladaMeshBuffer.h"
#include <SDatabaseCollada.h>


namespace irr
{
namespace scene
{

CColladaMesh::CColladaMesh(const collada::CColladaDatabase& database, const collada::SGeometry &geometry)
	: IColladaMesh(database)
	, Geometry(geometry)
{
	setUID(geometry.id);

	int meshBufferCount = Geometry.pMesh->meshBuffers.size();
	MeshBuffers.reallocate(meshBufferCount);
	Materials.reallocate(meshBufferCount);
	Materials.set_used(meshBufferCount);
	for(int i = 0; i < meshBufferCount; ++i)
	{
		CColladaMeshBuffer *meshBuffer = irrnew CColladaMeshBuffer(Geometry.pMesh->meshBuffers[i], *Geometry.pMesh);
		MeshBuffers.push_back(meshBuffer);
	}
	BoundingBox = Geometry.pMesh->boundingBox;
}

CColladaMesh::~CColladaMesh()
{
	for (u32 i = 0, sz = MeshBuffers.size(); i < sz; ++i)
	{
		MeshBuffers[i]->drop();
		MeshBuffers[i] = NULL;
	}
}

IColladaMesh::E_TYPE CColladaMesh::getType() const
{
	return ET_MESH;
}

u32 CColladaMesh::getMeshBufferCount() const
{
	return MeshBuffers.size();
}

IMeshBuffer* CColladaMesh::getMeshBuffer(u32 nr) const
{
	return MeshBuffers[nr];
}

IMeshBuffer* CColladaMesh::getMeshBuffer(const video::SMaterial& material) const
{
	/** Not implemented */
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	return 0;
}

const core::aabbox3d<f32>& CColladaMesh::getBoundingBox() const
{
	return BoundingBox;
}

void CColladaMesh::setBoundingBox( const core::aabbox3df& box)
{
	BoundingBox = box;
}

void CColladaMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	/** Not implemented */
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
}

core::stringc CColladaMesh::getUserProperty() const
{
	return "";
}

}
}

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
