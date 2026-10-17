//-*-c++-*-
#ifndef __C_SCENE_I_BATCH_MESH_H_INCLUDED__
#define __C_SCENE_I_BATCH_MESH_H_INCLUDED__

#include "IMesh.h"
#include "IBatchList.h"

namespace irr
{
namespace scene
{

class IBatchMesh;

struct IBatchSortCallback
{
	virtual void operator () (IBatchMesh* batchMesh,
							  u32 oldBatch,
							  u32 oldSegment,
							  u32 newBatch,
							  u32 newSegment) = 0;
};

struct IBatchSegmentContentUpdateCallback
{
	virtual void operator () (IBatchMesh* batchMesh,
							  u32 batch,
							  u32 segments,
							  bool verticesUpdated,
							  bool indicesUpdate) = 0;
};

//!
class IBatchMesh
	: public IMesh
	, public IBatchList
{
public:

	//!
	virtual void updateBoundingBox() = 0;

	//!
	virtual const core::aabbox3df& getBatchBoundingBox(u32 batch) const = 0;

	//!
	virtual void clear() = 0;

	//! Sorts the batches such that those with a solid material are packed
	//! starting a index 0, and transparent batches after.
	/**
	   \param driver Used to query material renderers to determine transparency.
	   \return The number of solid batches.
	 */
	virtual u32 sort(const video::IVideoDriver* driver) = 0;

	//!
	/** \warning Internally only keeps a copy of the pointer. */
	void setSortCallback(IBatchSortCallback* callback);

	//!
	virtual void updateSegmentContent(u32 batch,
									  u32 segment,
									  const IMeshBuffer* content,
									  u32 attributeMask = u32(-1),
									  bool updateIndices = true,
									  bool useMaterial = true) = 0;

	//!
	/** \warning Internally only keeps a copy of the pointer. */
	void setSegmentContentUpdateCallback(IBatchSegmentContentUpdateCallback* callback);

	//!
	virtual void setRenderLayer(u32 batch, u32 segment, s16 layer) = 0;

	//!
	virtual s16 getRenderLayer(u32 batch, u32 segment) const = 0;

protected:

	//!
	IBatchMesh();

	//!
	void sortCallback(u32 oldBatch,
					  u32 oldSegment,
					  u32 newBatch,
					  u32 newSegment);

	//!
	void segmentContentUpdateCallback(u32 oldBatch,
									  u32 oldSegment,
									  bool verticesUpdated,
									  bool indicesUpdate);

private:

	IBatchSortCallback* SortCallback;
	IBatchSegmentContentUpdateCallback* SegmentContentUpdateCallback;
};

} // end namespace scene
} // end namespace irr

#endif
