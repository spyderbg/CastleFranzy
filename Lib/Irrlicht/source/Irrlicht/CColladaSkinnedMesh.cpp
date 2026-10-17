#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSkinnedMesh.h"
#include "CColladaMesh.h"
#include "matrix4.h"
#include "ISceneNode.h"
#include "CColladaMeshBufferProxy.h"
#include "coreutil.h"
#ifdef _IRR_USE_WII_DEVICE_
#include <revolution.h>
#endif

namespace irr
{

#ifdef _DEBUG
namespace video
{
extern u8* LastProcessBufferAllocationRealStart;
extern u8* LastProcessBufferAllocationRealEnd;
}
#endif

namespace scene
{

using core::stepPointer;

CColladaSkinnedMesh::CColladaSkinnedMesh(
	const collada::CColladaDatabase& collada, 
	collada::SController& controller,
	res::vector<res::String>* skeletons,
	collada::IRootSceneNode* rootSceneNode,
	bool useProcessBuffer,
	video::IVideoDriver* driver
)
	: IColladaSkinnedMesh(collada)
	, Source(0)
	, Skin(*controller.pSkin)
	, Skeleton(skeletons)
	, SkeletonNode(0)
	, SkeletonMtxChacheIsDirty(false)
	, SkeletonMtxPtrChacheIsDirty(true)
	, SelfAttach(false)
{
	setUID(controller.id);
	instanciateMesh(rootSceneNode);
	prepareSkinBuffers(useProcessBuffer, driver);
	
	core::matrix4 bsm;
	bsm = bsm * Skin.bindShapeMatrix;
	BindShapeMatrixIsIdentity = bsm.isIdentity();
}

CColladaSkinnedMesh::~CColladaSkinnedMesh()
{
#ifdef _DEBUG
	for (u32 i = 0, cnt = VertexBuffers.size(); i < cnt; ++i)
	{
		// Use internal or temporary buffers
		_IRR_DEBUG_BREAK_IF(VertexBuffers[i].MeshBuffer->getReferenceCount() != 1);
	}
#endif
	if (Source)
	{
		Source->drop();
	}
}

IColladaMesh::E_TYPE CColladaSkinnedMesh::getType() const
{
	return ET_SKINNED_MESH;
}

int
CColladaSkinnedMesh::instanciateMesh(collada::IRootSceneNode* root)
{
	_IRR_DEBUG_BREAK_IF(Skin.source.ptr[0] != '#');

	const char *url = Skin.source.ptr + 1;

	IColladaMesh* source = getCollada().constructGeometry(url, root);
	if (source == 0)
	{
		source = getCollada().constructController(url, root);
	}
	if (source == 0)
	{
		return 0;
	}

	int meshBufferCount = source->getMeshBufferCount();
	Materials.reallocate(meshBufferCount);
	Materials.set_used(meshBufferCount);
	for (int i = 0; i < meshBufferCount; ++i)
	{
		Materials[i] = NULL;
	}
	
	Source = source;
	Box = Source->getBoundingBox();
	return 1;
}

void
CColladaSkinnedMesh::prepareSkinBuffers(bool useProcessBuffer,
										video::IVideoDriver* driver)
{
	IMeshBuffer* meshBuffer = Source->getMeshBuffer(0);

	_IRR_DEBUG_BREAK_IF(meshBuffer->getVertexType() != video::EVT_COMPONENT_ARRAYS);
	video::S3DVertexComponentArrays *vertices
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			meshBuffer->getVertices()
		);

	VertexBuffers.reallocate(Source->getMeshBufferCount());
	for (u32 i = 0, cnt = Source->getMeshBufferCount(); i < cnt; i++)
	{
		IMeshBuffer* srcMb = Source->getMeshBuffer(i);
		collada::CMeshBufferProxy* proxy = irrnew collada::CMeshBufferProxy();
		proxy->setReferences(
			srcMb,
			reinterpret_cast<video::S3DVertexComponentArrays*>(srcMb->getVertices()),
			&Box
		);
		proxy->setIsDynamic(true);
		VertexBuffers.push_back(SBuffer(proxy));
		proxy->drop();

		// compute bone index start
		for (u32 v = 0, vStart = proxy->getVertexIndexStart();
			 v < vStart;
			 ++v)
		{
			VertexBuffers[i].BoneStart += Skin.vcount[v] * 2;
		}

		if(!useProcessBuffer)
		{
			_IRR_DEBUG_BREAK_IF(driver == NULL);
			video::E_DRIVER_ALLOCATION_RESULT result
				= driver->getProcessBuffer(
					(proxy->getMaterial().getFlag(video::EMF_LIGHTING)
					 ? (video::EVA_POSITION | video::EVA_NORMAL)
					 : video::EVA_POSITION),
					video::EPBT_STATIC,
					proxy,
					true);
			_IRR_DEBUG_BREAK_IF(result & video::EDAR_FAILED);
			HasOutputBuffer = true;
		}
		else
		{
			video::S3DVertexComponentArrays* vertices
				= reinterpret_cast<video::S3DVertexComponentArrays*>(
					proxy->getVertices()
				);
			vertices->Position = vertices->Normal = NULL;
		}
	}
}

void
CColladaSkinnedMesh::computeBoundingBox()
{
	prepareSkeletonMtxCache();

	const u8 boneCounts = SkeletonMatrixCache.size();

	if(Skin.jointsBoundingBoxes.size())
	{
		Box = Skin.jointsBoundingBoxes[0];
		SkeletonMatrixCache[0].SkeletonMatrix->transformBox(Box);

		for(int boneNum = 1; boneNum < boneCounts; boneNum++)
		{
			core::aabbox3df box = Skin.jointsBoundingBoxes[boneNum];
			SkeletonMatrixCache[boneNum].SkeletonMatrix->transformBox(box);
			Box.addInternalBox(box);
		}
	}
	else
	{
		Box.MaxEdge = Box.MinEdge = SkeletonMatrixCache[0].SkeletonMatrix->getTranslation();

		for(int boneNum = 1; boneNum < boneCounts; boneNum++)
		{
			Box.addInternalPoint(SkeletonMatrixCache[boneNum].SkeletonMatrix->getTranslation());
		}
	}
}

void
CColladaSkinnedMesh::skin(u32 buffer)
{
	//PrepareSkeletonMtxCache();
	if(SkeletonMtxChacheIsDirty)
	{
		prepareSkeletonMtxCache();
	}

	IMeshBuffer* meshBuffer = Source->getMeshBuffer(buffer);
	
	u32 start = meshBuffer->getVertexIndexStart();
	u32 end = meshBuffer->getVertexIndexEnd();
	video::SS3DVertexComponentArrays* srcVertice
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			meshBuffer->getVertices()
		);
	core::vector3df* vertexPtr
		= core::stepPointer(srcVertice->Position,
							srcVertice->PositionStride * start);
	u32 vertexPtrStride = srcVertice->PositionStride;
	
	// TODO: Preprocess bindShapeMatrix or use compatible format with matrix4
	//collada::SMatrix &bindShapeMtx = m_Skin.bindShapeMatrix; 
	//memcpy(bindShapeMtx.pointer(), &m_Skin.bindShapeMatrix, sizeof(m_Skin.bindShapeMatrix));

	IMeshBuffer* dstMeshBuffer = getMeshBuffer(buffer);
	video::SS3DVertexComponentArrays* dstVertices
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			dstMeshBuffer->getVertices()
		);
	_IRR_DEBUG_BREAK_IF(dstMeshBuffer->getVertexIndexStart() != start
						|| dstMeshBuffer->getVertexIndexEnd() != end);
	u32 posStride = dstVertices->PositionStride;
	core::vector3df* posIt
		= core::stepPointer(dstVertices->Position,
							posStride * start);

	u32 boneIt = VertexBuffers[buffer].BoneStart;
	if (dstVertices->Normal != NULL && srcVertice->Normal != NULL)
	{
		u32 normalStride = dstVertices->NormalStride;
		core::vector3df* normalIt
			= core::stepPointer(dstVertices->Normal,
								normalStride * start);
		core::vector3df* normalPtr
			= core::stepPointer(srcVertice->Normal,
								srcVertice->NormalStride * start);
		u32 normalPtrStride = srcVertice->NormalStride;
		core::vector3df newVector;
		core::vector3df newNormal;

		for (u32 vertexIt = start; vertexIt < end; ++vertexIt)
		{
			core::vector3df positionCumulator(0.0, 0.0, 0.0);
			core::vector3df normalCumulator(0.0, 0.0, 0.0);
			const core::vector3df& vertex = *vertexPtr;
			const core::vector3df& normal = *normalPtr;

			const u8 boneCounts = Skin.vcount[vertexIt];
			f32 scalarWeigths = 0.0f;

			for(int boneNum = 0;
				boneNum < boneCounts;
				++boneNum, boneIt += 2)
			{
				const u32 boneId = Skin.v[boneIt];
				const u32 weightId = Skin.v[boneIt + 1];
				f32 weight = Skin.weights[weightId];

				if(weight)
				{
					const core::matrix4& boneMtx
						= SkeletonMatrixCache[boneId].MatrixCache;
				
					boneMtx.transformVect(newVector, vertex);
					boneMtx.rotateVect(newNormal, normal);
					
					positionCumulator += newVector * weight;
					normalCumulator += newNormal * weight;
					scalarWeigths += weight;
				}
			}
			
			if (scalarWeigths != 1.00000f)
			{
				float revScalar = 1.0f / scalarWeigths;
				positionCumulator *= revScalar;
				normalCumulator *= revScalar;
			}
			
			*posIt = positionCumulator;
			*normalIt = normalCumulator;

			posIt = stepPointer(posIt, posStride);
			_IRR_DEBUG_BREAK_IF((u8*)posIt > video::LastProcessBufferAllocationRealEnd);

			normalIt = stepPointer(normalIt, normalStride);

			vertexPtr = stepPointer(vertexPtr, vertexPtrStride);
			normalPtr = stepPointer(normalPtr, normalPtrStride);
		}	
	}
	else
	{
		core::vector3df newVector;

		for (u32 vertexIt = start; vertexIt < end; ++vertexIt)
		{
			core::vector3df positionCumulator(0.0, 0.0, 0.0);
			const core::vector3df& vertex = *vertexPtr;

			const u8 boneCounts = Skin.vcount[vertexIt];
			f32 scalarWeigths = 0.0f;

			for(int boneNum = 0; boneNum < boneCounts;
				++boneNum, boneIt += 2)
			{
				const u32 boneId = Skin.v[boneIt];
				const u32 weightId = Skin.v[boneIt + 1];
				f32 weight = Skin.weights[weightId];

				if(weight)
				{
					const core::matrix4& boneMtx
						= SkeletonMatrixCache[boneId].MatrixCache;
				
					boneMtx.transformVect(newVector, vertex);
					
					positionCumulator += newVector * weight;
					scalarWeigths += weight;
				}
			}
			
			if(scalarWeigths != 1.00000f )
			{
				float revScalar = 1.0f / scalarWeigths;
				positionCumulator *= revScalar;
			}
			
			*posIt = positionCumulator;

			posIt = stepPointer(posIt, posStride);
			vertexPtr = stepPointer(vertexPtr, vertexPtrStride);
		}	
	}

#ifdef _IRR_USE_WII_DEVICE_
	// Flush simultanously position and normal
	DCFlushRangeNoSync(dstVertices->Position,
					   dstVertices->PositionStride * (end - start));
#endif
}

void 
CColladaSkinnedMesh::prepareSkeletonMtxCache()
{
	/*if(!SkeletonMtxChacheIsDirty)
		return;*/

	if (isSkinningEnabled())
	{
		prepareSkeletonMtxPtrCache();
		for(u32 i = 0, sz = SkeletonMatrixCache.size(); i < sz; ++i)
		{
			SMatrixCacheEntry& entry = SkeletonMatrixCache[i];
			entry.MatrixCache = *entry.SkeletonMatrix * Skin.bindPoses[i];
			if(!BindShapeMatrixIsIdentity)
			{
				entry.MatrixCache = entry.MatrixCache * Skin.bindShapeMatrix;
			}
		}
		SkeletonMtxChacheIsDirty = false;
	}
}

void 
CColladaSkinnedMesh::prepareSkeletonMtxPtrCache()
{
	if(!(isSkinningEnabled() && SkeletonMtxPtrChacheIsDirty))
		return;

	_IRR_DEBUG_BREAK_IF(SkeletonNode == 0);

	if (SkeletonMatrixCache.size() != Skin.nodes.size())
	{
		SkeletonMatrixCache.reallocate(Skin.nodes.size());
		SkeletonMatrixCache.set_used(Skin.nodes.size());
	}
	for(int i = 0, sz = Skin.nodes.size(); i < sz; i++)
	{
		const scene::ISceneNode* node
			= SkeletonNode->getSceneNodeFromScopeID(Skin.nodes[i]);
		SkeletonMatrixCache[i].SkeletonMatrix
			= (node
			   ? SkeletonMatrixCache[i].SkeletonMatrix = &node->getAbsoluteTransformation()
			   : NULL);
	}
	SkeletonMtxPtrChacheIsDirty = false;
}

void
CColladaSkinnedMesh::attach(ISceneNode* newSkeleton)
{
	SkeletonMtxChacheIsDirty = true;
	SkeletonMtxPtrChacheIsDirty = true;
	const ISceneNode* oldSkeleton = SkeletonNode;
	SkeletonNode = newSkeleton;
	prepareSkeletonMtxPtrCache();
}

void
CColladaSkinnedMesh::setIsSkinningEnabled(bool value)
{
	SkeletonMtxChacheIsDirty = !isSkinningEnabled() && value;
	if (!hasOutputBuffer())
	{
		for (u32 i = 0, cnt = VertexBuffers.size(); i < cnt; ++i)
		{
			video::S3DVertexComponentArrays* dstVertices
				= reinterpret_cast<video::S3DVertexComponentArrays*>(
					VertexBuffers[i].MeshBuffer->getVertices()
				);
			if (value)
			{
				dstVertices->Position = NULL;
				dstVertices->Normal = NULL;
			}
			else
			{
				video::S3DVertexComponentArrays* srcVertices
					= reinterpret_cast<video::S3DVertexComponentArrays*>(
						Source->getMeshBuffer(i)->getVertices()
					);
				dstVertices->Position = srcVertices->Position;
				dstVertices->PositionStride = srcVertices->PositionStride;
				dstVertices->Normal = srcVertices->Normal;
				dstVertices->NormalStride = srcVertices->NormalStride;
			}
		}
	}
	IColladaSkinnedMesh::setIsSkinningEnabled(value);
}

void
CColladaSkinnedMesh::onAnimate(u32 timeMs)
{
	Source->onAnimate(timeMs);
	SkeletonMtxChacheIsDirty = true;
}

video::E_DRIVER_ALLOCATION_RESULT
CColladaSkinnedMesh::onPrepareBufferForRendering(E_PREPARE_BUFFER_STEP step,
												 video::IVideoDriver* driver,
												 u32 buffer)
{
	_IRR_DEBUG_BREAK_IF(buffer >= getMeshBufferCount());

	// The preparation step may be different for underlying object, this could
	// cause some problems.
	video::E_DRIVER_ALLOCATION_RESULT result
		= Source->onPrepareBufferForRendering(step, driver, buffer); 

	ReleaseSourceProcessBuffer =
		step == EPBS_RENDERING && (result & video::EDAR_SUCCESS);

	if (isSkinningEnabled())
	{
		IMeshBuffer* mb = VertexBuffers[buffer].MeshBuffer;
		result = driver->getProcessBuffer(
			(mb->getMaterial().getFlag(video::EMF_LIGHTING)
			 ? (video::EVA_POSITION | video::EVA_NORMAL)
			 : video::EVA_POSITION),
			(step == EPBS_REGISTRATION
			 ? video::EPBT_STATIC
			 : video::EPBT_DYNAMIC),
			mb
		);
		if (result & video::EDAR_SUCCESS)
		{
			HasOutputBuffer = true;
			skin(buffer);
		}		
	}
	return result;
}

void
CColladaSkinnedMesh::releaseProcessBuffer(video::IVideoDriver* driver,
										  u32 buffer)
{
	if (ReleaseSourceProcessBuffer)
	{
		Source->releaseProcessBuffer(driver, buffer);
		ReleaseSourceProcessBuffer = false;
	}
	IColladaSkinnedMesh::releaseProcessBuffer(driver, buffer);
}

u32
CColladaSkinnedMesh::getMeshBufferCount() const
{
	return Source->getMeshBufferCount();
}

IMeshBuffer*
CColladaSkinnedMesh::getMeshBuffer(u32 nr) const
{
	return VertexBuffers[nr].MeshBuffer;
}

const core::aabbox3d<f32>&
CColladaSkinnedMesh::getBoundingBox() const
{
	if (SkeletonMtxChacheIsDirty)
	{
		const_cast<CColladaSkinnedMesh*>(this)->computeBoundingBox();
		SkeletonMtxChacheIsDirty = false;
	}
	return Box;
}

} // namespace scene
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
