#include "stdafx.h"
#include "CSkinnedMeshSceneNode.h"

#include "CAnimatedMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "irros.h"
#include "CShadowVolumeSceneNode.h"
#include "IAnimatedMeshMD3.h"
#include "CSkinnedMesh.h"
#include "IDummyTransformationSceneNode.h"
#include "IBoneSceneNode.h"
#include "IMaterialRenderer.h"
#include "IMesh.h"
#include "IMeshCache.h"
#include "IAnimatedMesh.h"
#include "quaternion.h"

using namespace irr;
using namespace scene;

//! renders the node.
void CSkinnedMeshSceneNode::render(void* /*renderData*/)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;


	bool isTransparentPass =
		SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;

	f32 frame = getFrameNr();

	scene::IMesh* m;

	m = Mesh->getMesh((s32)frame, 255, StartFrame, EndFrame);

	if ( 0 == m )
	{
		#ifdef _DEBUG
			os::Printer::log("Animated Mesh returned no mesh to render.", Mesh->getDebugName(), ELL_WARNING);
		#endif
	}

	driver->setTransform(video::ETS_WORLD, getRelativeTransformation());

	// render original meshes
	
	for (u32 i=0; i<m->getMeshBufferCount(); ++i)
	{
		video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Materials[i].getMaterialType());
		bool transparent = (rnd && rnd->isTransparent());

		// only render transparent buffer if this is the transparent render pass
		// and solid only in solid pass
		if (transparent == isTransparentPass)
		{
			scene::IMeshBuffer* mb = m->getMeshBuffer(i);
			driver->setMaterial(Materials[i]);
			driver->drawMeshBuffer(mb);
		}
	}

	// for debug purposes only:
	if ( DebugDataVisible & scene::EDS_BBOX )
	{
		video::SMaterial deb_m;
		deb_m.setFlag(video::EMF_LIGHTING, false);
		driver->setMaterial(deb_m);
		driver->draw3DBox(m->getBoundingBox(), video::SColor(255,255,255,255));
	}
	
}
