#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CTriangleSelector.h"
#include "ISceneNode.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{

//! constructor
CTriangleSelector::CTriangleSelector(const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif
}


//! constructor
CTriangleSelector::CTriangleSelector(const IMesh* mesh, const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	const u32 cnt = mesh->getMeshBufferCount();
	u32 totalFaceCount = 0;
	for (u32 j=0; j<cnt; ++j)
		totalFaceCount += mesh->getMeshBuffer(j)->getIndexCount();
	totalFaceCount /= 3;
	Triangles.reallocate(totalFaceCount);

	for (u32 i=0; i<cnt; ++i)
	{
		const IMeshBuffer* buf = mesh->getMeshBuffer(i);
		const u32 idxCnt = buf->getIndexCount();
		const u16* const indices = buf->getIndices();
		
		if(buf->getVertexType() != video::EVT_COMPONENT_ARRAYS)
		{
			for (u32 j=0; j<idxCnt; j+=3)
			{
	#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
					  Triangles.push_back(core::triangle3df(
								  buf->getPosition(indices[j+0]),
								  buf->getPosition(indices[j+1]),
								  buf->getPosition(indices[j+2])));
	#else
					  Triangles.push_back(core::triangle3df(
								  buf->getPosition(indices[j+2]),
								  buf->getPosition(indices[j+1]),
								  buf->getPosition(indices[j+0])));
	#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
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
						  Triangles.push_back(core::triangle3df(
									  accessor[indices[j+0]],
									  accessor[indices[j+1]],
									  accessor[indices[j+2]]));
		#else
						  Triangles.push_back(core::triangle3df(
									  accessor[indices[j+2]],
									  accessor[indices[j+1]],
									  accessor[indices[j+0]]));
		#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
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
						Triangles.push_back(triangle);
			#else
						accessor.get(indices[j+2], triangle.pointA);
						accessor.get(indices[j+1], triangle.pointB);
						accessor.get(indices[j+0], triangle.pointC);
						Triangles.push_back(triangle);
			#endif //_IRR_USE_RIGHT_HAND_CONVENTION_
					}
				}

			}
		}
	}
}


//! constructor
CTriangleSelector::CTriangleSelector(const core::aabbox3d<f32>& box, const ISceneNode* node)
: SceneNode(node)
{
	#ifdef _DEBUG
	setDebugName("CTriangleSelector");
	#endif

	// TODO
}


//! Gets all triangles.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount, 
					const core::matrix4* transform) const
{
	s32 cnt = Triangles.size();
	if (cnt > arraySize)
		cnt = arraySize;

	core::matrix4 mat;

	if (transform)
		mat = *transform;

	if (SceneNode)
		mat *= SceneNode->getAbsoluteTransformation();

	for (s32 i=0; i<cnt; ++i)
	{
		triangles[i] = Triangles[i];
		mat.transformVect(triangles[i].pointA);
		mat.transformVect(triangles[i].pointB);
		mat.transformVect(triangles[i].pointC);
	}

	outTriangleCount = cnt;
}



//! Gets all triangles which lie within a specific bounding box.
void CTriangleSelector::getTriangles(core::triangle3df* triangles, 
					s32 arraySize, s32& outTriangleCount, 
					const core::aabbox3d<f32>& box,
					const core::matrix4* transform) const
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Gets all triangles which have or may have contact with a 3d line.
void CTriangleSelector::getTriangles(core::triangle3df* triangles,
					s32 arraySize, s32& outTriangleCount,
					const core::line3d<f32>& line,
					const core::matrix4* transform) const
{
	// return all triangles
	getTriangles(triangles, arraySize, outTriangleCount, transform);
}


//! Returns amount of all available triangles in this selector
s32 CTriangleSelector::getTriangleCount() const
{
	return Triangles.size();
}



} // end namespace scene
} // end namespace irr

