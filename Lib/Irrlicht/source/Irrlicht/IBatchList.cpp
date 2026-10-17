#include "stdafx.h"
#include "IBatchList.h"

#include "CBatchBuffer.h"
#include "irrProcessBufferHeap.h"
#include "irrArray.h"

namespace irr
{
namespace scene
{

IBatchList::IBatchList()
	: CompileCallback(NULL)
{
}

IBatchList::~IBatchList()
{
}

void
IBatchList::setSegmentCompileInfo(u32 batch,
								  u32 segment,
								  const SCompileInfo& data)
{
	if (CompileCallback)
	{
		(*CompileCallback)(this, batch, segment, data);
	}
}

void
IBatchList::setCompileCallback(IBatchCompileCallback* callback)
{
	CompileCallback = callback;
}

void
IBatchList::quantizeComponents(bool normalInShort,
							   bool positionInShort)
{
	for (u32 i = 0, cnt = getBatchCount(); i < cnt; ++i)
	{
		getBuffer(i)->quantizeComponents(normalInShort, positionInShort);
	}
}

namespace
{

struct SBufferEntry
{
	u32 Size;
	CBatchBuffer* Buffer;

	SBufferEntry(CBatchBuffer* buffer)
		: Size(buffer->getVertexBufferSize())
		, Buffer(buffer)
	{
	}

	bool operator < (const SBufferEntry& other) const
	{
		return other.Size < Size;
	}
};

}

//!
void
IBatchList::transferToHardwareBuffers(E_HARDWARE_MAPPING mappingHint,
									  u32 maxTotalSize)
{
	u32 totalSize = 0;
	u32 batchCount = getBatchCount();
	for (u32 i = 0; i < batchCount; ++i)
	{
		totalSize += getBuffer(i)->getVertexBufferSize();
	}
	if (maxTotalSize == 0)
	{
		maxTotalSize = totalSize;
	}

	if (maxTotalSize < totalSize)
	{
		// The problem here is equivalent to solve the Knapsack problem, which
		// is NP-hard, so we use a greedy algorithm approximation to select the
		// buffers
		core::array<SBufferEntry, irrProcessBufferAllocator<SBufferEntry> >
			entries(batchCount);
		entries.set_used(batchCount);
		for (u32 i = 0; i < batchCount; ++i)
		{
			entries[i] = SBufferEntry(getBuffer(i));
		}
		entries.sort();
		totalSize = 0;
		for (u32 i = 0; i < batchCount && totalSize < maxTotalSize; ++i)
		{
			u32 sz = totalSize + entries[i].Size;
			if (sz <= maxTotalSize)
			{
				totalSize = sz;
				entries[i].Buffer->transferToHardwareBuffer(mappingHint);
			}
		}
	}
	else
	{
		for (u32 i = 0; i < batchCount; ++i)
		{
			getBuffer(i)->transferToHardwareBuffer(mappingHint);
		}
	}
}

} // end namespace scene
} // end namespace irr
