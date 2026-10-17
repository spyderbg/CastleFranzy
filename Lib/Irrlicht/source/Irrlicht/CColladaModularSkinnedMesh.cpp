#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaModularSkinnedMesh.h"
#include "coreutil.h"
#include "irros.h"
#include "CColladaMeshBufferRef.h"
#include "S3DVertexComponentArrays.h"
#include "IColladaRootSceneNode.h"

namespace irr
{
namespace scene
{

CColladaModularSkinnedMesh::SModularMeshBuffer::SModularMeshBuffer()
	: MeshBuffer(0), Data(0), DataSize(0)
{
}

CColladaModularSkinnedMesh::SModularMeshBuffer::~SModularMeshBuffer()
{
	if(Data)
		delete Data;
	if(MeshBuffer)
		MeshBuffer->drop();
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT 
CColladaModularSkinnedMesh::SModularMeshBuffer::realloc(u32 size)
{
	if (Data)
	{
		delete[] Data;
		Data = NULL;
		DataSize = 0;
	}
	if (size > 0)
	{
		Data = irrnew u8[size];
		if (Data == NULL)
		{
			return EMUR_OUT_OF_MEMORY;
		}
		DataSize = size;
	}
	return EMUR_SUCCESS;
}
CColladaModularSkinnedMesh::CColladaModularSkinnedMesh(
	const collada::CColladaDatabase& collada,
	collada::SInstanceModularSkin* controller,
	collada::IRootSceneNode* rootSceneNode,
	int maxBufferSize,
	bool useProcessBuffer,
	video::IVideoDriver* driver
)
	: IColladaSkinnedMesh(collada)
	, AllocPolicy(EAP_GROWING)
	, IsBoxDirty(true)
	, ModularSkin(*controller)
	, RootSceneNode(rootSceneNode)
{
	_IRR_DEBUG_BREAK_IF(controller == NULL);

	//setUID(controller->id);
	//u32 count = controller->pModularSkin->sources.size();
	u32 count = ModularSkin.categories.size() + ModularSkin.modules.size();

	if (maxBufferSize > 0)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
		/*Data = irrnew u8[maxBufferSize];
		DataSize = maxBufferSize;
		AllocPolicy = EAP_FIXED;*/
	}
	else if (maxBufferSize == 0)
	{
		AllocPolicy = EAP_TRIMMED;
	}

	E_MODULE_UPDATE_RESULT result = setModuleCount(count, false);
	_IRR_DEBUG_BREAK_IF(result != EMUR_SUCCESS);

	for (u32 i = 0; i < count; ++i)
	{
		_IRR_DEBUG_BREAK_IF(ModularSkin.categories[i].modules[0].type != collada::SInstance::ITController);
		// allow null initialization; this will actually set up an empty module
		// slot
		setCategoryModule(i, getModuleId(ModularSkin.categories[i].selection));
		/*if (ModularSkin.categories[i].modules[0].pController->url)
			//&& controller->pModularSkin->sources[i].url.ptr[0])
		{
			IColladaMesh* source = collada.constructController(
				ModularSkin.categories[i].modules[0].pController,
				rootSceneNode
			);
			if (source)
			{
				if (source->getType() == IColladaMesh::ET_SKINNED_MESH)
					//|| source->getType() == IColladaMesh::ET_MODULAR_SKINNED_MESH)
				{
					Modules[i] = static_cast<IColladaSkinnedMesh*>(source);
				}
				else // wrong type, drop it
				{
					_IRR_DEBUG_BREAK_IF("Wrong source type for SModularSkin");
					source->drop();
				}
			}
		}*/
	}
	computeBoundingBox();
}

/*CColladaModularSkinnedMesh::CColladaModularSkinnedMesh(
	const collada::CColladaDatabase& collada,
	int moduleCount,
	IColladaSkinnedMesh** modules,
	int maxBufferSize,
	bool useProcessBuffer
)
	: IColladaSkinnedMesh(collada)
	, AllocPolicy(EAP_GROWING)
	, Data(NULL)
	, DataSize(0)
	, IsBoxDirty(true)
{
	if (maxBufferSize > 0)
	{
		Data = irrnew u8[maxBufferSize];
		DataSize = maxBufferSize;
		AllocPolicy = EAP_FIXED;
	}
	else if (maxBufferSize == 0)
	{
		AllocPolicy = EAP_TRIMMED;
	}

	E_MODULE_UPDATE_RESULT result = setModules(modules, moduleCount);
	_IRR_DEBUG_BREAK_IF(result != EMUR_SUCCESS);

	computeBoundingBox();
}*/

CColladaModularSkinnedMesh::~CColladaModularSkinnedMesh()
{
	for (u32 i = 0, sz = Modules.size(); i < sz; ++i)
	{
		if (Modules[i])
		{
			Modules[i]->drop();
		}
	}
}

int 
CColladaModularSkinnedMesh::getCategoryId(const char *name) const
{
	for(int i = 0; i < ModularSkin.categories.size(); i++)
	{
		if(ModularSkin.categories[i].name == name)
			return i;
	}
	return -1;
}

const char * 
CColladaModularSkinnedMesh::getCategoryName(int categoryId) const
{
	if(categoryId >= ModularSkin.categories.size())
		return 0;
	return ModularSkin.categories[categoryId].name;
}

int 
CColladaModularSkinnedMesh::getModuleId(const char *name) const
{
	for (int i = 0; i < ModularSkin.categories.size(); ++i)
	{
		for (int j = 0; j < ModularSkin.categories[i].modules.size(); ++j)
		{
			if (ModularSkin.categories[i].modules[j].pController->url == name)
			{
				return j;
			}
		}
	}
	return -1;
}

const char*
CColladaModularSkinnedMesh::getModuleName(int categoryId, int moduleId) const
{
	if (categoryId >= ModularSkin.categories.size()
		|| moduleId >= ModularSkin.categories[categoryId].modules.size())
	{
		return 0;
	}
	return ModularSkin.categories[categoryId].modules[moduleId].pController->url;
}

void 
CColladaModularSkinnedMesh::setCategoryModule(int categoryId, int moduleId)
{
	_IRR_DEBUG_BREAK_IF(categoryId < 0 || (u32)categoryId >= Modules.size());
	_IRR_DEBUG_BREAK_IF(moduleId < -1 || moduleId >= ModularSkin.categories[categoryId].modules.size());

	if (CurrentModulesId[categoryId] == moduleId)
	{
		return;
	}

	if (Modules[categoryId])
	{
		Modules[categoryId]->drop();
		Modules[categoryId] = 0;
		CurrentModulesId[categoryId] = -1;
	}

	if (moduleId != -1)
	{
		IColladaMesh* source = getCollada().constructController(
			ModularSkin.categories[categoryId].modules[moduleId].pController,
			RootSceneNode
		);
		
		if (source)
		{
			_IRR_DEBUG_BREAK_IF(source->getType() != IColladaMesh::ET_SKINNED_MESH);
			Modules[categoryId] = static_cast<IColladaSkinnedMesh*>(source);
			CurrentModulesId[categoryId] = moduleId;
		}
	}

	updateBuffer(!hasOutputBuffer());
}

void 
CColladaModularSkinnedMesh::setCategoryModule(const char* categoryName, const char* moduleName)
{
	setCategoryModule(getCategoryId(categoryName), getModuleId(moduleName));
}

int 
CColladaModularSkinnedMesh::getCategoryCount() const
{
	return ModularSkin.categories.size();
}

int 
CColladaModularSkinnedMesh::getCategoryModuleCount(int categoryId) const
{
	_IRR_DEBUG_BREAK_IF(categoryId < 0 || (u32)categoryId >= Modules.size());
	return ModularSkin.categories[categoryId].modules.size();
}

int 
CColladaModularSkinnedMesh::getCurrentModuleId(int categoryId) const
{
	return CurrentModulesId[categoryId];
}

const char*
CColladaModularSkinnedMesh::getCurrentModuleName(int categoryId) const
{
	int moduleId = getCurrentModuleId(categoryId);
	if (moduleId == -1)
	{
		return 0;
	}
	else
	{
		return ModularSkin.categories[categoryId].modules[moduleId].pController->url;
	}
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT
CColladaModularSkinnedMesh::realloc(u32 size)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return EMUR_SUCCESS;
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT
CColladaModularSkinnedMesh::updateBuffer(bool useProcessBuffer)
{
	const u32 modCount = Modules.size();
	E_MODULE_UPDATE_RESULT result = EMUR_SUCCESS;
	// Count total number of materials
	ModularMeshBuffers.clear();
	Materials.clear();
	for (u32 i = 0; i < modCount; ++i)
	{
		IColladaSkinnedMesh* mod = Modules[i];
		if (mod)
		{
			for(u32 j = 0; j < 1/*mod->getMeshBufferCount()*/; j++)
			{
				const collada::SMaterial &colladaMaterial = *mod->getMaterial(j);
				collada::CMaterial* mat2 = RootSceneNode->hasLibraryMaterial(colladaMaterial.id);
				if(!mat2)
				{
					mat2 = RootSceneNode->addLibraryMaterial(colladaMaterial.id, getCollada());
				}
				
				video::SMaterial& material = mat2->get();

				bool found = false;
				for(u32 k = 0; k < ModularMeshBuffers.size(); k++)
				{
					if(ModularMeshBuffers[k].Material == material)
					{
						found = true;
						ModularMeshBuffers[k].Modules.push_back(i);
						break;
					}
				}

				if(!found)
				{
					SModularMeshBuffer modularMeshBuffer;
					modularMeshBuffer.Modules.push_back(i);
					modularMeshBuffer.Material = material;
					ModularMeshBuffers.push_back(modularMeshBuffer);
					Materials.push_back(&colladaMaterial);
				}
			}
		}
	}
	// Count total number of indices and vertices, and what vertex attributes
	// are required
	Materials.reallocate(ModularMeshBuffers.size());
	Materials.set_used(ModularMeshBuffers.size());

	for (u32 materialIt = 0; materialIt < ModularMeshBuffers.size(); ++materialIt)
	{
		SModularMeshBuffer &modularMeshBuffer = ModularMeshBuffers[materialIt];
		video::SMaterial* material = NULL;
		const collada::SMaterial* colladaMaterial = NULL;
		u32 totalIndexCount = 0;
		u32 totalVertexCount = 0;
		u32 vertexSize = useProcessBuffer ? 0 : sizeof(core::vector3df);
		video::S3DVertexComponentArrays outputBuffer;
			
		for (u32 i = 0; i < modularMeshBuffer.Modules.size(); ++i)
		{
			IColladaSkinnedMesh* mod = Modules[modularMeshBuffer.Modules[i]];
			if (mod)
			{
				if (material == NULL)
				{
					material = &mod->getMeshBuffer(0)->getMaterial();
					colladaMaterial = mod->getMaterial(0);
				}

				_IRR_DEBUG_BREAK_IF(mod->getMeshBuffer(0)->getVertexType() != video::EVT_COMPONENT_ARRAYS);
				video::S3DVertexComponentArrays* modVertices
					= reinterpret_cast<video::S3DVertexComponentArrays*>(
						mod->getMeshBuffer(0)->getVertices()
					);
				// Below is like this in anticipation of multiple material support
				if (!useProcessBuffer
					&& modVertices->Normal
					&& outputBuffer.Normal == NULL)
				{
					_IRR_DEBUG_BREAK_IF(modVertices->NormalType
										!= video::S3DVertexComponentArrays::ECT_FLOAT);
					outputBuffer.Normal = modVertices->Normal;
					vertexSize += sizeof(core::vector3df);
				}
				if (modVertices->TexCoord[0].Coord && outputBuffer.TexCoord[0].Coord == NULL)
				{
					_IRR_DEBUG_BREAK_IF(modVertices->TexCoord[0].Type
										!= video::S3DVertexComponentArrays::ECT_FLOAT
										&& "Type other than float not implemented");
					outputBuffer.TexCoord[0].Coord = modVertices->TexCoord[0].Coord;
					vertexSize += sizeof(core::vector2df);
				}
				if (modVertices->TexCoord[1].Coord && outputBuffer.TexCoord[1].Coord == NULL)
				{
					_IRR_DEBUG_BREAK_IF(modVertices->TexCoord[1].Type
										!= video::S3DVertexComponentArrays::ECT_FLOAT
										&& "Type other than float not implemented");
					outputBuffer.TexCoord[1].Coord = modVertices->TexCoord[1].Coord;
					vertexSize += sizeof(core::vector2df);
				}
				if (modVertices->Color0 && outputBuffer.Color0 == NULL)
				{
					_IRR_DEBUG_BREAK_IF(modVertices->Color0Type
										!= video::S3DVertexComponentArrays::ECT_UNSIGNED_BYTE
										&& "Type other than ubyte not implemented");
					outputBuffer.Color0 = modVertices->Color0;
					vertexSize += sizeof(video::SColor);
				}
				if (modVertices->Color1 && outputBuffer.Color1 == NULL)
				{
					_IRR_DEBUG_BREAK_IF(modVertices->Color1Type
										!= video::S3DVertexComponentArrays::ECT_UNSIGNED_BYTE
										&& "Type other than ubyte not implemented");
					outputBuffer.Color1 = modVertices->Color1;
					vertexSize += sizeof(video::SColor);
				}
				// ...

					u32 meshBufferCount = 1; //mod->getMeshBufferCount();
				for (u32 j = 0; j < meshBufferCount; ++j)
				{
					totalIndexCount	+= mod->getMeshBuffer(j)->getIndexCount();
				}
				totalVertexCount += mod->getVertexCount();
			}
		}

		// TODO: switch indices to u32 if too large, but 16 bits allow for at least
		// 21845 triangles (assuming a dense use of the vertex array with no vertex
		// sharing), which should be more than enough for the near future for
		// skinned meshes
		_IRR_DEBUG_BREAK_IF(totalIndexCount > u16(-1));

		// sanity check
		_IRR_DEBUG_BREAK_IF(totalIndexCount > 0 && totalVertexCount == 0);

		// rounding to pointer size should do the trick to round up to the natural
		// alignement of the current architecture
		u32 indexDataOffset = core::roundup<void*>(totalVertexCount * vertexSize);
		u32 requiredSize = indexDataOffset + totalIndexCount * sizeof(u16);

			
		switch (AllocPolicy)
		{
			case EAP_TRIMMED :
			{
					result = modularMeshBuffer.realloc(requiredSize);
			}
			break;

			case EAP_GROWING :
			{
					if (requiredSize > modularMeshBuffer.DataSize)
				{
						result = modularMeshBuffer.realloc(requiredSize);
				}
			}
			break;

			case EAP_FIXED :
			{
					if (requiredSize > modularMeshBuffer.DataSize)
				{
					c8 msg[128];
					sprintf(msg,
							"failed to allocate buffer of size %u (max set to %u)",
							requiredSize,
								modularMeshBuffer.DataSize);
					os::Printer::log(msg,
									 "CColladaModularSkinnedMesh",
									 ELL_ERROR);
					result = EMUR_MAX_BUFFER_SIZE_EXCEEDED;
				}
			}
			break;
		}

		if (result != EMUR_SUCCESS)
		{
			// TODO: add "fail gracefully" code here
			return result;
		}

		// Setup position/normal buffers
		u32 attribOffset = 0;
		if (!useProcessBuffer)
		{
			outputBuffer.Position = reinterpret_cast<core::vector3df*>(modularMeshBuffer.Data);
			if (outputBuffer.Normal)
			{
				outputBuffer.Normal = outputBuffer.Position + 1;
				outputBuffer.PositionStride
					= outputBuffer.NormalStride
					= vertexSize;
				attribOffset = 2 * sizeof(core::vector3df);
			}
			else
			{
				attribOffset = outputBuffer.PositionStride = sizeof(core::vector3df);
			}
		}
		else
		{
			// shouldn't be necessary, but this should avoid future unwanted bugs
			outputBuffer.Position = NULL;
			outputBuffer.Normal = NULL;
		}

		// Setup the rest
		u8* output = modularMeshBuffer.Data + attribOffset;
		if (outputBuffer.TexCoord[0].Coord)
		{
			outputBuffer.TexCoord[0].Coord = reinterpret_cast<core::vector2df*>(output);
			outputBuffer.TexCoord[0].Stride = vertexSize;
			output += sizeof(core::vector2df);
		}
		if (outputBuffer.TexCoord[1].Coord)
		{
			outputBuffer.TexCoord[1].Coord = reinterpret_cast<core::vector2df*>(output);
			outputBuffer.TexCoord[1].Stride = vertexSize;
			output += sizeof(core::vector2df);
		}
		if (outputBuffer.Color0)
		{
			outputBuffer.Color0 = reinterpret_cast<video::SColor*>(output);
			outputBuffer.Color0Stride = vertexSize;
			output += sizeof(video::SColor);
		}
		if (outputBuffer.Color1)
		{
			outputBuffer.Color1 = reinterpret_cast<video::SColor*>(output);
			outputBuffer.Color1Stride = vertexSize;
			output += sizeof(video::SColor);
		}
		// ...

		// TODO: to support multiple material, we have to sort input mesh buffers
		// and generate a list of indices per material

		video::S3DVertexComponentArrays outputAttribs = outputBuffer;
		u16* outputIndices = reinterpret_cast<u16*>(modularMeshBuffer.Data + indexDataOffset);
		u32 indexOffset = 0;
		for (u32 i = 0; i < modularMeshBuffer.Modules.size(); ++i)
		{
			IColladaSkinnedMesh* mod = Modules[modularMeshBuffer.Modules[i]];
			if (mod)
			{
				video::S3DVertexComponentArrays* modVertices
					= reinterpret_cast<video::S3DVertexComponentArrays*>(
						mod->getMeshBuffer(0)->getVertices()
					);
			
				// Below is like this in anticipation of multiple material support
				u32 vertexCount = mod->getVertexCount();
				u32 vertexByteCount = vertexCount * vertexSize;
	#define COPY_ATTRIB(attrib, stride)										\
				if (outputBuffer.attrib)									\
				{															\
					if (modVertices->attrib)								\
					{														\
						outputAttribs.attrib								\
							= core::copyArray(outputAttribs.attrib,			\
											  vertexSize,					\
											  modVertices->attrib,			\
											  modVertices->stride,			\
											  vertexCount);					\
					}														\
					else													\
					{														\
						outputAttribs.attrib								\
							= core::stepPointer(outputAttribs.attrib,		\
												vertexByteCount);			\
					}														\
				}

				COPY_ATTRIB(TexCoord[0].Coord, TexCoord[0].Stride);
				COPY_ATTRIB(TexCoord[1].Coord, TexCoord[1].Stride);
				COPY_ATTRIB(Color0, Color0Stride);
				COPY_ATTRIB(Color1, Color1Stride);
				// ...
	#undef COPY_ATTRIB

				// setup indices
				u32 meshBufferCount = 1;//mod->getMeshBufferCount();
				for (u32 j = 0; j < meshBufferCount; ++j)
				{
					IMeshBuffer* mb = mod->getMeshBuffer(j); 
					_IRR_DEBUG_BREAK_IF(mb == NULL);
					_IRR_DEBUG_BREAK_IF(mod->getMeshBuffer(j)->getIndexType()
										!= video::EIT_16BIT);
					u16* indices = mb->getIndices();
					for (u16* indicesEnd = indices + mb->getIndexCount();
						 indices != indicesEnd;
						 ++indices, ++outputIndices)
					{
						*outputIndices = *indices + indexOffset;
					}
				}
				indexOffset += vertexCount;
			}
		}

		// setup mesh buffers
		// TODO: multi material support
		if (modularMeshBuffer.Modules.size() > 0)
		{
			//_IRR_DEBUG_BREAK_IF(colladaMaterial == NULL);
			//_IRR_DEBUG_BREAK_IF(material == NULL);
			if (modularMeshBuffer.MeshBuffer == 0)
			{
				// TODO: fix vertex index range
				modularMeshBuffer.MeshBuffer = irrnew CColladaMeshBufferRef(
					*material,
					outputBuffer,
					0,
					totalVertexCount,
					reinterpret_cast<u16*>(modularMeshBuffer.Data + indexDataOffset),
					totalIndexCount,
					Box
				);
				modularMeshBuffer.MeshBuffer->setIsDynamic(true);
			}
			else
			{
				// TODO: fix vertex index range
				static_cast<CColladaMeshBufferRef*>(modularMeshBuffer.MeshBuffer)->setReferences(
					*material,
					outputBuffer,
					0,
					totalVertexCount,
						reinterpret_cast<u16*>(modularMeshBuffer.Data + indexDataOffset),
					totalIndexCount,
					Box
				);
			}
			setMaterial(materialIt, *colladaMaterial);
		}
	}		

	return result;
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT
CColladaModularSkinnedMesh::setModuleCount(u32 count,
										   bool withUpdate)
{
	u32 oldSize = Modules.size();

	// Drop references if count < old size
	for (u32 i = count; i < oldSize; ++i)
	{
		Modules[i]->drop();
		Modules[i] = NULL;
	}

	Modules.reallocate(count);
	Modules.set_used(count);

	CurrentModulesId.reallocate(count);
	CurrentModulesId.set_used(count);

	// Initialize if count > old size
	for (u32 i = oldSize; i < count; ++i)
	{
		Modules[i] = NULL;
		CurrentModulesId[i] = -1;
	}
	if (withUpdate && count < oldSize)
	{
		return updateBuffer(!hasOutputBuffer());
	}
	return EMUR_SUCCESS;
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT
CColladaModularSkinnedMesh::setModule(u32 id,
									  IColladaSkinnedMesh* module)
{
	if (Modules[id] != module)
	{
		if (module)
		{
			module->grab();
		}
		if (Modules[id])
		{
			Modules[id]->drop();
		}
		Modules[id] = module;
		return updateBuffer(!hasOutputBuffer());
	}
	return EMUR_SUCCESS;
}

CColladaModularSkinnedMesh::E_MODULE_UPDATE_RESULT
CColladaModularSkinnedMesh::setModules(IColladaSkinnedMesh** modules,
									   u32 count)
{
	if (count == 0)
	{
		count = Modules.size();
	}
	setModuleCount(count, false);
	for (u32 i = 0; i < count; ++i)
	{
		IColladaSkinnedMesh* mod = Modules[i];
		if (modules[i])
		{
			modules[i]->grab();
		}
		Modules[i] = modules[i];
		if (mod)
		{
			mod->drop();
		}
	}
	return updateBuffer(!hasOutputBuffer());
}

void
CColladaModularSkinnedMesh::setIsSkinningEnabled(bool value)
{
	if (hasOutputBuffer())
	{
		for (u32 i = 0, sz = Modules.size(); i < sz; ++i)
		{
			if (Modules[i])
			{
				Modules[i]->setIsSkinningEnabled(value);
			}
		}
		IColladaSkinnedMesh::setIsSkinningEnabled(value);
	}
	else
	{
		// We need to change all MeshBuffers, or force a working buffer, in
		// which case the construction should probably have set this
		_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED or use a private working buffer");
	}
}

IColladaMesh::E_TYPE
CColladaModularSkinnedMesh::getType() const
{
	return ET_MODULAR_SKINNED_MESH;
}

void
CColladaModularSkinnedMesh::onAnimate(u32 timeMs)
{
	for (u32 i = 0, sz = Modules.size(); i < sz; ++i)
	{
		if (Modules[i])
		{
			Modules[i]->onAnimate(timeMs);
		}
	}
	IsBoxDirty = true;
}

void
CColladaModularSkinnedMesh::skin(u32 buffer)
{
	// TODO: Support sub-material
	const int SKINNING_BUFFER = 0;

	SModularMeshBuffer &modularMeshBuffer = ModularMeshBuffers[buffer];
	const video::SS3DVertexComponentArrays &outputBuffer = *(const video::SS3DVertexComponentArrays*)modularMeshBuffer.MeshBuffer->getVertices();

	_IRR_DEBUG_BREAK_IF(getMeshBuffer(buffer)->getVertexIndexStart() != 0);

	core::vector3df* positions = outputBuffer.Position;
	u32 posStride = outputBuffer.PositionStride;
	core::vector3df* normals = outputBuffer.Normal;
	u32 normalStride = outputBuffer.NormalStride;

	for (u32 i = 0, sz = modularMeshBuffer.Modules.size(); i < sz; ++i)
	{
		IColladaSkinnedMesh* mesh = Modules[modularMeshBuffer.Modules[i]];
		if (mesh)
		{
			IMeshBuffer* mb = mesh->getMeshBuffer(SKINNING_BUFFER);
			video::S3DVertexComponentArrays* localOutputBuffer
				= reinterpret_cast<video::S3DVertexComponentArrays*>(
					mb->getVertices()
				);

			// save pointers
			core::vector3df* oldPosition
				= localOutputBuffer->Position;
			u32 oldPositionStride
				= localOutputBuffer->PositionStride;

			core::vector3df* oldNormal
				= localOutputBuffer->Normal;
			u32 oldNormalStride
				= localOutputBuffer->NormalStride;

			// override destinations, where we need to offset pointers to
			// compensate for vertex index start position, like
			// IVideoDriver::getProcessBuffer does
			u32 vertexStart = mb->getVertexIndexStart();

			localOutputBuffer->Position
				= core::stepPointer(positions,
									- (int)(posStride * vertexStart));
			localOutputBuffer->PositionStride = posStride;

			if (normals)
			{
				localOutputBuffer->Normal
					= core::stepPointer(normals,
										- (int)(normalStride * vertexStart));
				localOutputBuffer->NormalStride = normalStride;
			}
			else
			{
				localOutputBuffer->Normal = NULL;
			}

			// process
			mesh->skin(SKINNING_BUFFER);

			// pass to next output pointers
			u32 cnt = mb->getVertexCount();

			positions = core::stepPointer(
				positions,
				cnt * outputBuffer.PositionStride
			);
			if (normals)
			{
				normals = core::stepPointer(
					normals,
					cnt * outputBuffer.NormalStride
				);
			}
			
			// restore pointers
			localOutputBuffer->Position
				= oldPosition;
			localOutputBuffer->PositionStride
				= oldPositionStride;

			localOutputBuffer->Normal
				= oldNormal;
			localOutputBuffer->NormalStride
				= oldNormalStride;
		}
	}
}

video::E_DRIVER_ALLOCATION_RESULT
CColladaModularSkinnedMesh::onPrepareBufferForRendering(
	E_PREPARE_BUFFER_STEP step,
	video::IVideoDriver* driver,
	u32 buffer
)
{
	//_IRR_DEBUG_BREAK_IF(buffer != 0);
	video::E_DRIVER_ALLOCATION_RESULT result = video::EDAR_NOT_FOUND;
	if (isSkinningEnabled())
	{
		IMeshBuffer* mb = ModularMeshBuffers[buffer].MeshBuffer;
		result = driver->getProcessBuffer(
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
			skin(buffer);
		}
	}
	return result;
}

u32
CColladaModularSkinnedMesh::getMeshBufferCount() const
{
	return ModularMeshBuffers.size();
}

IMeshBuffer*
CColladaModularSkinnedMesh::getMeshBuffer(u32 nr) const
{
	return ModularMeshBuffers[nr].MeshBuffer;
}

void
CColladaModularSkinnedMesh::computeBoundingBox()
{
	u32 i = 0, sz = Modules.size();

	// search first non null module to setup bounding box
	for(; i < sz; ++i)
	{
		if (Modules[i])
		{
			Box = Modules[i]->getBoundingBox();
			break;
		}
	}

	// union with the rest
	for(++i; i < sz; ++i)
	{
		if (Modules[i])
		{
			Box.addInternalBox(Modules[i]->getBoundingBox());
		}
	}

	IsBoxDirty = false;
}

const core::aabbox3d<f32>&
CColladaModularSkinnedMesh::getBoundingBox() const
{
	if (IsBoxDirty)
	{
		const_cast<CColladaModularSkinnedMesh*>(this)->computeBoundingBox();
	}
	return Box;
}

} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
