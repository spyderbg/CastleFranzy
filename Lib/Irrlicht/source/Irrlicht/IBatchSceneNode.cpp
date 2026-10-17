#include "stdafx.h"
#include "IBatchSceneNode.h"

#include "CBatchListCompileData.h"

#include "irrProcessBufferHeap.h"
#include "IMaterialRenderer.h"
#include "CBatchBuffer.h"

namespace irr
{
namespace scene
{

#ifdef _DEBUG
u32&
getVisibleSegmentCount()
{
	static u32 VisibleSegmentCount = 0;
	return VisibleSegmentCount;
}

u32&
getVisibleSolidSegmentCount()
{
	static u32 VisibleSolidSegmentCount = 0;
	return VisibleSolidSegmentCount;
}

u32&
getTotalSegmentCount()
{
	static u32 TotalSegmentCount = 0;
	return TotalSegmentCount;
}

u32&
getRenderedBatchCount()
{
	static u32 RenderedBatchCount = 0;
	return RenderedBatchCount;
}

u32&
getRenderedDirtyBatchCount()
{
	static u32 RenderedDirtyBatchCount = 0;
	return RenderedDirtyBatchCount;
}
#endif

namespace
{

struct SSetSegmentLayerCompileCallback
	: IBatchCompileCallback
{
	ISceneManager* SceneManager;
	IBatchCompileCallback* OtherCallback;

	SSetSegmentLayerCompileCallback(ISceneManager* sceneManager,
									IBatchCompileCallback* other)
		: SceneManager(sceneManager)
		, OtherCallback(other)
	{
	}

	virtual void operator () (IBatchList* batchList,
							  u32 batch,
							  u32 segment,
							  const SCompileInfo& data)
	{
		int layer;
		SceneManager->getCurrentRenderedNode(NULL, &layer);
		static_cast<IBatchMesh*>(batchList)->setRenderLayer(
			batch, segment, static_cast<s16>(layer)
		);
		if (OtherCallback)
		{
			(*OtherCallback)(batchList, batch, segment, data);
		}
	}
};

} // end anonymous namespace

IBatchSceneNode::IBatchSceneNode(s32 id,
								 IBatchMesh* batchMesh,
								 bool registerSolidBatchesOnce)
	: IMeshSceneNode(id)
	, BatchMesh(batchMesh)
	, BatchCullingType(EAC_FRUSTUM_BOX)
	, SegmentCullingType(EAC_FRUSTUM_BOX)
	, MaxIndexCountSolid(0)
	, MaxIndexCountTransparent(0)
	, SolidBatchCount(-1)
	, RegisterSolidBatchesOnce(registerSolidBatchesOnce)
	, HasVisibleIndexCache(true)
	, VisibleIndices(NULL)
	, VisibleSegments(NULL)
	, CurrentTransparentBatch(u32(-1))
{
	if (BatchMesh)
	{
		BatchMesh->grab();
	}
}

IBatchSceneNode::~IBatchSceneNode()
{
	if (VisibleSegments)
	{
		delete[] VisibleSegments;
	}
	if (BatchMesh)
	{
		BatchMesh->drop();
	}
	BatchMesh = NULL;
	setVisibleIndexCache(false);
}

// ISceneNode interface --------------------------------------------------------

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
const core::aabbox3d<f32>&
IBatchSceneNode::getTransformedBoundingBox() const
{
	return getBoundingBox();
}
#else
const core::aabbox3d<f32>
IBatchSceneNode::getTransformedBoundingBox() const
{
	core::aabbox3d<f32> box = getBoundingBox();
	return box;
}
#endif

const core::aabbox3d<f32>&
IBatchSceneNode::getBoundingBox() const
{
	return BatchMesh->getBoundingBox();
}

void
IBatchSceneNode::render(void* renderData)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (driver == NULL)
	{
		return;
	}

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	bool batchingEnabled = driver->getOption(video::EVDO_BATCHING);
	if (batchingEnabled)
	{
		driver->setOption(video::EVDO_BATCHING, false);
	}

	if (renderData)
	{
		u32 batch = reinterpret_cast<u32>(renderData);
		if (batch > u32(u16(-1)))
		{
			_IRR_DEBUG_BREAK_IF(
				(batch & u32(u16(-1))) >= BatchMesh->getMeshBufferCount()
				|| (batch >> 16) > BatchMesh->getSegmentCount(batch & u32(u16(-1)))
				|| (batch >> 16) == 0
			);

			// transparent batches are encoded in two 16 bits number:
			// (segment + 1, batch)
			renderTransparentSegment(driver,
									 batch & u32(u16(-1)),
									 (batch >> 16) - 1);
#ifdef _DEBUG
			batch = batch & u32(u16(-1));
#endif
		}
		else
		{
			--batch;
			_IRR_DEBUG_BREAK_IF(batch
								>= BatchMesh->getMeshBufferCount());
			// minus one because renderData == 0 means to draw all solid batches
			renderSolidBatch(driver, batch);
		}
#ifdef _DEBUG
		if (DebugDataVisible & EDS_BBOX)
		{
			renderBatchBBoxes(driver, batch);
		}
#endif
	}
	else
	{
		_IRR_DEBUG_BREAK_IF(
			SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT
		);
		for (u32 i = 0; i < SolidBatchCount; ++i)
		{
			renderSolidBatch(driver, i);
#ifdef _DEBUG
			if (DebugDataVisible & EDS_BBOX)
			{
				renderBatchBBoxes(driver, i);
			}
#endif
		}
	}

	if (batchingEnabled)
	{
		driver->setOption(video::EVDO_BATCHING, true);
	}
}

void
IBatchSceneNode::serializeAttributes(
	io::IAttributes* out,
	io::SAttributeReadWriteOptions* options
) const
{
	if (!out)
	{
		return;
	}
	ISceneNode::serializeAttributes(out, options);

	// R/W attributes
	out->addEnum("SegmentAutomaticCulling",
				 (s32)getSegmentAutomaticCulling(),
				 AutomaticCullingNames);
	out->addBool("RegisterSolidBatchesOnce", RegisterSolidBatchesOnce);

	out->addBool("HasVisibleIndexCache", hasVisibleIndexCache());

	// RO attributes
	out->addInt("Max index count (solid)", MaxIndexCountSolid, true);
	out->addInt("Max index count (transparent)", MaxIndexCountTransparent, true);
	out->addInt("Solid batch count", SolidBatchCount, true);
}

void
IBatchSceneNode::deserializeAttributes(
	io::IAttributes* in,
	io::SAttributeReadWriteOptions* options
)
{
	if (!in)
	{
		return;
	}
	ISceneNode::deserializeAttributes(in, options);
	setSegmentAutomaticCulling(
		static_cast<E_CULLING_TYPE>(
			in->getAttributeAsEnumeration("SegmentAutomaticCulling",
										  AutomaticCullingNames)
		)
	);
	RegisterSolidBatchesOnce
		= in->getAttributeAsBool("RegisterSolidBatchesOnce");

	bool visibleIndexCache = in->getAttributeAsBool("HasVisibleIndexCache");
	if (visibleIndexCache != hasVisibleIndexCache())
	{
		setVisibleIndexCache(visibleIndexCache);
	}
}

u32
IBatchSceneNode::getMaterialCount() const
{
	return BatchMesh->getMeshBufferCount();
}

video::SMaterial&
IBatchSceneNode::getMaterial(u32 num)
{
	return BatchMesh->getMeshBuffer(num)->getMaterial();
}

// IMeshSceneNode interface ----------------------------------------------------

IMesh*
IBatchSceneNode::getMesh()
{
	return getBatchMesh();
}

bool
IBatchSceneNode::isReadOnlyMaterials() const
{
	return true;
}

// unsupported

void
IBatchSceneNode::setMesh(IMesh* mesh)
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

void
IBatchSceneNode::setReadOnlyMaterials(bool readonly)
{
	_IRR_DEBUG_BREAK_IF("NOT SUPPORTED");
}

// this class ------------------------------------------------------------------

void
IBatchSceneNode::compile(const core::array<ISceneNode*>& nodes,
						 bool skipPostCompile,
						 u32 maxVertexBufferSize,
						 u32 maxIndexBufferSize)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (driver == NULL)
		return;

	if (nodes.size() == 0)
		return;

	if (BatchMesh == NULL)
		BatchMesh = allocateDefaultBatchMesh();
	BatchMesh->clear();

	bool wasTransparentPassRegisteredAsSolid = SceneManager->isTransparentPassRegisteredAsSolid();
	if (!wasTransparentPassRegisteredAsSolid)
	{
		SceneManager->setRegisterTransparentPassAsSolid(true);
	}

	// overwrite (wrap) previous callback
	SSetSegmentLayerCompileCallback callback(SceneManager, BatchMesh->getCompileCallback());
	BatchMesh->setCompileCallback(&callback);

	video::CBatchListCompileData compileData(BatchMesh,
											 maxVertexBufferSize,
											 maxIndexBufferSize);
	driver->beginCompile(&compileData);
	for(int i = nodes.size() - 2; i >= 0; i--)
		nodes[i]->OnRegisterSceneNode();
	SceneManager->drawAll(nodes[nodes.size() - 1]);
	driver->endCompile();

	// restore previous callback
	BatchMesh->setCompileCallback(callback.OtherCallback);

	if (!wasTransparentPassRegisteredAsSolid)
	{
		SceneManager->setRegisterTransparentPassAsSolid(false);
	}

	SolidBatchCount = BatchMesh->sort(driver);
	CurrentTransparentBatch = SolidBatchCount; // anyone would do

	BatchMesh->updateBoundingBox();

	if (!skipPostCompile)
	{
		postCompile();
	}
}

void
IBatchSceneNode::compile(ISceneNode* root,
						 bool skipPostCompile,
						 u32 maxVertexBufferSize,
						 u32 maxIndexBufferSize)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (driver == NULL)
	{
		return;
	}

	if (root == NULL)
	{
		root = SceneManager->getRootSceneNode();
	}

	// ensure the batch node is not visible to make sure it is not self-compiled
	bool wasVisible = isVisible();
	if (wasVisible)
	{
		setVisible(false);
	}

	if (BatchMesh == NULL)
	{
		BatchMesh = allocateDefaultBatchMesh();
	}
	BatchMesh->clear();

	bool wasTransparentPassRegisteredAsSolid
		= SceneManager->isTransparentPassRegisteredAsSolid();
	if (!wasTransparentPassRegisteredAsSolid)
	{
		SceneManager->setRegisterTransparentPassAsSolid(true);
	}

	// overwrite (wrap) previous callback
	SSetSegmentLayerCompileCallback callback(SceneManager,
											 BatchMesh->getCompileCallback());
	BatchMesh->setCompileCallback(&callback);

	video::CBatchListCompileData compileData(BatchMesh,
											 maxVertexBufferSize,
											 maxIndexBufferSize);
	driver->beginCompile(&compileData);
	SceneManager->drawAll(root);
	driver->endCompile();

	// restore previous callback
	BatchMesh->setCompileCallback(callback.OtherCallback);

	if (!wasTransparentPassRegisteredAsSolid)
	{
		SceneManager->setRegisterTransparentPassAsSolid(false);
	}

	SolidBatchCount = BatchMesh->sort(driver);
	CurrentTransparentBatch = SolidBatchCount; // anyone would do

	BatchMesh->updateBoundingBox();

	// revert visibility changes
	if (wasVisible)
	{
		setVisible(true);
	}

	if (!skipPostCompile)
	{
		postCompile();
	}
}

void
IBatchSceneNode::postCompile()
{
	u32 mbCnt = BatchMesh->getMeshBufferCount();

	// Check if solid batch count was initialized; if not, it means we have a
	// call to postCompile() without a compile() call on this node (i.e. we have
	// a sharing node)
	if (SolidBatchCount == u32(-1))
	{
		_IRR_DEBUG_BREAK_IF(BatchMesh == NULL);
		_IRR_DEBUG_BREAK_IF(SceneManager == NULL);
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		SolidBatchCount = BatchMesh->getBatchCount();
		for (u32 i = 0; i < SolidBatchCount; )
		{
			video::IMaterialRenderer* mr = driver->getMaterialRenderer(
				BatchMesh->getMeshBuffer(i)->getMaterial().getMaterialType()
			);
			if (mr && mr->isTransparent())
			{
				--SolidBatchCount;
			}
			else
			{
				++i;
			}
		}
		CurrentTransparentBatch = SolidBatchCount;
	}

	// allocated segment visibility buffer
	u32 totalSegmentCount = 0;
	for (u32 i = 0; i < mbCnt; ++i)
	{
		totalSegmentCount += BatchMesh->getSegmentCount(i);
	}

	// Allocate enough space for all segments + visibility info per batch
	if (VisibleSegments)
	{
		delete[] VisibleSegments;
		VisibleSegments = NULL;
	}
	VisibleSegments = irrnew u32 [totalSegmentCount
								  + mbCnt * sizeof(SSegmentVisibilityInfo)];

#ifdef _DEBUG
	getTotalSegmentCount() += totalSegmentCount;
#endif

	MaxIndexCountSolid = 0;
	MaxIndexCountTransparent = 0;

	// reused as offset
	totalSegmentCount = mbCnt * sizeof(SSegmentVisibilityInfo) / sizeof(u32);

	for (u32 i = 0; i < mbCnt; ++i)
	{
		u32 cnt = BatchMesh->getSegmentCount(i);

		// Setup batch offsets
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(i);
		info.Offset = totalSegmentCount;

		// initialize each batch visible segment count to 0
		info.Count = info.LastCount = 0;
		info.Flags = 1; // put as dirty

		totalSegmentCount += cnt;

		// Update max index count
		u32 indexCount = BatchMesh->getMeshBuffer(i)->getIndexCount();
		if (i < SolidBatchCount)
		{
			if (indexCount > MaxIndexCountSolid)
			{
				MaxIndexCountSolid = indexCount;
			}
		}
		else
		{
			if (indexCount > MaxIndexCountTransparent)
			{
				MaxIndexCountTransparent = indexCount;
			}
		}
	}

	// update index cache
	invalidateVisibleIndexCache();
}

void
IBatchSceneNode::updateIndices(u32 batch,
							   CBatchBuffer* mb,
							   const u16* indexBuffer)
{
	u32 vertexCount = mb->getVertexCount();
	u32 indexStart, indexEnd;
	SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);
	mb->clearIndices();
	for (u32
			 *si = getVisibleSegments(batch),
			 *siEnd = si + info.Count;
		 si != siEnd;
		 ++si)
	{
		BatchMesh->getSegmentIndexRange(batch, *si, indexStart, indexEnd);
		mb->append(indexBuffer + indexStart,
				   // we must put vertexCount here to avoid offsetting (yeah,
				   // it's ugly)
				   vertexCount,
				   (indexEnd - indexStart) / 3,
				   scene::EPT_TRIANGLES);
	}
#ifdef _DEBUG
	++getRenderedDirtyBatchCount();
#endif
	// clear dirty flag
	info.Flags = 0;
}

void
IBatchSceneNode::renderSolidBatch(video::IVideoDriver* driver,
								  u32 batch)
{
	_IRR_DEBUG_BREAK_IF(batch >= SolidBatchCount);
	SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);
	if (info.Count > 0)
	{
		CBatchBuffer* mb = static_cast<CBatchBuffer*>(BatchMesh->getMeshBuffer(batch));

		SScopedProcessArray<u16> workingIndexBuffer;
		void* indexBuffer = NULL;
		u32 indexSize = 0;
		bool owns = false;
		bool doSwap = false;

		if (info.Count < BatchMesh->getSegmentCount(batch))
		{
#ifdef _DEBUG
			u32 initialIndexSize = mb->getIndexCount();
#endif
			if (hasVisibleIndexCache())
			{
				SVisibleIndexCacheInfo& indexInfo
					= getVisibleIndexCacheInfo(batch);
				_IRR_DEBUG_BREAK_IF(indexInfo.Size > initialIndexSize);
				indexBuffer = getVisibleIndexCache(batch);
				indexSize = indexInfo.Size * sizeof(u16);
				mb->swapIndexBuffer(indexBuffer, indexSize, owns);
				if (info.Flags)
				{
					updateIndices(batch, mb, reinterpret_cast<u16*>(indexBuffer));
					indexInfo.Size = mb->getIndexCount();
				}
			}
			else
			{
				workingIndexBuffer.reset(MaxIndexCountSolid);
				indexBuffer = workingIndexBuffer.get();
				_IRR_DEBUG_BREAK_IF(indexBuffer == NULL);
				mb->swapIndexBuffer(indexBuffer, indexSize, owns);
				updateIndices(batch, mb, reinterpret_cast<u16*>(indexBuffer));
			}
			doSwap = true;
			_IRR_DEBUG_BREAK_IF(mb->getIndexCount() >= initialIndexSize);
		}

		driver->setMaterial(mb->getMaterial());
		driver->drawMeshBuffer(mb);

		// put back original
		if (doSwap)
		{
			mb->swapIndexBuffer(indexBuffer, indexSize, owns);
		}

#ifdef _DEBUG
		++getRenderedBatchCount();
#endif
	}
	info.LastCount = info.Count;
}

void
IBatchSceneNode::flushTransparentBatch(video::IVideoDriver* driver)
{
	SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(CurrentTransparentBatch);

	if (info.Count > 0) // it will be zero on the first transparent batch
						// rendered in a frame
	{
		CBatchBuffer* mb = static_cast<CBatchBuffer*>(
			BatchMesh->getMeshBuffer(CurrentTransparentBatch)
		);

#ifdef _DEBUG
		u32 initialIndexSize = mb->getIndexCount();
#endif

		SScopedProcessArray<u16> workingIndexBuffer;
		void* indexBuffer = NULL;
		u32 indexSize = 0;
		bool owns = false;

 		if (info.Count > 1)
		{
			workingIndexBuffer.reset(MaxIndexCountTransparent);
			indexBuffer = workingIndexBuffer.get();
			_IRR_DEBUG_BREAK_IF(indexBuffer == NULL);
			mb->swapIndexBuffer(indexBuffer, indexSize, owns);
			updateIndices(CurrentTransparentBatch,
						  mb,
						  reinterpret_cast<u16*>(indexBuffer));
		}
		else
		{
			// if there is only one visible segment, we can avoid copying
			// indices by "offsetting" the index buffer contained in the batch
			// buffer

			u32 indexStart, indexEnd;
			u32 segment = getVisibleSegments(CurrentTransparentBatch)[0];
			BatchMesh->getSegmentIndexRange(CurrentTransparentBatch,
											segment,
											indexStart,
											indexEnd);
			indexBuffer = reinterpret_cast<u16*>(mb->getIndices()) + indexStart;
			indexSize = (indexEnd - indexStart) * sizeof(u16);

			mb->swapIndexBuffer(indexBuffer, indexSize, owns);
		}

		_IRR_DEBUG_BREAK_IF(mb->getIndexCount() > initialIndexSize);

		_IRR_DEBUG_BREAK_IF(!driver->getMaterialRenderer(
								mb->getMaterial().getMaterialType()
							)->isTransparent());
		driver->setMaterial(mb->getMaterial());
		driver->drawMeshBuffer(mb);

		// put back original
		mb->swapIndexBuffer(indexBuffer, indexSize, owns);

#ifdef _DEBUG
		++getRenderedBatchCount();
#endif

		// clear the batch visible segments
		info.Count = 0;
	}
	// Don't care for transparent batches:
	//info.LastCount = info.Count;
}

void
IBatchSceneNode::setBatchAutomaticCulling(E_CULLING_TYPE ct)
{
	BatchCullingType = ct;
}

void
IBatchSceneNode::setSegmentAutomaticCulling(E_CULLING_TYPE ct)
{
	SegmentCullingType = ct;
}

void
IBatchSceneNode::setVisibleIndexCache(bool useCache)
{
	bool oldUseCache = hasVisibleIndexCache();
	HasVisibleIndexCache = useCache;
	if (!useCache && oldUseCache)
	{
		delete[] VisibleIndices;
		VisibleIndices = 0;
	}
	else if (useCache
			 && !oldUseCache
			 && BatchMesh && BatchMesh->getBatchCount() > 0)
	{
		// If we were not using an index cache and we want and the node was
		// compiled, then we have to compute the cache
		invalidateVisibleIndexCache();
	}
}

void
IBatchSceneNode::invalidateVisibleIndexCache(u32 batch)
{
	_IRR_DEBUG_BREAK_IF(batch != u32(-1)
						&& batch >= BatchMesh->getBatchCount());

	// nothing to do if we have no cache
	if (!hasVisibleIndexCache())
	{
		return;
	}

	if (batch != u32(-1))
	{
		// This is the case where we force the cache to be dirty
		getSegmentVisibilityInfo(batch).Flags |= 1;
		return;
	}

	// Visible index cache is good only for solid batches

	if (SolidBatchCount == 0)
	{
		return;
	}

	// With no batch id provided, we rebuild the cache entirely

	u32 batchCount = SolidBatchCount;
	u32 totalSize = 0;
	for (u32 b = 0; b < batchCount; ++b)
	{
		totalSize += BatchMesh->getBuffer(b)->getIndexCount();
	}

	// The base offset provides space at the start of the index cache buffer to
	// put offset information for each batch
	u32 baseOffset
		= (batchCount * sizeof(SVisibleIndexCacheInfo)) / sizeof(u16);

	if (VisibleIndices)
	{
		delete[] VisibleIndices;
	}
	VisibleIndices = irrnew u16 [baseOffset + totalSize];
	_IRR_DEBUG_BREAK_IF(VisibleIndices == NULL);

	// Setup offsets for each batch
	totalSize = baseOffset;
	for (u32 b = 0; b < batchCount; ++b)
	{
		new (reinterpret_cast<SVisibleIndexCacheInfo*>(VisibleIndices) + b)
			SVisibleIndexCacheInfo(totalSize);
		totalSize += BatchMesh->getBuffer(b)->getIndexCount();

		// also, ensure the cache is marked as dirty
		getSegmentVisibilityInfo(b).Flags |= 1;
	}
}

u32
IBatchSceneNode::clearVisibleSegments()
{
	u32 mbCnt = BatchMesh->getMeshBufferCount();
	for (u32 i = 0; i < mbCnt; ++i)
	{
		getSegmentVisibilityInfo(i).Count = 0;
	}
	return mbCnt;
}

void
IBatchSceneNode::registerSolidBatches()
{
	for (u32 i = 0; i < SolidBatchCount; ++i)
	{
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(i);
		if (info.Count > 0)
		{
			IMeshBuffer* mb = BatchMesh->getMeshBuffer(i);
			SceneManager->registerNodeForRendering(
				this,
				&BatchMesh->getMeshBuffer(i)->getMaterial(),
				reinterpret_cast<void*>(i + 1),
				ESNRP_SOLID
			);
		}
		info.FrameCount = os::Timer::getTickCount();
		info.Flags |= info.LastCount != info.Count;
	}
}

void
IBatchSceneNode::registerTransparentBatches()
{
	for (u32 i = SolidBatchCount, cnt = BatchMesh->getBatchCount(); i < cnt; ++i)
	{
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(i);
		IMeshBuffer* mb = BatchMesh->getMeshBuffer(i);
		for (u32
				 *si = getVisibleSegments(i),
				 *siEnd = si + info.Count;
			 si != siEnd;
			 ++si)
		{
			core::vector3df pos = BatchMesh->getSegmentCenter(i, *si);
			SceneManager->registerNodeForRendering(
				this,
				&mb->getMaterial(),
				reinterpret_cast<void*>((*si + 1) << 16 | i),
				ESNRP_TRANSPARENT,
				&pos,
				BatchMesh->getRenderLayer(i, *si)
			);
		}
		info.FrameCount = os::Timer::getTickCount();
		// Reset the back back to 0 for later accumulation during rendering
		info.Count = 0;
		// flags are not used by transparent batches
		//info.Flags |= info.LastCount != info.Count;
	}
}

u32
IBatchSceneNode::updateInfo(u32 start, u32 end)
{
	// makes sense only for transparent batches
	_IRR_DEBUG_BREAK_IF(SolidBatchCount > 0
						&& (start >= SolidBatchCount || end > SolidBatchCount));

	u32 totalVisible = 0;
	for (u32 i = start; i < end; ++i)
	{
		SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(i);
		totalVisible += info.Count;
		if (info.Count > 0)
		{	
			info.FrameCount = os::Timer::getTickCount();
			info.Flags |= info.LastCount != info.Count;
		}
	}
	return totalVisible;
}

} // end namespace scene
} // end namespace irr
