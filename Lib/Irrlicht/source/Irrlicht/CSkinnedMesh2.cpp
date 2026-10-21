#include "stdafx.h"
#include "CSkinnedMesh2.h"
#include <IMeshBuffer.h>
#include <CMeshBuffer.h>
using namespace irr;
using namespace scene;

CSkinnedMesh2::CSkinnedMesh2() 
: m_pSource(0)
, m_pSkin(0)
, m_pSkeleton(0)
, m_bSkeletonMtxChacheIsDirty(true) 
, m_bSkeletonMtxPtrChacheIsDirty(true)
, m_pWorkingMeshBuffer(0)
, m_box()
{

};

template<typename T>
inline void movePointer(T *&ptr, irr::s32 iStride)
{
	ptr = (T*)(((c8*)ptr) + iStride);
}

void
CSkinnedMesh2::skin()
{
	prepareSkeletonMtxCache();
	
	const s32 vertexCount = m_pSource->getMeshBuffer(0)->getVertexCount();
	IMeshBuffer *pMeshBuffer = m_pSource->getMeshBuffer(0);
	const core::matrix4 & bindShapeMtx = m_pSkin->m_bindShapeMtx;

	u32 boneIt = 0;

	IMeshBuffer * pMB = m_pWorkingMeshBuffer;
	core::vector3df *posIt = &pMB->getPosition(0);
	s32 posStride = static_cast<s32>(reinterpret_cast<char*>(&pMB->getPosition(1)) - reinterpret_cast<char*>(posIt));

	core::vector3df *vertexPtr = &pMeshBuffer->getPosition(0);
	s32 vertexStride = static_cast<s32>(reinterpret_cast<char*>(&pMeshBuffer->getPosition(1)) - reinterpret_cast<char*>(vertexPtr));

	UNIMPLEMENTED("Normal animation");
	/*core::vector3df *normalPtr = &pMeshBuffer->getNormal(0);
	s32 normalStride = (s32)&pMeshBuffer->getNormal(1) - (s32)normalPtr;*/

	
	for(s32 vertexIt = 0; vertexIt < vertexCount; vertexIt++)
	{

		core::vector3df cumulator(0.0, 0.0, 0.0);
		const core::vector3df & vertex = *vertexPtr;

		const u8 boneCounts = m_pSkin->m_bonesPerVertex[vertexIt];
		f32 scalarWeigths = 0.0f;

		for(int boneNum = 0; boneNum < boneCounts;
			boneNum++, boneIt+=2)
		{
			const u32 boneId = m_pSkin->m_bonesRefIdPerVertex[boneIt];
			const u32 weightId = m_pSkin->m_bonesRefIdPerVertex[boneIt+1];
			f32 weight = m_pSkin->m_weights[weightId];

			if(weight)
			{
				const core::matrix4 & boneMtx = m_SkeletonMtxCache[boneId];
				core::vector3df newVector;

				bindShapeMtx.transformVect(newVector, vertex);
				boneMtx.transformVect(newVector);
				
				cumulator += newVector * weight;
				scalarWeigths += weight;
			}
		}
		
		if(scalarWeigths != 1.00000 )
		{
			cumulator /= scalarWeigths;
		}
		
		*posIt = cumulator;
		movePointer(posIt, posStride);
		movePointer(vertexPtr, vertexStride);
	}	

	m_SkinningTranslation = pMeshBuffer->getPosition(0) - pMB->getPosition(0);

	m_box = pMeshBuffer->getBoundingBox();
	m_box.MaxEdge -=  m_SkinningTranslation;
	m_box.MinEdge -=  m_SkinningTranslation;
}

void 
CSkinnedMesh2::prepareSkeletonMtxCache()
{
	/*if(!m_bSkeletonMtxChacheIsDirty)
		return;*/

	prepareSkeletonMtxPtrCache();

	for(u32 i = 0; i < m_SkeletonMtxCache.size(); i++)
	{
		m_SkeletonMtxCache[i] = *m_SkeletonMtxPtrCache[i] * m_pSkin->m_invBindMtx[i];
	}
	m_bSkeletonMtxChacheIsDirty = false;
}

void 
CSkinnedMesh2::prepareSkeletonMtxPtrCache()
{
	if(!m_bSkeletonMtxPtrChacheIsDirty)
		return;

	m_SkeletonMtxPtrCache.reallocate(m_pSkin->m_bonesName.size());
	m_SkeletonMtxPtrCache.set_used(m_pSkin->m_bonesName.size());
	m_SkeletonMtxCache.reallocate(m_pSkin->m_bonesName.size());
	m_SkeletonMtxCache.set_used(m_pSkin->m_bonesName.size());
	for(u32 i = 0; i < m_pSkin->m_bonesName.size(); i++)
	{
		const ISceneNode *pNode = 0;
		pNode = m_pSkeleton->getSceneNodeFromScopeID(m_pSkin->m_bonesName[i].c_str());
		_IRR_DEBUG_BREAK_IF(pNode == 0);
		m_SkeletonMtxPtrCache[i] = &pNode->getAbsoluteTransformation();
	}
	m_bSkeletonMtxPtrChacheIsDirty = false;
}


const IMesh *			
CSkinnedMesh2::setSource(const IMesh *pNewSource)
{
	const IMesh *oldSource = m_pSource;
	if(oldSource)
		oldSource->drop();
	m_pSource = pNewSource;
	m_pSource->grab();

	if(m_pWorkingMeshBuffer)
		m_pWorkingMeshBuffer->drop();
	
	switch(m_pSource->getMeshBuffer(0)->getVertexType())
	{
	case video::EVT_STANDARD:
			m_pWorkingMeshBuffer = irrnew SMeshBuffer(*(SMeshBuffer*)m_pSource->getMeshBuffer(0));
		
		break;
	case video::EVT_2TCOORDS:
		{
			m_pWorkingMeshBuffer = irrnew SMeshBufferLightMap(*(SMeshBufferLightMap*)m_pSource->getMeshBuffer(0));
		}
		break;
	case video::EVT_TANGENTS:
		{
			m_pWorkingMeshBuffer = irrnew SMeshBufferTangents(*(SMeshBufferTangents*)m_pSource->getMeshBuffer(0));
		}
		break;
	}// end switch

	/*for(int i = 0; i < m_VertexBuffers.size(); i++)
	{
		delete m_VertexBuffers[i];
	}

	m_VertexBuffers.reallocate(m_pSource->getMeshBufferCount());
	m_VertexBuffers.set_used(m_pSource->getMeshBufferCount());
	for(int i = 0; i < m_pSource->getMeshBufferCount(); i++)
	{
		m_VertexBuffers[i] = irrnew CMeshBufferProxy();
		m_VertexBuffers[i]->setReferences(m_pSource->getMeshBuffer(0), m_pWorkingMeshBuffer);
	}*/

	return oldSource;
}

const CSkin *
CSkinnedMesh2::setSkin(const CSkin *pNewSkin)
{
	m_bSkeletonMtxChacheIsDirty = true;
	m_bSkeletonMtxPtrChacheIsDirty = true;
	const CSkin *oldSkin = m_pSkin;
	if(oldSkin)
		oldSkin->drop();
	m_pSkin = pNewSkin;
	m_pSkin->grab();
	return oldSkin;
}

const CSkeleton *
CSkinnedMesh2::setSkeleton(const CSkeleton *pNewSkeleton)
{
	m_bSkeletonMtxChacheIsDirty = true;
	m_bSkeletonMtxPtrChacheIsDirty = true;
	const CSkeleton *oldSkeleton = m_pSkeleton;
	bool deleted = true;
	if(oldSkeleton)
	{
		deleted = oldSkeleton->drop();
	}

	pNewSkeleton->grab();
	m_pSkeleton = pNewSkeleton;

	if(deleted)
		return 0;
	else
		return oldSkeleton;
}


void 
CSkinnedMesh2::OnAnimate(irr::u32 timeMs)
{
	skin();
}
