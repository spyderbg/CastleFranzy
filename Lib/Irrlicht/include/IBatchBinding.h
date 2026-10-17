//-*-c++-*-
#ifndef __I_VIDEO_BATCH_BINDING_H_INCLUDED__
#define __I_VIDEO_BATCH_BINDING_H_INCLUDED__

#include "IDriverBinding.h"
#include "IBatchList.h"

namespace irr
{
namespace video
{

//!
class IBatchBinding : public IDriverBinding
{
  public:

	scene::IBatchList* getBatchList() const
	{
		return BatchList;
	}

	u32 getBatch() const
	{
		return Batch;
	}

	u32 getSegment() const
	{
		return Segment;
	}

	void setInBatch(scene::IBatchList* batchList,
					u32 batch,
					u32 segment)
	{
		_IRR_DEBUG_BREAK_IF(batchList && (batch >= batchList->getBatchCount()
							|| segment >= batchList->getSegmentCount(batch)));
		BatchList = batchList;
		Batch = batch;
		Segment = segment;
	}

protected:

	IBatchBinding()
		: BatchList(NULL)
		, Batch(0)
		, Segment(0)
	{
	}

	scene::IBatchList* BatchList;
	u32 Batch;
	u32 Segment;
};

} // end namespace video
} // end namespace irr

#endif
