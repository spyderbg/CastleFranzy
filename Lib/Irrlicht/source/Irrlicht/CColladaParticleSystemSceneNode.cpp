#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaParticleSystemSceneNode.h"
#include "CColladaParticleSystemForceSceneNode.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"
#include "IColladaRootSceneNode.h"

extern void* IrrAlloc(size_t size, irr::MemHint hint);
extern void IrrFree(void* ptr);

namespace irr
{

#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_

//NOTE-FH: These are temp definition
void* ps::PS_NEW(size_t size)
{
	//return malloc(size);
	return IrrAlloc(size, MEMHINT_NONE);
}

void ps::PS_FREE(void* p)
{
	//free(p);
	IrrFree(p);
}
#endif

namespace collada
{

CParticleSystemSceneNode::CParticleSystemSceneNode(const CColladaDatabase &database, 
												   SEmitter &emitter, 
												   res::vector<res::String> *forceNodes, 
												   collada::IRootSceneNode *pRoot)
	: IObject(database)
	, m_meshBuffer(0)
	, m_psViewMat(0)
	, m_pEmitter(&emitter)
	, m_pRoot(pRoot)
	, m_pForceNodes(forceNodes)
	, m_bHasUVAnimation(false)
{
#ifdef _DEBUG
	setDebugName("CColladaParticleSystemSceneNode");
#endif

	setUID(emitter.id);
	m_pRoot->addParticleSystem(this);
}

CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	if (m_particleSystem) {
		delete m_particleSystem;
	}
	if (m_meshBuffer) {
		delete m_meshBuffer;
	}
	for(int i = 0; i < m_Materials.size(); ++i)
	{
		m_Materials[i]->drop();
	}
	m_Materials.clear();
#endif
}

int CParticleSystemSceneNode::getParticleCount()
{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	return m_particleSystem->getParticleCount();
#else
	return 0;
#endif
}

void CParticleSystemSceneNode::initParticleSystem()
{
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_

	m_particleSystem = ps::PSManager::getInstance().createPCloudSystem();

	//Emitter
	m_particleSystem->setParameter(EMITTER_TYPE_N, m_pEmitter->shape.shapeElement.iType);
	switch (m_pEmitter->shape.shapeElement.iType) {
		case SShapeElement::SE_BOX:
			m_particleSystem->setParameter(RADIUS_LENGTH_N, m_pEmitter->shape.shapeElement.pBox->length);
			m_particleSystem->setParameter(WIDTH_N, m_pEmitter->shape.shapeElement.pBox->width);
			m_particleSystem->setParameter(HEIGHT_N, m_pEmitter->shape.shapeElement.pBox->height);
			break;
		case SShapeElement::SE_SPHERE:
			m_particleSystem->setParameter(RADIUS_LENGTH_N, m_pEmitter->shape.shapeElement.pSphere->radius);
			break;
		case SShapeElement::SE_CYLINDER:
			m_particleSystem->setParameter(RADIUS_LENGTH_N, m_pEmitter->shape.shapeElement.pCylinder->radius);
			m_particleSystem->setParameter(HEIGHT_N, m_pEmitter->shape.shapeElement.pCylinder->height);
			break;
	}

	//Generation
	m_particleSystem->setParameter(MAX_PARTICLES_N, m_pEmitter->emission.maxParticles);
	m_particleSystem->setParameter(BIRTH_RATE_N, m_pEmitter->emission.birthRate);		
	
	//Life
	m_particleSystem->setParameter(LIFE_N, m_pEmitter->particleLife.life);
	m_particleSystem->setParameter(LIFE_VARIATION_N, m_pEmitter->particleLife.lifeVariance);

	//Size
	m_particleSystem->setParameter(TARGET_SIZE_N, m_pEmitter->particleSize.size);
	m_particleSystem->setParameter(SIZE_VARIATION_N, m_pEmitter->particleSize.sizeVariance);
	m_particleSystem->setParameter(SIZE_GROWTH_TIME_N, m_pEmitter->particleSize.sizeGrowFor);
	m_particleSystem->setParameter(SIZE_FADE_TIME_N, m_pEmitter->particleSize.sizeFadeFor);

	//Motion
	m_particleSystem->setParameter(SPEED_N, m_pEmitter->particleMotion.speed);
	m_particleSystem->setParameter(SPEED_VARIATION_N, m_pEmitter->particleMotion.speedVariance);
	switch (m_pEmitter->particleMotion.direction.type) {
		case SDirection::DT_RANDOM_VECTOR:
			m_particleSystem->setParameter(DIRECTION_N, core::vector3df());
			break;
		case SDirection::DT_VARIABLE_VECTOR:
			m_particleSystem->setParameter(DIRECTION_N, m_pEmitter->particleMotion.direction.pVariableVector->vector);
			m_particleSystem->setParameter(DIR_VARIATION_N, m_pEmitter->particleMotion.direction.pVariableVector->variance);
			break;
		case SDirection::DT_DIRECTION_TARGET:
			//TODO-FH:
			//m_particleSystem->setParameter(DIRECTION_N, m_pEmitter->particleMotion.direction.pDirectionTarget->url);
			m_particleSystem->setParameter(DIR_VARIATION_N, m_pEmitter->particleMotion.direction.pDirectionTarget->variance);
			break;
	}

	//Color
	m_particleSystem->setParameter(ANIMKEY_MAPPING_TYPE_N, m_pEmitter->particleAnimationScaling.method);
	m_particleSystem->setParameter(ANIM_OFFSET_N, m_pEmitter->particleAnimationOffset.offset);
	m_particleSystem->setParameter(ANIM_OFFSET_VARIATION_N, m_pEmitter->particleAnimationOffset.offsetVariance);
	m_particleSystem->setParameter(ANIM_LENGTH_N, m_pEmitter->particleAnimationScaling.srcLength);
	m_particleSystem->setParameter(ANIM_LENGTH_VARIATION_N, m_pEmitter->particleAnimationScaling.srcVariance);
	m_particleSystem->setParameter(ANIM_SCALE_MULTIPLIER_N, m_pEmitter->particleAnimationScaling.dstMultiplier);
	m_particleSystem->setParameter(ANIM_SCALE_MULTIPLIER_VARIATION_N, m_pEmitter->particleAnimationScaling.dstVariance);

	//TODO-FH: Particle Spin

	//TODO-FH: Particle Type

	//NOTE-FH: Currently the particle expect a CPSQuadMeshBuffer to work
	//RenderData
	m_meshBuffer = irrnew ps::CPSQuadMeshBuffer();

	m_particleSystem->setParameter(RENDER_MESH_N, static_cast<scene::IMeshBuffer*>(m_meshBuffer));
	m_psViewMat = reinterpret_cast<core::matrix4*>(m_particleSystem->getParameter(VIEW_MATRIX_N));
	
	m_particleSystem->setWorldMatrixPtr(&AbsoluteTransformation);

	m_particleSystem->init();
#endif
}

void CParticleSystemSceneNode::OnRegisterSceneNode()
{
	//TODO-FH:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	if (m_particleSystem->getParticleCount()) 
	{
		SceneManager->registerNodeForRendering(this, &getMaterial(0), 0, scene::ESNRP_TRANSPARENT);
	}
#endif
}

void CParticleSystemSceneNode::OnAnimate(u32 timeMs)
{
	ISceneNode::OnAnimate(timeMs);

#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	m_psTimeMs = timeMs;  //TODO-FH:
	m_particleSystem->update(static_cast<float>(m_psTimeMs / 1000.0f));
#endif
}

void CParticleSystemSceneNode::render(void*)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	
	if (!driver)
		return;

	scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
	if (camera)
	{
		*m_psViewMat = camera->getViewFrustum()->Matrices[video::ETS_VIEW];
		SceneManager->getVideoDriver()->Orientation3DViewTransform(m_psViewMat->pointer(), SceneManager->getVideoDriver()->getOrientation3D());
	}
	else
		*m_psViewMat = core::matrix4();


	driver->setTransform(video::ETS_WORLD, core::matrix4());
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	ps::PRenderData& renderdata = m_particleSystem->getRenderData();

	if(m_bHasUVAnimation)
	{
		getMaterial(0).setTextureMatrix(0, core::matrix4());
	}

	driver->setMaterial(getMaterial(0));

	driver->drawVertexPrimitiveList(&renderdata.m_vertices, &renderdata.m_indices[0], 0, m_particleSystem->getParticleCount() * renderdata.m_riElementCount, m_particleSystem->getParticleCount()*2, video::EVT_COMPONENT_ARRAYS, scene::EPT_TRIANGLES, video::EIT_16BIT);
#endif

}

void CParticleSystemSceneNode::prepareMaterial()
{
	int iMaterialCount = m_OriginalMaterials.size();
	m_Materials.reallocate(iMaterialCount);
	for(int i = 0; i < iMaterialCount; ++i)
	{
		collada::SEffect *pColladaFx = m_OriginalMaterials[i]->instanceEffect.pEffect;
		//video::SMaterial mat;

		collada::CMaterial *pMat2 = m_pRoot->hasLibraryMaterial(m_OriginalMaterials[i]->id);
		if(!pMat2)
		{
			pMat2 = m_pRoot->addLibraryMaterial(m_OriginalMaterials[i]->id, getCollada());

			// TODO: Remove this post modification when artist will be able 
			//       to set properly the material in MAX.
			if(pMat2->get().isTransparent() && pMat2->get().getMaterialType() != video::EMT_TRANSPARENT_ADD_COLOR)
			{
				pMat2->get().setMaterialType(video::EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA);
			}
		}
		pMat2->grab();
		m_Materials.push_back(pMat2);
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
		//TODO-FH: cleaner 
		m_meshBuffer->setMaterial(pMat2->get());
		////NOTE-FH: Temp settings////
		pMat2->get().setFlag(irr::video::EMF_LIGHTING, false);
		//////////////////////////////
		SAnimation *pAnimation = pMat2->getCollada().getAnimation(pMat2->getOriginal().instanceEffect.url + 1, SChannel::CT_EFFECT_DIFFUSE);
		if(pAnimation != 0)
		{
			m_particleSystem->setParameter(DIFFUSE_COLOR_TRACK_N, pMat2->getCollada().getAnimation(pMat2->getOriginal().instanceEffect.url + 1, SChannel::CT_EFFECT_DIFFUSE));
		}
		else
		{
			m_particleSystem->setParameter(DIFFUSE_COLOR_TRACK_N, pMat2->getCollada().getAnimation(pMat2->getOriginal().instanceEffect.url + 1, SChannel::CT_EFFECT_DIFFUSE_A));
		}

		SAnimation *pUVAnim = pMat2->getCollada().getAnimation(pMat2->getOriginal().instanceEffect.url + 1, SChannel::CT_EFFECT_TRANSFORM);
		m_bHasUVAnimation = pUVAnim != 0;
		m_particleSystem->setParameter(TEXTURE_TRANSFORM_TRACK_N, pUVAnim);
#endif
	}
}

video::SMaterial &
CParticleSystemSceneNode::getMaterial(u32 num) 
{ 
	return m_Materials[num]->get(); 
}

//! Attach the particle system to the forces include under a provided node
void 
CParticleSystemSceneNode::attach(ISceneNode* node)
{
	int iForceCount = m_pForceNodes->size();
	m_Forces.reallocate(iForceCount);
	m_Forces.set_used(iForceCount);

	for(int i = 0; i < iForceCount; i++)
	{
		scene::ISceneNode *pNode = node->getSceneNodeFromUID(((*m_pForceNodes)[i]) + 1);
		if(pNode)
		{
			core::list<scene::ISceneNode*>::ConstIterator childIt = pNode->getChildren().begin();
			for(; childIt != pNode->getChildren().end(); childIt++)
			{
				if((*childIt)->getType() == scene::ESNT_COLLADA_FORCE)
				{
					particle_system::CForceSceneNode *forceSceneNode = (particle_system::CForceSceneNode *)*childIt;
					forceSceneNode->bind(this);
				}
			}
		}
	}
}


}; //end namespace collada
}; //end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
