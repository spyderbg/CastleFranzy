//-*-c++-*-
#ifndef __I_SCENE_BATCH_SCENE_NODE_H_INLUDED__
#define __I_SCENE_BATCH_SCENE_NODE_H_INLUDED__

#include "IrrCompileConfig.h"

#include "IMeshSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "IMeshBuffer.h"
#include "IBatchMesh.h"

namespace irr
{
namespace scene
{

#ifdef _DEBUG
u32& getVisibleSegmentCount();
u32& getVisibleSolidSegmentCount();
u32& getTotalSegmentCount();
u32& getRenderedBatchCount();
u32& getRenderedDirtyBatchCount();
#endif

class IBatchSceneNode : public IMeshSceneNode
{
public:

	//!
	virtual ~IBatchSceneNode();

	// ISceneNode interface ----------------------------------------------------

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
	virtual const core::aabbox3d<f32>& getTransformedBoundingBox() const;
#else
	virtual const core::aabbox3d<f32> getTransformedBoundingBox() const;
#endif

	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual void render(void* renderData = 0);
	virtual void serializeAttributes(io::IAttributes* out,
									 io::SAttributeReadWriteOptions* options = 0) const;
	virtual void deserializeAttributes(io::IAttributes* in,
									   io::SAttributeReadWriteOptions* options = 0);
	u32 getMaterialCount() const;
	video::SMaterial& getMaterial(u32 num);

	// IMeshScene interface ----------------------------------------------------

	virtual IMesh* getMesh();
	virtual bool isReadOnlyMaterials() const;

	// unsupported
	virtual void setMesh(IMesh* mesh);
	virtual void setReadOnlyMaterials(bool readonly);

	// this class --------------------------------------------------------------

	//!
	IBatchMesh* getBatchMesh() const
	{
		return BatchMesh;
	}

	//! Compiles the scene under given root (using drawAll(root)), or the whole
	//! scene.
	/** \param root Root of scene to compile. If null, compiles the whole
		enclosing scene manager.
		\param skipPostCompile If false, postCompile() will not be called
		internally and will have to be manually be called.
		\param maxVertexBufferSize Allocation size of working buffer for vertex
		data.
		\param maxIndexBufferSize Allocation size of working buffer for index
		data.
		\note The scene manager should be updated and configured for culling
		appropriately. */
	virtual void compile(
		ISceneNode* root = 0,
		bool skipPostCompile = false,
		u32 maxVertexBufferSize = _IRR_DEFAULT_COMPILED_BATCH_MAX_VERTEX_BUFFER_SIZE_,
		u32 maxIndexBufferSize = _IRR_DEFAULT_COMPILED_BATCH_MAX_INDEX_BUFFER_SIZE_
	);

	virtual void compile(
		const core::array<ISceneNode*>& nodes,
		bool skipPostCompile = false,
		u32 maxVertexBufferSize = _IRR_DEFAULT_COMPILED_BATCH_MAX_VERTEX_BUFFER_SIZE_,
		u32 maxIndexBufferSize = _IRR_DEFAULT_COMPILED_BATCH_MAX_INDEX_BUFFER_SIZE_
	);

	//! Prepares internal buffer data based on the last call to compile().
	/** This method is usually automatically called by compile(), but it can
		also be manually called for, e.g. batch scene nodes that share a batch
		mesh, where only one node must compile the mesh. When sharing, explicit
		calls to postCompile() must of course be done after calling compile() on
		one of the sharing nodes, and postCompile() needs not be called on the
		node on which compile() was called, unless the skip post compile
		parameter was set to false. */
	virtual void postCompile();

	//!
	E_CULLING_TYPE getBatchAutomaticCulling() const
	{
		return BatchCullingType;
	}

	//!
	void setBatchAutomaticCulling(E_CULLING_TYPE ct);

	//!
	E_CULLING_TYPE getSegmentAutomaticCulling() const;

	//!
	void setSegmentAutomaticCulling(E_CULLING_TYPE ct);

	//!
	bool hasVisibleIndexCache() const;

	//!
	void setVisibleIndexCache(bool useCache);

	//! Forces an index cache to be dirty if batch id is provided, otherwise, it
	//! does a full initialization of the index cache (if one is in use).
	void invalidateVisibleIndexCache(u32 batch = u32(-1));

protected:

	//!
	virtual void renderBatchBBoxes(video::IVideoDriver* driver, u32 batch) = 0;

	struct SSegmentVisibilityInfo
	{
		u32 Count;
		u32 LastCount;
		u32 FrameCount;
		u32 Flags;
		u32 Offset;

		SSegmentVisibilityInfo()
			: Count(0)
			, LastCount(0)
			, FrameCount(0)
			, Flags(1)
			, Offset(0)
		{
		}
	};

	struct SVisibleIndexCacheInfo
	{
		u32 Size;
		u32 Offset;

		explicit SVisibleIndexCacheInfo(u32 offset = 0)
			: Size(0)
			, Offset(offset)
		{
		}
	};

	//!
	explicit IBatchSceneNode(s32 id = -1,
							 IBatchMesh* batchMesh = NULL,
							 bool registerSolidBatchesOnce = true);

	//!
	virtual IBatchMesh* allocateDefaultBatchMesh() const = 0;

	//!
	const SSegmentVisibilityInfo& getSegmentVisibilityInfo(u32 batch) const;

	//!
	SSegmentVisibilityInfo& getSegmentVisibilityInfo(u32 batch);

	//!
	u32* getVisibleSegments(u32 batch) const;

	//!
	void addVisibleSegmentRaw(u32 batch, u32 segment);

	//!
	u32 clearVisibleSegments();

	//! Registers solid batches.
	void registerSolidBatches();

	//! Registers transparent batches.
	void registerTransparentBatches();

	//!
	u32 updateInfo(u32 start, u32 end);

	//!
	void updateIndices(u32 batch,
					   CBatchBuffer* mb,
					   const u16* indexBuffer);

	//!
	void renderSolidBatch(video::IVideoDriver* driver,
						  u32 batch);

	//!
	void renderTransparentSegment(video::IVideoDriver* driver,
								  u32 batch,
								  u32 segment);

	//!
	void flushTransparentBatch(video::IVideoDriver* driver);

	//!
	u16* getVisibleIndexCache(u32 batch);

	//!
	SVisibleIndexCacheInfo& getVisibleIndexCacheInfo(u32 batch);

	//!
	IBatchMesh* BatchMesh;

	//!
	E_CULLING_TYPE BatchCullingType;

	//!
	E_CULLING_TYPE SegmentCullingType;

	//!
	u32 MaxIndexCountSolid;

	//!
	u32 MaxIndexCountTransparent;

	//!
	u32 SolidBatchCount;

	//!
	bool RegisterSolidBatchesOnce;

private:

	//! Flag that tells whether or not to cache visible indices for each
	//! batches.
	bool HasVisibleIndexCache;

	//! Used to cache visible indices for each batches.
	u16* VisibleIndices;

	//!
	u32* VisibleSegments;

	//!
	u32 CurrentTransparentBatch;
};

inline
E_CULLING_TYPE
IBatchSceneNode::getSegmentAutomaticCulling() const
{
	return SegmentCullingType;
}

inline
bool
IBatchSceneNode::hasVisibleIndexCache() const
{
	return HasVisibleIndexCache;
}

inline
const IBatchSceneNode::SSegmentVisibilityInfo&
IBatchSceneNode::getSegmentVisibilityInfo(u32 batch) const
{
	return reinterpret_cast<const SSegmentVisibilityInfo*>(VisibleSegments)[batch];
}

inline
IBatchSceneNode::SSegmentVisibilityInfo&
IBatchSceneNode::getSegmentVisibilityInfo(u32 batch)
{
	return reinterpret_cast<SSegmentVisibilityInfo*>(VisibleSegments)[batch];
}

inline
u32*
IBatchSceneNode::getVisibleSegments(u32 batch) const
{
	return VisibleSegments + getSegmentVisibilityInfo(batch).Offset;
}

inline
void
IBatchSceneNode::addVisibleSegmentRaw(u32 batch, u32 segment)
{
	// add it to the list
	getVisibleSegments(batch)[getSegmentVisibilityInfo(batch).Count++] = segment;
#ifdef _DEBUG
	++getVisibleSegmentCount();
	if (RegisterSolidBatchesOnce && batch < SolidBatchCount)
	{
		++getVisibleSolidSegmentCount();
	}
#endif
}

inline
u16*
IBatchSceneNode::getVisibleIndexCache(u32 batch)
{
	_IRR_DEBUG_BREAK_IF(batch >= SolidBatchCount);
	return VisibleIndices + getVisibleIndexCacheInfo(batch).Offset;
}

inline
IBatchSceneNode::SVisibleIndexCacheInfo&
IBatchSceneNode::getVisibleIndexCacheInfo(u32 batch)
{
	_IRR_DEBUG_BREAK_IF(batch >= SolidBatchCount);
	return reinterpret_cast<SVisibleIndexCacheInfo*>(VisibleIndices)[batch];
}

inline
void
IBatchSceneNode::renderTransparentSegment(video::IVideoDriver* driver,
										  u32 batch,
										  u32 segment)
{
	_IRR_DEBUG_BREAK_IF(batch < SolidBatchCount);
	SSegmentVisibilityInfo& info = getSegmentVisibilityInfo(batch);

	// We need to flush (render) the transparent batch that was being rendered
	// if we are starting to render a new batch
	if (info.Count == 0) 
	{
		flushTransparentBatch(driver);
		CurrentTransparentBatch = batch;
	}

	addVisibleSegmentRaw(batch, segment);

	// if the next node to render is not this node, we need to flush
	if (SceneManager->getNextRenderedNode() != this)
	{
		flushTransparentBatch(driver);
	}
}

} // end namespace scene
} // end namespace irr

#endif
