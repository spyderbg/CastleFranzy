#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaMorphingMesh.h"

#include "CColladaMeshBufferProxy.h"
#include "CColladaMesh.h"

namespace irr
{
namespace collada
{

CColladaMorphingMesh::CColladaMorphingMesh(
	const collada::CColladaDatabase& collada, 
	collada::SController& controller,
	bool useProcessBuffer,
	video::IVideoDriver* driver
)
	: IColladaMesh(collada)
	, Morph(*controller.pMorph)
{
	setUID(controller.id);
	instanciateMesh();
	prepareMorphBuffers(useProcessBuffer, driver);
}

CColladaMorphingMesh::~CColladaMorphingMesh(void)
{
	for(int j = 0, sz = VertexBuffers.size(); j < sz; ++j)
	{
		bool deleted = VertexBuffers[j]->drop();
		_IRR_DEBUG_BREAK_IF(!deleted); // Use internal or temporary buffers
	}
}

scene::IColladaMesh::E_TYPE
CColladaMorphingMesh::getType() const
{
	return ET_MORPHING_MESH;
}

int
CColladaMorphingMesh::instanciateMesh()
{
	_IRR_DEBUG_BREAK_IF(Morph.source.ptr[0] != '#');

	const char* url = Morph.source.ptr + 1;

	Targets.reallocate(Morph.pTargets.size() + 1);

	scene::IColladaMesh* source = getCollada().constructGeometry(url);
	if(source == 0)
	{
		source = getCollada().constructController(url);
	}

	_IRR_DEBUG_BREAK_IF(source == 0);

	Targets.push_back(STarget(source, 1.0f));
	source->drop();

	for(int i = 0, sz = Morph.pTargets.size(); i < sz; ++i)
	{
		_IRR_DEBUG_BREAK_IF(Morph.pTargets[i] == 0);
		scene::IColladaMesh* targetSource = getCollada().constructGeometry(Morph.pTargets[i]);

		_IRR_DEBUG_BREAK_IF(targetSource == 0);
		Targets.push_back(STarget(targetSource, Morph.weights[i]));
		targetSource->drop();
	}

	int meshBufferCount = source->getMeshBufferCount();
	Materials.reallocate(meshBufferCount);
	Materials.set_used(meshBufferCount);
	
	return source != 0;
}

void
CColladaMorphingMesh::prepareMorphBuffers(bool useProcessBuffer,
										  video::IVideoDriver* driver)
{
	scene::IMeshBuffer* meshBuffer = getSource()->getMeshBuffer(0);

	_IRR_DEBUG_BREAK_IF(meshBuffer->getVertexType() != video::EVT_COMPONENT_ARRAYS);
	video::S3DVertexComponentArrays *vertices
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			meshBuffer->getVertices()
		);

	VertexBuffers.reallocate(getSource()->getMeshBufferCount());
	VertexBuffers.set_used(getSource()->getMeshBufferCount());
	for (u32 i = 0, cnt = getSource()->getMeshBufferCount(); i < cnt; i++)
	{
		scene::IMeshBuffer* srcMb = getSource()->getMeshBuffer(i);
		collada::CMeshBufferProxy* proxy = irrnew collada::CMeshBufferProxy();
		proxy->setReferences(
			srcMb,
			reinterpret_cast<video::S3DVertexComponentArrays*>(srcMb->getVertices()),
			&getSource()->getBoundingBox()
		);
		proxy->setIsDynamic(true);
		VertexBuffers[i] = proxy;

		if (!useProcessBuffer)
		{
			_IRR_DEBUG_BREAK_IF(driver == NULL);
			video::E_DRIVER_ALLOCATION_RESULT result
				= driver->getProcessBuffer(
					(proxy->getMaterial().getFlag(video::EMF_LIGHTING)
					 ? (video::EVA_POSITION | video::EVA_NORMAL)
					 : video::EVA_POSITION),
					video::EPBT_STATIC,
					proxy,
					true);
			_IRR_DEBUG_BREAK_IF(result & video::EDAR_FAILED);
			HasOutputBuffer = true;
		}
		else
		{
			video::S3DVertexComponentArrays* vertices
				= reinterpret_cast<video::S3DVertexComponentArrays*>(
					proxy->getVertices()
				);
			vertices->Position = vertices->Normal = NULL;
		}
	}
}

void
CColladaMorphingMesh::releaseProcessBuffer(video::IVideoDriver* driver,
										   u32 buffer)
{
	scene::IMeshBuffer* mb = getMeshBuffer(buffer);
	video::S3DVertexComponentArrays* components
		= reinterpret_cast<video::S3DVertexComponentArrays*>(
			mb->getVertices()
		);
	driver->releaseProcessBuffer(video::EPBT_DYNAMIC,
								 components->Position,
								 mb->getVertexIndexStart(),
								 components->PositionStride);
	components->Position = NULL;
	components->Normal = NULL;
	HasOutputBuffer = false;
}

template<class T>
void
addWeightedVertex(T* working,
				  u32 wStride,
				  const T* target,
				  u32 stride,
				  irr::f32 weight,
				  u32 count)
{
	if(weight != 0.0f)
	{
		if(weight != 1.0f)
		{
			for (u32 i = 0; i < count; ++i)
			{
				*working += *target * weight;
				working = core::stepPointer(working, wStride);
				target = core::stepPointer(target, stride);
			}
		}
		else
		{
			for (u32 i = 0; i < count; ++i)
			{
				*working += *target;
				working = core::stepPointer(working, wStride);
				target = core::stepPointer(target, stride);
			}
		}
	}
}

template<class T>
void
setWeightedVertex(T* working,
				  u32 wStride,
				  const T* target,
				  u32 stride,
				  irr::f32 weight,
				  u32 count)
{
	if(weight != 0.0f)
	{
		if(weight != 1.0f)
		{
			for (u32 i = 0; i < count; ++i)
			{
				*working = *target * weight;
				working = core::stepPointer(working, wStride);
				target = core::stepPointer(target, stride);
			};
		}
		else
		{
			if(stride != sizeof(T) || wStride != sizeof(T))
			{
				for(u32 i = 0; i < count; ++i)
				{
					*working = *target;
					working = core::stepPointer(working, wStride);
					target = core::stepPointer(target, stride);
				};
			}
			else
			{
				memcpy(working, target, count * sizeof(T));
			}
		}
	}
}

void
CColladaMorphingMesh::morph(u32 buffer)
{
	scene::IMeshBuffer* meshBuffer = getSource()->getMeshBuffer(buffer);

	_IRR_DEBUG_BREAK_IF(meshBuffer->getVertexType() != video::EVT_COMPONENT_ARRAYS);

	u32 start = meshBuffer->getVertexIndexStart();
	u32 end = meshBuffer->getVertexIndexEnd();
	
	const u32 targetCount = getTargetCount();

	setWeight(0, 1.0f);
	
	if (!isRelative())
	{
		for (u32 i = 1; i < targetCount; ++i)
		{
			getWeightRef(0) -= Targets[i].Weight;			
		}
	}

	// Initialize
	u32 startIndex = 0;
	for (; startIndex < targetCount; ++startIndex)
	{
		if (getWeight(startIndex) != 0)
		{
			break;
		}
	}
	
	video::S3DVertexComponentArrays* vertices 
		= reinterpret_cast<video::SS3DVertexComponentArrays*>(
			Targets[startIndex].Mesh->getMeshBuffer(buffer)->getVertices()
		);
	_IRR_DEBUG_BREAK_IF(getMesh(startIndex)->getMeshBuffer(buffer)->getVertexType()
						!= video::EVT_COMPONENT_ARRAYS);

	scene::IMeshBuffer* dstMeshBuffer = getMeshBuffer(buffer);
	video::S3DVertexComponentArrays* dstVertices 
		= reinterpret_cast<video::SS3DVertexComponentArrays*>(
			dstMeshBuffer->getVertices()
		);
	_IRR_DEBUG_BREAK_IF(dstMeshBuffer->getVertexIndexStart() != start
						|| dstMeshBuffer->getVertexIndexEnd() != end);
	core::vector3df* dstPosition
		= core::stepPointer(dstVertices->Position,
							dstVertices->PositionStride * start);
	core::vector3df* dstNormal = NULL;

	u32 vertexCount = end - start;
	_IRR_DEBUG_BREAK_IF(dstVertices->Position == 0);
	setWeightedVertex(dstPosition,
					  dstVertices->PositionStride,
					  core::stepPointer(vertices->Position,
										vertices->PositionStride * start),
					  vertices->PositionStride,
					  getWeight(startIndex),
					  vertexCount);

	if (dstVertices->Normal != NULL && vertices->Normal != NULL)
	{
		dstNormal = core::stepPointer(dstVertices->Normal,
									  dstVertices->NormalStride * start);
		setWeightedVertex(dstNormal,
						  dstVertices->NormalStride,
						  core::stepPointer(vertices->Normal,
											vertices->NormalStride * start),
						  vertices->NormalStride,
						  getWeight(startIndex),
						  vertexCount);
	}

	for (u32 i = startIndex + 1; i < targetCount; ++i)
	{
		if (getWeight(i))
		{
			vertices = reinterpret_cast<video::S3DVertexComponentArrays*>(
				Targets[i].Mesh->getMeshBuffer(buffer)->getVertices()
			);

			addWeightedVertex(dstPosition,
							  dstVertices->PositionStride,
							  core::stepPointer(vertices->Position,
												vertices->PositionStride * start),
							  vertices->PositionStride,
							  getWeight(i),
							  vertexCount);
			if (dstVertices->Normal != NULL && vertices->Normal != NULL)
			{
				addWeightedVertex(dstNormal,
								  dstVertices->NormalStride,
								  stepPointer(vertices->Normal,
											  vertices->NormalStride * start),
								  vertices->NormalStride,
								  getWeight(i),
								  vertexCount);
			}
		}
	}
#ifdef _IRR_USE_WII_DEVICE_
	DCFlushRangeNoSync(core::stepPointer(dstVertices->Position,
										 dstVertices->PositionStride * start),
					   dstVertices->PositionStride * (end - start));
	// Shouldn't be necessary, it is always interleaved now
// 	if (!(reinterpret_cast<u8*>(dstVertices->Normal)
// 		  - reinterpret_cast<u8*>(dstVertices->Position)
// 		  <= dstVertices->PositionStride - sizeof(core::vector3df)))
// 	{
// 		DCFlushRangeNoSync(core::stepPointer(dstVertices->Normal,
// 											 dstVertices->NormalStride * start),
// 						   dstVertices->NormalStride * (end - start));
// 	}
#endif
}

video::E_DRIVER_ALLOCATION_RESULT
CColladaMorphingMesh::onPrepareBufferForRendering(E_PREPARE_BUFFER_STEP step,
												  video::IVideoDriver* driver,
												  u32 buffer)
{
	_IRR_DEBUG_BREAK_IF(buffer >= getMeshBufferCount());

	// This was not done before; should we enable it?
// 	video::E_DRIVER_ALLOCATION_RESULT result
// 		= getSource()->onPrepareBufferForRendering(step, driver, buffer); 

	scene::IMeshBuffer* mb = VertexBuffers[buffer];
	video::E_DRIVER_ALLOCATION_RESULT result = driver->getProcessBuffer(
		(mb->getMaterial().getFlag(video::EMF_LIGHTING)
		 ? (video::EVA_POSITION | video::EVA_NORMAL)
		 : video::EVA_POSITION),
		(step == EPBS_REGISTRATION
		 ? video::EPBT_STATIC
		 : video::EPBT_DYNAMIC),
		mb
	);
	if (result & video::EDAR_SUCCESS)
	{
		HasOutputBuffer = true;
		morph(buffer);
	}
	return result;
}

u32
CColladaMorphingMesh::getMeshBufferCount() const
{
	return getSource()->getMeshBufferCount();
}

scene::IMeshBuffer*
CColladaMorphingMesh::getMeshBuffer(u32 nr) const
{
	return VertexBuffers[nr];
}

scene::IMeshBuffer*
CColladaMorphingMesh::getMeshBuffer(const video::SMaterial& material) const
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	return 0;
}

const core::aabbox3d<f32>&
CColladaMorphingMesh::getBoundingBox() const
{
	return getSource()->getBoundingBox();
}

void
CColladaMorphingMesh::setBoundingBox(const core::aabbox3df& box)
{
	_IRR_DEBUG_BREAK_IF("UNIMPLEMENTED");
	//getSource()->setBoundingBox(box);
}

void
CColladaMorphingMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	_IRR_DEBUG_BREAK_IF("UNIMPLEMENTED");
	//getSource()->setMaterialFlag(flag, newvalue);
}

core::stringc
CColladaMorphingMesh::getUserProperty() const
{
	return "";
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
