#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaFactory.h"
#include "CColladaMesh.h"
#include "CEmptySceneNode.h"
#include "CColladaAnimationTrack.h"
#include "CColladaSceneNodeAnimator.h"
#include "CColladaMorphingMesh.h"
#include "CColladaSkinnedMesh.h"
#include "CColladaModularSkinnedMesh.h"
#include "CColladaMeshSceneNode.h"
#include "CColladaSkinnedMeshSceneNode.h"
#include "IColladaRootSceneNode.h"
#include "CColladaRootSceneNode.h"
#include "CColladaSceneNode.h"
#include "CColladaLightSceneNode.h"
#include "CColladaCameraSceneNode.h"
#include "CColladaParticleSystemSceneNode.h"
#include "CColladaParticleSystemGravitySceneNode.h"
#include "CColladaParticleSystemWindSceneNode.h"
#include "CColladaParticleSystemDeflectorSceneNode.h"
#include "CColladaBillboardSceneNode.h"
#include "CColladaModularSkinnedMeshSceneNode.h"

namespace irr
{
namespace collada
{

CAnimationTrack*
CColladaFactory::createAnimation()
{
	return 0;
}

CSceneNodeAnimator*
CColladaFactory::createAnimator(const CColladaDatabase& database, SLibraryAnimationClips* clips)
{
	return irrnew CSceneNodeAnimator(database, *clips);
}

CLightSceneNode*
CColladaFactory::createLight(const CColladaDatabase& database, SLight* light)
{
	return irrnew CLightSceneNode(database, *light);
}

CMaterial*
CColladaFactory::createMaterial(const CColladaDatabase& database, SMaterial* material, IRootSceneNode* root)
{
	return irrnew CMaterial(database, *material, root);
}

scene::CColladaMesh*
CColladaFactory::createGeometry(const CColladaDatabase& database, SGeometry* geometry) {
	return irrnew scene::CColladaMesh(database, *geometry);
}

CColladaMorphingMesh*
CColladaFactory::createMorph(const CColladaDatabase& database, SController* controller)
{
	return irrnew CColladaMorphingMesh(database, *controller);
}

CSceneNode*
CColladaFactory::createBillboard(const CColladaDatabase& database, SNode* node)
{
	_IRR_DEBUG_BREAK_IF(node->pBillboard == 0);
	return irrnew CBillboardSceneNode(database, node);
}

CSceneNode*
CColladaFactory::createNode(const CColladaDatabase& database, SNode* node)
{
	return irrnew CSceneNode(database, node);
}

CCameraSceneNode*
CColladaFactory::createCameraNode(const CColladaDatabase& database, SCamera *camera)
{
	return irrnew CCameraSceneNode(database, *camera);
}

scene::CColladaMeshSceneNode*
CColladaFactory::createMeshNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties)
{
	return irrnew scene::CColladaMeshSceneNode(mesh, root);
}

CSkinnedMeshSceneNode*
CColladaFactory::createSkinNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties)
{
	return irrnew CSkinnedMeshSceneNode(mesh, root);
}

CModularSkinnedMeshSceneNode*
CColladaFactory::createModularSkinNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties)
{
	return irrnew CModularSkinnedMeshSceneNode(mesh, root);
}

CRootSceneNode* 
CColladaFactory::createScene(const CColladaDatabase& database)
{
	return irrnew CRootSceneNode(database);
}

scene::CColladaSkinnedMesh*
CColladaFactory::createSkin(const CColladaDatabase& database, SController* controller, res::vector<res::String>* skeletons, IRootSceneNode* root)
{
	return irrnew scene::CColladaSkinnedMesh(database, *controller, skeletons, root);
}

scene::CColladaModularSkinnedMesh*
CColladaFactory::createModularSkin(const CColladaDatabase& database, SInstanceModularSkin* controller, IRootSceneNode* root)
{
	return irrnew scene::CColladaModularSkinnedMesh(database, controller, root);
}

CParticleSystemSceneNode*
CColladaFactory::createParticleSystem(const CColladaDatabase& database, SEmitter* emitter, res::vector<res::String>* forces, IRootSceneNode* root)
{
	CParticleSystemSceneNode* ps = irrnew CParticleSystemSceneNode(database, *emitter, forces, root);
	ps->initParticleSystem();
	return ps;
}

particle_system::CForceSceneNode*
CColladaFactory::createParticleSystemGravityForce(const CColladaDatabase& database, SForce* force)
{
	return irrnew particle_system::CGravityForceSceneNode(database, *force);
}

particle_system::CForceSceneNode*
CColladaFactory::createParticleSystemWindForce(const CColladaDatabase& database, SForce* force)
{
	return irrnew particle_system::CWindForceSceneNode(database, *force);
}

particle_system::CForceSceneNode*
CColladaFactory::createParticleSystemDeflectorForce(const CColladaDatabase& database, SForce* force)
{
	return irrnew particle_system::CDeflectorForceSceneNode(database, *force);
}


}; //namespace collada
}; //namespace irr

#endif 
