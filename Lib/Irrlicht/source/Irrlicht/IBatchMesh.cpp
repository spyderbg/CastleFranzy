#include "stdafx.h"
#include "IBatchMesh.h"

namespace irr
{
namespace scene
{

IBatchMesh::IBatchMesh()
	: SortCallback(NULL),
	  SegmentContentUpdateCallback(NULL)
{
}

void
IBatchMesh::sortCallback(u32 oldBatch,
						 u32 oldSegment,
						 u32 newBatch,
						 u32 newSegment)
{
	if (SortCallback)
	{
		(*SortCallback)(this, oldBatch, oldSegment, newBatch, newSegment);
	}
}

void
IBatchMesh::setSortCallback(IBatchSortCallback* callback)
{
	SortCallback = callback;
}

void
IBatchMesh::segmentContentUpdateCallback(u32 batch,
										 u32 segment,
										 bool verticesUpdated,
										 bool indicesUpdated)
{
	if (SegmentContentUpdateCallback)
	{
		(*SegmentContentUpdateCallback)(this,
										batch,
										segment,
										verticesUpdated,
										indicesUpdated);
	}
}

void
IBatchMesh::setSegmentContentUpdateCallback(IBatchSegmentContentUpdateCallback* callback)
{
	SegmentContentUpdateCallback = callback;
}

} // end namespace scene
} // end namespace irr
