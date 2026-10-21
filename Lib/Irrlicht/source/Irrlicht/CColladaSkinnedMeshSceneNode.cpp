#include "stdafx.h"
#include <cstdint>
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSkinnedMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "IMaterialRenderer.h"
#include "IAnimatedMesh.h"
#include "irrProcessBufferHeap.h"

#ifdef _IRR_USE_WII_DEVICE_
#    include <revolution/gx.h>
#endif

namespace irr
{
namespace collada
{

CSkinnedMeshSceneNode::CSkinnedMeshSceneNode(
	scene::IColladaMesh* mesh,
	IRootSceneNode* root,
	SNode* node,
	s32 id,
	const core::vector3df& position,
	const core::quaternion& rotation,
	const core::vector3df& scale
)
	: scene::CColladaMeshSceneNode(mesh, root, node, id, position, rotation, scale)
{
#ifdef _DEBUG
	setDebugName("CColladaSkinnedMeshSceneNode");
#endif
	
	setAutomaticCulling(scene::EAC_FRUSTUM_BOX);
}

scene::ESCENE_NODE_TYPE
CSkinnedMeshSceneNode::getType() const
{
	return scene::ESNT_COLLADA_SKIN_MESH;
}

void CSkinnedMeshSceneNode::OnAnimate(u32 timeMs)
{
	if (IsVisible)
	{
		ISceneNode::OnAnimate(timeMs);
		Mesh->onAnimate(timeMs);
	}
}	

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
const core::aabbox3d<f32>& CSkinnedMeshSceneNode::getTransformedBoundingBox() const
{
	return getBoundingBox();
}
#else
const core::aabbox3d<f32> CSkinnedMeshSceneNode::getTransformedBoundingBox() const
{
	core::aabbox3d<f32> box = getBoundingBox();
	return box;
}
#endif

//! renders the node.
void CSkinnedMeshSceneNode::render(void* renderData)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;

	if (isSkinningEnabled()
		// still render as if skinning was enabled if the mesh has a working
		// buffer, this actually implements a "freeze"
		|| Mesh->hasOutputBuffer())
	{
		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix); // set Identity
	}
	else if (Mesh->getType() == scene::IColladaMesh::ET_SKINNED_MESH)
	{
		if (static_cast<scene::CColladaSkinnedMesh*>(Mesh)->isBindShapeMatrixIdentity())
		{
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		}
		else
		{
			driver->setTransform(
				video::ETS_WORLD,
				(AbsoluteTransformation
				 * static_cast<scene::CColladaSkinnedMesh*>(Mesh)->getBindShapeMatrix())
			);
		}
	}
	else
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	}

	bool doSkinning
		= (isSkinningEnabled()
		   && !Mesh->hasOutputBuffer());

#ifdef _DEBUG
	if (renderData)
#endif
	{
		u32 i = static_cast<u32>(reinterpret_cast<std::uintptr_t>(renderData) - 1);
		_IRR_DEBUG_BREAK_IF(i >= Mesh->getMeshBufferCount());
		scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
		if (mb)
		{
			video::E_DRIVER_ALLOCATION_RESULT result = video::EDAR_FAILED; // anything but success
			if (doSkinning)
			{
				result = Mesh->onPrepareBufferForRendering(scene::IColladaMesh::EPBS_RENDERING,
														   driver,
														   i);
#ifdef _IRR_USE_WII_DEVICE_
				GXInvalidateVtxCache();
#endif
			}

			// on readonly, we index Mesh's mesh buffers because the source might be different
			const video::SMaterial& material = (ReadOnlyMaterials
												? mb->getMaterial()
												: Materials[i]->get());
			driver->setMaterial(material);
			driver->drawMeshBuffer(mb);

			if (result & video::EDAR_SUCCESS)
			{
#ifdef _IRR_USE_WII_DEVICE_
				GXDrawDone();
#endif
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
		if ( DebugDataVisible & scene::EDS_HALF_TRANSPARENCY )
		{
			video::SMaterial mat;
			for (u32 g = 0; g < Mesh->getMeshBufferCount(); ++g)
			{
				mat = Materials[g]->get();
				mat.setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
				driver->setMaterial(mat);
				driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
			}
		}

		// render original meshes
		if ( renderMeshes )
		{
			for (u32 i = 0, nbBuf = Mesh->getMeshBufferCount(); i < nbBuf; ++i)
			{
				scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
				if (mb)
				{
					// on readonly, we index Mesh's mesh buffers because the source might be different
					const video::SMaterial& material = (ReadOnlyMaterials
														? mb->getMaterial()
														: Materials[i]->get());

					video::IMaterialRenderer* rnd = driver->getMaterialRenderer(material.getMaterialType());
					bool transparent = (rnd && rnd->isTransparent());
					
					// only render transparent buffer if this is the transparent render pass
					// and solid only in solid pass
					if (transparent == isTransparentPass) 
					{
						video::E_DRIVER_ALLOCATION_RESULT result = video::EDAR_FAILED; // anything but success
						if (doSkinning)
						{
							result = Mesh->onPrepareBufferForRendering(scene::IColladaMesh::EPBS_RENDERING,
																	   driver,
																	   i);
#ifdef _IRR_USE_WII_DEVICE_
							GXInvalidateVtxCache();
#endif
						}
						driver->setMaterial(material);
						driver->drawMeshBuffer(mb);

						if (result & video::EDAR_SUCCESS)
						{
#ifdef _IRR_USE_WII_DEVICE_
							GXDrawDone();
#endif
							static_cast<scene::IColladaSkinnedMesh*>(Mesh)
								->releaseProcessBuffer(driver, i);
						}
					}
				}
			}
		}

		video::SMaterial m;
		m.setFlag(video::EMF_LIGHTING, false);
		driver->setMaterial(m);

		const core::aabbox3d<f32>&Box = getTransformedBoundingBox();

		if ( DebugDataVisible & scene::EDS_BBOX )
		{
			driver->draw3DBox(Box, video::SColor(255,255,255,255));
		}
		if ( DebugDataVisible & scene::EDS_BBOX_BUFFERS )
		{
			for (u32 g=0; g < Mesh->getMeshBufferCount(); ++g)
			{
				driver->draw3DBox(
					Mesh->getMeshBuffer(g)->getBoundingBox(),
					video::SColor(255,190,128,128));
			}
		}

		if ( DebugDataVisible & scene::EDS_NORMALS )
		{
			scene::IAnimatedMesh * arrow = SceneManager->addArrowMesh (
				"__debugnormal", video::SColor(0xFF,0xEC,0xEC,0x00),
					video::SColor(0xFF,0x99,0x99,0x00), 4, 8, 1.f, 0.6f, 0.05f,
					0.3f);
			if ( 0 == arrow )
			{
				arrow = SceneManager->getMesh ( "__debugnormal" );
			}
			scene::IMesh *mesh = arrow->getMesh(0);

			// find a good scaling factor

			core::matrix4 m2;

			// draw normals
			for (u32 g = 0; g < mesh->getMeshBufferCount(); ++g)
			{
				const scene::IMeshBuffer* mb = mesh->getMeshBuffer(g);
				const u32 vSize = video::getVertexPitchFromType(mb->getVertexType());
				const video::S3DVertex* v = ( const video::S3DVertex*)mb->getVertices();
				for ( u32 i=0; i != mb->getVertexCount(); ++i )
				{
					// align to v->Normal
					core::quaternion quatRot(v->Normal.X, 0.f, -v->Normal.X, 1+v->Normal.Y);
					quatRot.normalize();
					quatRot.getMatrix(m2);

					m2.setTranslation(v->Pos);
					m2*=AbsoluteTransformation;

					driver->setTransform(video::ETS_WORLD, m2);
					for (u32 a = 0; a != mesh->getMeshBufferCount(); ++a)
						driver->drawMeshBuffer(mesh->getMeshBuffer(a));

					v = (const video::S3DVertex*) ( (u8*) v + vSize );
				}
			}
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		}

		// show mesh
		if ( DebugDataVisible & scene::EDS_MESH_WIRE_OVERLAY )
		{
			m.setFlag(video::EMF_WIREFRAME, true);
			driver->setMaterial(m);

			for (u32 g=0; g < Mesh->getMeshBufferCount(); ++g)
			{
				driver->drawMeshBuffer( Mesh->getMeshBuffer(g) );
			}
		}
	}
#endif
}

void CSkinnedMeshSceneNode::setIsSkinningEnabled(bool value)
{
	static_cast<scene::IColladaSkinnedMesh*>(Mesh)->setIsSkinningEnabled(value);
}

}; // namespace collada
}; // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
