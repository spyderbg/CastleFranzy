//-*-c++-*-
#ifndef __C_SCENE_BATCH_GRID_SCENE_NODE_IMPL_H_INCLUDED__
#define __C_SCENE_BATCH_GRID_SCENE_NODE_IMPL_H_INCLUDED__

#include "CBatchGridSceneNode.h"

#include "irrProcessBufferHeap.h"
#include "SIntersector.h"

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

template <typename SegmentData>
inline
CBatchGridSceneNode<SegmentData>::CBatchGridSceneNode(
	const core::dimension2d<u32>& dims,
	u32 upAxis,
	s32 id,
	IBatchMesh* batchMesh
)
	: CBatchSceneNode<SegmentData>(id, batchMesh)
	, GridDimensions(dims)
	, UpAxis(3) // force setting in setUpAxis below
	, Grid(NULL)
{
#ifdef _DEBUG
	this->setDebugName("CBatchGridSceneNode");
#endif
	setUpAxis(upAxis);
}

template <typename SegmentData>
inline
CBatchGridSceneNode<SegmentData>::~CBatchGridSceneNode()
{
	if (Grid)
	{
		delete[] Grid;
	}
}

// ISceneNode interface --------------------------------------------------------

template <typename SegmentData>
inline
ESCENE_NODE_TYPE
CBatchGridSceneNode<SegmentData>::getType() const
{
	return ESNT_BATCH_GRID_SCENE_NODE;
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::OnRegisterSceneNode()
{
	if (this->isVisible() && Grid != NULL)
	{
		// clear current visible segments
		u32 mbCnt = this->clearVisibleSegments();

		if (this->SceneManager->isCullingEnabled())
		{
			if (!this->SceneManager->isCulled(this))
			{
				this->SceneManager->disableCulling();

				// Process grid element visibility

				ICameraSceneNode* cam = this->SceneManager->getActiveCamera();
				const SViewFrustum* frustum = cam->getViewFrustum();
				core::position2d<u32> frustumLo, frustumHi;
				getRange(frustum->getBoundingBox(),
						 frustumLo,
						 frustumHi);
				switch (this->getSegmentAutomaticCulling())
				{
					case EAC_OFF :
					case EAC_BOX :
					case EAC_FRUSTUM_SPHERE :
					{
						for (u32 x = frustumLo.X; x < frustumHi.X; ++x)
						{
							for (u32 y = frustumLo.Y; y < frustumHi.Y; ++y)
							{
								addVisibleCell(getCell(x, y));
							}
						}
					}
					break;

					case EAC_FRUSTUM_BOX :
					{
						addVisibleCells<SFrustumBoxIntersector>(*frustum,
																frustumLo,
																frustumHi);
					}
					break;

					case EAC_FRUSTUM_BOX_3 :
					{
						addVisibleCells<SFrustumBoxIntersector3>(*frustum,
																 frustumLo,
																 frustumHi);
					}
					break;
				}

				// Register render jobs
				if (this->RegisterSolidBatchesOnce)
				{
					// We need to update info of solid batches
					u32 totalSegment = updateInfo(0, this->SolidBatchCount);
					if (totalSegment > 0)
					{
						this->SceneManager->registerNodeForRendering(
							this,
							NULL,
							NULL
						);
					}
					this->registerTransparentBatches();
				}
				else
				{
					this->registerSolidBatches();
					this->registerTransparentBatches();
				}

				this->SceneManager->enableCulling();
			}

			// Necessary?
			IBatchSceneNode::OnRegisterSceneNode();
		}
		else
		{
			CBatchSceneNode<SegmentData>::OnRegisterSceneNode();
		}
	}
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::serializeAttributes(
	io::IAttributes* out,
	io::SAttributeReadWriteOptions* options
) const
{
	if (!out)
	{
		return;
	}
	CBatchSceneNode<SegmentData>::serializeAttributes(out, options);

	out->addInt("UpAxis", getUpAxis());
	out->addInt("GridWidth", getDimensions().Width);
	out->addInt("GridHeight", getDimensions().Height);
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::deserializeAttributes(
	io::IAttributes* in,
	io::SAttributeReadWriteOptions* options
)
{
	if (!in)
	{
		return;
	}
	CBatchSceneNode<SegmentData>::deserializeAttributes(in, options);

	setUpAxis(in->getAttributeAsInt("UpAxis"));
	core::dimension2d<u32> dims(in->getAttributeAsInt("GridWidth"),
								in->getAttributeAsInt("GridHeight"));
	setDimensions(dims);
}

// CBatchSceneNode overrides ---------------------------------------------------

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::postCompile()
{
	CBatchSceneNode<SegmentData>::postCompile();

	const core::aabbox3df& batchBBox = this->getBoundingBox();
	Dimensions.Width = batchBBox.MaxEdge[Axis[0]] - batchBBox.MinEdge[Axis[0]];
	Dimensions.Height = batchBBox.MaxEdge[Axis[1]] - batchBBox.MinEdge[Axis[1]];
	Origin.X = batchBBox.MinEdge[Axis[0]];
	Origin.Y = batchBBox.MinEdge[Axis[1]];

	// build the grid
	build();
}

// this class ------------------------------------------------------------------

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::setDimensions(const core::dimension2d<u32>& dims)
{
	if (dims != GridDimensions)
	{
		GridDimensions = dims;
		if (Grid)
		{
			delete[] Grid;
			Grid = NULL;
			build();
		}
	}
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::setUpAxis(u32 upAxis)
{
	if (upAxis != UpAxis)
	{
		UpAxis = upAxis;
		switch (upAxis)
		{
			case 0 :
			{
				Axis[0] = 1;
				Axis[1] = 2;
			}
			break;

			case 1 :
			{
				Axis[0] = 0;
				Axis[1] = 2;
			}
			break;

			case 2:
			{
				Axis[0] = 0;
				Axis[1] = 1;
			}
			break;

			default:
			{
				_IRR_DEBUG_BREAK_IF(upAxis >= 3);
			}
		}
		build();
	}
}


template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::getRange(const core::aabbox3df& bbox,
										   core::position2d<u32>& lo,
										   core::position2d<u32>& hi) const
{
	core::position2df gp = mapToGrid(bbox.MinEdge[Axis[0]],
									 bbox.MinEdge[Axis[1]]);
	lo.X = core::clamp(core::floor32(gp.X),
					   0,
					   s32(GridDimensions.Width));
	lo.Y = core::clamp(core::floor32(gp.Y),
					   0,
					   s32(GridDimensions.Height));
	gp = mapToGrid(bbox.MaxEdge[Axis[0]], bbox.MaxEdge[Axis[1]]);
	hi.X = core::clamp(core::ceil32(gp.X),
					   0,
					   s32(GridDimensions.Width));
	hi.Y = core::clamp(core::ceil32(gp.Y),
					   0,
					   s32(GridDimensions.Height));
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::build()
{
	if (this->BatchMesh == NULL || this->BatchMesh->getMeshBufferCount() == 0)
	{
		return;
	}

	if (Grid == NULL)
	{
		Grid = irrnew Cell [GridDimensions.Width * GridDimensions.Height];
	}
	else
	{
		for (u32 i = 0, iEnd = GridDimensions.Width * GridDimensions.Height;
			 i < iEnd;
			 ++i)
		{
			Grid[i].clear();
		}
	}

	// clear grid

	const core::aabbox3df& batchBBox = this->getBoundingBox();
	core::dimension2df bboxDims(batchBBox.MaxEdge[Axis[0]] - batchBBox.MinEdge[Axis[0]],
								batchBBox.MaxEdge[Axis[1]] - batchBBox.MinEdge[Axis[1]]);

	u32 mbCnt = this->BatchMesh->getMeshBufferCount();
	core::position2d<u32> lo, hi;
	for (u32 i = 0; i < mbCnt; ++i)
	{
		for (u32 s = 0, sCnt = this->BatchMesh->getSegmentCount(i); s < sCnt; ++s)
		{
			Segment* seg = reinterpret_cast<Segment*>(
				this->BatchMesh->getSegmentData(i, s)
			);
			_IRR_DEBUG_BREAK_IF(seg->getBoundingBox() == NULL);
			getRange(*seg->getBoundingBox(), lo, hi);
			for (u32 x = lo.X; x < hi.X; ++x)
			{
				for (u32 y = lo.Y; y < hi.Y; ++y)
				{
					getCell(x, y).push_back(SegmentId(i, s));
				}
			}
		}
	}
}

template <typename SegmentData>
inline
void
CBatchGridSceneNode<SegmentData>::makeFrustumLocal(SViewFrustum& frustum) const
{
	f32 scx = Dimensions.Width / GridDimensions.Width;
	f32 scy = Dimensions.Height / GridDimensions.Height;
	for (u32 i = 0; i < 6; ++i)
	{
		frustum.planes[i].D += (frustum.planes[i].Normal[Axis[0]] * Origin.X
								+ frustum.planes[i].Normal[Axis[1]] * Origin.Y);
		frustum.planes[i].Normal[Axis[0]] *= scx;
		frustum.planes[i].Normal[Axis[1]] *= scy;
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
