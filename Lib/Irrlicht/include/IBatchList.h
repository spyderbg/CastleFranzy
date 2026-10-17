//-*-c++-*-
#ifndef __C_SCENE_BATCH_LIST_H_INCLUDED__
#define __C_SCENE_BATCH_LIST_H_INCLUDED__

#include "irrTypes.h"
#include "aabbox3d.h"
#include "EHardwareBufferFlags.h"
#include "irrList.h"

namespace irr
{
namespace scene
{

class CBatchBuffer;
class IMeshBuffer;
class IBatchList;

//!
struct SCompileInfo
{
	const IMeshBuffer* SourceBuffer;
};

//!
struct IBatchCompileCallback
{
	virtual void operator () (IBatchList* batchList,
							  u32 batch,
							  u32 segment,
							  const SCompileInfo& data) = 0;
};

//!
class IBatchList
{
public:

	//!
	virtual ~IBatchList();

	//!
	virtual u32 addBatch() = 0;

	//!
	virtual u32 addSegment(u32 batch,
						   u16 vertexStart,
						   u16 vertexEnd,
						   u32 indexStart,
						   u32 indexEnd) = 0;

	//!
	virtual u32 getBatchCount() const = 0;

	//!
	virtual void setBuffer(u32 batch, CBatchBuffer* buffer) = 0;

	//!
	virtual CBatchBuffer* getBuffer(u32 batch) const = 0;

	//!
	virtual u32 getSegmentCount(u32 batch) const = 0;

	//!
	virtual void getSegmentVertexRange(u32 batch,
									   u32 segment,
									   u16& start,
									   u16& end) const = 0;

	//!
	virtual void getSegmentIndexRange(u32 batch,
									  u32 segment,
									  u32& start,
									  u32& end) const = 0;

	//!
	virtual void* getSegmentData(u32 batch, u32 segment) = 0;

	//!
	virtual const void* getSegmentData(u32 batch, u32 segment) const = 0;

	//!
	virtual void getSegmentBoundingBox(u32 batch,
									   u32 segment,
									   core::aabbox3df& bbox) const = 0;

	//!
	virtual core::vector3df getSegmentCenter(u32 batch,
											 u32 segment) const = 0;

	//!
	virtual void setSegmentCompileInfo(u32 batch,
									   u32 segment,
									   const SCompileInfo& data);

	//!
	/** \warning Internally only keeps a copy of the pointer. */
	void setCompileCallback(IBatchCompileCallback* callback);

	//!
	/** \warning Internally only keeps a copy of the pointer. */
	IBatchCompileCallback* getCompileCallback() const
	{
		return CompileCallback;
	}

	//! Simple helper to quantize components of all buffers the same way.
	/**
	   \param normalInShort Converts normals to u16 instead of u8.
	   \param positionInShort Converts positions to u16 instead of leaving them
	   in float.
	 */
	virtual void quantizeComponents(bool normalInShort = false,
									bool positionInShort = false);

	/**
	   Creates a hardware buffer for all buffers if maxTotalSize == 0 for those
	   whose total size does not exceed maxTotalSize and releases the client
	   memory copy.
	 */
	void transferToHardwareBuffers(E_HARDWARE_MAPPING mappingHint = EHM_STATIC,
								   u32 maxTotalSize = 0);

protected:

	//!
	IBatchList();

private:

	IBatchCompileCallback* CompileCallback;
};

} // end namespace scene
} // end namespace irr

#endif
