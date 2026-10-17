#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "IMaterialRenderer.h"
#include "IAnimatedMesh.h"

namespace irr
{
namespace scene
{

//! Constructor
CColladaMeshSceneNode::CColladaMeshSceneNode(
	IColladaMesh* mesh,
	collada::IRootSceneNode* root,
	collada::SNode* node,
	s32 id,
	const core::vector3df& position,
	const core::quaternion& rotation,
	const core::vector3df& scale
)
	: IMeshSceneNode(id, position, rotation, scale)
	, Mesh(mesh)
	, Node(node)
	, Root(root)
	, ReadOnlyMaterials(false)
	, MaterialChanged(true)
{
#ifdef _DEBUG
	setDebugName("CColladaMeshSceneNode");
#endif

	_IRR_DEBUG_BREAK_IF(Mesh == 0);
	Mesh->grab();
}

CColladaMeshSceneNode::~CColladaMeshSceneNode()
{
	_IRR_DEBUG_BREAK_IF(Mesh == 0);
	Mesh->drop();
	for (u32 i = 0; i < Materials.size(); ++i)
	{
		Materials[i]->drop();
		Materials[i] = NULL;
	}
}

const char*
CColladaMeshSceneNode::getUID() const
{
	if (Node)
	{
		return Node->id;
	}
	else
	{
		return Mesh->getUID();
	}
}

scene::ESCENE_NODE_TYPE
CColladaMeshSceneNode::getType() const
{
	return scene::ESNT_COLLADA_MESH;
}

IMesh*
CColladaMeshSceneNode::getMesh(void) 
{ 
	return Mesh; 
}

void
CColladaMeshSceneNode::setMesh(IMesh*)
{ 
	_IRR_DEBUG_BREAK_IF("NOT ALLOWED");
}

const core::aabbox3d<f32>&
CColladaMeshSceneNode::getBoundingBox() const
{
	_IRR_DEBUG_BREAK_IF(Mesh == 0);
	return Mesh->getBoundingBox();
}

void
CColladaMeshSceneNode::OnAnimate(u32 timeMs)
{
	if (IsVisible)
	{
		ISceneNode::OnAnimate(timeMs);
		Mesh->onAnimate(timeMs);
	}
}

void
CColladaMeshSceneNode::setReadOnlyMaterials(bool readonly) 
{
	ReadOnlyMaterials = true;	
}

bool
CColladaMeshSceneNode::isReadOnlyMaterials() const
{
	return ReadOnlyMaterials;
}

video::SMaterial&
CColladaMeshSceneNode::getMaterial(u32 num)
{
	return Materials[num]->get();
}

u32
CColladaMeshSceneNode::getMaterialCount() const
{
	return Materials.size();
}

void
CColladaMeshSceneNode::setVisible(bool isVisible)
{
	IMeshSceneNode::setVisible(isVisible);

	// no need to set to visible, OnRegisterSceneNode will set this once this
	// node is made visible
	if (!isVisible)
	{
		for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
			{
				mb->setIsVisible(false);
			}
		}
	}
}

void
CColladaMeshSceneNode::notifyVisibilityChanged(bool visibility)
{
	IMeshSceneNode::notifyVisibilityChanged(visibility);

	// no need to set to visible, OnRegisterSceneNode will set this once this
	// node is made visible
	if (!visibility)
	{
		for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
			{
				mb->setIsVisible(false);
			}
		}
	}
}

void
CColladaMeshSceneNode::onMaterialChanged()
{
	// because this node supports rendering of mixed mode meshes consisting of 
	// transparent and solid material at the same time, we need to go through all 
	// materials, check of what type they are and register this node for the right
	// render pass according to that.

	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	TransparentMaterialCount = 0;
	SolidMaterialCount = 0;

	MaterialsRenderSet.reallocate(Mesh->getMeshBufferCount());
	MaterialsRenderSet.set_used(Mesh->getMeshBufferCount());

	// count transparent and solid materials in this scene node
	if (ReadOnlyMaterials && Mesh)
	{
		// count mesh materials 

		for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
		{
			scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			video::IMaterialRenderer* rnd
				= mb ? driver->getMaterialRenderer(mb->getMaterial().getMaterialType()) : 0;

			if (rnd && rnd->isTransparent()) 
			{
				MaterialsRenderSet[i] = ERS_TRANSPARENT;
				++TransparentMaterialCount;
			}
			else 
			{
				MaterialsRenderSet[i] = ERS_SOLID;
				++SolidMaterialCount;
			}
		}
	}
	else
	{
		// count copied materials 

		for (u32 i = 0, sz = Materials.size(); i < sz; ++i)
		{
			video::IMaterialRenderer* rnd =
				driver->getMaterialRenderer(Materials[i]->get().getMaterialType());

			if (rnd && rnd->isTransparent()) 
			{
				MaterialsRenderSet[i] = ERS_TRANSPARENT;
				++TransparentMaterialCount;
			}
			else 
			{
				MaterialsRenderSet[i] = ERS_SOLID;
				++SolidMaterialCount;
			}
		}	
	}
}

//! frame
void CColladaMeshSceneNode::OnRegisterSceneNode()
{
	if (IsVisible && Mesh)
	{
		if(MaterialChanged)
		{
			onMaterialChanged();
			MaterialChanged = false;
		}

		// register according to material types counted
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		if (driver == NULL)
		{
			return;
		}
#ifdef _DEBUG
		if (DebugDataVisible)
		{
			if (SceneManager->registerNodeForRendering(this, 0, 0) != 0)
			{
				for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
				{
					Mesh->onPrepareBufferForRendering(IColladaMesh::EPBS_REGISTRATION,
													  driver,
													  i);
				}
			}
		}
		else
#endif
		if (SceneManager->isCulled(this))
		{
			for (u32 i = 1, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
			{
				IMeshBuffer* mb = Mesh->getMeshBuffer(i);
				if (mb)
				{
					mb->setIsVisible(false);
				}
			}
		}
		else
		{
			E_CULLING_TYPE oldCullingType = AutomaticCullingState;
			AutomaticCullingState = EAC_OFF;
			for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
			{
				IMeshBuffer* mb = getMeshBuffer(i);
				if (mb)
				{
					mb->setIsVisible(true);
					const video::SMaterial& material = (ReadOnlyMaterials
														? mb->getMaterial()
														: Materials[i]->get());
					video::E_DRIVER_ALLOCATION_RESULT prepareResult
						= Mesh->onPrepareBufferForRendering(
							IColladaMesh::EPBS_REGISTRATION,
							driver,
							i
						);
					if (prepareResult == video::EDAR_SUCCESS
						|| prepareResult == video::EDAR_NOT_FOUND)
					{
						SceneManager->registerNodeForRendering(
							this,
							&material,
							(void*)(i + 1),
							(MaterialsRenderSet[i] == ERS_SOLID
							 ? scene::ESNRP_SOLID
							 : scene::ESNRP_TRANSPARENT)
						);
					}
					else if (prepareResult
							 == video::EDAR_SUCCESS_AND_SKIP_RENDERING)
					{
						// dummy call to force updating the bounding box (if
						// culling was disabled), which might be required by
						// what caused the rendering to be skipped
						Mesh->getBoundingBox();
					}
				}
			}
			AutomaticCullingState = oldCullingType;
		}
		ISceneNode::OnRegisterSceneNode();
	}
}

IMeshBuffer* CColladaMeshSceneNode::getMeshBuffer(u32 materialIdx)
{
	return Mesh->getMeshBuffer(materialIdx);
}

void CColladaMeshSceneNode::prepareMaterial()
{
	//Clear old materials
	for (u32 i = 0; i < Materials.size(); ++i)
	{
		Materials[i]->drop();
	}
	Materials.clear();

	notifyMaterialChanged();
	//Prepare new materials
	int materialCount = Mesh->getMeshBufferCount();
	Materials.reallocate(materialCount);
	for(int i = 0; i < materialCount; ++i)
	{
		collada::SEffect* colladaFx = Mesh->getMaterial(i)->instanceEffect.pEffect;
		//video::SMaterial mat;

		collada::CMaterial* mat2 = Root->hasLibraryMaterial(Mesh->getMaterial(i)->id);
		if(!mat2)
		{
			mat2 = Root->addLibraryMaterial(Mesh->getMaterial(i)->id, Mesh->getCollada());
		}
		mat2->grab();
		Materials.push_back(mat2);
	}
}

void CColladaMeshSceneNode::notifyMaterialChanged()
{
	MaterialChanged = true;
}

int CColladaMeshSceneNode::getRenderVertexCount(void* renderData) 
{
	u32 i = (u32)renderData - 1;
	IMeshBuffer* mb = Mesh->getMeshBuffer(i);
	return mb->getVertexCount();
}

//! renders the node.
void CColladaMeshSceneNode::render(void* renderData)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

#ifdef _DEBUG
	if (renderData)
#endif
	{
		u32 i = (u32)renderData - 1;
		_IRR_DEBUG_BREAK_IF(i >= Mesh->getMeshBufferCount());
		IMeshBuffer* mb = Mesh->getMeshBuffer(i);
		if (mb)
		{
			video::E_DRIVER_ALLOCATION_RESULT result = video::EDAR_NOT_FOUND;
			if (!Mesh->hasOutputBuffer())
			{
				result = Mesh->onPrepareBufferForRendering(
					IColladaMesh::EPBS_RENDERING,
					driver,
					i
				);
			}

			const video::SMaterial& material = (ReadOnlyMaterials
												? mb->getMaterial()
												: Materials[i]->get());
			driver->setMaterial(material);
			driver->drawMeshBuffer(mb);

			if (result & video::EDAR_SUCCESS)
			{
				Mesh->releaseProcessBuffer(driver, i);
			}
		}
	}
#ifdef _DEBUG
	else // null render is used to render debug data
	{
		_IRR_DEBUG_BREAK_IF(DebugDataVisible == 0);

		bool isTransparentPass = 
			SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

		// overwrite half transparency
		bool renderMeshes = true;
		if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY)
		{
			video::SMaterial mat;
			for (u32 g = 0, cnt = Mesh->getMeshBufferCount(); g < cnt; ++g)
			{
				mat = Materials[g]->get();
				mat.setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
				driver->setMaterial(mat);
				driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
				renderMeshes = false;
			}
		}

		// render original meshes
		if ( renderMeshes )
		{
			for (u32 i = 0, cnt = Mesh->getMeshBufferCount(); i < cnt; ++i)
			{
				// only render transparent buffer if this is the transparent render pass
				// and solid only in solid pass
				if ((MaterialsRenderSet[i] != 0) == isTransparentPass)
				{
					if (!Mesh->hasOutputBuffer())
					{
						Mesh->onPrepareBufferForRendering(IColladaMesh::EPBS_RENDERING,
														  driver,
														  i);
					}

					scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
					if (mb)
					{
						const video::SMaterial& material = (ReadOnlyMaterials
															? mb->getMaterial()
															: Materials[i]->get());
						driver->setMaterial(material);
						driver->drawMeshBuffer(mb);
					}

					if (Mesh->hasOutputBuffer())
					{
						Mesh->releaseProcessBuffer(driver, i);
					}
				}
			}
		}

		video::SMaterial m;
		m.setFlag(video::EMF_LIGHTING, false);
		driver->setMaterial(m);
		const core::aabbox3d<f32>& Box = Mesh->getBoundingBox();

		if (DebugDataVisible & scene::EDS_BBOX)
		{
			driver->draw3DBox(Box, video::SColor(255,255,255,255));
		}
		if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
		{
			for (u32 g = 0, cnt = Mesh->getMeshBufferCount(); g < cnt; ++g)
			{
				driver->draw3DBox(
					Mesh->getMeshBuffer(g)->getBoundingBox(),
					video::SColor(255,190,128,128)
				);
			}
		}

		if (DebugDataVisible & scene::EDS_NORMALS)
		{
			IAnimatedMesh* arrow = SceneManager->addArrowMesh (
				"__debugnormal", video::SColor(0xFF,0xEC,0xEC,0x00),
				video::SColor(0xFF,0x99,0x99,0x00), 4, 8, 1.f, 0.6f, 0.05f,
				0.3f
			);
			if ( 0 == arrow )
			{
				arrow = SceneManager->getMesh ( "__debugnormal" );
			}
			IMesh* mesh = arrow->getMesh(0);

			// find a good scaling factor

			core::matrix4 m2;

			// draw normals
			for (u32 g = 0, cnt = Mesh->getMeshBufferCount(); g < cnt; ++g)
			{
				const scene::IMeshBuffer* mb = Mesh->getMeshBuffer(g);
				const u32 vSize = video::getVertexPitchFromType(mb->getVertexType());
				const video::S3DVertex* v = ( const video::S3DVertex*)mb->getVertices();
				for ( u32 i=0; i != mb->getVertexCount(); ++i )
				{
					// align to v->Normal
					core::quaternion quatRot(v->Normal.X, 0.f, -v->Normal.X, 1+v->Normal.Y);
					quatRot.normalize();
					quatRot.getMatrix(m2);

					m2.setTranslation(v->Pos);
					m2 *= AbsoluteTransformation;

					driver->setTransform(video::ETS_WORLD, m2);
					for (u32 a = 0, cnt = mesh->getMeshBufferCount(); a < cnt; ++a)
						driver->drawMeshBuffer(mesh->getMeshBuffer(a));

					v = (const video::S3DVertex*) ( (u8*) v + vSize );
				}
			}
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		}

		// show mesh
		if (DebugDataVisible & scene::EDS_MESH_WIRE_OVERLAY)
		{
			m.setFlag(video::EMF_WIREFRAME, true);
			driver->setMaterial(m);

			for (u32 g = 0, cnt = Mesh->getMeshBufferCount(); g < cnt; ++g)
			{
				driver->drawMeshBuffer( Mesh->getMeshBuffer(g) );
			}
		}
	}
#endif
}

} // namespace scene
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
