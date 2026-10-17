#include "stdafx.h"
#include "IrrCompileConfig.h"
#include "CMeshCollision.h"

#include "IMesh.h"
#include "IMeshBuffer.h"

#include "ISceneNode.h"
#include "IAnimatedMeshSceneNode.h"
#include "CMeshSceneNode.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
#include "CColladaSkinnedMeshSceneNode.h"
#endif

namespace irr
{
namespace scene
{

//! constructor
CMeshCollision::CMeshCollision(const IMesh* mesh, 
							   const ISceneNode* node)
	: SceneNode(node)
	, Mesh(mesh)
	, MeshIsInWorldspace(false)
{
}

//! constructor
CMeshCollision::CMeshCollision(const ISceneNode* node)
	: SceneNode(node)
	, Mesh(NULL)
	, MeshIsInWorldspace(false)
{
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
	collada::CSkinnedMeshSceneNode* skinnedNode = NULL;
#endif

	switch(SceneNode->getType())
	{
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
	case ESNT_COLLADA_SKIN_MESH:
	case ESNT_COLLADA_MODULAR_SKIN_MESH:
		skinnedNode = (collada::CSkinnedMeshSceneNode*)SceneNode;
		// support skinned mesh when they have a working buffer
		if (skinnedNode->isSkinningEnabled() || 
			((IColladaMesh*)skinnedNode->getMesh())->hasOutputBuffer())
		{
			// skinned mesh is in world space
			MeshIsInWorldspace = true;
		}
		Mesh = skinnedNode->getMesh();

		// Skinned mesh without output buffer are not supported...
		if (skinnedNode->isSkinningEnabled() && 
			!((IColladaMesh*)skinnedNode->getMesh())->hasOutputBuffer())
		{
			Mesh = NULL;
		}
		break;

	case ESNT_COLLADA_MESH:
#endif
	case ESNT_MESH:
	case ESNT_BATCH_SCENE_NODE:
	case ESNT_BATCH_GRID_SCENE_NODE:
		Mesh = ((IMeshSceneNode*)SceneNode)->getMesh();
		break;
		
	case ESNT_ANIMATED_MESH:
		IAnimatedMeshSceneNode* node = (IAnimatedMeshSceneNode*)SceneNode;
		Mesh = node->getMesh()->getMesh((s32)node->getFrameNr(),255, node->getStartFrame(), node->getEndFrame());
		break;
	}
}

bool 
CMeshCollision::isRayCollideMesh(const core::line3df& ray) const
{
	//_IRR_DEBUG_BREAK_IF(Mesh == NULL || SceneNode == NULL);
	if(Mesh == NULL || SceneNode == NULL)
		return false;

	core::line3d<f32> line(ray);
	// Change the ray to the mesh's objectspace
	if(!MeshIsInWorldspace)
	{
		core::matrix4 mat;
		SceneNode->getAbsoluteTransformation().getInverse(mat);

		mat.transformVect(line.start);
		mat.transformVect(line.end);
	}

	// Find min/max values for each axis to make faster checks
	Min.set(core::min_(line.start.X, line.end.X),
			core::min_(line.start.Y, line.end.Y),
			core::min_(line.start.Z, line.end.Z));
	Max.set(core::max_(line.start.X, line.end.X),
			core::max_(line.start.Y, line.end.Y),
			core::max_(line.start.Z, line.end.Z));
	RayVect = line.getVector().normalize();
	RayLength = line.getLengthSQ();

	const u32 cnt = Mesh->getMeshBufferCount();

	for (u32 i=0; i<cnt; ++i)
	{
		const IMeshBuffer* buf = Mesh->getMeshBuffer(i);
		const u32 idxCnt = buf->getIndexCount();
		const u16* const indices = buf->getIndices();
		
		if(buf->getVertexType() != video::EVT_COMPONENT_ARRAYS)
		{
			for (u32 j=0; j<idxCnt; j+=3)
			{
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
				core::triangle3df triangle(buf->getPosition(indices[j+0]),
											    buf->getPosition(indices[j+1]),
											   buf->getPosition(indices[j+2]));
#else
				core::triangle3df triangle(buf->getPosition(indices[j+2]),
											    buf->getPosition(indices[j+1]),
											    buf->getPosition(indices[j+0]));
#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
				if(checkCollision(triangle,line))
					return true;
			}
		}
		else
		{
			video::S3DVertexComponentArrays *pVertices = (video::S3DVertexComponentArrays *)buf->getVertices();
			video::S3DVertexComponentArrays::SAccessorEx<core::vector3d, float> accessor;

			if(pVertices->getPositionAccessor(accessor) == 0)
			{
				for (u32 j=0; j<idxCnt; j+=3)
				{
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
					core::triangle3df triangle(accessor[indices[j+0]],
												    accessor[indices[j+1]],
												    accessor[indices[j+2]]);
#else
					core::triangle3df triangle(accessor[indices[j+2]],
												    accessor[indices[j+1]],
												    accessor[indices[j+0]]);
#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
					if(checkCollision(triangle,line))
						return true;
				}
			}
			else
			{
				video::S3DVertexComponentArrays::SAccessorVector3ds accessor;

				if(pVertices->getPositionAccessor(accessor) == 0)
				{
					core::triangle3df triangle;
					
					for (u32 j=0; j<idxCnt; j+=3)
					{	
#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
						accessor.get(indices[j+0], triangle.pointA);
						accessor.get(indices[j+1], triangle.pointB);
						accessor.get(indices[j+2], triangle.pointC);
#else
						accessor.get(indices[j+2], triangle.pointA);
						accessor.get(indices[j+1], triangle.pointB);
						accessor.get(indices[j+0], triangle.pointC);
#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
						if(checkCollision(triangle,line))
							return true;
					}
				}

			}
		}
	}
	return false;
}

bool CMeshCollision::checkCollision( const core::triangle3df &triangle, const core::line3df &line ) const
{
	if(Min.X > triangle.pointA.X && Min.X > triangle.pointB.X && Min.X > triangle.pointC.X)
		return false;
	if(Max.X < triangle.pointA.X && Max.X < triangle.pointB.X && Max.X < triangle.pointC.X)
		return false;
	if(Min.Y > triangle.pointA.Y && Min.Y > triangle.pointB.Y && Min.Y > triangle.pointC.Y)
		return false;
	if(Max.Y < triangle.pointA.Y && Max.Y < triangle.pointB.Y && Max.Y < triangle.pointC.Y)
		return false;
	if(Min.Z > triangle.pointA.Z && Min.Z > triangle.pointB.Z && Min.Z > triangle.pointC.Z)
		return false;
	if(Max.Z < triangle.pointA.Z && Max.Z < triangle.pointB.Z && Max.Z < triangle.pointC.Z)
		return false;

	core::vector3df intersection;
	if (triangle.getIntersectionWithLine(line.start, RayVect, intersection))
	{
		const f32 tmp = intersection.getDistanceFromSQ(line.start);
		const f32 tmp2 = intersection.getDistanceFromSQ(line.end);

		if (tmp < RayLength && tmp2 < RayLength)
		{
			return true;
		}
	}
	return false;
}

}
}
