//-*-c++-*-
#ifndef __C_SCENE_BATCH_MESH_IMPL_H_INCLUDED__
#define __C_SCENE_BATCH_MESH_IMPL_H_INCLUDED__

#include "CBatchMesh.h"

#include "irrGeometryUtil.h"
#include "irrProcessBufferHeap.h"
#include "coreutil.h"
#include "IBatchBinding.h"

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

// IBatchList interface --------------------------------------------------------

template <typename SegmentData>
inline
u32
CBatchMesh<SegmentData>::addBatch()
{
	u32 id = Batches.size();
	Batches.push_back(SBatch(Segments.size()));
	return id;
}

template <typename SegmentData>
inline
u32
CBatchMesh<SegmentData>::addSegment(u32 batch,
									u16 vertexStart,
									u16 vertexEnd,
									u32 indexStart,
									u32 indexEnd)
{
	_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
	SBatch& b = Batches[batch];
	Segments.push_back(Segment(vertexStart, vertexEnd, indexStart, indexEnd));
	u32 segId = b.SegmentEnd - b.SegmentStart;
	++b.SegmentEnd;
	_IRR_DEBUG_BREAK_IF(b.SegmentEnd != Segments.size());
	_IRR_DEBUG_BREAK_IF(
		b.SegmentEnd - 1 > b.SegmentStart
		&& ((Segments[b.SegmentEnd - 1].getVertexStart()
			 != Segments[b.SegmentEnd - 2].getVertexEnd())
			|| (Segments[b.SegmentEnd - 1].getIndexStart()
				!= Segments[b.SegmentEnd - 2].getIndexEnd()))
	);
	return segId;
}

#ifdef _DEBUG
template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::setBuffer(u32 batch, CBatchBuffer* buffer)
{
	detail::CBatchMeshBase::setBuffer(batch, buffer);
	for (u16 i = Batches[batch].SegmentStart; i < Batches[batch].SegmentEnd; ++i)
	{
		_IRR_DEBUG_BREAK_IF(Segments[i].getVertexStart() >= buffer->getVertexCount());
		_IRR_DEBUG_BREAK_IF(Segments[i].getVertexEnd() > buffer->getVertexCount());
		_IRR_DEBUG_BREAK_IF(Segments[i].getIndexStart() >= buffer->getIndexCount());
		_IRR_DEBUG_BREAK_IF(Segments[i].getIndexEnd() > buffer->getIndexCount());
	}
}
#endif

template <typename SegmentData>
inline
u32
CBatchMesh<SegmentData>::getSegmentCount(u32 batch) const
{
	_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
	const SBatch& b = Batches[batch];
	return b.SegmentEnd - b.SegmentStart;
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::getSegmentVertexRange(u32 batch,
											   u32 segment,
											   u16& start,
											   u16& end) const
{
	const Segment& seg = getSegment(batch, segment);
	start = seg.getVertexStart();
	end = seg.getVertexEnd();
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::getSegmentIndexRange(u32 batch,
											  u32 segment,
											  u32& start,
											  u32& end) const
{
	const Segment& seg = getSegment(batch, segment);
	start = seg.getIndexStart();
	end = seg.getIndexEnd();
}

template <typename SegmentData>
inline
void*
CBatchMesh<SegmentData>::getSegmentData(u32 batch, u32 segment)
{
	return &getSegment(batch, segment);
}

template <typename SegmentData>
inline
const void*
CBatchMesh<SegmentData>::getSegmentData(u32 batch, u32 segment) const
{
	return &getSegment(batch, segment);
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::getSegmentBoundingBox(u32 batch,
											   u32 segment,
											   core::aabbox3df& bbox) const
{
	const Segment& seg = getSegment(batch, segment);
	Batches[batch].Buffer->getBoundingBox(seg.getVertexStart(),
										  seg.getVertexEnd(),
										  bbox);
}

template <typename SegmentData>
inline
core::vector3df
CBatchMesh<SegmentData>::getSegmentCenter(u32 batch,
										  u32 segment) const
{
	const Segment& seg = getSegment(batch, segment);
	if (seg.getBoundingBox())
	{
		return seg.getBoundingBox()->getCenter();
	}
	core::aabbox3df bbox;
	getSegmentBoundingBox(batch, segment, bbox);
	return bbox.getCenter();
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::setSegmentCompileInfo(u32 batch,
											   u32 segment,
											   const SCompileInfo& data)
{
	IBatchMesh::setSegmentCompileInfo(batch, segment, data);
	if (data.SourceBuffer && data.SourceBuffer->isDynamic())
	{
		Segment& seg = getSegment(batch, segment);
		seg.setSourceBuffer(data.SourceBuffer);
	}
}

// IBatchMesh interface --------------------------------------------------------

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::updateBoundingBox()
{
	u32 batchCount = Batches.size();
	if (IsStaticBoundingBoxDirty)
	{
		bool needsInit = true;
		for (u32 b = 0; b < batchCount; ++b)
		{
			SBatch& batch = Batches[b];
			bool batchNeedsInit = true;
			core::aabbox3df batchbbox;
			for (u32 s = 0, segCnt = batch.StaticSegmentCount; s < segCnt; ++s)
			{
				core::aabbox3df segbbox;
				getSegmentBoundingBox(b, s, segbbox);
				if (batchNeedsInit)
				{
					batchbbox = segbbox;
					batchNeedsInit = false;
				}
				else
				{
					batchbbox.addInternalBox(segbbox);
				}
			}
			batch.StaticBBox = batchbbox;
			batch.Buffer->setBoundingBox(batchbbox);
			if (needsInit)
			{
				StaticBBox = batchbbox;
				needsInit = false;
			}
			else
			{
				StaticBBox.addInternalBox(batchbbox);
			}
		}
		BBox = StaticBBox;
		BBoxFrame = os::Timer::getTickCount();
		IsStaticBoundingBoxDirty = false;
	}

	if (BatchWithDynamicSegmentsCount > 0)
	{
		bool needsInit = true;
		if (BatchWithDynamicSegmentsCount < batchCount)
		{
			BBox = StaticBBox;
			needsInit = false;
		}
		core::aabbox3df batchbbox;
		for (u32 b = 0, dynCnt = BatchWithDynamicSegmentsCount; dynCnt > 0; ++b)
		{
			SBatch& batch = Batches[b];
			u32 s = batch.StaticSegmentCount;
			u32 segCnt = batch.SegmentEnd - batch.SegmentStart;
			if (s < segCnt)
			{
				_IRR_DEBUG_BREAK_IF(segCnt == 0);
				if (batch.StaticSegmentCount == 0)
				{
					batchbbox
						= getSegment(b, s).getSourceBuffer()->getBoundingBox();
					s = 1;
				}
				else
				{
					batchbbox = batch.StaticBBox;
				}
				for (; s < segCnt; ++s)
				{
					_IRR_DEBUG_BREAK_IF(
						getSegment(b, s).getSourceBuffer() == NULL
					);
					batchbbox.addInternalBox(
						getSegment(b, s).getSourceBuffer()->getBoundingBox()
					);					
				}
				batch.Buffer->setBoundingBox(batchbbox);
				--dynCnt;
				if (needsInit)
				{
					BBox = batchbbox;
					needsInit = false;
				}
				else
				{
					BBox.addInternalBox(batchbbox);
				}
			}
			_IRR_DEBUG_BREAK_IF(b >= Batches.size());
		}
		BBoxFrame = os::Timer::getTickCount();
	}
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::clear()
{
	// Unbind dynamic segments
	for (u32 b = 0, btCnt = Batches.size(); b < btCnt; ++b)
	{
		u32 segCnt = getSegmentCount(b);
		for (u32 s = 0; s < segCnt; ++s)
		{
			Segment& seg = getSegment(b, s);
			if (seg.getSourceBuffer() != NULL)
			{
				static_cast<video::IBatchBinding*>(seg.getSourceBuffer()->getDriverBinding())->setInBatch(NULL, 0, 0);
			}
		}
	}

	Batches.clear();
	Segments.clear();
}

template <typename SegmentData>
inline
u32
CBatchMesh<SegmentData>::sort(const video::IVideoDriver* driver)
{
	BatchWithDynamicSegmentsCount = 0;
	IsStaticBoundingBoxDirty = true;
	core::array<u32, irrProcessBufferAllocator<u32> > reordering(Batches.size());
	reordering.set_used(Batches.size());
	u32 solidEnd = CBatchMeshBase::sort(driver, reordering.pointer());
	for (u32 b = 0, cnt = reordering.size(); b < cnt; ++b)
	{
		u32 segCnt = getSegmentCount(b);
		Batches[b].StaticSegmentCount = 0;
		core::array<u32, irrProcessBufferAllocator<u32> >
			segmentReordering(segCnt);
		segmentReordering.set_used(segCnt);
		for (u32 s = 0; s < segCnt; ++s)
		{
			segmentReordering[s] = s;
		}
		u32 staticSegmentsEnd = segCnt;
		u32 dynamicVertexCount = 0;
		u32 dynamicIndexCount = 0;
		u16 dynamicVertexOffset = 0;
		u32 dynamicIndexOffset = 0;

		// First pass to check to every segment are of the same type (static or
		// dynamic)
		for (u32 s = 0; s < segCnt; s++)
		{
			Segment& seg = getSegment(b, s);
			if (seg.getSourceBuffer() != 0)
			{
				--staticSegmentsEnd;
			}
		}
		Batches[b].StaticSegmentCount = staticSegmentsEnd;

		// if all segments are of the same type, then we can skip over the
		// per-batch sorting
		if (staticSegmentsEnd > 0 && staticSegmentsEnd < segCnt)
		{
			// Reset for second pass to swap segment info
			staticSegmentsEnd = segCnt;
			for (u32 s = 0; s < staticSegmentsEnd; )
			{
				Segment& seg = getSegment(b, s);
				if (seg.getSourceBuffer() != 0)
				{
					--staticSegmentsEnd;
					dynamicVertexCount += (seg.getVertexEnd() - seg.getVertexStart());
					dynamicIndexCount += (seg.getIndexEnd() - seg.getIndexStart());

					//core::swap(seg, lastStaticSeg);
					//core::swap(segmentReordering[s], segmentReordering[staticSegmentsEnd]);

					// we need to use this kind of sorting because we'll move data blocks
					// attached to static segments without aditionaly memory
					Segment crtSeg = getSegment(b, s);
					u32 crtSegReordIdx = segmentReordering[s];
					for (u32 sIdx = s; sIdx < segCnt - 1; sIdx++)
					{
						getSegment(b, sIdx) = getSegment(b, sIdx + 1);
						segmentReordering[sIdx] = segmentReordering[sIdx + 1];
					}
					getSegment(b, segCnt - 1) = crtSeg;
					segmentReordering[segCnt - 1] = crtSegReordIdx;
				}
				else
				{
					dynamicVertexOffset += seg.getVertexEnd() - seg.getVertexStart();
					dynamicIndexOffset += seg.getIndexEnd() - seg.getIndexStart();
					++s;
				}
			}
			_IRR_DEBUG_BREAK_IF(staticSegmentsEnd
								!= Batches[b].StaticSegmentCount);

			++BatchWithDynamicSegmentsCount;

			u32 oneVertexSize
				= reinterpret_cast<const video::S3DVertexComponentArrays*>(
					Batches[b].Buffer->getVertices()
				)->PositionStride;
			SScopedProcessArray<u8> tmpVertexData(oneVertexSize * dynamicVertexCount);
			SScopedProcessArray<u16> tmpIndexData(dynamicIndexCount);
			_IRR_DEBUG_BREAK_IF(tmpVertexData.get() == NULL);
			_IRR_DEBUG_BREAK_IF(tmpIndexData.get() == NULL);

			u32 tmpVertexOffset = 0;
			u32 tmpIndexOffset = 0;

			u8* vertexData = reinterpret_cast<u8*>(Batches[b].Buffer->getVertexBuffer());
			u16* indexData = reinterpret_cast<u16*>(Batches[b].Buffer->getIndexBuffer());

			// save dynamic vertex and index data to temporary buffers
			for (u32 s = staticSegmentsEnd; s < segCnt; ++s)
			{
				Segment& seg = getSegment(b, s);

				u16 vertexStep = seg.getVertexEnd() - seg.getVertexStart();
				u32 vertexByteStep = vertexStep * oneVertexSize;
				_IRR_DEBUG_BREAK_IF(
					tmpVertexData.get() + (tmpVertexOffset + vertexByteStep)
					> tmpVertexData.get() + oneVertexSize * dynamicVertexCount
				);
				memcpy(tmpVertexData.get() + tmpVertexOffset,
					   vertexData + seg.getVertexStart() * oneVertexSize,
					   vertexStep * oneVertexSize);
				tmpVertexOffset += vertexByteStep;

				u16 indexDelta = dynamicVertexOffset - seg.getVertexStart();
				seg.setVertexStart(dynamicVertexOffset);

				u32 indexStep = seg.getIndexEnd() - seg.getIndexStart();
				for (u16
						 *dst = tmpIndexData.get() + tmpIndexOffset,
						 *dstEnd = dst + indexStep,
						 *src = indexData + seg.getIndexStart();
					 dst != dstEnd;
					 ++dst, ++src)
				{
					//_IRR_DEBUG_BREAK_IF(dst >= tmpIndexData.get() + dynamicIndexCount);
					*dst = *src + indexDelta;
				}
				tmpIndexOffset += indexStep;
				seg.setIndexStart(dynamicIndexOffset);

				dynamicIndexOffset += indexStep;
				dynamicVertexOffset += vertexStep;
			}

			// pack together static vertex data
			u16 vertexOffset = 0;
			u32 indexOffset = 0;
			for (u32 s = 0; s < staticSegmentsEnd; ++s)
			{
				Segment& seg = getSegment(b, s);

				u16 vertexStep = seg.getVertexEnd() - seg.getVertexStart();
				_IRR_DEBUG_BREAK_IF(vertexOffset + vertexStep
									> (u16)Batches[b].Buffer->getVertexCount());
				memmove(vertexData + vertexOffset * oneVertexSize,
						vertexData + seg.getVertexStart() * oneVertexSize,
						vertexStep * oneVertexSize);
				
				s32 indexDelta = vertexOffset - seg.getVertexStart();
				seg.setVertexStart(vertexOffset);

				u32 indexStep = seg.getIndexEnd() - seg.getIndexStart();
				for (u16
						 *dst = indexData + indexOffset,
						 *dstEnd = dst + indexStep,
						 *src = indexData + seg.getIndexStart();
					 dst != dstEnd;
					 ++dst, ++src)
				{
					_IRR_DEBUG_BREAK_IF(
						dst >= indexData + (Batches[b].Buffer->getIndexCount()
											- dynamicIndexCount)
					);
					*dst = *src + indexDelta;
				}

				seg.setIndexStart(indexOffset);

				indexOffset += indexStep;
				vertexOffset += vertexStep;
			}

			// append dynamic data
			memcpy(vertexData + vertexOffset * oneVertexSize,
				   tmpVertexData.get(),
				   dynamicVertexCount * oneVertexSize);
			memcpy(indexData + indexOffset,
				   tmpIndexData.get(),
				   dynamicIndexCount * 2);
		}

		// Update segment compile info
		if (reordering[b] != b || Batches[b].StaticSegmentCount < segCnt)
		{
			for (u32 s = 0; s < segCnt; ++s)
			{
				Segment& seg = getSegment(b, s);
				if (seg.getSourceBuffer() != NULL)
				{
					static_cast<video::IBatchBinding*>(
						seg.getSourceBuffer()->getDriverBinding()
					)->setInBatch(this, b, s);
				}
				if (segmentReordering[s] != s)
				{
					sortCallback(reordering[b], segmentReordering[s], b, s);
				}
			}
		}
#ifdef _DEBUG
		// big validation
		CBatchBuffer* buffer = Batches[b].Buffer;
		for (u32 s = 0, cnt = getSegmentCount(b); s < cnt; ++s)
		{
			Segment& seg = getSegment(b, s);
			_IRR_DEBUG_BREAK_IF(seg.getIndexStart() >= buffer->getIndexCount());
			_IRR_DEBUG_BREAK_IF(seg.getIndexEnd() > buffer->getIndexCount());
			u16* indices = reinterpret_cast<u16*>(buffer->getIndices());
			u16 indexRange = seg.getIndexEnd() - seg.getIndexStart();
			for (u16
					 *i = indices + seg.getIndexStart(),
					 *iend = i + indexRange;
				 i != iend;
				 ++i)
			{
				_IRR_DEBUG_BREAK_IF(*i < seg.getVertexStart());
				_IRR_DEBUG_BREAK_IF(*i >= seg.getVertexEnd());
			}

			if (s > 0)
			{
				Segment& prevSeg = getSegment(b, s - 1);
				_IRR_DEBUG_BREAK_IF(seg.getVertexStart() != prevSeg.getVertexEnd());
				_IRR_DEBUG_BREAK_IF(seg.getIndexStart() != prevSeg.getIndexEnd());
			}
		}
#endif
	}
	return solidEnd;
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::updateSegmentContent(u32 batch,
											  u32 segment,
											  const IMeshBuffer* content,
											  u32 attributeMask,
											  bool updateIndices,
											  bool useMaterial)
{
	Segment& seg = getSegment(batch, segment);
	CBatchBuffer* buffer = getBuffer(batch);

	u16 vertexStart = static_cast<u16>(content->getVertexIndexStart());
	if (attributeMask != 0)
	{
		u16 vertexEnd = static_cast<u16>(content->getVertexIndexEnd());
		_IRR_DEBUG_BREAK_IF(u16(seg.getVertexStart() + vertexEnd - vertexStart)
							> seg.getVertexEndStorage());
		buffer->overwrite(
			*reinterpret_cast<const video::S3DVertexComponentArrays*>(
				content->getVertices()
			),
			vertexStart,
			vertexEnd,
			seg.getVertexStart(),
			attributeMask,
			useMaterial ? &content->getMaterial() : NULL
		);

		seg.setVertexEnd(seg.getVertexStart() + u16(vertexEnd - vertexStart));
	}

	if (updateIndices)
	{
		_IRR_DEBUG_BREAK_IF(seg.getIndexStart() + content->getIndexCount()
							> seg.getIndexEndStorage());
		_IRR_DEBUG_BREAK_IF(content->getIndexType() != video::EIT_16BIT);
		// TODO: support more primitive types
		_IRR_DEBUG_BREAK_IF(content->getPrimitiveType() != EPT_TRIANGLES);
		u32 indexCount = content->getIndexCount();
		buffer->overwrite(reinterpret_cast<const u16*>(content->getIndices()),
						  seg.getVertexStart(),
						  indexCount / 3,
						  EPT_TRIANGLES,
						  seg.getIndexStart());

		seg.setIndexEnd(seg.getIndexStart() + indexCount);
	}

	segmentContentUpdateCallback(batch,
								 segment,
								 attributeMask != 0,
								 updateIndices);
}

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::setRenderLayer(u32 batch, u32 segment, s16 layer)
{
	getSegment(batch, segment).setRenderLayer(layer);
}

template <typename SegmentData>
inline
s16
CBatchMesh<SegmentData>::getRenderLayer(u32 batch, u32 segment) const
{
	return getSegment(batch, segment).getRenderLayer();
}

// this class ------------------------------------------------------------------

template <typename SegmentData>
inline
void
CBatchMesh<SegmentData>::strip()
{
	if (Batches.size() != Batches.allocated_size())
	{
		Batches.reallocate(Batches.size());
	}
	if (Segments.size() != Segments.allocated_size())
	{
		Segments.reallocate(Segments.size());
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
