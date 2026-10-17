//-*-c++-*-
#pragma once

#include "resVector.h"
#include "resFile.h"

namespace irr
{

namespace scene
{
	class ISceneNode;
	class IColladaMesh;
	class CColladaMesh;
	class CEmptySceneNode;
	class CColladaMeshSceneNode;
	class CColladaSkinnedMesh;
	class CColladaModularSkinnedMesh;
}

namespace collada
{
class CColladaDatabase;

struct SGeometry;
struct SLight;
struct SMaterial;
struct SController;
struct SCamera;
struct SLibraryAnimationClips;
struct SNode;
struct SEmitter;
struct SForce;
struct SInstanceModularSkin;

class CAnimationTrack;
class CSceneNodeAnimator;
class CMaterial;
class IRootSceneNode;
class CRootSceneNode;
class CColladaMorphingMesh;
class CCameraSceneNode;
class CSkinnedMeshSceneNode;
class CSceneNode;
class CLightSceneNode;
class CParticleSystemSceneNode;
class CModularSkinnedMeshSceneNode;

namespace particle_system
{
class CForceSceneNode;	
}

class CColladaFactory
{
public:
	//TODO: createImage
	CColladaFactory() {}
	virtual ~CColladaFactory() {}

	virtual CAnimationTrack*					createAnimation();
	virtual CSceneNodeAnimator*					createAnimator(const CColladaDatabase& database, SLibraryAnimationClips* clips);
	virtual CLightSceneNode*					createLight(const CColladaDatabase& database, SLight* light);
	virtual CMaterial*							createMaterial(const CColladaDatabase& database, SMaterial* material, IRootSceneNode* root);
	virtual scene::CColladaMesh*				createGeometry(const CColladaDatabase& database, SGeometry* geometry);
	virtual CColladaMorphingMesh*				createMorph(const CColladaDatabase& database, SController* controller); 
	virtual CSceneNode*							createNode(const CColladaDatabase& database, SNode* node = 0);
	virtual CSceneNode*							createBillboard(const CColladaDatabase& database, SNode* node);
	virtual CCameraSceneNode*					createCameraNode(const CColladaDatabase& database, SCamera* camera);
	virtual scene::CColladaMeshSceneNode*		createMeshNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties);
	virtual CSkinnedMeshSceneNode*				createSkinNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties);
	virtual CModularSkinnedMeshSceneNode*		createModularSkinNode(const CColladaDatabase& database, scene::IColladaMesh* mesh, IRootSceneNode* root, void* userProperties);
	virtual CRootSceneNode*						createScene(const CColladaDatabase& database);
	virtual scene::CColladaSkinnedMesh*			createSkin(const CColladaDatabase& database, SController* controller, res::vector<res::String>* skeletons, IRootSceneNode* root);
	virtual scene::CColladaModularSkinnedMesh*	createModularSkin(const CColladaDatabase& database, SInstanceModularSkin* controller, IRootSceneNode* root);
	virtual CParticleSystemSceneNode*			createParticleSystem(const CColladaDatabase& database, SEmitter* emitter, res::vector<res::String>* forces, IRootSceneNode* root);
	virtual	particle_system::CForceSceneNode*	createParticleSystemGravityForce(const CColladaDatabase& database, SForce* force);
	virtual particle_system::CForceSceneNode*	createParticleSystemWindForce(const CColladaDatabase& database, SForce* force);
	virtual particle_system::CForceSceneNode*	createParticleSystemDeflectorForce(const CColladaDatabase& database, SForce* force);

};

} //namespace collada
} //namespace irr
