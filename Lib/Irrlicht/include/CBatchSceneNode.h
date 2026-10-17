//-*-c++-*-
#ifndef __C_SCENE_BATCH_SCENE_NODE_H_INLUDED__
#define __C_SCENE_BATCH_SCENE_NODE_H_INLUDED__

#include "IrrCompileConfig.h"

#include "IBatchSceneNode.h"
#include "CBatchMesh.h"
#include "SViewFrustum.h"

namespace irr
{
namespace scene
{

/**
   SegmentData must provide the following interface:
   u16 getVertexStart() const;
   u16 getVertexEnd() const;
   u16 getIndexStart() const;
   u16 getIndexEnd() const;
   u32 getVisibleFrame() const;
   void setVisibleFrame(u32) const;
   const core::aabbox3df& getBoundingBox() const;
   void setBoundingBox(const core::aabbox3df&) const;
 */
template <typename SegmentData = SBoundedSegment>
class CBatchSceneNode : public IBatchSceneNode
{
public:

	typedef SegmentData Segment;

	//!
	explicit CBatchSceneNode(s32 id = -1,
							 IBatchMesh* batchMesh = NULL);

	// ISceneNode interface ----------------------------------------------------

	ESCENE_NODE_TYPE getType() const;

	virtual void OnRegisterSceneNode();

	// IBatchSceneNode interface -----------------------------------------------

	virtual void postCompile();

protected:

	//!
	virtual IBatchMesh* allocateDefaultBatchMesh() const;

	//!
	void renderBatchBBoxes(video::IVideoDriver* driver, u32 batch);

	// this class --------------------------------------------------------------

	//!
	void addVisibleSegmentRaw(u32 batch, u32 segment, Segment& segData)
	{
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);
		// The batch is flagged as dirty if a segment which was not visible
		// last frame is now visible
		info.Flags |= info.FrameCount != segData.getVisibleFrame();
		segData.setVisibleFrame(os::Timer::getTickCount());
		IBatchSceneNode::addVisibleSegmentRaw(batch, segment);
	}

	//!
	bool addVisibleSegment(u32 batch,
						   u32 segment,
						   Segment& segData,
						   const SViewFrustum& frustum)
	{
		if (segData.getVisibleFrame() != os::Timer::getTickCount()
			&& segData.isVisible()
			&& ((segData.getSourceBuffer()
				 && segData.getSourceBuffer()->isVisible())
				|| (!segData.getSourceBuffer()
					&& frustum.intersects(segData.getBoundingBox()))))
		{
			addVisibleSegmentRaw(batch, segment, segData);
			return true;
		}
		return false;
	}

	//!
	bool addVisibleSegment(u32 batch,
						   u32 segment,
						   Segment& segData)
	{
		if (segData.getVisibleFrame() != os::Timer::getTickCount()
			&& segData.isVisible())
		{
			addVisibleSegmentRaw(batch, segment, segData);
			return true;
		}
		return false;
	}

	//!
	bool addVisibleSegment(u32 batch, u32 segment)
	{
		return addVisibleSegment(
			batch,
			segment,
			*reinterpret_cast<Segment*>(
				BatchMesh->getSegmentData(batch, segment)
			)
		);
	}

	//!
	template <typename Intersector>
	void addVisibleSegments(u32 batch, Intersector inter)
	{
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);
		for (u32 s = 0, cnt = BatchMesh->getSegmentCount(batch); s < cnt; ++s)
		{
			Segment* seg = reinterpret_cast<Segment*>(
				BatchMesh->getSegmentData(batch, s)
			);
			_IRR_DEBUG_BREAK_IF(seg->getBoundingBox() == NULL);
			if (seg->getVisibleFrame() != os::Timer::getTickCount()
				&& seg->isVisible()
				&& ((seg->getSourceBuffer()
					 && seg->getSourceBuffer()->isVisible())
					|| !seg->getSourceBuffer()
					&& inter(*seg->getBoundingBox())))
			{
				addVisibleSegmentRaw(batch, s, *seg);
			}
		}
	}

	//!
	core::array<core::aabbox3df> StaticBBoxes;
};

} // end namespace scene
} // end namespace irr

#ifndef _IRR_EXPLICIT_TEMPLATE_INSTANTIATION_
#    include "CBatchSceneNode_impl.h"
#endif

#endif
