#include "stdafx.h"
#include "CBatchMesh.h"

namespace irr
{
namespace scene
{
namespace detail
{

//------------------------------------------------------------------------------
// CBatchMeshBase
//------------------------------------------------------------------------------

CBatchMeshBase::CBatchMeshBase()
	: BatchWithDynamicSegmentsCount(0)
	, BBoxFrame(-1)
	, IsStaticBoundingBoxDirty(true)
{
}

// IMesh interface -------------------------------------------------------------

u32
CBatchMeshBase::getMeshBufferCount() const
{
	return Batches.size();
}

IMeshBuffer*
CBatchMeshBase::getMeshBuffer(u32 nr) const
{
	return Batches[nr].Buffer;
}

IMeshBuffer*
CBatchMeshBase::getMeshBuffer(const video::SMaterial& material) const
{
	for (u32 i = 0, cnt = getMeshBufferCount(); i < cnt; ++i)
	{
		IMeshBuffer* mb = getMeshBuffer(i);
		if (mb && mb->getMaterial().matches(material))
		{
			return mb;
		}
	}
	return NULL;
}

const core::aabbox3d<f32>&
CBatchMeshBase::getBoundingBox() const
{
	if (areBoundingBoxesDirty())
	{
		const_cast<CBatchMeshBase*>(this)->updateBoundingBox();
	}
	return BBox;
}

void
CBatchMeshBase::setBoundingBox(const core::aabbox3df& box)
{
	BBoxFrame = os::Timer::getTickCount();
	BBox = box;
}

void
CBatchMeshBase::setMaterialFlag(video::E_MATERIAL_FLAG /*flag*/,
								bool /*newvalue*/)
{
	_IRR_DEBUG_BREAK_IF("ILLEGAL");
}

// IBatchList interface --------------------------------------------------------

u32
CBatchMeshBase::getBatchCount() const
{
	return Batches.size();
}

void
CBatchMeshBase::setBuffer(u32 batch, CBatchBuffer* buffer)
{
	_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
	Batches[batch].setBuffer(buffer);
}

CBatchBuffer*
CBatchMeshBase::getBuffer(u32 batch) const
{
	return static_cast<CBatchBuffer*>(getMeshBuffer(batch));
}

void
CBatchMeshBase::quantizeComponents(bool normalInShort,
								   bool positionInShort)
{
	for (u32 i = 0, cnt = getBatchCount(); i < cnt; ++i)
	{
		if (Batches[i].StaticSegmentCount == getSegmentCount(i))
		{
			getBuffer(i)->quantizeComponents(normalInShort, positionInShort);
		}
	}
}

// IBatchMesh interface -------------------------------------------------------

const core::aabbox3df&
CBatchMeshBase::getBatchBoundingBox(u32 batch) const
{
	if (areBoundingBoxesDirty())
	{
		const_cast<CBatchMeshBase*>(this)->updateBoundingBox();
	}
	_IRR_DEBUG_BREAK_IF(batch >= Batches.size());
	return Batches[batch].Buffer->getBoundingBox();
}

// this class ------------------------------------------------------------------

u32
CBatchMeshBase::sort(const video::IVideoDriver* driver,
					 u32* reordering)
{
	u32 solidEnd = Batches.size();
	for (u32 i = 0; i < solidEnd; ++i)
	{
		reordering[i] = i;
	}
	for (u32 i = 0; i < solidEnd; )
	{
		video::IMaterialRenderer* mr = driver->getMaterialRenderer(
			Batches[i].Buffer->getMaterial().getMaterialType()
		);
		if (mr && mr->isTransparent())
		{
			--solidEnd;
			core::swap(Batches[solidEnd], Batches[i]);
			core::swap(reordering[solidEnd], reordering[i]);
		}
		else
		{
			++i;
		}
	}
	return solidEnd;
}

CBatchMeshBase::SBatch::SBatch()
	: Buffer(NULL)
	, SegmentStart(0)
	, SegmentEnd(0)
	, StaticSegmentCount(0)
{
}

CBatchMeshBase::SBatch::SBatch(u16 segmentStart)
	: Buffer(NULL)
	, SegmentStart(segmentStart)
	, SegmentEnd(segmentStart)
	, StaticSegmentCount(0)
{
}

CBatchMeshBase::SBatch::SBatch(const SBatch& other)
	: Buffer(other.Buffer)
	, SegmentStart(other.SegmentStart)
	, SegmentEnd(other.SegmentEnd)
	, StaticSegmentCount(other.StaticSegmentCount)
{
	if (Buffer)
	{
		Buffer->grab();
	}
}

CBatchMeshBase::SBatch::~SBatch()
{
	setBuffer(NULL);
}

CBatchMeshBase::SBatch&
CBatchMeshBase::SBatch::operator = (const SBatch& other)
{
	setBuffer(other.Buffer);
	SegmentStart = other.SegmentStart;
	SegmentEnd = other.SegmentEnd;
	StaticSegmentCount = other.StaticSegmentCount;
	return *this;
}

void
CBatchMeshBase::SBatch::setBuffer(CBatchBuffer* buffer)
{
	if (buffer)
	{
		buffer->grab();
	}
	if (Buffer)
	{
		Buffer->drop();
	}
	Buffer = buffer;
}

} // end namespace detail
} // end namespace scene
} // end namespace irr
