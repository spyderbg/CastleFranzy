//-*-c++-*-
#ifndef __C_SCENE_BATCH_MESH_H_INCLUDED__
#define __C_SCENE_BATCH_MESH_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "IMesh.h"
#include "irrArray.h"
#include "IBatchMesh.h"
#include "CBatchBuffer.h"
#include "irros.h"

namespace irr
{
namespace scene
{

//!
struct SBasicSegment
{
private:

	s16 RenderLayer;
	u16 VertexStart;
	u16 VertexEnd;
	u16 VertexEndStorage;
	u32 IndexStart;
	u32 IndexEnd;
	u32 IndexEndStorage;

public:

	//!
	SBasicSegment()
		: RenderLayer(0)
		, VertexStart(0)
		, VertexEnd(0)
		, VertexEndStorage(0)
		, IndexStart(0)
		, IndexEnd(0)
		, IndexEndStorage(0)
	{
	}

	//!
	SBasicSegment(u16 vertexStart,
				  u16 vertexEnd,
				  u32 indexStart,
				  u32 indexEnd)
		: RenderLayer(0)
		, VertexStart(vertexStart)
		, VertexEnd(vertexEnd)
		, VertexEndStorage(vertexEnd)
		, IndexStart(indexStart)
		, IndexEnd(indexEnd)
		, IndexEndStorage(indexEnd)
	{
	}

	//!
	s16 getRenderLayer() const
	{
		return RenderLayer;
	}

	//!
	void setRenderLayer(s16 layer)
	{
		RenderLayer = layer;
	}

	//!
	u16 getVertexStart() const
	{
		return VertexStart;
	}

	//!
	u16 getVertexEnd() const
	{
		return VertexEnd;
	}

	//!
	u16 getVertexEndStorage() const
	{
		return VertexEndStorage;
	}

	//!
	void setVertexStart(u16 vertexStart)
	{
		VertexEnd = vertexStart + (getVertexEnd() - getVertexStart());
		VertexEndStorage
			= vertexStart + (getVertexEndStorage() - getVertexStart());
		VertexStart = vertexStart;
	}

	//!
	void setVertexEnd(u16 vertexEnd)
	{
		_IRR_DEBUG_BREAK_IF(vertexEnd < getVertexStart()
							|| vertexEnd > getVertexEndStorage());
		VertexEnd = vertexEnd;
	}

	//!
	u32 getIndexStart() const
	{
		return IndexStart;
	}

	//!
	u32 getIndexEnd() const
	{
		return IndexEnd;
	}

	//!
	u32 getIndexEndStorage() const
	{
		return IndexEndStorage;
	}

	//!
	void setIndexEnd(u32 indexEnd)
	{
		_IRR_DEBUG_BREAK_IF(indexEnd < getIndexStart()
							|| indexEnd > getIndexEndStorage());
		IndexEnd = indexEnd;
	}

	//!
	void setIndexStart(u32 indexStart)
	{
		IndexEnd = indexStart + (getIndexEnd() - getIndexStart());
		IndexEndStorage = indexStart + (getIndexEndStorage() - getIndexStart());
		IndexStart = indexStart;
	}

	//!
	void setSourceBuffer(const IMeshBuffer* /*buffer*/)
	{
	}

	//!
	const IMeshBuffer* getSourceBuffer() const
	{
		return NULL;
	}

	//!
	const core::aabbox3df* getBoundingBox() const
	{
		return NULL;
	}

	//!
	/** \note This member function is static to help compilers consider its call
		as a compile-time constant. */
	static bool isVisible()
	{
		return true;
	}
};

//!
struct SBoundedSegment : SBasicSegment
{
private:

	const IMeshBuffer* SourceBuffer;
	const core::aabbox3df* BBox;

	// could be packed externally into a bitset for less memory consumption
	// (most likely close to a 32 to 1 ratio), but it would be less
	// efficient from a CPU cache point of view due to access to non-localized
	// data
	u32 VisibleFrame;

	//! Explicit visibility flag, akin to ISceneNode explicit visibility
	bool IsVisible;

public:

	//!
	SBoundedSegment()
		: SourceBuffer(NULL)
		, BBox(NULL)
		, VisibleFrame(0)
		, IsVisible(true)
	{
	}

	//!
	SBoundedSegment(u16 vertexStart,
					u16 vertexEnd,
					u32 indexStart,
					u32 indexEnd)
		: SBasicSegment(vertexStart, vertexEnd, indexStart, indexEnd)
 		, SourceBuffer(NULL)
		, BBox(NULL)
		, VisibleFrame(0)
		, IsVisible(true)
	{
	}

	//!
	~SBoundedSegment()
	{
		setSourceBuffer(NULL);
	}

	//!
	void setSourceBuffer(const IMeshBuffer* buffer)
	{
		if (buffer)
		{
			setBoundingBox(&buffer->getBoundingBox());
		}
		else
		{
			BBox = NULL;
		}
		SourceBuffer = buffer;
	}

	//!
	const IMeshBuffer* getSourceBuffer() const
	{
		return SourceBuffer;
	}

	//!
	const core::aabbox3df* getBoundingBox() const
	{
		return BBox;
	}

	//!
	void setBoundingBox(const core::aabbox3df* bbox)
	{
		BBox = bbox;
	}

	//!
	u32 getVisibleFrame() const
	{
		return VisibleFrame;
	}

	//!
	void setVisibleFrame(u32 value)
	{
		VisibleFrame = value;
	}

	//! Returns the state of this segment's explicit visibility.
	/** \see setVisible(bool) */
	bool isVisible() const
	{
		return IsVisible;
	}

	//! Sets the explicit visibility of this segment.
	/** Explicit visibility is set  */
	void setVisible(bool value)
	{
		IsVisible = value;
	}
};

namespace detail
{

class CBatchMeshBase : public IBatchMesh
{
public:

	// IMesh interface ---------------------------------------------------------

	virtual u32 getMeshBufferCount() const;
	virtual IMeshBuffer* getMeshBuffer(u32 nr) const;
	virtual IMeshBuffer* getMeshBuffer(const video::SMaterial& material) const;
	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual void setBoundingBox(const core::aabbox3df& box);
	virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

	// IBathcList interface ----------------------------------------------------

	//!
	virtual u32 getBatchCount() const;

	//!
	virtual void setBuffer(u32 batch, CBatchBuffer* buffer);

	//!
	virtual CBatchBuffer* getBuffer(u32 batch) const;

	//!
	virtual void quantizeComponents(bool normalInShort, bool positionInShort);

	// IBatchMesh interface ----------------------------------------------------

	//!
	virtual const core::aabbox3df& getBatchBoundingBox(u32 batch) const;

	// this class --------------------------------------------------------------

protected:

	//!
	CBatchMeshBase();

	//!
	u32 sort(const video::IVideoDriver* driver,
			 u32* reordering);

	//!
	bool areBoundingBoxesDirty() const
	{
		return (IsStaticBoundingBoxDirty
				|| (BatchWithDynamicSegmentsCount > 0
					&& BBoxFrame != os::Timer::getTickCount()));
	}

	//!
	struct SBatch
	{
		CBatchBuffer* Buffer;

		u16 SegmentStart;
		u16 SegmentEnd;
		u16 StaticSegmentCount;

		core::aabbox3df StaticBBox;

		SBatch();
		SBatch(u16 segmentStart);
		SBatch(const SBatch& other);
		~SBatch();
		SBatch& operator = (const SBatch& other);

		void setBuffer(CBatchBuffer* buffer);
	};

	core::array<SBatch> Batches;
	core::aabbox3df StaticBBox;
	core::aabbox3df BBox;
	u32 BatchWithDynamicSegmentsCount;
	u32 BBoxFrame;
	bool IsStaticBoundingBoxDirty;
};

} // end namespace detail

/**
   SegmentData must provide the following interface:
   \code
   SegmentData();
   SegmentData(u16 vertexStart, u16 vertexEnd, u32 indexStart, u32 indexEnd);
   SegmentData(const SegmentData& other);
   SegmentData& operator=(const SegmentData& other);
   u16 getVertexStart() const;
   u16 getVertexEnd() const;
   void setVertexStart(u16 vertexStart);
   u32 getIndexStart() const;
   u32 getIndexEnd() const;
   void setIndexStart(u16 vertexStart);
   void setSourceBuffer(const IMeshBuffer* buffer);
   const IMeshBuffer* getSourceBuffer() const;
   const core::aabbox3df* getBoundingBox() const;
   \endcode
 */
template <typename SegmentData>
class CBatchMesh : public detail::CBatchMeshBase
{
public:

	// IBatchList interface ----------------------------------------------------

	//!
	virtual u32 addBatch();

	//!
	virtual u32 addSegment(u32 batch,
						   u16 vertexStart,
						   u16 vertexEnd,
						   u32 indexStart,
						   u32 indexEnd);

#ifdef _DEBUG
	//!
	virtual void setBuffer(u32 batch, CBatchBuffer* buffer);
#endif

	//!
	virtual u32 getSegmentCount(u32 batch) const;

	//!
	virtual void getSegmentVertexRange(u32 batch,
									   u32 segment,
									   u16& start,
									   u16& end) const;

	//!
	virtual void getSegmentIndexRange(u32 batch,
									  u32 segment,
									  u32& start,
									  u32& end) const;

	//!
	virtual void* getSegmentData(u32 batch, u32 segment);

	//!
	virtual const void* getSegmentData(u32 batch, u32 segment) const;

	//!
	virtual void getSegmentBoundingBox(u32 batch,
									   u32 segment,
									   core::aabbox3df& bbox) const;

	//!
	virtual core::vector3df getSegmentCenter(u32 batch,
											 u32 segment) const;

	//!
	virtual void setSegmentCompileInfo(u32 batch,
									   u32 segment,
									   const SCompileInfo& data);

	// IBatchMesh interface ----------------------------------------------------

	//! Computes the bounding box of the whole batch list and of each of its
	//! batches.
	virtual void updateBoundingBox();

	//!
	virtual void clear();

	//!
	virtual u32 sort(const video::IVideoDriver* driver);

	//!
	virtual void updateSegmentContent(u32 batch,
									  u32 segment,
									  const IMeshBuffer* content,
									  u32 attributeMask = u32(-1),
									  bool updateIndices = true,
									  bool useMaterial = true);

	//!
	virtual void setRenderLayer(u32 batch, u32 segment, s16 layer);

	//!
	virtual s16 getRenderLayer(u32 batch, u32 segment) const;

	// this class --------------------------------------------------------------

	typedef SegmentData Segment;
	typedef Segment* SegmentIterator;
	typedef const Segment* ConstSegmentIterator;

	//! Ensures internal allocations tightly fit data.
	void strip();

	//!
	const Segment& getSegment(u32 batch, u32 segment) const
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		const SBatch& b = Batches[batch];
		segment += b.SegmentStart;
		_IRR_DEBUG_BREAK_IF(segment >= b.SegmentEnd);
		return Segments[segment];
	}

	//!
	Segment& getSegment(u32 batch, u32 segment)
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		const SBatch& b = Batches[batch];
		segment += b.SegmentStart;
		_IRR_DEBUG_BREAK_IF(segment >= b.SegmentEnd);
		return Segments[segment];
	}
	
	//!
	SegmentIterator segmentBegin(u32 batch)
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		return Segments.pointer() + Batches[batch].SegmentStart;
	}

	//!
	SegmentIterator segmentEnd(u32 batch)
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		return Segments.pointer() + Batches[batch].SegmentEnd;
	}

	//!
	ConstSegmentIterator segmentBegin(u32 batch) const
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		return Segments.const_pointer() + Batches[batch].SegmentStart;
	}

	//!
	ConstSegmentIterator segmentEnd(u32 batch) const
	{
		_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
		return Segments.const_pointer() + Batches[batch].SegmentEnd;
	}
	
	//! Provides an iterator on segments throughout all batches.
	SegmentIterator segmentBegin()
	{
		return Segments.pointer();
	}

	//!
	SegmentIterator segmentEnd()
	{
		return Segments.pointer() + Segments.size();
	}

	//! Provides a const iterator on segments throughout all batches.
	ConstSegmentIterator segmentBegin() const
	{
		return Segments.const_pointer();
	}

	//!
	ConstSegmentIterator segmentEnd() const
	{
		return Segments.const_pointer() + Segments.size();
	}

	core::array<Segment> Segments;
};

} // end namespace scene
} // end namespace irr

#ifndef _IRR_EXPLICIT_TEMPLATE_INSTANTIATION_
#    include "CBatchMesh_impl.h"
#endif

#endif
