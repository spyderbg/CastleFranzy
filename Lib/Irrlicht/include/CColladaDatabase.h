//-*-c++-*_
#ifndef __CCOLLADADATABASE_H__
#define __CCOLLADADATABASE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaResFileManager.h"
//#include "CColladaMaterial.h"
//#include "CColladaFactory.h"

class CColladaMesh;

namespace irr
{
namespace scene
{
	class ISceneNode;
	class IColladaMesh;
	class CColladaMesh;
	class CColladaSkinnedMesh;
	class CChannelAnimator;
	class CSceneNodeAnimatorChannelLibrary;
	class CLightSceneNode;
}

namespace collada
{
namespace particle_system
{
	class CForceSceneNode;
}
	
	class CParticleSystemSceneNode;
	class CAnimationTrack;
	class CSceneNodeAnimator;
	class IColladaRootSceneNode;
	class IRootSceneNode;
	class CCameraSceneNode;
	class CColladaFactory;
	class CMaterial;
	class CImage;
	class CAnimationTrackEx;


class CColladaDatabase
{
public:

	enum E_VERSION_CHECK_BEHAVIOR
	{
		EVCB_LOAD_ANYWAY,
		EVCB_DONT_LOAD,
	};

private:

	CResFile* ResFile;

	inline SCollada& getCollada() const
	{
		_IRR_DEBUG_BREAK_IF(ResFile == NULL);
		return *(SCollada*)ResFile->getDataPtr();
	}

	CColladaFactory* Factory;
	static CColladaFactory DefaultFactory;
	static bool OptimizeSceneNodes;
	static E_VERSION_CHECK_BEHAVIOR VersionCheckBehavior;

public:

	static E_VERSION_CHECK_BEHAVIOR getVersionCheckBehavior()
	{
		return VersionCheckBehavior;
	}
	static void setVersionCheckBehavior(E_VERSION_CHECK_BEHAVIOR behavior)
	{
		VersionCheckBehavior = behavior;
	}

	static void optimizeSceneNodes(bool optimize)
	{
		OptimizeSceneNodes = optimize;
	}

	enum E_DATABASE_TYPE
	{
		EDT_ANIMATION		= 1,
		EDT_ANIMATION_CLIP	= 2,
		EDT_IMAGE			= 4,
		EDT_EFFECT			= 8,
		EDT_MATERIAL		= 16,
		EDT_GEOMETRY		= 32,
		EDT_CONTROLLER		= 64,
		EDT_VISUAL_SCENE	= 128,
		EDT_SCENE			= 256,
		EDT_ALL				= 0xFFFFFFFF,
	};

	CColladaDatabase(const CColladaDatabase& cpyOf)
		: ResFile(cpyOf.ResFile), Factory(cpyOf.Factory)
	{
		// if ref count is zero, it means that resFile is actually being
		// destroyed (or there is a missing grab somewhere)
		if (ResFile && ResFile->getReferenceCount() > 0)
		{
			ResFile->grab();
		}
	}

	explicit CColladaDatabase(CResFile* resFile = NULL, CColladaFactory* factory = &DefaultFactory)
		: ResFile(resFile), Factory(factory)
	{
		// if ref count is zero, it means that resFile is actually being
		// destroyed (or there is a missing grab somewhere)

		if (ResFile && ResFile->getReferenceCount() > 0)
		{
			ResFile->grab();
		}
	}

	CColladaDatabase(const char* url, CColladaFactory* factory = &DefaultFactory);
	explicit CColladaDatabase(io::IReadFile* file, CColladaFactory* factory = &DefaultFactory);
	CColladaDatabase(io::IReadFile* file, bool refData, CColladaFactory* factory = &DefaultFactory);

	CColladaDatabase(const CColladaDatabase* caller, const char* url, CColladaFactory* factory = &DefaultFactory);
	~CColladaDatabase();

	CColladaDatabase& operator=(const CColladaDatabase& rhs)
	{
		// if ref count is zero, it means that resFile is actually being destroyed
		// (or there is a missing grab somewhere)
		if (ResFile && ResFile->getReferenceCount() > 0)
		{
			ResFile->drop();
			if (collada::CResFileManager::getInst()->isAutoUnload()
				&& ResFile->getReferenceCount() == 1)
			{
				collada::CResFileManager::getInst()->unload(ResFile->absoluteFilename.c_str());
			}
			ResFile = 0;
		}

		ResFile = rhs.ResFile;
		Factory = rhs.Factory;
		// if ref count is zero, it means that resFile is actually being
		// destroyed (or there is a missing grab somewhere)
		if (ResFile && ResFile->getReferenceCount() > 0)
		{
			ResFile->grab();
		}
		return *this;
	}

	bool operator==(const CColladaDatabase& rhs)
	{
		return ResFile == rhs.ResFile;
	}

	operator bool () const
	{
		return isValid();
	}

	bool isValid() const
	{
		return ResFile != NULL;
	}

	bool isSampled() const
	{
		return getCollada().configurations.samplingRate != 0;
	}
	
	int getSamplingRate() const
	{
		return getCollada().configurations.samplingRate;
	}

	
	SConfig::E_UpVector getUpVector() const
	{
		return getCollada().configurations.upVector;
	};

	const SEventsTrack * getEventsTrack() const
	{
		return getCollada().libraryAnimations.eventsTrack;
	}

	void* find(const char* url /*in*/, unsigned int& typeMask /*in_out*/) const;

	bool				getDefaultValue(const char* url, SChannel::Type type, void** value, int res0 = 0) const;
	bool				getDefaultValue(const SAnimation*, void** value) const;
	bool				getDefaultValue(const SChannel*, void** value) const;
	SAnimation*			getBlendableAnimation(const SAnimation*) const;
	SAnimation*			getBlendableAnimation(const SChannel*) const;

	int					getAnimationCount() const { return getCollada().libraryAnimations.animations.size(); }
	int					getAnimationClipCount() const { return getCollada().libraryAnimationClips.animationClips.size(); }
	int					getLightCount() const {return getCollada().libraryLights.lights.size(); }
	int		 			getCameraCount() const {return getCollada().libraryCameras.cameras.size(); }
	int					getImageCount() const { return getCollada().libraryImages.images.size(); }
	int					getEffectCount() const { return getCollada().libraryEffects.effects.size(); }
	int					getMaterialCount() const { return getCollada().libraryMaterials.materials.size(); }
	int					getGeometryCount() const { return getCollada().libraryGeometries.geometries.size(); }
	int					getControllerCount() const { return getCollada().libraryControllers.controllers.size(); }
	int					getVisualSceneCount() const { return getCollada().libraryVisualScene.visualScenes.size(); }
	int					getSceneCount() const { return 1; }

	SAnimation*			getAnimation(const char* url, SChannel::Type type, unsigned char reserved0 = -1) const;
	SAnimation*			getAnimation(const char* url) const;
	SAnimationClip*		getAnimationClip(const char* url) const;
	SLight*				getLight(const char* url) const;
	SCamera*			getCamera(const char* url) const;
	SImage*				getImage(const char* url) const;
	SEffect*			getEffect(const char* url) const;
	SMaterial*			getMaterial(const char* url) const;
	SMaterial*			getMaterial(const char* filename, const char* url) const;
	SGeometry*			getGeometry(const char* url) const;
	SController*		getController(const char* url) const;
	SVisualScene*		getVisualScene(const char* url) const;
	SNode*				getNode(const char* url) const;
	SNode*				getNode(const char* url, SNode& node) const;
	SEmitter*			getEmitter(const char* url) const;
	SForce*				getForce(const char* url) const;

	const char* 		getVersion() const;
	SAnimation* 		getAnimation(int index) const;
	SAnimationClip* 	getAnimationClip(int index) const;
	SLight* 			getLight(int index) const;
	SCamera* 			getCamera(int index) const;
	SImage* 			getImage(int index) const;
	SEffect* 			getEffect(int index) const;
	SMaterial* 			getMaterial(int index) const;
	SGeometry* 			getGeometry(int index) const;
	SController* 		getController(int index) const;
	SVisualScene* 		getVisualScene(int index) const;
	SEmitter* 			getEmitter(int index) const;
	SForce* 			getForce(int index) const;

	SLibraryAnimationClips*  	getAnimationClipLibrary() const;
	SScene* 				 	getScene() const;
	const char* 				getAbsoluteFilename() const { return ResFile ? ResFile->absoluteFilename.c_str() : NULL; }

	static const CAnimationTrackEx*  getAnimationTrackEx(const SAnimation* animator);
	static const CAnimationTrackEx*  getAnimationTrackEx(const SChannel* channel);

	CAnimationTrack* 		constructAnimation(const char* url) const;
	CAnimationTrack* 		constructAnimation(int index) const;
	CAnimationTrack* 		constructAnimation(SAnimation* animator) const;

	CSceneNodeAnimator* 	constructAnimator() const;
	static CSceneNodeAnimator* 	constructAnimator(const char* filename, CColladaFactory* factory = &DefaultFactory);
	static CSceneNodeAnimator* 	constructAnimator(io::IReadFile* file, CColladaFactory* factory = &DefaultFactory);
	static CSceneNodeAnimator* 	constructAnimator(io::IReadFile* file, bool refData, CColladaFactory* factory = &DefaultFactory);
	
	scene::CLightSceneNode*   constructLight(int index, IRootSceneNode* root) const;
	scene::CLightSceneNode*   constructLight(const char* url, IRootSceneNode* root) const;
	scene::CLightSceneNode*   constructLight(SLight* light, IRootSceneNode* root) const;

	CMaterial* 				constructMaterial(int index, IRootSceneNode* root = 0) const;
	CMaterial* 				constructMaterial(const char* url, IRootSceneNode* root = 0) const;
	CMaterial* 				constructMaterial(SMaterial* material, IRootSceneNode* root = 0) const;
	
	CImage* 				constructImage(int index, IRootSceneNode* root = 0) const;
	CImage* 				constructImage(const char* url, IRootSceneNode* root = 0) const;
	CImage* 				constructImage(SImage* image, IRootSceneNode* root = 0) const;

	CCameraSceneNode* 		constructCamera(int index, IRootSceneNode* root = 0) const;
	CCameraSceneNode* 		constructCamera(const char* url, IRootSceneNode* root = 0) const;
	CCameraSceneNode* 		constructCamera(SCamera* camera, IRootSceneNode* root = 0) const;
	CCameraSceneNode* 		constructCamera(SInstanceCamera* instance, IRootSceneNode* root = 0) const;

	scene::CColladaMesh* 	constructGeometry(const char* url, IRootSceneNode* root = 0) const;
	scene::CColladaMesh* 	constructGeometry(int index, IRootSceneNode* root = 0) const;
	scene::CColladaMesh* 	constructGeometry(SGeometry* geometry, IRootSceneNode* root = 0) const;
	scene::CColladaMesh* 	constructGeometry(SInstanceGeometry* instance, IRootSceneNode* root = 0) const;
	scene::CColladaMesh*    constructGeometry(const char* file, const char* url, IRootSceneNode* root) const;

	scene::IColladaMesh* 	constructMorph(SController* controller, IRootSceneNode* root = 0) const;
	scene::IColladaMesh* 	constructSkin(SController* controller, IRootSceneNode* root = 0) const;
	scene::IColladaMesh* 	constructModularSkin(SInstanceModularSkin* controller, IRootSceneNode* root = 0) const;
	
	scene::IColladaMesh* 	constructController(const char* url, IRootSceneNode* root = 0) const;
	scene::IColladaMesh* 	constructController(int index, IRootSceneNode* root = 0) const;
	scene::IColladaMesh* 	constructController(SController* geometry, IRootSceneNode* root = 0) const;
	scene::IColladaMesh* 	constructController(SInstanceController* instance, IRootSceneNode* root = 0) const;

	scene::ISceneNode* 		constructNode(SNode* node) const;
	scene::ISceneNode* 		constructNode(const char* url) const;
	scene::ISceneNode* 		constructNode(SNode* node, IRootSceneNode* root) const;
	static scene::ISceneNode* 		constructNode(const char* filename, const char* url, CColladaFactory* factory = &DefaultFactory);
	static scene::ISceneNode* 		constructNode(io::IReadFile* file, const char* url, CColladaFactory* factory = &DefaultFactory);
	static scene::ISceneNode* 		constructNode(io::IReadFile* file, const char* url, bool refData, CColladaFactory* factory = &DefaultFactory);

	CParticleSystemSceneNode* 	constructEmitter(const char* url, res::vector<res::String> *forceNodes, IRootSceneNode* root) const;
	CParticleSystemSceneNode* 	constructEmitter(int index, res::vector<res::String> *forceNodes, IRootSceneNode* root) const;
	CParticleSystemSceneNode* 	constructEmitter(SInstanceEmitter* instance, IRootSceneNode* root) const;
	CParticleSystemSceneNode* 	constructEmitter(SEmitter* emitter, res::vector<res::String> *forceNodes, IRootSceneNode* root) const;

	particle_system::CForceSceneNode* 	constructForce(const char* url, IRootSceneNode* root) const;
	particle_system::CForceSceneNode* 	constructForce(int index, IRootSceneNode* root) const;
	particle_system::CForceSceneNode* 	constructForce(SInstanceForce* instance, IRootSceneNode* root) const;
	particle_system::CForceSceneNode* 	constructForce(SForce* force, IRootSceneNode* root) const;

	scene::ISceneNode* 		constructVisualScene(const char* url, IRootSceneNode* root = 0) const;
	scene::ISceneNode* 		constructVisualScene(int index, IRootSceneNode* root = 0) const;
	scene::ISceneNode* 		constructVisualScene(SVisualScene* visualScene, IRootSceneNode* root = 0) const;

	scene::ISceneNode* 		constructScene() const;
	scene::ISceneNode* 		constructScene(bool createAndAddAnimator) const;
	static scene::ISceneNode* 		constructScene(const char* filename, CColladaFactory* factory = &DefaultFactory);
	static scene::ISceneNode* 		constructScene(const char* filename, bool bCreateAndAddAnimator, CColladaFactory* factory = &DefaultFactory);
	static scene::ISceneNode* 		constructScene(io::IReadFile* file, bool bCreateAndAddAnimator, CColladaFactory* factory = &DefaultFactory);
	static scene::ISceneNode* 		constructScene(io::IReadFile* file, bool bCreateAndAddAnimator, bool refData, CColladaFactory* factory = &DefaultFactory);
	
	void	attachSkin(IRootSceneNode* root, scene::ISceneNode* attach) const;
};

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADADATABASE_H__
