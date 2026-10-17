//-*-c++-*-
#ifndef __C_SCENE_BATCH_SCENE_NODE_IMPL_H_INCLUDED__
#define __C_SCENE_BATCH_SCENE_NODE_IMPL_H_INCLUDED__

#include "CBatchSceneNode.h"

#include "SIntersector.h"

#ifdef _DEBUG
#    include "EDebugSceneTypes.h"
#endif

#ifdef _IRR_EXPLICIT_TEMPLATE_INSTANTIATION_
#    ifdef inline
#        define _IRR_OLD_INLINE_ inline
#        undef inline
#    endif
#    define inline
#endif

namespace irr
{
namespace scene
{

template <typename SegmentData>
inline
CBatchSceneNode<SegmentData>::CBatchSceneNode(s32 id,
											  IBatchMesh* batchMesh)
	: IBatchSceneNode(id, batchMesh)
{	
#ifdef _DEBUG
	setDebugName("CBatchSceneNode");
#endif
}

template <typename SegmentData>
inline
ESCENE_NODE_TYPE
CBatchSceneNode<SegmentData>::getType() const
{
	return ESNT_BATCH_SCENE_NODE;
}

template <typename SegmentData>
inline
void
CBatchSceneNode<SegmentData>::OnRegisterSceneNode()
{
	if (isVisible())
	{
		// clear current visible segments
		u32 mbCnt = clearVisibleSegments();

		if (!SceneManager->isCulled(this))
		{
			bool isCullingEnabled = SceneManager->isCullingEnabled();
			E_CULLING_TYPE bct = EAC_OFF;
			E_CULLING_TYPE sct = EAC_OFF;
			ICameraSceneNode* cam = NULL;
			if (isCullingEnabled)
			{
				bct = getBatchAutomaticCulling();
				sct = getSegmentAutomaticCulling();
				SceneManager->disableCulling();
				cam = SceneManager->getActiveCamera();
			}
			for (u32 i = 0; i < mbCnt; ++i)
			{
				if (bct == EAC_OFF
					|| !SceneManager->isCulled(BatchMesh->getBatchBoundingBox(i),
											   bct))
				{
					switch (sct)
					{
						case EAC_OFF :
						case EAC_FRUSTUM_SPHERE :
						{
							if (hasVisibleIndexCache())
							{
								addVisibleSegments(i, SUniverseIntersector());
							}
							else
							{
								getSegmentVisibilityInfo(i).Count
									= BatchMesh->getSegmentCount(i);
#ifdef _DEBUG
								getVisibleSegmentCount()
									+= getSegmentVisibilityInfo(i).Count;
								if (RegisterSolidBatchesOnce
									&& i < SolidBatchCount)
								{
									getVisibleSolidSegmentCount()
										+= getSegmentVisibilityInfo(i).Count;
								}							   
#endif
							}
						}
						break;

						case EAC_BOX :
						{
							addVisibleSegments(
								i,
								SBoxIntersector(*cam->getViewFrustum())
							);
						}
						break;

						case EAC_FRUSTUM_BOX :
						{
							addVisibleSegments(
								i,
								SFrustumBoxIntersector(*cam->getViewFrustum())
							);
						}
						break;

						case EAC_FRUSTUM_BOX_3 :
						{
							addVisibleSegments(
								i,
								SFrustumBoxIntersector3(*cam->getViewFrustum())
							);
						}
						break;

						default :
						{
							_IRR_DEBUG_BREAK_IF("not supported");
						}
					}
				}
			}
			if (RegisterSolidBatchesOnce)
			{
				// We need to update info of solid batches
				u32 totalVisible = updateInfo(0, SolidBatchCount);
				if (totalVisible > 0)
				{
					SceneManager->registerNodeForRendering(
						this,
						NULL,
						NULL
					);
				}
				registerTransparentBatches();
			}
			else
			{
				registerSolidBatches();
				registerTransparentBatches();
			}
			if (isCullingEnabled)
			{
				SceneManager->enableCulling();
			}
		}

		// Necessary?
		IBatchSceneNode::OnRegisterSceneNode();
	}
}

// IBatchSceneNode interface ---------------------------------------------------

template <typename SegmentData>
inline
void
CBatchSceneNode<SegmentData>::postCompile()
{
	IBatchSceneNode::postCompile();

	StaticBBoxes.clear();

	u32 boxCount = 0;
	for (u32 i = 0, mbCnt = BatchMesh->getMeshBufferCount(); i < mbCnt; ++i)
	{
		for (u32 s = 0, cnt = BatchMesh->getSegmentCount(i); s < cnt; ++s)
		{
			Segment* seg = reinterpret_cast<Segment*>(
				BatchMesh->getSegmentData(i, s)
			);
			if (seg->getBoundingBox() == NULL)
			{
				++boxCount;
			}
		}
	}

	StaticBBoxes.reallocate(boxCount);
	StaticBBoxes.set_used(boxCount);
	boxCount = 0;
	for (u32 i = 0, mbCnt = BatchMesh->getMeshBufferCount(); i < mbCnt; ++i)
	{
		// Compute the bounding box of each static segment
		for (u32 s = 0, cnt = BatchMesh->getSegmentCount(i); s < cnt; ++s)
		{
			Segment* seg = reinterpret_cast<Segment*>(
				BatchMesh->getSegmentData(i, s)
			);
			if (seg->getBoundingBox() == NULL)
			{
				BatchMesh->getSegmentBoundingBox(i, s, StaticBBoxes[boxCount]);
				seg->setBoundingBox(&StaticBBoxes[boxCount]);
				++boxCount;
			}
		}
	}
}

template <typename SegmentData>
inline
IBatchMesh*
CBatchSceneNode<SegmentData>::allocateDefaultBatchMesh() const
{
	return irrnew CBatchMesh<SBoundedSegment>;
}

template <typename SegmentData>
inline
void
CBatchSceneNode<SegmentData>::renderBatchBBoxes(video::IVideoDriver* driver,
												u32 batch)
{
	video::SMaterial m;
	m.setFlag(video::EMF_LIGHTING, false);
	driver->setMaterial(m);

	driver->draw3DBox(BatchMesh->getBatchBoundingBox(batch),
					  video::SColor(255, 0, 0, 255));
	SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);
	for (u32
			 *si = getVisibleSegments(batch),
			 *siEnd = si + info.Count;
		 si != siEnd;
		 ++si)
	{
		const Segment* seg = reinterpret_cast<const Segment*>(
			BatchMesh->getSegmentData(batch, *si)
		);
		_IRR_DEBUG_BREAK_IF(seg->getBoundingBox() == NULL);
		driver->draw3DBox(*seg->getBoundingBox(),
						  video::SColor(255, 0, 255, 0));
	}
}

} // end namespace scene
} // end namespace irr

#ifdef _IRR_EXPLICIT_TEMPLATE_INSTANTIATION_
#    undef inline
#    ifdef _IRR_OLD_INLINE_
#        define inline _IRR_OLD_INLINE_
#        undef inline
#    endif
#endif

#endif
