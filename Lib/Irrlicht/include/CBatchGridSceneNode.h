//-*-c++-*-
#ifndef __C_SCENE_BATCH_GRID_SCENE_NODE_H_INCLUDED__
#define __C_SCENE_BATCH_GRID_SCENE_NODE_H_INCLUDED__

#include "CBatchSceneNode.h"
#include <vector>
#include "irrAllocator.h"

namespace irr
{
namespace scene
{

/**
   See CBatchSceneNode<SegmentData> for requirements on SegmentData.
 */
template <typename SegmentData = SBoundedSegment>
class CBatchGridSceneNode : public CBatchSceneNode<SegmentData>
{
public:

	typedef SegmentData Segment;

	//!
	CBatchGridSceneNode(const core::dimension2d<u32>& dims,
						u32 upAxis,
						s32 id = -1,
						IBatchMesh* batchMesh = NULL);

	//!
	virtual ~CBatchGridSceneNode();

	// ISceneNode interface ----------------------------------------------------

	ESCENE_NODE_TYPE getType() const;

	virtual void OnRegisterSceneNode();
	virtual void serializeAttributes(io::IAttributes* out,
									 io::SAttributeReadWriteOptions* options = 0) const;
	virtual void deserializeAttributes(io::IAttributes* in,
									   io::SAttributeReadWriteOptions* options = 0);

	// IBatchSceneNode interface -----------------------------------------------

	virtual void postCompile();

	// this class --------------------------------------------------------------

	//!
	const core::dimension2d<u32>& getDimensions() const
	{
		return GridDimensions;
	}

	//!
	void setDimensions(const core::dimension2d<u32>& dims);

	//!
	u32 getUpAxis() const
	{
		return UpAxis;
	}

	//!
	void setUpAxis(u32 upAxis);

protected:

	struct SegmentId
	{
		u32 Batch;
		u32 Index;

		SegmentId(u32 batch, u32 index)
			: Batch(batch)
			, Index(index)
		{
		}
	};

	typedef std::vector<SegmentId, core::irrAllocator<SegmentId> > Cell;

	//!
	void build();

	//!
	core::position2df mapToGrid(f32 x, f32 y) const
	{
		return core::position2d<f32>(
			(x - Origin.X) * GridDimensions.Width / Dimensions.Width,
			(y - Origin.Y) * GridDimensions.Height / Dimensions.Height
		);
	}

	//!
	void getRange(const core::aabbox3df& bbox,
				  core::position2d<u32>& lo,
				  core::position2d<u32>& hi) const;

	//!
	Cell& getCell(u32 x, u32 y)
	{
		_IRR_DEBUG_BREAK_IF(Grid == NULL);
		return Grid[x * GridDimensions.Height + y];
	}

	//!
	void addVisibleCell(const Cell& cell)
	{
		for (typename Cell::const_iterator
				 i = cell.begin(),
				 iEnd = cell.end();
			 i != iEnd;
			 ++i)
		{
			this->addVisibleSegment(i->Batch, i->Index);
		}
	}

	//!
	template <typename Intersector>
	void addVisibleCell(const Cell& cell,
						Intersector inter)
	{
		for (typename Cell::const_iterator
				 i = cell.begin(),
				 iEnd = cell.end();
			 i != iEnd;
			 ++i)
		{
			Segment* seg = reinterpret_cast<Segment*>(
				this->BatchMesh->getSegmentData(i->Batch, i->Index)
			);
			_IRR_DEBUG_BREAK_IF(seg->getBoundingBox() == NULL);
			if (seg->getVisibleFrame() != os::Timer::getTickCount()
				&& seg->isVisible()
				&& ((seg->getSourceBuffer()
					 && seg->getSourceBuffer()->isVisible())
					|| !seg->getSourceBuffer()
					&& inter(*seg->getBoundingBox())))
			{
				this->addVisibleSegment(i->Batch, i->Index, *seg);
			}
		}
	}

	//!
	void makeFrustumLocal(SViewFrustum& frustum) const;

	//!
	template <typename Intersector>
	void addVisibleCells(const SViewFrustum& frustum,
						 const core::position2d<u32>& frustumLo,
						 const core::position2d<u32>& frustumHi)
	{
		// transform frustum into grid space
		SViewFrustum localFrustum = frustum;
		makeFrustumLocal(localFrustum);

		core::aabbox3df cellBBox;
		{
			const core::aabbox3df& nodeBBox = this->getBoundingBox();
			cellBBox.MinEdge[UpAxis] = nodeBBox.MinEdge[UpAxis];
			cellBBox.MaxEdge[UpAxis] = nodeBBox.MaxEdge[UpAxis];
		}

		cellBBox.MaxEdge[Axis[0]] = frustumLo.X;
		for (u32 x = frustumLo.X; x < frustumHi.X; ++x)
		{
			cellBBox.MinEdge[Axis[0]] = cellBBox.MaxEdge[Axis[0]];
			cellBBox.MaxEdge[Axis[0]] = x + 1;
			
			cellBBox.MaxEdge[Axis[1]] = frustumLo.Y;
			for (u32 y = frustumLo.Y; y < frustumHi.Y; ++y)
			{
				cellBBox.MinEdge[Axis[1]] = cellBBox.MaxEdge[Axis[1]];
				cellBBox.MaxEdge[Axis[1]] = y + 1;
				if (Intersector(localFrustum)(cellBBox))
				{
					addVisibleCell(getCell(x, y), Intersector(frustum));
				}
			}
		}
	}

	//!
	core::dimension2d<u32> GridDimensions;

	//!
	u32 UpAxis;

	//!
	u32 Axis[2];

	//!
	core::position2df Origin;

	//!
	core::dimension2df Dimensions;

	//!
	Cell* Grid;
};

} // end namespace scene
} // end namespace irr

#ifndef _IRR_EXPLICIT_TEMPLATE_INSTANTIATION_
#    include "CBatchGridSceneNode_impl.h"
#endif

#endif
